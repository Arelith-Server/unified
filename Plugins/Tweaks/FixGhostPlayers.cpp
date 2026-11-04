#include "nwnx.hpp"

#include "API/CAppManager.hpp"
#include "API/CServerExoApp.hpp"
#include "API/CServerExoAppInternal.hpp"
#include "API/CNetLayer.hpp"
#include "API/CNetLayerPlayerInfo.hpp"
#include "API/CNWSCreature.hpp"
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

// Set on an orphan's creature while it is removed, so leave scripts can skip work
// keyed on the player name or CD key (the netlayer slot no longer has them).
static const char *ORPHAN_LEAVE_VAR = "NWNX_TWEAKS_GHOST_PLAYER_LEAVE";
static constexpr uint32_t NO_PLAYER = 0xFFFFFFFF;
static constexpr uint32_t MAX_PLAYERS = 255; // CNetLayer::MAX_PLAYERS

static Hooks::Hook s_PlayerInfoInitializeHook;
static Hooks::Hook s_PlayerListChangeHook;
static Hooks::Hook s_SaveServerCharacterHook;
static Hooks::Hook s_DropTURDHook;
static bool s_bInHook;
static uint32_t s_nJustLeft = NO_PLAYER;   // last ID that PlayerListChange(FALSE) ran for
static CNWSPlayer *s_pOrphan;              // player whose save and TURD are suppressed

static CNWSPlayer* FindPlayer(CServerExoAppInternal *pInternal, uint32_t nPlayerId)
{
    auto &list = pInternal->m_lstPlayerList;
    for (int32_t i = 0; i < list.num; i++)
        if (list.element[i] && list.element[i]->m_nPlayerID == nPlayerId)
            return list.element[i];
    return nullptr;
}

// The engine cannot remove this player any more: its netlayer slot is gone or now
// belongs to another login. Do what PlayerListChange() does on a leave, but do not
// save the character or drop a TURD: the .bic on disk may be newer than this copy,
// and the slot's player name and CD key are not this player's any more.
static void RemoveOrphan(CServerExoApp *pServer, CServerExoAppInternal *pInternal, CNWSPlayer *pPlayer, const CExoString &sAddress)
{
    const uint32_t nPlayerId = pPlayer->m_nPlayerID;
    LOG_WARNING("Removing orphaned player object 0x%08x for player ID %u (slot address '%s'). The character is not saved.",
                pPlayer->m_oidNWSObject, nPlayerId, sAddress.CStr());

    if (auto *pCreature = pServer->GetCreatureByGameObjectID(pPlayer->m_oidNWSObject))
    {
        CExoString sVar = ORPHAN_LEAVE_VAR;
        pCreature->m_ScriptVars.SetInt(sVar, 1);
    }

    s_pOrphan = pPlayer;
    pInternal->RemovePCFromWorld(pPlayer);
    s_pOrphan = nullptr;

    // Scripts may have changed the list, so remove by pointer.
    pInternal->m_lstPlayerList.Remove(pPlayer);
    pInternal->m_lstClientDisconnectPending.Remove(nPlayerId);

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
    while (nPlayerId < MAX_PLAYERS && pNetLayer->GetPlayerInfo(nPlayerId) != pInfo)
        nPlayerId++;
    if (nPlayerId == MAX_PLAYERS || !FindPlayer(pInternal, nPlayerId))
        return;

    const CExoString sAddress = pNetLayer->GetPlayerAddress(nPlayerId, true);

    // Only the slot's own login may take the engine's leave (with its save), and only
    // when the engine has not just run that leave for this slot. That is the case when
    // a second connect request resets a slot that still has a logged-in player. On the
    // DisconnectPlayer path the engine has already removed the slot's own player, so
    // anything still left is an older copy and must not be saved.
    bool bEngineLeave = s_nJustLeft != nPlayerId
                     && pInfo->m_bPlayerInUse && (pInfo->m_bPlayerPrivileges || pInfo->m_bGameMasterPrivileges);

    for (int32_t nLeft = pInternal->m_lstPlayerList.num; nLeft > 0; nLeft--)
    {
        auto *pPlayer = FindPlayer(pInternal, nPlayerId);
        if (!pPlayer)
            break;

        if (bEngineLeave)
        {
            bEngineLeave = false;
            LOG_WARNING("Netlayer slot %u (address '%s') is being reset while player object 0x%08x still uses it; removing it with the normal leave.",
                        nPlayerId, sAddress.CStr(), pPlayer->m_oidNWSObject);
            pServer->PlayerListChange(nPlayerId, false, false);
            if (FindPlayer(pInternal, nPlayerId) != pPlayer)
                continue;
            LOG_ERROR("Normal leave did not remove player object 0x%08x for slot %u.", pPlayer->m_oidNWSObject, nPlayerId);
        }

        RemoveOrphan(pServer, pInternal, pPlayer, sAddress);
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
                s_nJustLeft = NO_PLAYER;
                s_bInHook = false;
            }
            s_PlayerInfoInitializeHook->CallOriginal<void>(pThis);
        }, Hooks::Order::Early);

    s_PlayerListChangeHook = Hooks::HookFunction(&CServerExoAppInternal::PlayerListChange,
        +[](CServerExoAppInternal *pThis, uint32_t nPlayerId, BOOL bEnter, BOOL bPrimaryPlayer) -> void
        {
            s_PlayerListChangeHook->CallOriginal<void>(pThis, nPlayerId, bEnter, bPrimaryPlayer);
            if (!bEnter)
                s_nJustLeft = nPlayerId;
            else if (s_nJustLeft == nPlayerId)
                s_nJustLeft = NO_PLAYER;
        }, Hooks::Order::Early);

    s_SaveServerCharacterHook = Hooks::HookFunction(&CNWSPlayer::SaveServerCharacter,
        +[](CNWSPlayer *pThis, BOOL bBackupPlayer) -> BOOL
        {
            if (pThis == s_pOrphan)
                return false;
            return s_SaveServerCharacterHook->CallOriginal<BOOL>(pThis, bBackupPlayer);
        }, Hooks::Order::Earliest);

    s_DropTURDHook = Hooks::HookFunction(&CNWSPlayer::DropTURD,
        +[](CNWSPlayer *pThis) -> void
        {
            if (pThis == s_pOrphan)
                return;
            s_DropTURDHook->CallOriginal<void>(pThis);
        }, Hooks::Order::Earliest);
}

}
