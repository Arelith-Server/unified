#include "nwnx.hpp"

#include "API/CNWSCreature.hpp"
#include "API/CNWSItem.hpp"
#include "API/CNWSCombatRound.hpp"

namespace Arelith {

using namespace NWNXLib;
using namespace NWNXLib::API;

static Hooks::Hook s_getWeaponPower;

//called whenever weapon power for DR piercing is calculated
int32_t WeaponPowerHook(CNWSCreature* thisPtr, CNWSObject* pTarget, int32_t bOffhand)
{
    static CExoString s_WeaponPowerOverride = "WEAPON_POWER_OVERRIDE";
    CNWSItem* pAttackWeapon = nullptr;
    int32_t nWeaponPower = 0;

    if (thisPtr->m_pcCombatRound)
    {
        pAttackWeapon = (bOffhand) ? thisPtr->m_pcCombatRound->GetCurrentAttackWeapon(2) :  thisPtr->m_pcCombatRound->GetCurrentAttackWeapon(0);
        if (pAttackWeapon) 
        {
            nWeaponPower = (int32_t)pAttackWeapon->m_ScriptVars.GetInt(s_WeaponPowerOverride);
        }
    }
    int32_t nNormalPower = s_getWeaponPower->CallOriginal<int32_t>(thisPtr, pTarget, bOffhand);

    return (nWeaponPower > nNormalPower) ? nWeaponPower : nNormalPower;
}

void WeaponPower() __attribute__((constructor));

void WeaponPower()
{
    s_getWeaponPower = Hooks::HookFunction(&CNWSCreature::GetWeaponPower, &WeaponPowerHook, Hooks::Order::Final);
}
}
