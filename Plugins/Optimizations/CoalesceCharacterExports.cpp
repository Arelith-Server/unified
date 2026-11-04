#include "nwnx.hpp"

#include "API/CServerExoApp.hpp"
#include "API/CServerExoAppInternal.hpp"
#include "API/CNWSPlayer.hpp"

namespace Optimizations {

using namespace NWNXLib;
using namespace NWNXLib::API;

// ExportSingleCharacter() only queues a request; the engine exports the queued requests at one point in the
// server tick (CServerExoAppInternal::MainLoop): it walks the list in order, calling ExportPlayer (and so
// CNWSPlayer::SaveServerCharacter) for each entry that has a client, re-reading the list length every step, then
// clears the list. If a player is queued twice in a tick, the engine exports them twice.
//
// A request made outside any character save, for a player who is already in the list, is dropped: the list is
// only walked later, so the queued entry exports the character after this request was made.
//
// While a SaveServerCharacter is running (the drain itself, or a logout/portal save), requests are queued exactly
// as the engine does. Scripts run there (e.g. NWNX save event handlers) and the walk may already have passed the
// player's entry, or later handlers in the same tick may change the character after it; keeping every such request
// keeps the engine's behaviour.
//
// Main thread only, like the engine's export list.
static Hooks::Hook s_AddExportPlayersCharacterRequestHook;
static Hooks::Hook s_SaveServerCharacterHook;
static int s_nSaveDepth;
static uint64_t s_nCoalesced;

namespace {
struct SaveDepthGuard
{
    SaveDepthGuard() { s_nSaveDepth++; }
    ~SaveDepthGuard() { s_nSaveDepth--; }
};
}

static void AddExportPlayersCharacterRequest(CServerExoApp *pThis, OBJECT_ID nPlayerId)
{
    if (s_nSaveDepth == 0)
    {
        const auto& requests = pThis->m_pcExoAppInternal->m_exportPlayerCharacterRequests;
        for (int32_t i = 0; i < requests.num; i++)
        {
            if (requests.element[i] == nPlayerId)
            {
                s_nCoalesced++;
                LOG_DEBUG("Character export for player id %u already queued this tick, not queueing it again (%llu so far)",
                    nPlayerId, (unsigned long long)s_nCoalesced);
                return;
            }
        }
    }
    s_AddExportPlayersCharacterRequestHook->CallOriginal<void>(pThis, nPlayerId);
}

static BOOL SaveServerCharacter(CNWSPlayer *pThis, BOOL bBackupPlayer)
{
    SaveDepthGuard guard;
    return s_SaveServerCharacterHook->CallOriginal<BOOL>(pThis, bBackupPlayer);
}

void CoalesceCharacterExports() __attribute__((constructor));
void CoalesceCharacterExports()
{
    if (Config::Get<bool>("COALESCE_CHARACTER_EXPORTS", false))
    {
        LOG_INFO("Repeated character export requests for the same player in one server tick will be exported once");
        s_AddExportPlayersCharacterRequestHook = Hooks::HookFunction(&CServerExoApp::AddExportPlayersCharacterRequest,
            &AddExportPlayersCharacterRequest, Hooks::Order::Late);
        s_SaveServerCharacterHook = Hooks::HookFunction(&CNWSPlayer::SaveServerCharacter, &SaveServerCharacter,
            Hooks::Order::Earliest);
    }
}

}
