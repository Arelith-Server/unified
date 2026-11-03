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
//   - an action is queued on it (CNWSObject::AddAction),
//   - and, checked at each sweep, if it gained a heartbeat script, actions or a non-permanent effect some other way.
// If the engine itself adds or removes a dormant object (CServerAIMaster::AddObject/RemoveObject), the plugin forgets
// it. Off unless NWNX_DORMANTOBJECTS_ENABLE=true.

#include "nwnx.hpp"

#include "API/CAppManager.hpp"
#include "API/CExoLinkedListInternal.hpp"
#include "API/CGameEffect.hpp"
#include "API/CNWSItem.hpp"
#include "API/CNWSObject.hpp"
#include "API/CNWSPlaceable.hpp"
#include "API/CServerAIList.hpp"
#include "API/CServerAIMaster.hpp"
#include "API/CServerExoApp.hpp"
#include "API/Constants.hpp"
#include "API/Globals.hpp"

#include <chrono>
#include <unordered_map>

namespace DormantObjects {

using namespace NWNXLib;
using namespace NWNXLib::API;
using namespace NWNXLib::API::Constants;

// Placeable script slots are EVENT_SCRIPT_PLACEABLE_ON_* - 9000; the heartbeat is 9004.
static constexpr int PLACEABLE_SCRIPT_HEARTBEAT = 4;

static std::unordered_map<ObjectID, int32_t> s_dormant; // object -> AI list it was taken from
static std::chrono::steady_clock::duration s_period;
static std::chrono::steady_clock::time_point s_nextSweep;
static bool s_placeables, s_items;

static Hooks::Hook s_UpdateStateHook;
static Hooks::Hook s_OnEffectAppliedHook;
static Hooks::Hook s_AddActionHook;
static Hooks::Hook s_AddObjectHook;
static Hooks::Hook s_RemoveObjectHook;

static bool EffectsNeverExpire(CNWSObject* pObject)
{
    for (int32_t i = 0; i < pObject->m_appliedEffects.num; i++)
    {
        auto* pEffect = pObject->m_appliedEffects.element[i];
        if (!pEffect)
            continue;
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

static bool IsDormant(CGameObject* pGameObject)
{
    if (!pGameObject)
        return false;

    if (s_placeables)
    {
        if (auto* pPlaceable = Utils::AsNWSPlaceable(pGameObject))
        {
            return !pPlaceable->m_bHasInventory &&
                   !pPlaceable->m_bTrapFlag &&
                   pPlaceable->m_sScripts[PLACEABLE_SCRIPT_HEARTBEAT].IsEmpty() &&
                   !HasQueuedActions(pPlaceable) &&
                   !pPlaceable->GetDead() &&
                   EffectsNeverExpire(pPlaceable);
        }
    }
    if (s_items)
    {
        if (auto* pItem = Utils::AsNWSItem(pGameObject))
            return !HasQueuedActions(pItem) && EffectsNeverExpire(pItem);
    }
    return false;
}

static void Wake(CServerAIMaster* pMaster, ObjectID oid)
{
    auto it = s_dormant.find(oid);
    if (it == s_dormant.end())
        return;
    int32_t nList = it->second;
    s_dormant.erase(it);
    pMaster->m_apGameAIList[nList].AddObject(oid);
}

static void Sweep(CServerAIMaster* pMaster)
{
    // Objects already dormant: forget destroyed ones, wake any that gained work some way the hooks don't see.
    int32_t nWoken = 0, nGone = 0;
    for (auto it = s_dormant.begin(); it != s_dormant.end();)
    {
        auto* pGameObject = Utils::GetGameObject(it->first);
        if (!pGameObject)
        {
            it = s_dormant.erase(it);
            nGone++;
        }
        else if (!IsDormant(pGameObject))
        {
            int32_t nList = it->second;
            ObjectID oid = it->first;
            it = s_dormant.erase(it);
            pMaster->m_apGameAIList[nList].AddObject(oid);
            nWoken++;
        }
        else
            ++it;
    }

    // One pass per list: keep order, drop dormant objects, keep the walk's resume position on the same object.
    int32_t nDropped = 0;
    int32_t nRemaining = 0;
    for (int32_t nList = AIPriority::MIN; nList <= AIPriority::MAX; nList++)
    {
        auto& list = pMaster->m_apGameAIList[nList];
        auto& objects = list.m_aoGameObjects;
        int32_t nKeep = 0, nNewPosition = 0;
        for (int32_t i = 0; i < objects.num; i++)
        {
            ObjectID oid = objects.element[i];
            if (IsDormant(Utils::GetGameObject(oid)))
            {
                s_dormant[oid] = nList;
                nDropped++;
                continue;
            }
            if (i < list.m_nPosition)
                nNewPosition++;
            objects.element[nKeep++] = oid;
        }
        objects.num = nKeep;
        list.m_nPosition = nNewPosition < nKeep ? nNewPosition : 0;
        nRemaining += nKeep;
    }

    LOG_INFO("Sweep: %d made dormant, %d woken, %d destroyed; %zu dormant, %d left in the AI lists",
             nDropped, nWoken, nGone, s_dormant.size(), nRemaining);
}

void DormantObjects() __attribute__((constructor));
void DormantObjects()
{
    if (!Config::Get<bool>("ENABLE", false))
        return;

    s_period = std::chrono::seconds(Config::Get<int32_t>("SWEEP_SECONDS", 10));
    s_placeables = Config::Get<bool>("PLACEABLES", true);
    s_items = Config::Get<bool>("ITEMS", true);
    s_nextSweep = std::chrono::steady_clock::now() + s_period;
    LOG_INFO("Enabled: sweep every %lld s, placeables %d, items %d",
             (long long)std::chrono::duration_cast<std::chrono::seconds>(s_period).count(), s_placeables, s_items);

    s_UpdateStateHook = Hooks::HookFunction(&CServerAIMaster::UpdateState,
        +[](CServerAIMaster* pThis) -> void
        {
            auto now = std::chrono::steady_clock::now();
            if (now >= s_nextSweep)
            {
                Sweep(pThis);
                s_nextSweep = now + s_period;
            }
            s_UpdateStateHook->CallOriginal<void>(pThis);
        }, Hooks::Order::Late);

    s_OnEffectAppliedHook = Hooks::HookFunction(&CServerAIMaster::OnEffectApplied,
        +[](CServerAIMaster* pThis, CNWSObject* pObject, CGameEffect* pEffect, BOOL bLoadingGame) -> int32_t
        {
            if (pObject)
                Wake(pThis, pObject->m_idSelf);
            return s_OnEffectAppliedHook->CallOriginal<int32_t>(pThis, pObject, pEffect, bLoadingGame);
        }, Hooks::Order::Early);

    s_AddActionHook = Hooks::HookFunction(&CNWSObject::AddAction,
        +[](CNWSObject* pThis, uint32_t nActionId, uint16_t nGroupId,
            uint32_t t1, void* p1, uint32_t t2, void* p2, uint32_t t3, void* p3, uint32_t t4, void* p4,
            uint32_t t5, void* p5, uint32_t t6, void* p6, uint32_t t7, void* p7, uint32_t t8, void* p8,
            uint32_t t9, void* p9, uint32_t t10, void* p10, uint32_t t11, void* p11, uint32_t t12, void* p12) -> void
        {
            if (pThis && !s_dormant.empty())
                Wake(Globals::AppManager()->m_pServerExoApp->GetServerAIMaster(), pThis->m_idSelf);
            s_AddActionHook->CallOriginal<void>(pThis, nActionId, nGroupId, t1, p1, t2, p2, t3, p3, t4, p4,
                t5, p5, t6, p6, t7, p7, t8, p8, t9, p9, t10, p10, t11, p11, t12, p12);
        }, Hooks::Order::Early);

    // The engine adding or removing an object we hold as dormant takes precedence: forget it.
    s_AddObjectHook = Hooks::HookFunction(&CServerAIMaster::AddObject,
        +[](CServerAIMaster* pThis, CNWSObject* pObject, int32_t nAILevel) -> BOOL
        {
            if (pObject && !s_dormant.empty())
                s_dormant.erase(pObject->m_idSelf);
            return s_AddObjectHook->CallOriginal<BOOL>(pThis, pObject, nAILevel);
        }, Hooks::Order::Early);

    s_RemoveObjectHook = Hooks::HookFunction(&CServerAIMaster::RemoveObject,
        +[](CServerAIMaster* pThis, CNWSObject* pObject) -> BOOL
        {
            if (pObject && !s_dormant.empty())
                s_dormant.erase(pObject->m_idSelf);
            return s_RemoveObjectHook->CallOriginal<BOOL>(pThis, pObject);
        }, Hooks::Order::Early);
}

}
