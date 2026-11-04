// EXPERIMENTAL. Takes objects that cannot change on their own out of the engine's per-tick AI list walk.
//
// CServerAIMaster::UpdateState walks five AI lists every tick under a 10 ms budget, calling AIUpdate on every
// object. On a big module most of those objects are placeables and items with nothing to do: no heartbeat, no
// queued actions and only permanent effects (an item carries one permanent effect per item property). Their
// AIUpdate reads the clock, walks a list of effects that can never expire, and returns.
//
// Every SWEEP_SECONDS this plugin compacts the AI lists, dropping such "dormant" objects and remembering which list
// each came from. A dormant object goes back into its list as soon as something can give it work:
//   - an effect is applied to it (CServerAIMaster::OnEffectApplied),
//   - an action is queued on it (CNWSObject::AddAction, AddActionToFront, AddActionAfterFront; PlayAnimation and
//     ActionDoDoorAction use the latter two),
//   - a script changes one of its event scripts (SetEventScript, CNWSVirtualMachineCommands::
//     ExecuteCommandEventScriptManagement), so a heartbeat set at runtime starts at once,
//   - it joins a conversation (CNWSObject::SetDialogOwner): UpdateState calls UpdateDialog on every object it walks,
//     and that is what plays a conversation's delayed entries,
//   - NWNX_Effect edits one of its applied effects in place (the NWNX_EFFECT_APPLIED_EFFECT_EDITED message),
//   - and, re-checked a slice per tick, if it gained a heartbeat script, actions, a non-permanent effect or a
//     conversation some other way.
// If the engine itself adds or removes a dormant object (CServerAIList::AddObject, CServerAIMaster::RemoveObject),
// the plugin forgets it.
//
// Off unless NWNX_DORMANTOBJECTS_ENABLE=true. If a hook the AI lists depend on fails to install the plugin stays
// inactive (fail closed). The SetEventScript and SetDialogOwner hooks only wake objects sooner, so if one of those
// fails the plugin logs a warning and the re-check does their job.
// NWNX_DormantObjects_Disable() wakes every dormant object and stops; NWNX_DormantObjects_Enable() starts it again.

#include "nwnx.hpp"

#include "API/CAppManager.hpp"
#include "API/CExoLinkedListInternal.hpp"
#include "API/CGameEffect.hpp"
#include "API/CNWSDoor.hpp"
#include "API/CNWSItem.hpp"
#include "API/CNWSTrigger.hpp"
#include "API/CNWSObject.hpp"
#include "API/CNWSPlaceable.hpp"
#include "API/CNWSVirtualMachineCommands.hpp"
#include "API/CServerAIList.hpp"
#include "API/CServerAIMaster.hpp"
#include "API/CServerExoApp.hpp"
#include "API/Constants.hpp"
#include "API/Globals.hpp"
#include "API/CVirtualMachine.hpp"
#include "API/StackElement.hpp"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace DormantObjects {

using namespace NWNXLib;
using namespace NWNXLib::API;
using namespace NWNXLib::API::Constants;

// Script slots are EVENT_SCRIPT_<TYPE>_ON_* minus the type's base: placeable heartbeat 9004, door heartbeat 10005,
// trigger heartbeat 7000.
static constexpr int PLACEABLE_SCRIPT_HEARTBEAT = 4;
static constexpr int DOOR_SCRIPT_HEARTBEAT = 5;
static constexpr int TRIGGER_SCRIPT_HEARTBEAT = 0;

// The heartbeat strings CNWSPlaceable::AIUpdate and CNWSDoor::AIUpdate test (8193.37-17: +0x418 and +0x350).
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
static_assert(offsetof(CNWSPlaceable, m_sScripts) + PLACEABLE_SCRIPT_HEARTBEAT * sizeof(CExoString) == 0x418);
static_assert(offsetof(CNWSDoor, m_sScripts) + DOOR_SCRIPT_HEARTBEAT * sizeof(CExoString) == 0x350);
// CNWSPlaceable::AIUpdate never takes its early return for a placeable with m_bDieWhenEmpty (+0x4fc; body bags):
// the full update destroys it if it has an inventory (m_bHasInventory, +0x4f0), the inventory is empty and nobody
// has it open. Without an inventory the full update has nothing more to do than the early return.
static_assert(offsetof(CNWSPlaceable, m_bDieWhenEmpty) == 0x4fc);
static_assert(offsetof(CNWSPlaceable, m_bHasInventory) == 0x4f0);
// CNWSObject::UpdateDialog returns at once when m_oidDialogOwner (+0x74) is OBJECT_INVALID. Otherwise the object is
// in a conversation (as its owner or a partner) and UpdateState's call to UpdateDialog plays delayed entries.
static_assert(offsetof(CNWSObject, m_oidDialogOwner) == 0x74);
#pragma GCC diagnostic pop

static std::unordered_map<ObjectID, int32_t> s_dormant; // object -> AI list it was taken from
// IDs to re-check, a bounded slice per tick. s_inRecheck holds the same IDs so none is queued twice; entries no
// longer in s_dormant (woken, or the engine took the object back) are dropped when the cursor reaches them.
static std::vector<ObjectID> s_recheck;
static std::unordered_set<ObjectID> s_inRecheck;
static size_t s_recheckPos;
static int32_t s_recheckPerTick;   // upper bound per tick
static double s_recheckSeconds;    // aim to re-check every dormant object once in this many seconds (0: always the bound)
static double s_recheckCredit;     // fractional checks carried between ticks
static std::chrono::steady_clock::time_point s_lastRecheck;
static bool s_doors, s_triggers, s_containers;
static std::chrono::steady_clock::duration s_period;
static std::chrono::steady_clock::time_point s_nextSweep;
static bool s_placeables, s_items;
static bool s_configured; // ENABLE was set and every hook installed
static bool s_active;     // sweeping now (NWNX_DormantObjects_Disable/Enable toggle this)
static int32_t s_nWoken, s_nHookWoken, s_nGone; // counted since the last sweep log line

static Hooks::Hook s_UpdateStateHook;
static Hooks::Hook s_OnEffectAppliedHook;
static Hooks::Hook s_AddActionHook;
static Hooks::Hook s_AddActionToFrontHook;
static Hooks::Hook s_AddActionAfterFrontHook;
static Hooks::Hook s_AddObjectHook;
static Hooks::Hook s_RemoveObjectHook;
static Hooks::Hook s_EventScriptHook;
static Hooks::Hook s_SetDialogOwnerHook;

static CServerAIMaster* AIMaster()
{
    return Globals::AppManager()->m_pServerExoApp->GetServerAIMaster();
}

static bool EffectsNeverExpire(CNWSObject* pObject)
{
    for (int32_t i = 0; i < pObject->m_appliedEffects.num; i++)
    {
        auto* pEffect = pObject->m_appliedEffects.element[i];
        if (!pEffect)
            continue;
        // UpdateEffectList does periodic work for these whatever their duration: a RunScript effect's interval
        // script runs for any object type, and Regenerate heals non-creatures too.
        if (pEffect->m_nType == EffectTrueType::RunScript || pEffect->m_nType == EffectTrueType::Regenerate)
            return false;
        auto nDuration = pEffect->GetDurationType();
        if (nDuration != EffectDurationType::Permanent &&
            nDuration != EffectDurationType::Equipped &&
            nDuration != EffectDurationType::Innate)
            return false;
    }
    return true;
}

static bool HasQueuedActions(CNWSObject* pObject)
{
    auto* pList = pObject->m_lQueuedActions.m_pcExoLinkedListInternal;
    return pList && pList->m_nCount > 0;
}

static bool InDialog(CNWSObject* pObject)
{
    return pObject->m_oidDialogOwner != Constants::OBJECT_INVALID;
}

static bool IsDormant(CGameObject* pGameObject)
{
    if (!pGameObject)
        return false;

    // CNWSPlaceable::AIUpdate and CNWSDoor::AIUpdate (8193.37-17) return at once when the heartbeat script, the
    // action queue and the effect list are all empty; otherwise they run the heartbeat timer, UpdateEffectList and
    // RunActions. Placeables have one more step: one with m_bDieWhenEmpty (a body bag) is destroyed by its own
    // AIUpdate once its inventory is empty and it isn't open, so those are never dormant. Inventory and trap flags
    // play no other part, so containers and trapped objects are as idle as any other. CNWSTrigger::AIUpdate only
    // runs the heartbeat timer and RunActions. UpdateState also calls UpdateDialog on every object it walks, which
    // plays the delayed entries of a conversation the object is in, so an object in a conversation is never dormant.
    switch (pGameObject->m_nObjectType)
    {
        case ObjectType::Placeable:
        {
            if (!s_placeables)
                return false;
            auto* pPlaceable = Utils::AsNWSPlaceable(pGameObject);
            return pPlaceable &&
                   (s_containers || (!pPlaceable->m_bHasInventory && !pPlaceable->m_bTrapFlag)) &&
                   !(pPlaceable->m_bHasInventory && pPlaceable->m_bDieWhenEmpty) &&
                   pPlaceable->m_sScripts[PLACEABLE_SCRIPT_HEARTBEAT].IsEmpty() &&
                   !HasQueuedActions(pPlaceable) &&
                   !InDialog(pPlaceable) &&
                   !pPlaceable->GetDead() &&
                   EffectsNeverExpire(pPlaceable);
        }
        case ObjectType::Item:
        {
            if (!s_items)
                return false;
            auto* pItem = Utils::AsNWSItem(pGameObject);
            return pItem && !HasQueuedActions(pItem) && !InDialog(pItem) && EffectsNeverExpire(pItem);
        }
        case ObjectType::Door:
        {
            if (!s_doors)
                return false;
            auto* pDoor = Utils::AsNWSDoor(pGameObject);
            return pDoor &&
                   pDoor->m_sScripts[DOOR_SCRIPT_HEARTBEAT].IsEmpty() &&
                   !HasQueuedActions(pDoor) &&
                   !InDialog(pDoor) &&
                   !pDoor->GetDead() &&
                   EffectsNeverExpire(pDoor);
        }
        case ObjectType::Trigger:
        {
            if (!s_triggers)
                return false;
            auto* pTrigger = Utils::AsNWSTrigger(pGameObject);
            return pTrigger &&
                   pTrigger->m_sScripts[TRIGGER_SCRIPT_HEARTBEAT].IsEmpty() &&
                   !HasQueuedActions(pTrigger) &&
                   !InDialog(pTrigger);
        }
        default:
            return false;
    }
}

// Puts oid back in its AI list if it's dormant. Returns true if it was.
static bool Wake(CServerAIMaster* pMaster, ObjectID oid)
{
    auto it = s_dormant.find(oid);
    if (it == s_dormant.end())
        return false;
    int32_t nList = it->second;
    s_dormant.erase(it);
    pMaster->m_apGameAIList[nList].AddObject(oid);
    return true;
}

// Puts every dormant object back in its AI list at once. CServerAIList::AddObject scans the whole list for a
// duplicate before appending, so calling it per object is quadratic: seconds of main thread with 100k+ dormant.
// This does the same duplicate check with a hash set and appends directly, which is all AddObject does.
static void WakeAll(CServerAIMaster* pMaster)
{
    std::vector<ObjectID> toWake[AIPriority::MAX + 1];
    auto take = [&](ObjectID oid, int32_t nList)
    {
        if (Utils::GetGameObject(oid))
            toWake[nList].push_back(oid);
    };
    // s_recheck holds every dormant object in roughly the order sweeps took them out; walk it first to keep that
    // order, then pick up anything not in it (there shouldn't be any).
    for (ObjectID oid : s_recheck)
    {
        auto it = s_dormant.find(oid);
        if (it == s_dormant.end())
            continue;
        take(oid, it->second);
        s_dormant.erase(it);
    }
    for (auto& [oid, nList] : s_dormant)
        take(oid, nList);
    s_dormant.clear();
    s_recheck.clear();
    s_inRecheck.clear();
    s_recheckPos = 0;

    for (int32_t nList = AIPriority::MIN; nList <= AIPriority::MAX; nList++)
    {
        if (toWake[nList].empty())
            continue;
        auto& objects = pMaster->m_apGameAIList[nList].m_aoGameObjects;
        std::unordered_set<ObjectID> present;
        present.reserve(objects.num + toWake[nList].size());
        for (int32_t i = 0; i < objects.num; i++)
            present.insert(objects.element[i]);
        // The engine grows this array with realloc, so do the same rather than CExoArrayList::Allocate (new[]).
        int32_t nNeeded = objects.num + (int32_t)toWake[nList].size();
        if (nNeeded > objects.array_size)
        {
            auto* pGrown = (ObjectID*)std::realloc(objects.element, nNeeded * sizeof(ObjectID));
            if (!pGrown)
            {
                LOG_ERROR("Out of memory growing AI list %d, waking one by one", nList);
                for (ObjectID oid : toWake[nList])
                    pMaster->m_apGameAIList[nList].AddObject(oid);
                continue;
            }
            objects.element = pGrown;
            objects.array_size = nNeeded;
        }
        for (ObjectID oid : toWake[nList])
        {
            if (present.insert(oid).second)
                objects.element[objects.num++] = oid;
        }
    }
}

// Re-check up to s_recheckPerTick dormant objects: forget destroyed ones, wake any that gained work in a way the
// hooks don't see (a heartbeat script set at runtime, a non-permanent effect, queued actions).
static void Recheck(CServerAIMaster* pMaster)
{
    // With RECHECK_SECONDS set, spread one pass over all dormant objects across that many seconds, whatever the tick
    // rate; RECHECK_PER_TICK stays the upper bound. Never more than once per object per tick.
    size_t nChecks = (size_t)s_recheckPerTick;
    if (s_recheckSeconds > 0)
    {
        auto now = std::chrono::steady_clock::now();
        double dt = std::chrono::duration<double>(now - s_lastRecheck).count();
        s_lastRecheck = now;
        s_recheckCredit += (double)s_recheck.size() * std::min(dt, 1.0) / s_recheckSeconds;
        nChecks = std::min((size_t)s_recheckCredit, nChecks);
        // Don't bank more than one tick's worth when the bound or an empty queue held it back.
        s_recheckCredit = std::min(s_recheckCredit - (double)nChecks, (double)s_recheckPerTick);
    }
    nChecks = std::min(s_recheck.size(), nChecks);
    for (size_t n = 0; n < nChecks && !s_recheck.empty(); n++)
    {
        if (s_recheckPos >= s_recheck.size())
            s_recheckPos = 0;
        ObjectID oid = s_recheck[s_recheckPos];
        bool bDrop = false;
        auto it = s_dormant.find(oid);
        if (it == s_dormant.end())
            bDrop = true; // woken or reclaimed by the engine already
        else if (auto* pGameObject = Utils::GetGameObject(oid); !pGameObject)
        {
            s_dormant.erase(it);
            s_nGone++;
            bDrop = true;
        }
        else if (!IsDormant(pGameObject))
        {
            int32_t nList = it->second;
            s_dormant.erase(it);
            pMaster->m_apGameAIList[nList].AddObject(oid);
            s_nWoken++;
            bDrop = true;
        }
        if (bDrop)
        {
            s_inRecheck.erase(oid);
            s_recheck[s_recheckPos] = s_recheck.back(); // swap-remove; re-examine this slot next
            s_recheck.pop_back();
        }
        else
            s_recheckPos++;
    }
}

static void Sweep(CServerAIMaster* pMaster)
{
    auto start = std::chrono::steady_clock::now();
    // One pass per list: keep order, drop dormant objects, keep the walk's resume position.
    // CServerAIList::GetNextObject increments m_nPosition before reading, so m_nPosition is the object processed
    // last; the next walk continues after it.
    int32_t nDropped = 0;
    int32_t nRemaining = 0;
    for (int32_t nList = AIPriority::MIN; nList <= AIPriority::MAX; nList++)
    {
        auto& list = pMaster->m_apGameAIList[nList];
        auto& objects = list.m_aoGameObjects;
        int32_t nKeep = 0, nNewPosition = 0;
        bool bPositionDropped = false;
        for (int32_t i = 0; i < objects.num; i++)
        {
            ObjectID oid = objects.element[i];
            if (IsDormant(Utils::GetGameObject(oid)))
            {
                s_dormant.emplace(oid, nList);
                if (s_inRecheck.insert(oid).second)
                    s_recheck.push_back(oid);
                if (i == list.m_nPosition)
                    bPositionDropped = true;
                nDropped++;
                continue;
            }
            if (i < list.m_nPosition)
                nNewPosition++;
            objects.element[nKeep++] = oid;
        }
        objects.num = nKeep;
        // If the last processed object was dropped, resume after the kept object before it.
        if (bPositionDropped)
            nNewPosition--;
        if (nNewPosition < 0)
            nNewPosition = 0;
        list.m_nPosition = nNewPosition < nKeep ? nNewPosition : 0;
        nRemaining += nKeep;
    }

    auto us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
    LOG_INFO("Sweep: %d made dormant; since last: %d woken by hooks, %d by re-check, %d destroyed; %zu dormant, "
             "%d left in the AI lists; took %lld us",
             nDropped, s_nHookWoken, s_nWoken, s_nGone, s_dormant.size(), nRemaining, (long long)us);
    s_nWoken = s_nHookWoken = s_nGone = 0;
}

static void WakeFromHook(CNWSObject* pObject)
{
    if (pObject && !s_dormant.empty() && Wake(AIMaster(), pObject->m_idSelf))
        s_nHookWoken++;
}

// Wakes oid if it's dormant and no longer qualifies (it gained work). A destroyed object isn't dormant either; Wake
// only re-adds what GetGameObject still finds, and the re-check forgets the rest.
static void WakeIfBusy(ObjectID oid)
{
    if (oid == Constants::OBJECT_INVALID || !s_dormant.count(oid))
        return;
    auto* pGameObject = Utils::GetGameObject(oid);
    if (pGameObject && !IsDormant(pGameObject) && Wake(AIMaster(), oid))
        s_nHookWoken++;
}

// A hook that failed to install leaves its trampoline pointing at the original function.
template <typename F>
static bool Installed(const Hooks::Hook& hook, F original)
{
    return hook && hook->GetOriginal() && hook->GetOriginal() != (void*)original;
}

void DormantObjects() __attribute__((constructor));
void DormantObjects()
{
    if (!Config::Get<bool>("ENABLE", false))
        return;

    auto nSweepSeconds = Config::Get<int32_t>("SWEEP_SECONDS", 10);
    if (nSweepSeconds <= 0)
    {
        LOG_WARNING("SWEEP_SECONDS must be above 0, using 10");
        nSweepSeconds = 10;
    }
    s_period = std::chrono::seconds(nSweepSeconds);
    s_placeables = Config::Get<bool>("PLACEABLES", true);
    s_items = Config::Get<bool>("ITEMS", true);
    // Off by default until each has been piloted on live.
    s_doors = Config::Get<bool>("DOORS", false);
    s_triggers = Config::Get<bool>("TRIGGERS", false);
    s_containers = Config::Get<bool>("CONTAINERS", false); // placeables with an inventory or a trap
    s_recheckSeconds = Config::Get<int32_t>("RECHECK_SECONDS", 0);
    if (s_recheckSeconds < 0)
    {
        LOG_WARNING("RECHECK_SECONDS must be 0 or above, using 0");
        s_recheckSeconds = 0;
    }
    s_lastRecheck = std::chrono::steady_clock::now();
    s_recheckPerTick = Config::Get<int32_t>("RECHECK_PER_TICK", 1000);
    if (s_recheckPerTick <= 0)
    {
        LOG_WARNING("RECHECK_PER_TICK must be above 0, using 1000");
        s_recheckPerTick = 1000;
    }
    else if (s_recheckPerTick > 100000)
    {
        LOG_WARNING("RECHECK_PER_TICK must be at most 100000, using 100000");
        s_recheckPerTick = 100000;
    }
    s_dormant.reserve(262144);
    s_inRecheck.reserve(262144);
    s_recheck.reserve(262144);
    s_nextSweep = std::chrono::steady_clock::now() + s_period;

    s_UpdateStateHook = Hooks::HookFunction(&CServerAIMaster::UpdateState,
        +[](CServerAIMaster* pThis) -> void
        {
            if (s_active)
            {
                Recheck(pThis);
                auto now = std::chrono::steady_clock::now();
                if (now >= s_nextSweep)
                {
                    Sweep(pThis);
                    s_nextSweep = now + s_period;
                }
            }
            s_UpdateStateHook->CallOriginal<void>(pThis);
        }, Hooks::Order::Late);

    s_OnEffectAppliedHook = Hooks::HookFunction(&CServerAIMaster::OnEffectApplied,
        +[](CServerAIMaster* pThis, CNWSObject* pObject, CGameEffect* pEffect, BOOL bLoadingGame) -> int32_t
        {
            if (pObject && !s_dormant.empty() && Wake(pThis, pObject->m_idSelf))
                s_nHookWoken++;
            return s_OnEffectAppliedHook->CallOriginal<int32_t>(pThis, pObject, pEffect, bLoadingGame);
        }, Hooks::Order::Early);

#define DORMANT_ACTION_HOOK(hook, function)                                                                             \
    hook = Hooks::HookFunction(&CNWSObject::function,                                                                  \
        +[](CNWSObject* pThis, uint32_t nActionId, uint16_t nGroupId,                                                  \
            uint32_t t1, void* p1, uint32_t t2, void* p2, uint32_t t3, void* p3, uint32_t t4, void* p4,                \
            uint32_t t5, void* p5, uint32_t t6, void* p6, uint32_t t7, void* p7, uint32_t t8, void* p8,                \
            uint32_t t9, void* p9, uint32_t t10, void* p10, uint32_t t11, void* p11, uint32_t t12, void* p12) -> void \
        {                                                                                                              \
            WakeFromHook(pThis);                                                                                       \
            hook->CallOriginal<void>(pThis, nActionId, nGroupId, t1, p1, t2, p2, t3, p3, t4, p4,                       \
                t5, p5, t6, p6, t7, p7, t8, p8, t9, p9, t10, p10, t11, p11, t12, p12);                                 \
        }, Hooks::Order::Early)

    DORMANT_ACTION_HOOK(s_AddActionHook, AddAction);
    // PlayAnimation clears the queue and uses AddActionToFront; ActionDoDoorAction and others use these two too.
    DORMANT_ACTION_HOOK(s_AddActionToFrontHook, AddActionToFront);
    DORMANT_ACTION_HOOK(s_AddActionAfterFrontHook, AddActionAfterFront);
#undef DORMANT_ACTION_HOOK

    // The engine adding or removing an object we hold as dormant takes precedence: forget it.
    // Adds are hooked on the list, not on CServerAIMaster::AddObject: that one starts with a short conditional jump
    // that funchook can't relocate, so hooking it fails. Every add goes through CServerAIList::AddObject anyway.
    s_AddObjectHook = Hooks::HookFunction(&CServerAIList::AddObject,
        +[](CServerAIList* pThis, ObjectID oid) -> BOOL
        {
            if (!s_dormant.empty())
                s_dormant.erase(oid);
            return s_AddObjectHook->CallOriginal<BOOL>(pThis, oid);
        }, Hooks::Order::Early);

    s_RemoveObjectHook = Hooks::HookFunction(&CServerAIMaster::RemoveObject,
        +[](CServerAIMaster* pThis, CNWSObject* pObject) -> BOOL
        {
            if (pObject && !s_dormant.empty())
                s_dormant.erase(pObject->m_idSelf);
            return s_RemoveObjectHook->CallOriginal<BOOL>(pThis, pObject);
        }, Hooks::Order::Early);

    // SetEventScript/GetEventScript. The object is the first argument, so on top of the VM stack before the call. A
    // dormant object whose new scripts give it work (a heartbeat) wakes now rather than at its next re-check.
    s_EventScriptHook = Hooks::HookFunction(&CNWSVirtualMachineCommands::ExecuteCommandEventScriptManagement,
        +[](CNWSVirtualMachineCommands* pThis, int32_t nCommandId, int32_t nParameters) -> int32_t
        {
            ObjectID oid = Constants::OBJECT_INVALID;
            if (!s_dormant.empty())
            {
                auto& stack = Globals::VirtualMachine()->m_cRunTimeStack;
                if (stack.m_nStackPointer > 0 && stack.m_pStackNodes)
                {
                    auto& top = stack.m_pStackNodes[stack.m_nStackPointer - 1];
                    if (top.m_nType == StackElement::OBJECT)
                        oid = top.m_nStackObjectID;
                }
            }
            auto nRet = s_EventScriptHook->CallOriginal<int32_t>(pThis, nCommandId, nParameters);
            WakeIfBusy(oid);
            return nRet;
        }, Hooks::Order::Early);

    // Every write of a valid ID to m_oidDialogOwner goes through SetDialogOwner (8193.37-17: StartDialog and the
    // CNWSDialog reply handling); StopDialog and the constructor only clear it. Wake the object as it joins.
    s_SetDialogOwnerHook = Hooks::HookFunction(&CNWSObject::SetDialogOwner,
        +[](CNWSObject* pThis, ObjectID oidDialogOwner) -> void
        {
            s_SetDialogOwnerHook->CallOriginal<void>(pThis, oidDialogOwner);
            if (oidDialogOwner != Constants::OBJECT_INVALID)
                WakeFromHook(pThis);
        }, Hooks::Order::Early);

    // NWNX_Effect_ReplaceEffect and ReplaceEffectByIndex edit applied effects in place (a permanent effect can become
    // temporary) and broadcast this. If the Effect plugin doesn't send it, nothing does, so subscribing is harmless.
    MessageBus::Subscribe("NWNX_EFFECT_APPLIED_EFFECT_EDITED",
        [](const std::vector<std::string>& message)
        {
            if (!message.empty() && !s_dormant.empty())
                WakeIfBusy((ObjectID)std::strtoul(message[0].c_str(), nullptr, 10));
        });

    // These two only wake objects sooner than the re-check would: warn and carry on without them.
    struct { const char* name; bool ok; } optionalHooks[] = {
        { "CNWSVirtualMachineCommands::ExecuteCommandEventScriptManagement",
          Installed(s_EventScriptHook, &CNWSVirtualMachineCommands::ExecuteCommandEventScriptManagement) },
        { "CNWSObject::SetDialogOwner",       Installed(s_SetDialogOwnerHook, &CNWSObject::SetDialogOwner) },
    };
    for (auto& h : optionalHooks)
    {
        if (!h.ok)
            LOG_WARNING("Hook on %s did not install; the objects it would wake wait for the re-check", h.name);
    }

    // NWNX's hook ASSERTs only log, so check every other hook ourselves and stay off if one is missing: a missing
    // wake or forget hook would leave objects frozen or in two lists.
    struct { const char* name; bool ok; } hooks[] = {
        { "CServerAIMaster::UpdateState",     Installed(s_UpdateStateHook, &CServerAIMaster::UpdateState) },
        { "CServerAIMaster::OnEffectApplied", Installed(s_OnEffectAppliedHook, &CServerAIMaster::OnEffectApplied) },
        { "CNWSObject::AddAction",            Installed(s_AddActionHook, &CNWSObject::AddAction) },
        { "CNWSObject::AddActionToFront",     Installed(s_AddActionToFrontHook, &CNWSObject::AddActionToFront) },
        { "CNWSObject::AddActionAfterFront",  Installed(s_AddActionAfterFrontHook, &CNWSObject::AddActionAfterFront) },
        { "CServerAIList::AddObject",         Installed(s_AddObjectHook, &CServerAIList::AddObject) },
        { "CServerAIMaster::RemoveObject",    Installed(s_RemoveObjectHook, &CServerAIMaster::RemoveObject) },
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
    LOG_INFO("Enabled: sweep every %d s, re-check every %.0f s (0: as fast as the bound) up to %d per tick, "
             "placeables %d (containers and traps %d), items %d, doors %d, triggers %d",
             nSweepSeconds, s_recheckSeconds, s_recheckPerTick, s_placeables, s_containers, s_items, s_doors, s_triggers);
}

// Wakes every dormant object and stops sweeping. Returns how many were woken.
NWNX_EXPORT ArgumentStack Disable(ArgumentStack&&)
{
    int32_t nWoken = (int32_t)s_dormant.size();
    s_active = false;
    auto start = std::chrono::steady_clock::now();
    if (s_configured)
        WakeAll(AIMaster());
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
    LOG_INFO("Disabled at runtime: %d dormant objects woken; took %lld us", nWoken, (long long)us);
    return nWoken;
}

// Starts sweeping again after Disable. Returns TRUE if the plugin is configured and now active.
NWNX_EXPORT ArgumentStack Enable(ArgumentStack&&)
{
    if (!s_configured)
        return 0;
    if (!s_active)
    {
        s_active = true;
        s_nextSweep = std::chrono::steady_clock::now();
        LOG_INFO("Enabled at runtime");
    }
    return 1;
}

// Returns the number of objects currently dormant, or -1 if the plugin isn't active.
NWNX_EXPORT ArgumentStack GetDormantCount(ArgumentStack&&)
{
    return s_active ? (int32_t)s_dormant.size() : -1;
}

// Logs what the AI lists still hold: per list, objects by type, and for placeables, doors, triggers and items the
// reasons they aren't dormant; then dormant objects by type, and creature effect counts. Read-only; one pass over
// the lists, so a few ms on a big module. Returns the number of objects left in the lists.
NWNX_EXPORT ArgumentStack LogCensus(ArgumentStack&&)
{
    auto* pMaster = AIMaster();
    int32_t nTotal = 0;
    int32_t nCreatures = 0, nCreatureFx = 0, nCreatureFxInert = 0, nCreaturesAllInert = 0;
    std::unordered_map<std::string, int32_t> reasons;
    for (int32_t nList = AIPriority::MIN; nList <= AIPriority::MAX; nList++)
    {
        auto& objects = pMaster->m_apGameAIList[nList].m_aoGameObjects;
        int32_t byType[ObjectType::MAX + 1] = {};
        for (int32_t i = 0; i < objects.num; i++)
        {
            auto* pGameObject = Utils::GetGameObject(objects.element[i]);
            if (!pGameObject)
                continue;
            uint8_t nType = pGameObject->m_nObjectType;
            if (nType <= ObjectType::MAX)
                byType[nType]++;
            auto* pObject = Utils::AsNWSObject(pGameObject);
            if (!pObject)
                continue;
            std::string why;
            auto fx = [&]() { if (!EffectsNeverExpire(pObject)) why += "fx-expiring "; else if (pObject->m_appliedEffects.num) why += "(fx-permanent) "; };
            if (nType == ObjectType::Placeable)
            {
                auto* p = Utils::AsNWSPlaceable(pGameObject);
                if (!p->m_sScripts[PLACEABLE_SCRIPT_HEARTBEAT].IsEmpty()) why += "hb ";
                if (p->m_bHasInventory) why += "inventory ";
                if (p->m_bTrapFlag) why += "trap ";
                if (p->m_bDieWhenEmpty) why += p->m_bHasInventory ? "die-when-empty " : "(die-when-empty, no inventory) ";
                if (HasQueuedActions(p)) why += "actions ";
                if (InDialog(p)) why += "dialog ";
                if (p->GetDead()) why += "dead ";
                fx();
            }
            else if (nType == ObjectType::Door)
            {
                auto* p = Utils::AsNWSDoor(pGameObject);
                if (!p->m_sScripts[DOOR_SCRIPT_HEARTBEAT].IsEmpty()) why += "hb ";
                if (HasQueuedActions(p)) why += "actions ";
                if (InDialog(p)) why += "dialog ";
                if (p->GetDead()) why += "dead ";
                fx();
            }
            else if (nType == ObjectType::Trigger)
            {
                auto* p = Utils::AsNWSTrigger(pGameObject);
                if (!p->m_sScripts[TRIGGER_SCRIPT_HEARTBEAT].IsEmpty()) why += "hb ";
                if (HasQueuedActions(p)) why += "actions ";
                if (InDialog(p)) why += "dialog ";
            }
            else if (nType == ObjectType::Item)
            {
                if (HasQueuedActions(pObject)) why += "actions ";
                if (InDialog(pObject)) why += "dialog ";
                fx();
            }
            else if (nType == ObjectType::Creature)
            {
                nCreatures++;
                int32_t nInert = 0;
                for (int32_t e = 0; e < pObject->m_appliedEffects.num; e++)
                {
                    auto* pEffect = pObject->m_appliedEffects.element[e];
                    if (!pEffect)
                        continue;
                    auto d = pEffect->GetDurationType();
                    bool bInterval = pEffect->m_nType == EffectTrueType::RunScript || pEffect->m_nType == EffectTrueType::Regenerate ||
                                     pEffect->m_nType == EffectTrueType::Poison || pEffect->m_nType == EffectTrueType::Disease;
                    if (!bInterval && (d == EffectDurationType::Permanent || d == EffectDurationType::Equipped || d == EffectDurationType::Innate))
                        nInert++;
                }
                nCreatureFx += pObject->m_appliedEffects.num;
                nCreatureFxInert += nInert;
                if (nInert == pObject->m_appliedEffects.num)
                    nCreaturesAllInert++;
                continue;
            }
            else
                continue;
            reasons[std::string(ObjectType::ToString(nType)) + " list " + std::to_string(nList) + ": " + (why.empty() ? "idle" : why)]++;
        }
        std::string line;
        for (int32_t t = 0; t <= ObjectType::MAX; t++)
            if (byType[t])
                line += std::string(" ") + ObjectType::ToString(t) + "=" + std::to_string(byType[t]);
        LOG_INFO("Census list %d: %d objects:%s", nList, objects.num, line.c_str());
        nTotal += objects.num;
    }
    for (auto& [k, v] : reasons)
        LOG_INFO("Census not dormant: %s x%d", k.c_str(), v);
    int32_t dormantByType[ObjectType::MAX + 1] = {};
    for (auto& [oid, nList] : s_dormant)
        if (auto* p = Utils::GetGameObject(oid); p && p->m_nObjectType <= ObjectType::MAX)
            dormantByType[p->m_nObjectType]++;
    std::string line;
    for (int32_t t = 0; t <= ObjectType::MAX; t++)
        if (dormantByType[t])
            line += std::string(" ") + ObjectType::ToString(t) + "=" + std::to_string(dormantByType[t]);
    LOG_INFO("Census dormant: %zu:%s", s_dormant.size(), line.c_str());
    LOG_INFO("Census creatures in lists: %d, effects %d (permanent/equipped/innate, no interval: %d), creatures with only "
             "such effects: %d", nCreatures, nCreatureFx, nCreatureFxInert, nCreaturesAllInert);
    return nTotal;
}

// Returns TRUE if oObject is currently dormant (out of the AI list walk).
NWNX_EXPORT ArgumentStack GetIsDormant(ArgumentStack&& args)
{
    const auto oid = args.extract<ObjectID>();
    return (int32_t)(s_dormant.find(oid) != s_dormant.end());
}

}
