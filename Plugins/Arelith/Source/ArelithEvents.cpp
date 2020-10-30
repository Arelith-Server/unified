#include "Source/ArelithEvents.hpp"
#include "API/CExoString.hpp"
#include "API/CNWSMessage.hpp"
#include "API/CVirtualMachine.hpp"
#include "API/CAppManager.hpp"
#include "API/CServerExoApp.hpp"
#include "API/CNWSCreature.hpp"
#include "API/CNWSItem.hpp"
#include "API/Functions.hpp"
#include "API/CNWSObject.hpp"
#include "API/Constants.hpp"
#include "API/CGameEffect.hpp"
#include "API/Globals.hpp"
#include "Plugin.hpp"
#include "Arelith.hpp"
#include "Utils.hpp"
#include "API/Functions.hpp"

namespace Arelith {

using namespace NWNXLib;
using namespace NWNXLib::API;
using namespace NWNXLib::API::Constants;

static NWNXLib::Hooking::FunctionHook* m_CanEquipWeaponHook=nullptr;
static NWNXLib::Hooking::FunctionHook* m_CanUnEquipWeaponHook=nullptr;
static NWNXLib::Hooking::FunctionHook* m_OnApplyDisarmHook=nullptr;


ArelithEvents::ArelithEvents(Services::HooksProxy* hooker)
{
    Arelith::InitOnFirstSubscribe("NWNX_ARELITH_.*", [hooker]() {
        hooker->RequestExclusiveHook<Functions::_ZN12CNWSCreature14CanEquipWeaponEP8CNWSItemPjiiP10CNWSPlayer, unsigned char, CNWSCreature*, CNWSItem*, int32_t*, int32_t, int32_t, CNWSPlayer*>(&CanEquipWeaponHook);
        m_CanEquipWeaponHook =  hooker->FindHookByAddress(Functions::_ZN12CNWSCreature14CanEquipWeaponEP8CNWSItemPjiiP10CNWSPlayer);
        hooker->RequestExclusiveHook<Functions::_ZN12CNWSCreature16CanUnEquipWeaponEP8CNWSItem, unsigned char, CNWSCreature*, CNWSItem*>(&CanUnEquipWeaponHook);
        m_CanUnEquipWeaponHook =  hooker->FindHookByAddress(Functions::_ZN12CNWSCreature16CanUnEquipWeaponEP8CNWSItem);
    });
}

unsigned char ArelithEvents::CanEquipWeaponHook( CNWSCreature *pCreature, CNWSItem *pItem, int32_t *nEquipToSlot, int32_t bEquipping, int32_t bDisplayFeedback, CNWSPlayer *pFeedbackPlayer)
{
    unsigned char retVal = m_CanEquipWeaponHook->CallOriginal<unsigned char>(pCreature, pItem, nEquipToSlot, bEquipping, 0, pFeedbackPlayer);

    std::string sResult = "";
    if (pCreature->m_bPlayerCharacter)
    {
        Arelith::PushEventData("WEAPON_OBJECT_ID", Utils::ObjectIDToString(pItem->m_idSelf)); //oidWeapon
        Arelith::PushEventData("CANEQUIPWEAPON_RESULT", std::to_string((unsigned char)retVal)); //original result

        Arelith::SignalEvent("NWNX_ARELITH_CANEQUIPWEAPON", pCreature->m_idSelf, &sResult);
    }
    retVal = (sResult == "") ? retVal : (unsigned char)atoi(sResult.c_str());
    if (retVal) m_CanEquipWeaponHook->CallOriginal<unsigned char>(pCreature, pItem, nEquipToSlot, bEquipping, bDisplayFeedback, pFeedbackPlayer);
    return retVal;
}

unsigned char ArelithEvents::CanUnEquipWeaponHook( CNWSCreature *pCreature, CNWSItem *pItem)
{
    unsigned char retVal = m_CanUnEquipWeaponHook->CallOriginal<unsigned char>(pCreature, pItem);

    std::string sResult = "";
    if (pCreature->m_bPlayerCharacter)
    {
        Arelith::PushEventData("WEAPON_OBJECT_ID", Utils::ObjectIDToString(pItem->m_idSelf)); //oidWeapon
        Arelith::PushEventData("CANUNEQUIPWEAPON_RESULT", std::to_string((unsigned char)retVal)); //original result
 
        Arelith::SignalEvent("NWNX_ARELITH_CANUNEQUIPWEAPON", pCreature->m_idSelf, &sResult);
    }

    return (sResult == "") ? retVal : (unsigned char)atoi(sResult.c_str());
}

}
