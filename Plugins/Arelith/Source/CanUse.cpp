#include "nwnx.hpp"

#include "API/CNWSCreature.hpp"
#include "API/CNWSCreatureStats.hpp"
#include "API/CNWSItem.hpp"

namespace Arelith {

using namespace NWNXLib;
using namespace NWNXLib::API;
using namespace NWNXLib::API::Constants;

static Hooks::Hook s_canUseItemHook;

// Note: This is run on every item in the inventory when opening the inventory. Be careful! It's heavy!
int32_t CanUseItemHook(CNWSCreature* thisPtr, CNWSItem* pItem, int32_t bIgnoreIdentifiedFlag)
{
    static std::string s_UpdateSkillScript = "arevt_canuse";

    static CExoString s_OverrideUsable = "OVERRIDE_USABLE";
    static CExoString s_OverrideUsableLore = "OVERRIDE_USABLE_LORE";
    static CExoString s_OverrideUsableUMD = "OVERRIDE_USABLE_UMD";
    static CExoString s_LoreSkillCache = "LORE_SKILL_CACHE";
    static CExoString s_ArelithUpdating = "NWNX_ARELITH_UPDATING";

    // For scrolls, we have a little hardcoded logic.
    // If the scroll isn't in the calling creature's inventory, it will force it to show up as unusable.
    // This covers things like stores or containers.
    if ((pItem->m_nBaseItem == BaseItem::SpellScroll || pItem->m_nBaseItem == BaseItem::EnchantedScroll) && pItem->m_oidPossessor != thisPtr->m_idSelf)
    {
        return 0;
    }

    int override = pItem->m_ScriptVars.GetInt(s_OverrideUsable);

    if (override)
    {
        // If override is set, let's check if we need to refresh the item. What we do here depends on base type.

        int lastSkill = 0;
        int curSkill = 0;

        if (pItem->m_nBaseItem == BaseItem::SpellScroll || pItem->m_nBaseItem == BaseItem::EnchantedScroll)
        {
            // Scrolls use lore - lore is taken from a cache on the PC.
            lastSkill = pItem->m_ScriptVars.GetInt(s_OverrideUsableLore);
            curSkill = thisPtr->m_ScriptVars.GetInt(s_LoreSkillCache);
        }
        else if (pItem->m_nBaseItem == BaseItem::MagicWand || pItem->m_nBaseItem == BaseItem::EnchantedWand)
        {
            // Wands use UMD, which is taken directgly from the creature stats.
            lastSkill = pItem->m_ScriptVars.GetInt(s_OverrideUsableUMD);
            curSkill = thisPtr->m_pStats->GetSkillRank(Skill::UseMagicDevice, nullptr);
        }

        if (lastSkill != curSkill)
        {
            // We need to update the flag since we have a mismatch now, so we call the hardcoded event.
            thisPtr->m_ScriptVars.SetObject(s_ArelithUpdating, pItem->m_idSelf);
            Utils::ExecuteScript(s_UpdateSkillScript, thisPtr->m_idSelf);

            // Override might have changed at this point, so we refresh it.
            override = pItem->m_ScriptVars.GetInt(s_OverrideUsable);
        }
    }

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