#include "nwnx.hpp"

#include "API/CServerExoApp.hpp"

namespace Optimizations {

using namespace NWNXLib;
using namespace NWNXLib::API;

// ExportSingleCharacter() only queues a request; the engine exports every queued request at one point in
// the server tick (CServerExoAppInternal::MainLoop), in queue order, and a player queued twice is exported
// twice, back to back with nothing in between that can change the character. Both writes produce the same
// file, so a request for a player who is already queued for this tick is dropped here.
static Hooks::Hook s_AddExportPlayersCharacterRequestHook;
static uint64_t s_nCoalesced;

static void AddExportPlayersCharacterRequest(CServerExoApp *pThis, OBJECT_ID nPlayerId)
{
    const auto& requests = pThis->GetExportPlayersCharacterRequests();
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
    s_AddExportPlayersCharacterRequestHook->CallOriginal<void>(pThis, nPlayerId);
}

void CoalesceCharacterExports() __attribute__((constructor));
void CoalesceCharacterExports()
{
    if (Config::Get<bool>("COALESCE_CHARACTER_EXPORTS", false))
    {
        LOG_INFO("Repeated character export requests for the same player in one server tick will be exported once");
        s_AddExportPlayersCharacterRequestHook = Hooks::HookFunction(&CServerExoApp::AddExportPlayersCharacterRequest,
            &AddExportPlayersCharacterRequest, Hooks::Order::Late);
    }
}

}
