// EXPERIMENTAL. Skips CNWSObject::UpdateEffectList on objects that have nothing on their effect list due yet.
//
// Every AI update (creatures, placeables, doors, items) calls UpdateEffectList, which walks the whole applied-effects
// list. On a creature most of that list is equipped and innate item-property effects that never expire, so the walk
// is almost always wasted: profiles put it at 8-10% of the main thread, nearly all of it loading effect pointers.
//
// This plugin keeps, per object, the earliest time anything on its list can be due, and skips the call until then.
// What UpdateEffectList does, from the 8193.37 disassembly (CNWSObject::UpdateEffectList at 0x569df0), per effect:
//   - Regenerate (7), Poison (35), Disease (5), Wounding (84), Swarm (85): periodic work on timers kept in the
//     effect's integers, every call. An object with any of these is never skipped.
//   - RunScript (96) with an interval script (string 3): runs the script on its interval. Never skipped. Without an
//     interval script it is an ordinary effect.
//   - Icon (67), on a creature, temporary, integer 1 still 0: once now + 5000 ms reaches the expiry it sets integer 1
//     and the creature's icon flash state. Due at expiry - 5 s.
//   - Any temporary effect: removed once now > expiry (CWorldTimer::CompareWorldTimes(now, expiry) == 1).
// Nothing else in the function does work, so if no effect is due the original call is a no-op and skipping it is
// exact. The same comparisons are used here, through the same timer (CServerExoApp::GetActiveTimer(m_idSelf)).
//
// The cached due time goes stale when the list or an effect on it changes. It is dropped:
//   - on CServerAIMaster::OnEffectApplied and OnEffectRemoved. CNWSObject::ApplyEffect, RemoveEffect and
//     RemoveEffectById are the only engine functions that change m_appliedEffects (binary scan), and each calls one
//     of these. In-place expiry changes in the engine (OnApplyTimestop, OnRemovePolymorph, ApplyEffect itself) all
//     happen inside them;
//   - when NWNX_Effect edits an applied effect in place (ReplaceEffect, ReplaceEffectByIndex), which broadcasts
//     NWNX_EFFECT_APPLIED_EFFECT_EDITED with the object ID;
//   - when the list's length differs from when the cache was built;
//   - every REFRESH_SECONDS of world time, as a backstop for any edit nothing above sees.
// After any call that is not skipped, the cache is rebuilt from the list as it stands after the call.
//
// Off unless NWNX_EFFECTLISTCACHE_ENABLE=true. If any hook fails to install the plugin stays inactive (fail closed).
// NWNX_EffectListCache_Disable() stops skipping at runtime; NWNX_EffectListCache_Enable() starts it again.

#include "nwnx.hpp"

#include "API/CAppManager.hpp"
#include "API/CExoString.hpp"
#include "API/CGameEffect.hpp"
#include "API/CNWSObject.hpp"
#include "API/CServerAIMaster.hpp"
#include "API/CServerExoApp.hpp"
#include "API/CWorldTimer.hpp"
#include "API/Constants.hpp"
#include "API/Globals.hpp"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <unordered_map>

namespace EffectListCache {

using namespace NWNXLib;
using namespace NWNXLib::API;
using namespace NWNXLib::API::Constants;

// The icon "about to expire" check in UpdateEffectList adds this to now before comparing with the expiry.
static constexpr uint32_t ICON_WARNING_MS = 5000;

struct Entry
{
    CNWSObject* pObject;      // guards against a reused object ID
    int32_t nEffects;         // m_appliedEffects.num when built
    uint32_t nBuiltDay, nBuiltTime;
    uint32_t nExpiryDay, nExpiryTime; // earliest temporary expiry, if bExpiry
    uint32_t nIconDay, nIconTime;     // earliest expiry of a creature's not-yet-warned temporary icon, if bIcon
    bool bExpiry;
    bool bIcon;
    bool bPeriodic; // something on the list does work every call: never skip
};

static std::unordered_map<ObjectID, Entry> s_cache;

static bool s_configured; // ENABLE was set and every hook installed
static bool s_active;     // skipping now (NWNX_EffectListCache_Disable/Enable toggle this)
static bool s_allObjects; // also placeables, doors and items, not just creatures
static bool s_verify;     // testing: on every skip, rebuild the entry and check nothing was due
static uint32_t s_refreshMs;
static std::chrono::steady_clock::duration s_logPeriod;
static std::chrono::steady_clock::time_point s_nextLog;

// Counted since the last log line (and in total for GetStat).
enum Stat { Calls, Skipped, Periodic, Full, Rebuilt, Invalidated, VerifyMisses, StatCount };
static uint64_t s_stats[StatCount], s_statsTotal[StatCount];

static Hooks::Hook s_UpdateEffectListHook;
static Hooks::Hook s_OnEffectAppliedHook;
static Hooks::Hook s_OnEffectRemovedHook;

static inline void Count(Stat s) { s_stats[s]++; }

static CWorldTimer* ActiveTimer(ObjectID oid)
{
    return Globals::AppManager()->m_pServerExoApp->GetActiveTimer(oid);
}

static inline bool Before(uint32_t d1, uint32_t t1, uint32_t d2, uint32_t t2)
{
    return d1 < d2 || (d1 == d2 && t1 < t2);
}

static bool HasIntervalScript(CGameEffect* pEffect)
{
    // UpdateEffectList copies string 3 and takes the interval branch if the copy's length is non-zero. Treat any
    // allocated, non-empty or non-zero-length string as an interval script; erring this way only costs a skip.
    const CExoString& s = pEffect->GetString(3);
    return s.m_nBufferLength != 0 || (s.m_sString && s.m_sString[0]);
}

static void Build(CNWSObject* pObject, CWorldTimer* pTimer, uint32_t nDay, uint32_t nTime, Entry& e)
{
    e = Entry{};
    e.pObject = pObject;
    e.nEffects = pObject->m_appliedEffects.num;
    e.nBuiltDay = nDay;
    e.nBuiltTime = nTime;
    const bool bCreature = pObject->m_nObjectType == ObjectType::Creature;

    for (int32_t i = 0; i < pObject->m_appliedEffects.num; i++)
    {
        auto* pEffect = pObject->m_appliedEffects.element[i];
        if (!pEffect)
            continue;
        switch (pEffect->m_nType)
        {
            case EffectTrueType::Regenerate:
            case EffectTrueType::Poison:
            case EffectTrueType::Disease:
            case EffectTrueType::Wounding:
            case EffectTrueType::Swarm:
                e.bPeriodic = true;
                return;
            case EffectTrueType::RunScript:
                if (HasIntervalScript(pEffect))
                {
                    e.bPeriodic = true;
                    return;
                }
                break;
            default:
                break;
        }
        if (pEffect->GetDurationType() != EffectDurationType::Temporary)
            continue;

        uint32_t nExpDay, nExpTime;
        pEffect->GetExpiryTime(&nExpDay, &nExpTime);
        // CompareWorldTimes returns -2 for a time of day outside the day: such an effect never expires in
        // UpdateEffectList either, and must not become the minimum.
        if (pTimer->CompareWorldTimes(nExpDay, nExpTime, nExpDay, nExpTime) != 0)
            continue;

        if (bCreature && pEffect->m_nType == EffectTrueType::Icon && pEffect->GetInteger(1) == 0)
        {
            if (!e.bIcon || Before(nExpDay, nExpTime, e.nIconDay, e.nIconTime))
            {
                e.bIcon = true;
                e.nIconDay = nExpDay;
                e.nIconTime = nExpTime;
            }
        }
        if (!e.bExpiry || Before(nExpDay, nExpTime, e.nExpiryDay, e.nExpiryTime))
        {
            e.bExpiry = true;
            e.nExpiryDay = nExpDay;
            e.nExpiryTime = nExpTime;
        }
    }
}

// Whether UpdateEffectList(nDay, nTime) could do anything on an object whose list matches e (e.bPeriodic false).
static bool Due(const Entry& e, CWorldTimer* pTimer, uint32_t nDay, uint32_t nTime)
{
    if (e.bExpiry && pTimer->CompareWorldTimes(nDay, nTime, e.nExpiryDay, e.nExpiryTime) == 1)
        return true;
    if (e.bIcon)
    {
        uint32_t nWarnDay, nWarnTime;
        if (pTimer->AddWorldTimes(nDay, nTime, 0, ICON_WARNING_MS, &nWarnDay, &nWarnTime) != 0)
            return true; // the engine would compare uninitialised values here; let it
        if (pTimer->CompareWorldTimes(nWarnDay, nWarnTime, e.nIconDay, e.nIconTime) >= 0)
            return true;
    }
    return false;
}

static bool RefreshDue(const Entry& e, uint32_t nDay, uint32_t nTime)
{
    if (s_refreshMs == 0)
        return false;
    return nDay != e.nBuiltDay || nTime < e.nBuiltTime || nTime - e.nBuiltTime >= s_refreshMs;
}

static void Invalidate(ObjectID oid)
{
    if (s_cache.erase(oid))
        Count(Invalidated);
}

static void LogAndCollect()
{
    auto now = std::chrono::steady_clock::now();
    if (now < s_nextLog)
        return;
    s_nextLog = now + s_logPeriod;

    size_t nBefore = s_cache.size();
    for (auto it = s_cache.begin(); it != s_cache.end();)
    {
        if (Utils::GetGameObject(it->first) != it->second.pObject)
            it = s_cache.erase(it);
        else
            ++it;
    }
    LOG_INFO("Calls %llu: %llu skipped, %llu periodic, %llu full (%llu rebuilt); %llu invalidated; %llu verify "
             "misses; %zu cached (%zu gone)",
             (unsigned long long)s_stats[Calls], (unsigned long long)s_stats[Skipped],
             (unsigned long long)s_stats[Periodic], (unsigned long long)s_stats[Full],
             (unsigned long long)s_stats[Rebuilt], (unsigned long long)s_stats[Invalidated],
             (unsigned long long)s_stats[VerifyMisses], s_cache.size(), nBefore - s_cache.size());
    for (int i = 0; i < StatCount; i++)
    {
        s_statsTotal[i] += s_stats[i];
        s_stats[i] = 0;
    }
}

static void UpdateEffectListHook(CNWSObject* pThis, uint32_t nDay, uint32_t nTime)
{
    if (!s_active || pThis->m_appliedEffects.num == 0 ||
        (!s_allObjects && pThis->m_nObjectType != ObjectType::Creature))
    {
        s_UpdateEffectListHook->CallOriginal<void>(pThis, nDay, nTime);
        return;
    }

    Count(Calls);
    if ((s_stats[Calls] & 0xFFF) == 0)
        LogAndCollect();

    const ObjectID oid = pThis->m_idSelf;
    CWorldTimer* pTimer = nullptr;
    auto it = s_cache.find(oid);
    if (it != s_cache.end())
    {
        const Entry& e = it->second;
        if (e.pObject == pThis && e.nEffects == pThis->m_appliedEffects.num && !RefreshDue(e, nDay, nTime))
        {
            if (e.bPeriodic)
            {
                // The list hasn't changed and it has periodic work: call through, no need to rebuild.
                Count(Periodic);
                s_UpdateEffectListHook->CallOriginal<void>(pThis, nDay, nTime);
                return;
            }
            pTimer = ActiveTimer(oid);
            if (!Due(e, pTimer, nDay, nTime))
            {
                if (!s_verify)
                {
                    Count(Skipped);
                    return;
                }
                Entry fresh;
                Build(pThis, pTimer, nDay, nTime, fresh);
                if (!fresh.bPeriodic && !Due(fresh, pTimer, nDay, nTime))
                {
                    Count(Skipped);
                    return;
                }
                Count(VerifyMisses);
                if (s_stats[VerifyMisses] + s_statsTotal[VerifyMisses] <= 100)
                    LOG_ERROR("Verify: stale cache on %x (%d effects): skipping would have missed due work",
                              oid, pThis->m_appliedEffects.num);
            }
        }
    }

    Count(Full);
    s_UpdateEffectListHook->CallOriginal<void>(pThis, nDay, nTime);

    // Rebuild from the list as it is now. Look the entry up again: the call may have run scripts that changed the
    // cache (and invalidated `it`).
    if (pThis->m_appliedEffects.num == 0)
    {
        s_cache.erase(oid);
        return;
    }
    if (!pTimer)
        pTimer = ActiveTimer(oid);
    Build(pThis, pTimer, nDay, nTime, s_cache[oid]);
    Count(Rebuilt);
}

// A hook that failed to install leaves its trampoline pointing at the original function.
template <typename F>
static bool Installed(const Hooks::Hook& hook, F original)
{
    return hook && hook->GetOriginal() && hook->GetOriginal() != (void*)original;
}

void EffectListCache() __attribute__((constructor));
void EffectListCache()
{
    if (!Config::Get<bool>("ENABLE", false))
        return;

    auto nRefreshSeconds = Config::Get<int32_t>("REFRESH_SECONDS", 6);
    if (nRefreshSeconds < 0 || nRefreshSeconds > 3600)
    {
        LOG_WARNING("REFRESH_SECONDS must be 0-3600, using 6");
        nRefreshSeconds = 6;
    }
    s_refreshMs = (uint32_t)nRefreshSeconds * 1000;
    auto nLogSeconds = Config::Get<int32_t>("LOG_SECONDS", 60);
    if (nLogSeconds <= 0)
        nLogSeconds = 60;
    s_logPeriod = std::chrono::seconds(nLogSeconds);
    s_nextLog = std::chrono::steady_clock::now() + s_logPeriod;
    s_allObjects = Config::Get<bool>("ALL_OBJECTS", false);
    s_verify = Config::Get<bool>("VERIFY", false);
    s_cache.reserve(16384);

    s_UpdateEffectListHook = Hooks::HookFunction(&CNWSObject::UpdateEffectList, &UpdateEffectListHook,
                                                 Hooks::Order::Default);

    s_OnEffectAppliedHook = Hooks::HookFunction(&CServerAIMaster::OnEffectApplied,
        +[](CServerAIMaster* pThis, CNWSObject* pObject, CGameEffect* pEffect, BOOL bLoadingGame) -> int32_t
        {
            if (pObject)
                Invalidate(pObject->m_idSelf);
            return s_OnEffectAppliedHook->CallOriginal<int32_t>(pThis, pObject, pEffect, bLoadingGame);
        }, Hooks::Order::Early);

    s_OnEffectRemovedHook = Hooks::HookFunction(&CServerAIMaster::OnEffectRemoved,
        +[](CServerAIMaster* pThis, CNWSObject* pObject, CGameEffect* pEffect) -> int32_t
        {
            if (pObject)
                Invalidate(pObject->m_idSelf);
            return s_OnEffectRemovedHook->CallOriginal<int32_t>(pThis, pObject, pEffect);
        }, Hooks::Order::Early);

    // NWNX_Effect edits applied effects in place (duration, expiry, integers, strings).
    MessageBus::Subscribe("NWNX_EFFECT_APPLIED_EFFECT_EDITED",
        [](const std::vector<std::string>& message)
        {
            if (!message.empty())
                Invalidate((ObjectID)std::strtoul(message[0].c_str(), nullptr, 10));
        });

    struct { const char* name; bool ok; } hooks[] = {
        { "CNWSObject::UpdateEffectList",     Installed(s_UpdateEffectListHook, &CNWSObject::UpdateEffectList) },
        { "CServerAIMaster::OnEffectApplied", Installed(s_OnEffectAppliedHook, &CServerAIMaster::OnEffectApplied) },
        { "CServerAIMaster::OnEffectRemoved", Installed(s_OnEffectRemovedHook, &CServerAIMaster::OnEffectRemoved) },
    };
    bool bAllHooked = !Config::Get<bool>("DEBUG_SIMULATE_HOOK_FAILURE", false);
    for (auto& h : hooks)
    {
        if (!h.ok)
        {
            LOG_ERROR("Hook on %s did not install", h.name);
            bAllHooked = false;
        }
    }
    if (!bAllHooked)
    {
        LOG_ERROR("Not enabled: not every hook installed (or DEBUG_SIMULATE_HOOK_FAILURE is set)");
        return;
    }

    s_configured = true;
    s_active = true;
    LOG_INFO("Enabled: refresh every %d s, all objects %d, verify %d", nRefreshSeconds, s_allObjects, s_verify);
}

// Stops skipping and drops the cache. Returns TRUE if it was active.
NWNX_EXPORT ArgumentStack Disable(ArgumentStack&&)
{
    bool bWas = s_active;
    s_active = false;
    s_cache.clear();
    LOG_INFO("Disabled at runtime");
    return (int32_t)bWas;
}

// Starts skipping again after Disable. Returns TRUE if the plugin is configured and now active.
NWNX_EXPORT ArgumentStack Enable(ArgumentStack&&)
{
    if (!s_configured)
        return 0;
    if (!s_active)
    {
        s_cache.clear();
        s_active = true;
        LOG_INFO("Enabled at runtime");
    }
    return 1;
}

// Returns a counter since startup: 0 calls considered, 1 skipped, 2 periodic, 3 full, 4 rebuilt, 5 invalidated,
// 6 verify misses, 7 objects cached now. -1 if the plugin isn't configured.
NWNX_EXPORT ArgumentStack GetStat(ArgumentStack&& args)
{
    const auto n = args.extract<int32_t>();
    if (!s_configured)
        return -1;
    if (n == StatCount)
        return (int32_t)s_cache.size();
    if (n < 0 || n > StatCount)
        return -1;
    return (int32_t)std::min<uint64_t>(s_statsTotal[n] + s_stats[n], INT32_MAX);
}

}
