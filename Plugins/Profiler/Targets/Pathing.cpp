#include "Targets/Pathing.hpp"

#include "API/CAppManager.hpp"
#include "API/CGameObject.hpp"
#include "API/CNWSArea.hpp"
#include "API/CNWSCreature.hpp"
#include "API/CNWSCreatureStats.hpp"
#include "API/CNWSModule.hpp"
#include "API/CNWSObject.hpp"
#include "API/CPathfindInformation.hpp"
#include "API/CServerExoApp.hpp"
#include "API/Constants.hpp"
#include "API/Functions.hpp"
#include "API/Globals.hpp"
#include "Timing.hpp"

#include <chrono>
#include <cstring>

namespace Profiler {

using namespace NWNXLib;
using namespace NWNXLib::API;
using namespace NWNXLib::API::Constants;
using namespace NWNXLib::API::Globals;
using namespace NWNXLib::Services;

static MetricsProxy* g_metrics;
static Hooks::Hook s_PlotPathHook;

static bool g_moverTags = false;
static std::chrono::nanoseconds g_slowThreshold{0}; // 0 = slow-call log off
static uint32_t g_slowMaxPerMinute = 10;

// Low-cardinality description of who is pathing. Only plain field reads.
static const char* GetMover(CNWSCreature* creature, CPathfindInformation* pfi)
{
    if (creature->m_pStats && creature->m_pStats->m_bIsDMCharacterFile)
        return "dm";
    if (creature->m_bPlayerCharacter)
        return pfi->m_bClientMoving ? "pc-click" : "pc-script";
    if (!creature->m_sTemplate.m_sString || !creature->m_sTemplate.m_sString[0])
        return "copy";
    return "npc";
}

static MetricData::Tags BuildTags(CPathfindInformation* pfi)
{
    MetricData::Tags tags;
    if (!pfi)
        return tags;

    CServerExoApp* server = AppManager()->m_pServerExoApp;
    CNWSCreature* creature = server->GetCreatureByGameObjectID(pfi->m_oidSelf);

    if (creature)
    {
        CExoString& resRef = creature->m_sTemplate;
        tags.emplace_back("ResRef", resRef.m_sString ? resRef.m_sString : "(unknown)");
        CNWSArea* area = server->GetAreaByGameObjectID(creature->m_oidArea);

        if (area)
        {
            std::string areaName = std::string(area->m_cResRef.GetResRef(), 16);
            tags.emplace_back("Area", areaName.empty() ? "(unknown)" : std::move(areaName));
        }

        if (g_moverTags)
        {
            tags.emplace_back("Mover", GetMover(creature, pfi));
            tags.emplace_back("HasTarget", pfi->m_oidMovingTo != OBJECT_INVALID ? "yes" : "no");
        }
    }

    return tags;
}

static std::string ResRefString(CResRef& resRef)
{
    const char* s = resRef.GetResRefStr();
    return std::string(s, strnlen(s, 16));
}

struct PathRequest
{
    OBJECT_ID oidSelf;
    OBJECT_ID oidMovingTo;
    float startX, startY, startZ;
    float endX, endY, endZ;
};

static void LogSlowCall(const PathRequest& req, CPathfindInformation* pfi, std::chrono::nanoseconds elapsed, int32_t ret)
{
    using clock = std::chrono::steady_clock;
    static clock::time_point s_windowStart;
    static uint32_t s_logged = 0;
    static uint32_t s_suppressed = 0;

    const auto now = clock::now();
    if (now - s_windowStart >= std::chrono::minutes(1))
    {
        if (s_suppressed)
            LOG_INFO("PlotPath slow-call log: suppressed %u lines in the last minute", s_suppressed);
        s_windowStart = now;
        s_logged = 0;
        s_suppressed = 0;
    }

    if (s_logged >= g_slowMaxPerMinute)
    {
        ++s_suppressed;
        return;
    }
    ++s_logged;

    CServerExoApp* server = AppManager()->m_pServerExoApp;
    std::string who = "(unknown)", mover = "-", areaResRef = "(none)", target = "(none)";

    if (auto* creature = server->GetCreatureByGameObjectID(req.oidSelf))
    {
        mover = GetMover(creature, pfi);
        if (creature->m_bPlayerCharacter && creature->m_pStats)
            who = creature->m_pStats->GetFullName().CStr();
        else
            who = creature->m_sTag.CStr();
        if (auto* area = server->GetAreaByGameObjectID(creature->m_oidArea))
            areaResRef = ResRefString(area->m_cResRef);
    }

    if (req.oidMovingTo != OBJECT_INVALID)
    {
        auto* gameObject = server->GetGameObject(req.oidMovingTo);
        auto* object = gameObject ? gameObject->AsNWSObject() : nullptr;
        target = object ? std::string(object->m_sTag.CStr()) : "(gone)";
    }

    LOG_INFO("Slow PlotPath: %.1f ms by '%s' (0x%x, %s) in %s from (%.1f, %.1f, %.1f) to (%.1f, %.1f, %.1f)"
             " target '%s' calls %d result %d",
             elapsed.count() / 1e6, who.c_str(), req.oidSelf, mover.c_str(), areaResRef.c_str(),
             req.startX, req.startY, req.startZ, req.endX, req.endY, req.endZ,
             target.c_str(), pfi->m_nCallsToPlotPath, ret);
}

static int32_t PlotPathHook(CNWSModule* module, CPathfindInformation* pfi, uint32_t timeSlice)
{
    if (g_slowThreshold.count() == 0 || !pfi)
    {
        // Same timing as before when the slow-call log is off.
        FastTimerScope timerScope(*g_metrics, "PlotPath", BuildTags(pfi));
        return s_PlotPathHook->CallOriginal<int32_t>(module, pfi, timeSlice);
    }

    // PlotPath may reset the path fields, so copy the request first.
    const PathRequest req = { pfi->m_oidSelf, pfi->m_oidMovingTo,
                              pfi->m_fNewPointStartX, pfi->m_fNewPointStartY, pfi->m_fNewPointStartZ,
                              pfi->m_fNewPointEndX, pfi->m_fNewPointEndY, pfi->m_fNewPointEndZ };

    const auto start = std::chrono::steady_clock::now();
    int32_t ret;
    {
        FastTimerScope timerScope(*g_metrics, "PlotPath", BuildTags(pfi));
        ret = s_PlotPathHook->CallOriginal<int32_t>(module, pfi, timeSlice);
    }
    const auto elapsed = std::chrono::steady_clock::now() - start;

    if (elapsed >= g_slowThreshold)
        LogSlowCall(req, pfi, std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed), ret);

    return ret;
}

Pathing::Pathing(MetricsProxy* metrics)
{
    g_metrics = metrics;

    g_moverTags = Config::Get<bool>("PATHING_MOVER_TAGS", false);

    if (Config::Get<bool>("PATHING_SLOW_LOG", false))
    {
        g_slowThreshold = std::chrono::milliseconds(Config::Get<uint32_t>("PATHING_SLOW_LOG_MS", 100));
        g_slowMaxPerMinute = Config::Get<uint32_t>("PATHING_SLOW_LOG_PER_MINUTE", 10);
        if (g_slowThreshold.count() == 0)
            g_slowThreshold = std::chrono::nanoseconds(1);
        LOG_INFO("PlotPath slow-call log: calls over %lld ms, at most %u lines a minute",
                 (long long)std::chrono::duration_cast<std::chrono::milliseconds>(g_slowThreshold).count(), g_slowMaxPerMinute);
    }

    if (g_moverTags)
        LOG_INFO("PlotPath timings tagged with Mover and HasTarget");

    s_PlotPathHook = Hooks::HookFunction(&CNWSModule::PlotPath, &PlotPathHook, Hooks::Order::Earliest);
}

}
