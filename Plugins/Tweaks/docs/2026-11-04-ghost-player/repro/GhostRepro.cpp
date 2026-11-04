// Test-only NWNX plugin that reproduces the 8193.37 ghost player at engine level.
// Not built by default and never meant for a live server. To build it, copy this
// folder to Plugins/GhostRepro and add it to the build.
//
// No client is needed. Once the module has run for DELAY_TICKS main-loop ticks, it
// runs three cases, each on its own unused netlayer slot, driving the same engine
// functions as the real sequences. Each case's first (older) player object gets a
// module NPC creature attached as its "character", so leaving it goes through the
// full RemovePCFromWorld (save, OnClientLeave, TURD) instead of the no-creature
// shortcut.
//
//   relogin (slot 200), the Distant Shores sequence:
//     validated slot + PlayerListChange(id, TRUE); then what a second BNCS on the
//     same connection does: CNetLayerPlayerInfo::Initialize() on the slot; then the
//     new login is validated and PlayerListChange(id, TRUE) runs again; then the
//     second session leaves: CNetLayer::DisconnectPlayer(id).
//   double (slot 201): two player objects already share the slot when the player
//     leaves through DisconnectPlayer (no reset in between).
//   orphan (slot 202): a player object whose slot is already free, left in the list
//     until shutdown.
//
// Character saves and TURD drops that reach the engine are logged ("GHOSTREPRO save"
// / "GHOSTREPRO turd") and then skipped, so the test never writes into the vault.
// Stop the server afterwards (console "exit"): on an unfixed server
// CServerExoAppInternal::Shutdown spins forever in DisconnectPlayer.
//
// Env: NWNX_GHOSTREPRO_DELAY_TICKS (default 200).

#include "nwnx.hpp"

#include "API/CAppManager.hpp"
#include "API/CServerExoApp.hpp"
#include "API/CServerExoAppInternal.hpp"
#include "API/CNetLayer.hpp"
#include "API/CNetLayerPlayerInfo.hpp"
#include "API/CNWSArea.hpp"
#include "API/CNWSCreature.hpp"
#include "API/CNWSModule.hpp"
#include "API/CNWSPlayer.hpp"

#include <cstring>

using namespace NWNXLib;
using namespace NWNXLib::API;

static Hooks::Hook s_MainLoopHook;
static Hooks::Hook s_SaveHook;
static Hooks::Hook s_TurdHook;

static CServerExoApp* Server() { return Globals::AppManager()->m_pServerExoApp; }
static CServerExoAppInternal* Internal() { return Server()->m_pcExoAppInternal; }

static int CountPlayers(uint32_t id)
{
    int n = 0;
    for (int i = 0; i < Internal()->m_lstPlayerList.num; i++)
        if (Internal()->m_lstPlayerList.element[i]->m_nPlayerID == id)
            n++;
    return n;
}

// What the netlayer has set by the time a validated login reaches PlayerListChange.
static void Validate(CNetLayerPlayerInfo *pInfo)
{
    pInfo->m_bPlayerInUse = true;
    pInfo->m_sPlayerName = CExoString("GhostReproPlayer");
    pInfo->m_bPlayerPrivileges = true;
}

static CNWSPlayer* Login(uint32_t id)
{
    Server()->PlayerListChange(id, true, false);
    return Internal()->m_lstPlayerList.element[0]; // PlayerListChange inserts at index 0
}

// Give the player object a module NPC as its creature, so its leave is a full one.
static void AttachCreature(CNWSPlayer *pPlayer)
{
    auto *pModule = Server()->GetModule();
    for (int a = 0; a < pModule->m_lstModuleAreaID.num; a++)
    {
        auto *pArea = Server()->GetAreaByGameObjectID(pModule->m_lstModuleAreaID.element[a]);
        if (!pArea)
            continue;
        for (int o = 0; o < pArea->m_aGameObjects.num; o++)
        {
            auto *pCreature = Server()->GetCreatureByGameObjectID(pArea->m_aGameObjects.element[o]);
            if (!pCreature || pCreature->m_bPlayerCharacter)
                continue;
            pCreature->m_bPlayerCharacter = true;
            pPlayer->m_oidNWSObject = pCreature->m_idSelf;
            LOG_INFO("GHOSTREPRO slot %u: player object %p has creature 0x%08x", pPlayer->m_nPlayerID, (void*)pPlayer, pCreature->m_idSelf);
            return;
        }
    }
    LOG_WARNING("GHOSTREPRO no NPC creature found to attach");
}

static void Report(const char *name, uint32_t id, const char *step)
{
    LOG_INFO("GHOSTREPRO %s %s: objects on slot=%d total=%d slotInUse=%d", name, step, CountPlayers(id),
             Internal()->m_lstPlayerList.num, Server()->GetNetLayer()->GetPlayerInfo(id)->m_bPlayerInUse);
}

static void RunCase(const char *name, uint32_t id)
{
    auto *pInfo = Server()->GetNetLayer()->GetPlayerInfo(id);
    LOG_INFO("GHOSTREPRO %s start slot=%u", name, id);

    Validate(pInfo);
    AttachCreature(Login(id));
    Report(name, id, "after first login");

    if (!strcmp(name, "orphan"))
    {
        // The slot is already free, as if some other path had reset it.
        pInfo->m_bPlayerInUse = false;
        pInfo->m_bPlayerPrivileges = false;
        Report(name, id, "orphan made");
        return;
    }

    if (!strcmp(name, "relogin"))
    {
        pInfo->Initialize(); // second BNCS on the same connection: same window, slot reset
        Validate(pInfo);
    }
    Login(id);
    Report(name, id, "after second login");

    Server()->GetNetLayer()->DisconnectPlayer(id, 5838, true, "");
    const int left = CountPlayers(id);
    LOG_INFO("GHOSTREPRO %s after leave: objects on slot=%d -> %s", name, left, left ? "GHOST PRESENT" : "NO GHOST");
}

static void GhostReproCtor() __attribute__((constructor));
static void GhostReproCtor()
{
    s_MainLoopHook = Hooks::HookFunction(&CServerExoAppInternal::MainLoop,
        +[](CServerExoAppInternal *pThis) -> BOOL
        {
            static uint32_t ticks = 0;
            if (pThis->GetModule() && ++ticks == Config::Get<uint32_t>("DELAY_TICKS", 200))
            {
                RunCase("relogin", 200);
                RunCase("double", 201);
                RunCase("orphan", 202);
                LOG_INFO("GHOSTREPRO all cases done");
            }
            return s_MainLoopHook->CallOriginal<BOOL>(pThis);
        }, Hooks::Order::Late);

    s_SaveHook = Hooks::HookFunction(&CNWSPlayer::SaveServerCharacter,
        +[](CNWSPlayer *pThis, BOOL) -> BOOL
        {
            LOG_INFO("GHOSTREPRO save reached the engine for player ID %u object 0x%08x (skipped)", pThis->m_nPlayerID, pThis->m_oidNWSObject);
            return true;
        }, Hooks::Order::Latest);

    s_TurdHook = Hooks::HookFunction(&CNWSPlayer::DropTURD,
        +[](CNWSPlayer *pThis) -> void
        {
            LOG_INFO("GHOSTREPRO turd reached the engine for player ID %u (skipped)", pThis->m_nPlayerID);
        }, Hooks::Order::Latest);
}
