// Test-only NWNX plugin that reproduces the 8193.37 ghost player at engine level.
// Not built by default and never meant for a live server. To build it, copy this
// folder to Plugins/GhostRepro and add it to the build.
//
// It drives the same engine functions as a real duplicate login, in the order seen
// in the Distant Shores log, on an unused netlayer slot (no client needed):
//   1. first login:  slot in use + player privileges, PlayerListChange(id, TRUE)
//   2. second BNCS on the same connection: HandleBNCSMessage gets the same window
//      back and calls CNetLayerPlayerInfo::Initialize() on the slot, then the new
//      login is validated and PlayerListChange(id, TRUE) runs again
//   3. the second session leaves: CNetLayer::DisconnectPlayer(id)
//   4. the test then counts player objects that still use the slot.
// Stop the server afterwards (docker stop / SIGTERM): on an unfixed server
// CServerExoAppInternal::Shutdown spins forever in DisconnectPlayer.
//
// Env: NWNX_GHOSTREPRO_SLOT (default 200), NWNX_GHOSTREPRO_DELAY_TICKS (default 200),
//      NWNX_GHOSTREPRO_ORPHAN=true (only make an orphan whose slot is already free,
//      to test the shutdown safety net).

#include "nwnx.hpp"

#include "API/CAppManager.hpp"
#include "API/CServerExoApp.hpp"
#include "API/CServerExoAppInternal.hpp"
#include "API/CNetLayer.hpp"
#include "API/CNetLayerPlayerInfo.hpp"
#include "API/CNWSPlayer.hpp"

using namespace NWNXLib;
using namespace NWNXLib::API;

static Hooks::Hook s_MainLoopHook;

static int CountPlayers(CServerExoAppInternal *pInternal, uint32_t id)
{
    int n = 0;
    for (int i = 0; i < pInternal->m_lstPlayerList.num; i++)
        if (pInternal->m_lstPlayerList.element[i]->m_nPlayerID == id)
            n++;
    return n;
}

static void Login(CNetLayerPlayerInfo *pInfo, const char *name)
{
    // What the netlayer has set by the time a validated login reaches PlayerListChange.
    pInfo->m_bPlayerInUse = true;
    pInfo->m_sPlayerName = CExoString(name);
    pInfo->m_bPlayerPrivileges = true;
}

static void RunScenario()
{
    const uint32_t id = Config::Get<uint32_t>("SLOT", 200);
    auto *pServer = Globals::AppManager()->m_pServerExoApp;
    auto *pInternal = pServer->m_pcExoAppInternal;
    auto *pInfo = pServer->GetNetLayer()->GetPlayerInfo(id);

    LOG_INFO("GHOSTREPRO start slot=%u players=%d", id, pInternal->m_lstPlayerList.num);

    Login(pInfo, "GhostReproPlayer");
    pServer->PlayerListChange(id, true, false);
    LOG_INFO("GHOSTREPRO after first login: objects on slot=%d total=%d", CountPlayers(pInternal, id), pInternal->m_lstPlayerList.num);

    if (Config::Get<bool>("ORPHAN", false))
    {
        // Safety-net test: leave a player whose slot is already free (as if some
        // other path had reset it), without going through Initialize().
        pInfo->m_bPlayerInUse = false;
        pInfo->m_bPlayerPrivileges = false;
        LOG_INFO("GHOSTREPRO orphan made: objects on slot=%d total=%d slotInUse=0", CountPlayers(pInternal, id), pInternal->m_lstPlayerList.num);
        return;
    }

    // Second BNCS on the same connection: same window, slot reset, new login.
    pInfo->Initialize();
    Login(pInfo, "GhostReproPlayer");
    pServer->PlayerListChange(id, true, false);
    LOG_INFO("GHOSTREPRO after second login: objects on slot=%d total=%d", CountPlayers(pInternal, id), pInternal->m_lstPlayerList.num);

    pServer->GetNetLayer()->DisconnectPlayer(id, 5838, true, "");
    const int left = CountPlayers(pInternal, id);
    LOG_INFO("GHOSTREPRO after leave: objects on slot=%d total=%d slotInUse=%d -> %s", left, pInternal->m_lstPlayerList.num,
             pInfo->m_bPlayerInUse, left ? "GHOST PRESENT" : "NO GHOST");
}

static void GhostReproCtor() __attribute__((constructor));
static void GhostReproCtor()
{
    s_MainLoopHook = Hooks::HookFunction(&CServerExoAppInternal::MainLoop,
        +[](CServerExoAppInternal *pThis) -> BOOL
        {
            static uint32_t ticks = 0;
            static bool done = false;
            if (!done && pThis->GetModule() && ++ticks == Config::Get<uint32_t>("DELAY_TICKS", 200))
            {
                done = true;
                RunScenario();
            }
            return s_MainLoopHook->CallOriginal<BOOL>(pThis);
        }, Hooks::Order::Late);
}
