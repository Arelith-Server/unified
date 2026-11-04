#include "nwnx.hpp"

#include "API/CAppManager.hpp"
#include "API/CServerExoApp.hpp"
#include "API/CServerExoAppInternal.hpp"
#include "API/CNWSPlayer.hpp"

namespace Optimizations {

using namespace NWNXLib;
using namespace NWNXLib::API;

// ExportSingleCharacter() only queues a request; the engine exports the queued requests at one point in the
// server tick (CServerExoAppInternal::MainLoop): it walks the list in order, calling ExportPlayer for each entry
// that has a client, re-reading the list length every step, then clears the list. ExportPlayer calls
// CNWSPlayer::SaveServerCharacter. A request added while the list is being walked (e.g. by a script run from
// inside a save, such as an NWNX save event handler) is still exported in that walk.
//
// A request is dropped here only if the same player has an entry that the walk has not reached yet: that entry
// will export the character after this request was made, so the result is the same. A request for a player
// whose entry has already been exported this tick is kept, as the engine would.
//
// s_nDrained counts the list entries the walk has passed. It is advanced when SaveServerCharacter starts (before
// any save event handler runs) to just past the first not-yet-passed entry for that player. Entries the walk
// passes without a save (no client, or ExportPlayer not saving) are passed with the next save; nothing can queue
// a request between the two, as no script runs in the walk outside SaveServerCharacter. A SaveServerCharacter
// call from elsewhere while requests are queued (logout, portal, ...) can only move s_nDrained too far, which
// keeps requests that could have been dropped and never drops one that is needed. The list is empty between
// ticks, which resets the count.
//
// (ExportPlayer itself cannot be hooked: it starts with a short conditional branch that the hook library cannot
// relocate.)
//
// Main thread only, like the engine's export list.
static Hooks::Hook s_AddExportPlayersCharacterRequestHook;
static Hooks::Hook s_SaveServerCharacterHook;
static int32_t s_nDrained;
static uint64_t s_nCoalesced;

static void AddExportPlayersCharacterRequest(CServerExoApp *pThis, OBJECT_ID nPlayerId)
{
    const auto& requests = pThis->m_pcExoAppInternal->m_exportPlayerCharacterRequests;
    if (requests.num == 0)
        s_nDrained = 0;
    for (int32_t i = s_nDrained; i < requests.num; i++)
    {
        if (requests.element[i] == nPlayerId)
        {
            s_nCoalesced++;
            LOG_DEBUG("Character export for player id %u already queued this tick, not queueing it again (%llu so far)",
                nPlayerId, (unsigned long long)s_nCoalesced);
            return;
        }
    }
    s_AddExportPlayersCharacterRequestHook->CallOriginal<void>(pThis, nPlayerId);
}

static BOOL SaveServerCharacter(CNWSPlayer *pThis, BOOL bBackupPlayer)
{
    const auto& requests = Globals::AppManager()->m_pServerExoApp->m_pcExoAppInternal->m_exportPlayerCharacterRequests;
    for (int32_t i = s_nDrained; i < requests.num; i++)
    {
        if (requests.element[i] == pThis->m_nPlayerID)
        {
            s_nDrained = i + 1;
            break;
        }
    }
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
