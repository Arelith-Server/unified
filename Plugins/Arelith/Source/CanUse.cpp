#include "nwnx.hpp"

#include "API/CNWSCreature.hpp"
#include "API/CNWSItem.hpp"

namespace Arelith {

using namespace NWNXLib;
using namespace NWNXLib::API;
using namespace NWNXLib::API::Constants;

static Hooks::Hook s_canUseItemHook;

// Note: This is run on every item in the inventory when opening the inventory. Be careful! It's heavy!
int32_t CanUseItemHook(CNWSCreature* thisPtr, CNWSItem* pItem, int32_t bIgnoreIdentifiedFlag)
{
    static CExoString s_OverrideUsable = "OVERRIDE_USABLE";
    const int override = pItem->m_ScriptVars.GetInt(s_OverrideUsable);

    if (override == 1) // Force allow, if we're not in polymorph
    {
        return thisPtr->m_bIsPolymorphed ? 0 : 1;
    }
    else if (override == -1) // Force disallow
    {
        return 0;
    }
    
    // No override - fall back to base game logic.

    return s_canUseItemHook->CallOriginal<int32_t>(thisPtr, pItem, bIgnoreIdentifiedFlag);
}

void CanUse() __attribute__((constructor));

void CanUse()
{
    s_canUseItemHook = Hooks::HookFunction(API::Functions::_ZN12CNWSCreature10CanUseItemEP8CNWSItemi, (void*)&CanUseItemHook, Hooks::Order::Earliest);
}

}