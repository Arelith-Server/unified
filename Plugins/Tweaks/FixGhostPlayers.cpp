#include "nwnx.hpp"

#include "API/CAppManager.hpp"
#include "API/CServerExoApp.hpp"
#include "API/CServerExoAppInternal.hpp"
#include "API/CNetLayer.hpp"
#include "API/CNetLayerPlayerInfo.hpp"
#include "API/CNWSMessage.hpp"
#include "API/CNWSPlayer.hpp"

#include <new>

// 8193.37 bug: a client that sends a second connect request (BNCS) on a connection
// that already has a sliding window gets the same window back. HandleBNCSMessage
// then calls CNetLayerPlayerInfo::Initialize() on that slot without removing the
// CNWSPlayer of the earlier login, and the new login adds a second CNWSPlayer with
// the same player ID. The earlier one can never be removed again: PlayerListChange()
// only removes players whose netlayer slot still has player or DM privileges, so
// CServerExoAppInternal::Shutdown() loops on it forever.
//
// Every place where the engine resets a netlayer slot goes through
// CNetLayerPlayerInfo::Initialize(), so this keeps one rule there: when a slot is
// reset, no CNWSPlayer may keep its player ID.

namespace Tweaks {

using namespace NWNXLib;
using namespace NWNXLib::API;

static Hooks::Hook s_PlayerInfoInitializeHook;
static Hooks::Hook s_SaveServerCharacterHook;
static bool s_bInHook;
static bool s_bSuppressSave;

static CNWSPlayer* FindPlayer(CServerExoAppInternal *pInternal, uint32_t nPlayerId, int32_t *pIndex = nullptr)
{
    auto &list = pInternal->m_lstPlayerList;
    for (int32_t i = 0; i < list.num; i++)
    {
        if (list.element[i] && list.element[i]->m_nPlayerID == nPlayerId)
        {
            if (pIndex)
                *pIndex = i;
            return list.element[i];
        }
    }
    return nullptr;
}

// The engine cannot remove this player any more because its netlayer slot is gone.
// Do what PlayerListChange() does on a leave, but do not save the character: the
// player's login was replaced, so its .bic on disk may be newer than this copy.
static void RemoveOrphan(CServerExoApp *pServer, CServerExoAppInternal *pInternal, CNWSPlayer *pPlayer, int32_t nIndex)
{
    LOG_WARNING("Removing orphaned player object for player ID %u (object 0x%08x); its netlayer slot is no longer in use. The character is not saved.",
                pPlayer->m_nPlayerID, pPlayer->m_oidNWSObject);

    s_bSuppressSave = true;
    pInternal->RemovePCFromWorld(pPlayer);
    s_bSuppressSave = false;

    pInternal->m_lstPlayerList.DelIndex(nIndex);

    auto *pMessage = pServer->GetNWSMessage();
    pMessage->SendServerToPlayerPlayerList_Delete(Constants::PLAYERID_ALL_PLAYERS, pPlayer);
    pMessage->SendServerToPlayerPlayerList_Delete(Constants::PLAYERID_ALL_GAMEMASTERS, pPlayer);
    pMessage->SendServerToPlayerDungeonMasterUpdatePartyList(Constants::PLAYERID_ALL_GAMEMASTERS);

    pPlayer->~CNWSPlayer();
    ::operator delete(pPlayer);
}

static void RemoveStalePlayers(CNetLayerPlayerInfo *pInfo)
{
    auto *pServer = Globals::AppManager() ? Globals::AppManager()->m_pServerExoApp : nullptr;
    if (!pServer || !pServer->m_pcExoAppInternal)
        return;
    auto *pInternal = pServer->m_pcExoAppInternal;
    if (pInternal->m_lstPlayerList.num == 0)
        return;
    auto *pNetLayer = pServer->GetNetLayer();
    if (!pNetLayer)
        return;

    // CNetLayerPlayerInfo has no player ID field; it is the index into the slot array.
    uint32_t nPlayerId = 0;
    constexpr uint32_t MAX_PLAYERS = 255; // CNetLayer::MAX_PLAYERS
    while (nPlayerId < MAX_PLAYERS && pNetLayer->GetPlayerInfo(nPlayerId) != pInfo)
        nPlayerId++;
    if (nPlayerId == MAX_PLAYERS)
        return;

    for (int32_t nLeft = pInternal->m_lstPlayerList.num; nLeft > 0; nLeft--)
    {
        int32_t nIndex;
        auto *pPlayer = FindPlayer(pInternal, nPlayerId, &nIndex);
        if (!pPlayer)
            return;

        if (pInfo->m_bPlayerInUse && (pInfo->m_bPlayerPrivileges || pInfo->m_bGameMasterPrivileges))
        {
            // The slot still describes the earlier login: let the engine run its own
            // leave (save, OnClientLeave, player list updates, "Left as a Player").
            LOG_WARNING("Netlayer slot %u is being reset while player object 0x%08x still uses it; removing it with the normal leave.",
                        nPlayerId, pPlayer->m_oidNWSObject);
            pServer->PlayerListChange(nPlayerId, false, false);
            if (FindPlayer(pInternal, nPlayerId) == pPlayer)
            {
                LOG_ERROR("Normal leave did not remove player object 0x%08x for slot %u.", pPlayer->m_oidNWSObject, nPlayerId);
                RemoveOrphan(pServer, pInternal, pPlayer, nIndex);
            }
        }
        else
        {
            RemoveOrphan(pServer, pInternal, pPlayer, nIndex);
        }
    }
}

void FixGhostPlayers() __attribute__((constructor));
void FixGhostPlayers()
{
    if (!Config::Get<bool>("FIX_GHOST_PLAYERS", false))
        return;

    LOG_INFO("Player objects are removed when their netlayer slot is reset (fixes ghost players and the shutdown hang).");

    s_PlayerInfoInitializeHook = Hooks::HookFunction(&CNetLayerPlayerInfo::Initialize,
        +[](CNetLayerPlayerInfo *pThis) -> void
        {
            if (!s_bInHook)
            {
                s_bInHook = true;
                RemoveStalePlayers(pThis);
                s_bInHook = false;
            }
            s_PlayerInfoInitializeHook->CallOriginal<void>(pThis);
        }, Hooks::Order::Early);

    s_SaveServerCharacterHook = Hooks::HookFunction(&CNWSPlayer::SaveServerCharacter,
        +[](CNWSPlayer *pThis, BOOL bBackupPlayer) -> BOOL
        {
            if (s_bSuppressSave)
                return false;
            return s_SaveServerCharacterHook->CallOriginal<BOOL>(pThis, bBackupPlayer);
        }, Hooks::Order::Earliest);
}

}
