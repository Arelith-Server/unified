#include "nwnx.hpp"

#include "API/CNWSCombatRound.hpp"
#include "API/CNWSCreature.hpp"
#include "API/CNWSScriptVarTable.hpp"

namespace Arelith {

using namespace NWNXLib;
using namespace NWNXLib::API;
using namespace NWNXLib::API::Constants;

static CExoString DISABLE_AOO("DISABLE_AOO");

static Hooks::Hook s_StartCombatRoundHook;
static Hooks::Hook s_EndCombatRoundHook;

void StartCombatRound(CNWSCombatRound* thisPtr, OBJECT_ID oidTarget)
{
    s_StartCombatRoundHook->CallOriginal<void>(thisPtr, oidTarget);

    if (thisPtr->m_pBaseCreature && thisPtr->m_pBaseCreature->m_ScriptVars.GetInt(DISABLE_AOO))
    {
        thisPtr->m_nAttacksOfOpportunity = 0;
    }
}

void EndCombatRound(CNWSCombatRound* thisPtr)
{
    s_EndCombatRoundHook->CallOriginal<void>(thisPtr);

    if (thisPtr->m_pBaseCreature && thisPtr->m_pBaseCreature->m_ScriptVars.GetInt(DISABLE_AOO))
    {
        thisPtr->m_nAttacksOfOpportunity = 0;
    }
}

void AttackOfOpportunity() __attribute__((constructor));

void AttackOfOpportunity()
{
    s_StartCombatRoundHook = Hooks::HookFunction(&CNWSCombatRound::StartCombatRound, &StartCombatRound, Hooks::Order::Final);
    s_EndCombatRoundHook = Hooks::HookFunction(&CNWSCombatRound::EndCombatRound, &EndCombatRound, Hooks::Order::Final);
}

}
