// EXPERIMENTAL. Two independent, opt-in cuts to the fixed per-creature cost of the engine's AI walk.
//
// 1. Height cache (NWNX_CREATUREAI_HEIGHT_CACHE=true)
//    CNWSCreature::AIUpdate sets m_vPosition.z = CNWSArea::ComputeHeight(m_vPosition) for every creature on every
//    update, moving or not. ComputeHeight finds the tile by scanning tile rows and columns, then walks the tile's
//    walkmesh (CNWTileSurfaceMesh::FindTriangle) for the triangle under the point. In 8193.37 the result depends only
//    on the area's tiles and on x and y: CNWSTile::ComputeHeight passes z = 0 to the walkmesh, and none of the
//    functions involved write anything except a one-off lazy CNWTileSurfaceMesh::LoadWalkMesh. So this caches
//    (area, tile array, x bits, y bits) -> height in a small direct-mapped table. A hit returns the exact float the
//    engine would. The table is marked stale when any area is destroyed (CNWSArea::~CNWSArea) and when a script
//    changes tiles (CNWSArea::SetTile, CNWSArea::SetTileset: SetTileJson and friends), and is cleared once on the next
//    lookup. NWNX_Area_RotateArea rewrites tiles without going through either, but it replaces the area's tile array,
//    which is part of the key; the first lookup that meets an entry of that area with the old array clears the table.
//    NWNX_Tileset only changes tiles while an area loads, before anything can be cached.
//
// 2. VERY_LOW throttle (NWNX_CREATUREAI_THROTTLE=true)
//    A creature at AI level VERY_LOW whose OnSpawn has run, in an area with no players, with nothing queued and not
//    in combat, has its AIUpdate run on one visit in THROTTLE_TICKS (default 4); the other visits return at once. Its
//    AIUpdate measures elapsed time from the world time it stored on its last real update, so timers, effect
//    durations, combat round and excited-state timers catch up in one step. The heartbeat and the NPC visible-list
//    (perception) update also run from AIUpdate (SpawnInHeartbeatPerception), and at VERY_LOW each waits for a number
//    of further visits once its interval has passed. Skipped visits are counted for them (CountSkippedChecks), so each
//    fires at most THROTTLE_TICKS - 1 visits later than without the throttle. Anything that gives the creature work
//    makes it run on its very next visit: a queued action, combat, a player entering its area, or its AI level being
//    raised. As a safety net a throttled creature also runs if THROTTLE_MAX_SKIP_MS (default 500) has passed since it
//    last ran.
//    With THROTTLE_WAITING=true a creature that is only waiting (an ActionWait at the head of the queue, followed by
//    nothing but ActionDoCommand) also counts as idle. Arelith's idle loop (gsAIActionNone: animation, ActionWait
//    2-4 s, ActionDoCommand to repeat) leaves most heartbeat-AI NPCs with a queue forever, so without this few live
//    creatures qualify. A Wait the engine queues in front of other work (pathfinding waits, random-walk pauses,
//    doors, follow, casting) has that work behind it, so it doesn't count.
//
// Each part installs its hooks only when enabled, and stays off if any of its hooks fails to install (fail closed).
// NWNX_CreatureAI_SetHeightCache / SetThrottle switch a configured part off and on at runtime.

#include "nwnx.hpp"

#include "API/CExoLinkedListInternal.hpp"
#include "API/CNWSArea.hpp"
#include "API/CNWSCombatRound.hpp"
#include "API/CNWSCreature.hpp"
#include "API/CNWSCreatureStats.hpp"
#include "API/CExoLinkedListNode.hpp"
#include "API/CNWSObjectActionNode.hpp"
#include "API/CResRef.hpp"
#include "API/CServerAIMaster.hpp"
#include "API/CAppManager.hpp"
#include "API/CServerExoApp.hpp"
#include "API/CWorldTimer.hpp"
#include "API/CNWSPlayer.hpp"
#include "API/Constants.hpp"
#include "API/Functions.hpp"
#include "API/Globals.hpp"

#include <pthread.h>

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstring>
#include <unordered_map>
#include <vector>

// Not in the API headers (SetTile and SetTileset aren't declared), so hooked by symbol like NWNXLib/POS.cpp does.
extern "C" void _ZN8CNWSAreaD1Ev(CNWSArea*);
extern "C" void _ZN8CNWSArea7SetTileEP8CNWSTileiii(CNWSArea*, CNWSTile*, int32_t, int32_t, int32_t);
extern "C" void _ZN8CNWSArea10SetTilesetERK7CResRef(CNWSArea*, const CResRef&);

namespace CreatureAI {

using namespace NWNXLib;
using namespace NWNXLib::API;
using namespace NWNXLib::API::Constants;

// The skip path mirrors CNWSCreature::SpawnInHeartbeatPerception (0x47a750 in 8193.37), which reads these by offset.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
static_assert(offsetof(CNWSObject, m_nAILevel) == 0x7c, "Check SpawnInHeartbeatPerception against the API");
static_assert(offsetof(CNWSCreature, m_bOnCreationScriptExecuted) == 0x540, "Check SpawnInHeartbeatPerception");
static_assert(offsetof(CNWSCreature, m_nLastHeartbeatScriptCalendarDay) == 0x544, "Check SpawnInHeartbeatPerception");
static_assert(offsetof(CNWSCreature, m_nLastHeartbeatScriptTimeOfDay) == 0x548, "Check SpawnInHeartbeatPerception");
static_assert(offsetof(CNWSCreature, m_nHeartbeatInterval) == 0x54c, "Check SpawnInHeartbeatPerception");
static_assert(offsetof(CNWSCreature, m_nLastVisibleCreaturesCheckCalendarDay) == 0x570, "Check SpawnInHeartbeatPerception");
static_assert(offsetof(CNWSCreature, m_nLastVisibleCreaturesCheckTimeOfDay) == 0x574, "Check SpawnInHeartbeatPerception");
static_assert(offsetof(CNWSCreature, m_nChecksVisibleList) == 0x578, "Check SpawnInHeartbeatPerception");
static_assert(offsetof(CNWSCreature, m_nChecksHeartbeat) == 0x57c, "Check SpawnInHeartbeatPerception");
static_assert(offsetof(CNWSCreature, m_nPersonalReputationHeartbeatCount) == 0x588, "Check SpawnInHeartbeatPerception");
static_assert(offsetof(CNWSCreature, m_bPlayerCharacter) == 0x894, "Check SpawnInHeartbeatPerception");
#pragma GCC diagnostic pop

static pthread_t s_mainThread;

static uint32_t FloatBits(float f)
{
    uint32_t u;
    std::memcpy(&u, &f, sizeof(u));
    return u;
}

static uint32_t NowMs()
{
    return (uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

// ---------------------------------------------------------------------------------------------------------------------
// Height cache
// ---------------------------------------------------------------------------------------------------------------------

struct HeightEntry
{
    CNWSArea* pArea;  // nullptr = empty
    CNWSTile* pTiles; // the area's tile array when cached: a replaced array misses
    ObjectID oidArea;
    uint32_t x, y;    // float bits
    float fHeight;
};

static constexpr uint32_t HEIGHT_BITS = 13; // 8192 entries, 256 KiB
static HeightEntry s_heights[1u << HEIGHT_BITS];
static bool s_heightConfigured, s_heightActive;
// Set when tiles change or an area goes away; the table is then cleared once, on the next lookup. SetTileJson and
// friends call SetTile once per tile, so clearing there would memset the whole table for every tile.
static bool s_heightStale;
static uint64_t s_heightHits, s_heightMisses, s_heightClears;

static Hooks::Hook s_ComputeHeightHook;
static Hooks::Hook s_AreaDtorHook;
static Hooks::Hook s_SetTileHook;
static Hooks::Hook s_SetTilesetHook;

static void ClearHeights()
{
    std::memset(s_heights, 0, sizeof(s_heights));
    s_heightStale = false;
    s_heightClears++;
}

static uint32_t HeightSlot(ObjectID oidArea, uint32_t x, uint32_t y)
{
    uint32_t h = x * 0x9E3779B1u;
    h ^= (y + 0x7F4A7C15u) * 0x85EBCA77u;
    h ^= oidArea * 0xC2B2AE3Du;
    h ^= h >> 15;
    return h & ((1u << HEIGHT_BITS) - 1);
}

static float ComputeHeightHook(CNWSArea* pThis, Vector v)
{
    if (!s_heightActive || !pThis || !pthread_equal(pthread_self(), s_mainThread))
        return s_ComputeHeightHook->CallOriginal<float>(pThis, v);
    if (s_heightStale)
        ClearHeights();

    uint32_t x = FloatBits(v.x), y = FloatBits(v.y);
    auto* e = &s_heights[HeightSlot(pThis->m_idSelf, x, y)];
    if (e->pArea == pThis && e->oidArea == pThis->m_idSelf)
    {
        if (e->pTiles == pThis->m_pTile)
        {
            if (e->x == x && e->y == y)
            {
                s_heightHits++;
                return e->fHeight;
            }
        }
        else
        {
            // This area's tile array was replaced (NWNX_Area_RotateArea). Drop everything, so no entry keyed on the
            // old array can match if the allocator later hands the same address to another new array.
            ClearHeights();
        }
    }
    float fHeight = s_ComputeHeightHook->CallOriginal<float>(pThis, v);
    *e = { pThis, pThis->m_pTile, pThis->m_idSelf, x, y, fHeight };
    s_heightMisses++;
    return fHeight;
}

// ---------------------------------------------------------------------------------------------------------------------
// VERY_LOW throttle
// ---------------------------------------------------------------------------------------------------------------------

struct ThrottleState
{
    uint32_t nLastRunMs;
    uint8_t nSkipped;
};

// The two containers are function-local statics: the plugin's constructor runs before this file's dynamic
// initialisers, so a namespace-scope container set up there would be constructed again afterwards.
static std::unordered_map<ObjectID, ThrottleState>& Throttled()
{
    static std::unordered_map<ObjectID, ThrottleState> s_map(4096);
    return s_map;
}
static uint32_t s_throttleTicks;
static uint32_t s_maxSkipMs;
static bool s_throttleConfigured, s_throttleActive;
static bool s_throttleWaiting;
// Internal action ids: CNWSVirtualMachineCommands::ExecuteCommandWait queues 0x1e (script ACTION_WAIT is 36), and
// CNWSObject::AddDoCommandAction (ActionDoCommand) queues 0x25.
static constexpr uint32_t ACTION_ID_WAIT = 0x1e;
static constexpr uint32_t ACTION_ID_DO_COMMAND = 0x25;
static uint64_t s_nSkipped, s_nRunThrottled, s_nRunOther;

static Hooks::Hook s_AIUpdateHook;

// Objects a connected player controls this tick (their character, or a creature they or a DM possess), sorted.
// Rebuilt once per tick before the AI walk; a possessed creature's area need not count a player, so check it directly.
static std::vector<ObjectID>& Controlled()
{
    static std::vector<ObjectID> s_vec = [] { std::vector<ObjectID> v; v.reserve(512); return v; }();
    return s_vec;
}
// Read once per tick, before the AI walk, so the per-creature path makes no clock or timer calls.
static uint32_t s_tickMs, s_tickWorldDay, s_tickWorldTime;

static void RefreshPerTick()
{
    s_tickMs = NowMs();
    Globals::AppManager()->m_pServerExoApp->GetWorldTimer()->GetWorldTime(&s_tickWorldDay, &s_tickWorldTime);
    auto& controlled = Controlled();
    controlled.clear();
    for (auto* pPlayer : Globals::AppManager()->m_pServerExoApp->GetPlayerList())
    {
        if (!pPlayer)
            continue;
        controlled.push_back(pPlayer->m_oidNWSObject);
        if (pPlayer->m_oidPCObject != pPlayer->m_oidNWSObject)
            controlled.push_back(pPlayer->m_oidPCObject);
    }
    std::sort(controlled.begin(), controlled.end());
}

static bool IsControlled(ObjectID oid)
{
    auto& controlled = Controlled();
    return !controlled.empty() && std::binary_search(controlled.begin(), controlled.end(), oid);
}

// THROTTLE_WAITING: the queue is an ActionWait with nothing behind it but ActionDoCommand nodes, which only run once
// the wait is over (Arelith's idle loop is Wait + DoCommand). AIActionWait compares world times, so a skipped visit
// only ends the wait up to THROTTLE_TICKS visits later. A Wait with any other action behind it is the engine pausing
// other work (AddPathfindingWaitActionToFront, random walk, follow, doors, counterspell, item cast): never throttled.
static bool IsOnlyWaiting(CExoLinkedListInternal* pList)
{
    auto* pNode = pList->pHead;
    if (!pNode || !pNode->pObject || static_cast<CNWSObjectActionNode*>(pNode->pObject)->m_nActionId != ACTION_ID_WAIT)
        return false;
    for (pNode = pNode->pNext; pNode; pNode = pNode->pNext)
    {
        if (!pNode->pObject || static_cast<CNWSObjectActionNode*>(pNode->pObject)->m_nActionId != ACTION_ID_DO_COMMAND)
            return false;
    }
    return true;
}

static bool CanThrottle(CNWSCreature* pCreature)
{
    if (pCreature->m_nAILevel != AIPriority::VeryLow)
        return false;
    // OnSpawn runs from SpawnInHeartbeatPerception on the creature's first AIUpdate: never delay it.
    if (!pCreature->m_bOnCreationScriptExecuted)
        return false;
    if (pCreature->m_bPlayerCharacter || !pCreature->m_pStats || pCreature->m_pStats->m_bIsPC)
        return false;
    auto* pList = pCreature->m_lQueuedActions.m_pcExoLinkedListInternal;
    if (pList && pList->m_nCount > 0 && (!s_throttleWaiting || !IsOnlyWaiting(pList)))
        return false;
    if (pCreature->m_bCombatState ||
        pCreature->m_oidAttackTarget != Constants::OBJECT_INVALID ||
        pCreature->m_oidAttemptedAttackTarget != Constants::OBJECT_INVALID)
        return false;
    if (pCreature->m_pcCombatRound && pCreature->m_pcCombatRound->m_bRoundStarted)
        return false;
    auto* pArea = pCreature->GetArea();
    if (!pArea || pArea->m_nPlayersInArea > 0)
        return false;
    if (pCreature->m_pStats->GetIsDM() || pCreature->GetDead() || IsControlled(pCreature->m_idSelf))
        return false;
    return true;
}

// Time-of-day part of (world time at the start of this tick - the given time), which is all the engine compares.
// The engine reads the object's active timer, which is the world timer unless the game is paused; the difference is
// a few ms at most.
static uint32_t TimeOfDayDiff(uint32_t nDay, uint32_t nTime)
{
    if (s_tickWorldDay == nDay && s_tickWorldTime >= nTime)
        return s_tickWorldTime - nTime;
    uint32_t nDiffDay = 0, nDiffMs = 0;
    Globals::AppManager()->m_pServerExoApp->GetWorldTimer()->SubtractWorldTimes(s_tickWorldDay, s_tickWorldTime,
        nDay, nTime, &nDiffDay, &nDiffMs);
    return nDiffMs;
}

// At VERY_LOW, SpawnInHeartbeatPerception(1) waits for further AIUpdate visits once an interval has passed, and a
// skipped visit would not count, stretching the interval. So do the engine's counting steps for a skipped visit; the
// event then fires on the first real run after the engine would have fired it, at most THROTTLE_TICKS - 1 visits late.
// Only reached for a non-PC VERY_LOW creature whose OnSpawn has run (CanThrottle).
static void CountSkippedChecks(CNWSCreature* pCreature)
{
    // Heartbeat (0x47a9a0): once m_nHeartbeatInterval ms have passed, count the visit; at VERY_LOW the heartbeat fires
    // when the count reaches interval / 64. A visit that counts without firing also steps the personal reputation
    // counter (0x47aa3b), which cleans the personal reputation list every 30 steps. On a visit where the engine
    // would fire, leave the reputation step to the real run that fires.
    if (TimeOfDayDiff(pCreature->m_nLastHeartbeatScriptCalendarDay, pCreature->m_nLastHeartbeatScriptTimeOfDay) >=
        pCreature->m_nHeartbeatInterval)
    {
        uint32_t nChecks = ++pCreature->m_nChecksHeartbeat;
        if (nChecks < (pCreature->m_nHeartbeatInterval >> 6))
        {
            if (pCreature->m_nPersonalReputationHeartbeatCount == 30)
            {
                pCreature->CleanOutPersonalReputationList();
                pCreature->m_nPersonalReputationHeartbeatCount = 1;
            }
            else
                pCreature->m_nPersonalReputationHeartbeatCount++;
        }
    }
    // Visible list (0x47ac78 -> 0x47ac92 -> 0x47a8e0): for a creature that isn't a PC or a possessed familiar, once
    // more than 3999 ms have passed since the last visible-list check, count the visit; at VERY_LOW UpdateVisibleList
    // runs when the count passes 149 (0x47a8f8), on the real run.
    if (!pCreature->m_bPlayerCharacter && !pCreature->GetIsPossessedFamiliar() &&
        TimeOfDayDiff(pCreature->m_nLastVisibleCreaturesCheckCalendarDay,
                      pCreature->m_nLastVisibleCreaturesCheckTimeOfDay) > 3999)
        pCreature->m_nChecksVisibleList++;
}

static void AIUpdateHook(CNWSCreature* pThis)
{
    if (s_throttleActive && pThis)
    {
        auto& throttled = Throttled();
        if (CanThrottle(pThis))
        {
            uint32_t nNow = s_tickMs;
            auto [it, bNew] = throttled.try_emplace(pThis->m_idSelf);
            auto& st = it->second;
            if (bNew)
            {
                // Spread creatures that start together over the cycle so they don't all run on the same tick.
                st.nSkipped = (uint8_t)(pThis->m_idSelf % s_throttleTicks);
                st.nLastRunMs = nNow;
            }
            if (st.nSkipped + 1u < s_throttleTicks && nNow - st.nLastRunMs < s_maxSkipMs)
            {
                st.nSkipped++;
                s_nSkipped++;
                CountSkippedChecks(pThis);
                return;
            }
            st.nSkipped = 0;
            st.nLastRunMs = nNow;
            s_nRunThrottled++;
        }
        else
        {
            // Out of the throttle: run on every visit from now on, and start a fresh cycle if it comes back.
            if (!throttled.empty())
                throttled.erase(pThis->m_idSelf);
            s_nRunOther++;
        }
    }
    s_AIUpdateHook->CallOriginal<void>(pThis);
}

// ---------------------------------------------------------------------------------------------------------------------
// Stats log, and forgetting destroyed creatures
// ---------------------------------------------------------------------------------------------------------------------

static Hooks::Hook s_UpdateStateHook;
static std::chrono::steady_clock::time_point s_nextLog, s_nextPurge;
static std::chrono::steady_clock::duration s_logPeriod;
static size_t s_nForgotten;

// Creatures destroyed while throttled leave an entry behind; drop them.
static void Purge()
{
    auto& throttled = Throttled();
    for (auto it = throttled.begin(); it != throttled.end();)
    {
        if (!Utils::AsNWSCreature(Utils::GetGameObject(it->first)))
        {
            it = throttled.erase(it);
            s_nForgotten++;
        }
        else
            ++it;
    }
}

static void LogStatsNow()
{
    if (s_throttleConfigured)
    {
        Purge();
        LOG_INFO("Throttle%s: %llu skipped, %llu run (throttled), %llu run (other); %zu tracked, %zu forgotten",
                 s_throttleActive ? "" : " (off)", (unsigned long long)s_nSkipped, (unsigned long long)s_nRunThrottled,
                 (unsigned long long)s_nRunOther, Throttled().size(), s_nForgotten);
        s_nSkipped = s_nRunThrottled = s_nRunOther = 0;
        s_nForgotten = 0;
    }
    if (s_heightConfigured)
    {
        LOG_INFO("Height cache%s: %llu hits, %llu misses, %llu clears", s_heightActive ? "" : " (off)",
                 (unsigned long long)s_heightHits, (unsigned long long)s_heightMisses,
                 (unsigned long long)s_heightClears);
        s_heightHits = s_heightMisses = s_heightClears = 0;
    }
}

static void InstallHousekeeping()
{
    auto nLogSeconds = Config::Get<int32_t>("LOG_SECONDS", 300);
    s_logPeriod = std::chrono::seconds(nLogSeconds > 0 ? nLogSeconds : 0);
    auto now = std::chrono::steady_clock::now();
    s_nextLog = nLogSeconds > 0 ? now + s_logPeriod : std::chrono::steady_clock::time_point::max();
    s_nextPurge = now + std::chrono::seconds(60);
    s_UpdateStateHook = Hooks::HookFunction(&CServerAIMaster::UpdateState,
        +[](CServerAIMaster* pThis) -> void
        {
            if (s_throttleActive)
                RefreshPerTick();
            s_UpdateStateHook->CallOriginal<void>(pThis);
            auto now = std::chrono::steady_clock::now();
            if (now >= s_nextLog)
            {
                LogStatsNow();
                s_nextLog = now + s_logPeriod;
            }
            if (now >= s_nextPurge)
            {
                if (s_throttleConfigured)
                    Purge();
                s_nextPurge = now + std::chrono::seconds(60);
            }
        }, Hooks::Order::Late);
}

// A hook that failed to install leaves its trampoline pointing at the original function.
template <typename F>
static bool Installed(const Hooks::Hook& hook, F original)
{
    return hook && hook->GetOriginal() && hook->GetOriginal() != (void*)original;
}

struct HookCheck { const char* name; bool ok; };

template <size_t N>
static bool AllInstalled(const char* part, const HookCheck (&hooks)[N])
{
    bool bAll = !Config::Get<bool>("DEBUG_SIMULATE_HOOK_FAILURE", false);
    for (auto& h : hooks)
    {
        if (!h.ok)
        {
            LOG_ERROR("%s: hook on %s did not install", part, h.name);
            bAll = false;
        }
    }
    if (!bAll)
        LOG_ERROR("%s not enabled: not every hook installed (or DEBUG_SIMULATE_HOOK_FAILURE is set)", part);
    return bAll;
}

static void InitHeightCache()
{
    s_ComputeHeightHook = Hooks::HookFunction(&CNWSArea::ComputeHeight, &ComputeHeightHook, Hooks::Order::Early);
    s_AreaDtorHook = Hooks::HookFunction(&_ZN8CNWSAreaD1Ev,
        +[](CNWSArea* pThis) -> void
        {
            if (s_heightConfigured)
                s_heightStale = true;
            s_AreaDtorHook->CallOriginal<void>(pThis);
        }, Hooks::Order::Early);
    s_SetTileHook = Hooks::HookFunction(&_ZN8CNWSArea7SetTileEP8CNWSTileiii,
        +[](CNWSArea* pThis, CNWSTile* pTile, int32_t a, int32_t b, int32_t c) -> void
        {
            s_SetTileHook->CallOriginal<void>(pThis, pTile, a, b, c);
            if (s_heightConfigured)
                s_heightStale = true;
        }, Hooks::Order::Early);
    s_SetTilesetHook = Hooks::HookFunction(&_ZN8CNWSArea10SetTilesetERK7CResRef,
        +[](CNWSArea* pThis, const CResRef& resRef) -> void
        {
            s_SetTilesetHook->CallOriginal<void>(pThis, resRef);
            if (s_heightConfigured)
                s_heightStale = true;
        }, Hooks::Order::Early);

    HookCheck hooks[] = {
        { "CNWSArea::ComputeHeight", Installed(s_ComputeHeightHook, &CNWSArea::ComputeHeight) },
        { "CNWSArea::~CNWSArea",     Installed(s_AreaDtorHook, &_ZN8CNWSAreaD1Ev) },
        { "CNWSArea::SetTile",       Installed(s_SetTileHook, &_ZN8CNWSArea7SetTileEP8CNWSTileiii) },
        { "CNWSArea::SetTileset",    Installed(s_SetTilesetHook, &_ZN8CNWSArea10SetTilesetERK7CResRef) },
    };
    if (!AllInstalled("Height cache", hooks))
    {
        // Take out the hooks that did install, so nothing of this part stays in the call path.
        s_ComputeHeightHook.reset(); s_AreaDtorHook.reset(); s_SetTileHook.reset(); s_SetTilesetHook.reset();
        return;
    }
    ClearHeights();
    s_heightClears = 0;
    s_heightConfigured = s_heightActive = true;
    LOG_INFO("Height cache enabled: %u entries", 1u << HEIGHT_BITS);
}

static void InitThrottle()
{
    auto nTicks = Config::Get<int32_t>("THROTTLE_TICKS", 4);
    if (nTicks < 2 || nTicks > 32)
    {
        LOG_WARNING("THROTTLE_TICKS must be 2-32, using 4");
        nTicks = 4;
    }
    s_throttleTicks = (uint32_t)nTicks;
    auto nMaxSkipMs = Config::Get<int32_t>("THROTTLE_MAX_SKIP_MS", 500);
    if (nMaxSkipMs < 50 || nMaxSkipMs > 6000)
    {
        LOG_WARNING("THROTTLE_MAX_SKIP_MS must be 50-6000, using 500");
        nMaxSkipMs = 500;
    }
    s_maxSkipMs = (uint32_t)nMaxSkipMs;
    s_throttleWaiting = Config::Get<bool>("THROTTLE_WAITING", false);

    s_AIUpdateHook = Hooks::HookFunction(Functions::_ZN12CNWSCreature8AIUpdateEv, &AIUpdateHook, Hooks::Order::Early);
    HookCheck hooks[] = {
        { "CNWSCreature::AIUpdate", Installed(s_AIUpdateHook, Functions::_ZN12CNWSCreature8AIUpdateEv) },
    };
    if (!AllInstalled("Throttle", hooks))
    {
        s_AIUpdateHook.reset();
        return;
    }
    s_throttleConfigured = s_throttleActive = true;
    LOG_INFO("Throttle enabled: VERY_LOW creatures in empty areas run 1 visit in %u (at least every %u ms); "
             "waiting creatures %s", s_throttleTicks, s_maxSkipMs, s_throttleWaiting ? "included" : "excluded");
}

void CreatureAI() __attribute__((constructor));
void CreatureAI()
{
    s_mainThread = pthread_self();
    bool bHeight = Config::Get<bool>("HEIGHT_CACHE", false);
    bool bThrottle = Config::Get<bool>("THROTTLE", false);
    if (!bHeight && !bThrottle)
        return;
    if (bHeight)
        InitHeightCache();
    if (bThrottle)
        InitThrottle();
    if (!s_heightConfigured && !s_throttleConfigured)
        return;

    InstallHousekeeping();
    // The throttle relies on this hook for the per-tick list of player-controlled objects.
    if (s_throttleConfigured && !Installed(s_UpdateStateHook, &CServerAIMaster::UpdateState))
    {
        LOG_ERROR("Throttle not enabled: hook on CServerAIMaster::UpdateState did not install");
        s_throttleConfigured = s_throttleActive = false;
        s_AIUpdateHook.reset();
    }
}

// Switches the height cache off (FALSE) or on (TRUE) at runtime. Returns TRUE if it is configured (enabled at
// startup with every hook installed), so the switch took effect.
NWNX_EXPORT ArgumentStack SetHeightCache(ArgumentStack&& args)
{
    const auto bOn = args.extract<int32_t>() != 0;
    if (!s_heightConfigured)
        return 0;
    if (bOn && !s_heightActive)
        ClearHeights();
    s_heightActive = bOn;
    LOG_DEBUG("Height cache %s at runtime", bOn ? "on" : "off");
    return 1;
}

// Switches the throttle off (FALSE) or on (TRUE) at runtime. Returns TRUE if it is configured, so the switch took
// effect. Switching off forgets every throttled creature, so each one runs on its next visit.
NWNX_EXPORT ArgumentStack SetThrottle(ArgumentStack&& args)
{
    const auto bOn = args.extract<int32_t>() != 0;
    if (!s_throttleConfigured)
        return 0;
    if (!bOn)
        Throttled().clear();
    else if (!s_throttleActive)
        RefreshPerTick(); // the per-tick state isn't kept up while off; the rest of this tick needs it current
    s_throttleActive = bOn;
    LOG_DEBUG("Throttle %s at runtime", bOn ? "on" : "off");
    return 1;
}

// Sets whether a creature that is only waiting (ActionWait, followed by nothing but ActionDoCommand) counts as idle
// (THROTTLE_WAITING). Returns TRUE if the throttle is configured, so the switch took effect.
NWNX_EXPORT ArgumentStack SetThrottleWaiting(ArgumentStack&& args)
{
    const auto bOn = args.extract<int32_t>() != 0;
    if (!s_throttleConfigured)
        return 0;
    s_throttleWaiting = bOn;
    LOG_DEBUG("Throttle waiting creatures %s at runtime", bOn ? "included" : "excluded");
    return 1;
}

// Returns TRUE if oCreature is currently being throttled (it was skipped on its last visit, or will be on its next).
NWNX_EXPORT ArgumentStack GetIsThrottled(ArgumentStack&& args)
{
    const auto oid = args.extract<ObjectID>();
    return (int32_t)(s_throttleActive && Throttled().find(oid) != Throttled().end());
}

// Logs the counters now (and resets them), as the periodic log line does.
NWNX_EXPORT ArgumentStack LogStats(ArgumentStack&&)
{
    LogStatsNow();
    return 0;
}

}
