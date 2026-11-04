#include "nwnx.hpp"

#include "API/CGameEffect.hpp"
#include "API/CNWItemProperty.hpp"
#include "API/CNWSCreature.hpp"
#include "API/CNWSObject.hpp"

// The Holy Avenger item property (itempropdef row 36) dispels on hit inside the engine:
// CNWSCreature::ApplyOnHitDispelMagic(CNWSObject*, CNWItemProperty*) builds an
// EffectDispelMagicAll and adds it to the attack, with no script involved. Skip it while
// the target has area transition protection (gs_a_enter), so it matches the scripted
// dispels, which inc_dispel already blocks during that window.
//
// Only this overload is hooked. The On Hit: Lesser Dispel / Dispel Magic / Greater
// Dispelling / Mord's Disjunction properties use the other overloads, which queue the
// spell's impact script, so inc_dispel already covers them.

namespace Arelith {

using namespace NWNXLib;
using namespace NWNXLib::API;

// EFFECT_TAG_TRANSITION_PROTECTION in inc_effecttags.nss.
static constexpr const char* TRANSITION_PROTECTION_TAG = "TransitionProtection";

static Hooks::Hook s_ApplyOnHitDispelMagicHook;

static void ApplyOnHitDispelMagicHook(CNWSCreature* pThis, CNWSObject* pTarget, CNWItemProperty* pProperty)
{
    if (pTarget)
    {
        for (auto* pEffect : pTarget->m_appliedEffects)
        {
            if (pEffect && pEffect->m_sCustomTag == TRANSITION_PROTECTION_TAG)
                return;
        }
    }

    s_ApplyOnHitDispelMagicHook->CallOriginal<void>(pThis, pTarget, pProperty);
}

void HolyAvengerTransitionGuard() __attribute__((constructor));
void HolyAvengerTransitionGuard()
{
    if (!Config::Get<bool>("HOLY_AVENGER_TRANSITION_GUARD", false))
        return;

    LOG_INFO("Holy Avenger on-hit dispel is skipped while the target has transition protection.");

    s_ApplyOnHitDispelMagicHook = Hooks::HookFunction(
        static_cast<void (CNWSCreature::*)(CNWSObject*, CNWItemProperty*)>(&CNWSCreature::ApplyOnHitDispelMagic),
        &ApplyOnHitDispelMagicHook, Hooks::Order::Early);
}

}
