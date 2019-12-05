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

static NWNXLib::Hooking::FunctionHook* m_CanUseItemHook=nullptr;
static NWNXLib::Hooking::FunctionHook* m_CanEquipWeaponHook=nullptr;
static NWNXLib::Hooking::FunctionHook* m_CanUnEquipWeaponHook=nullptr;
static NWNXLib::Hooking::FunctionHook* m_OnApplyDisarmHook=nullptr;


ArelithEvents::ArelithEvents(ViewPtr<Services::HooksProxy> hooker)
{
    Arelith::InitOnFirstSubscribe("NWNX_ARELITH_.*", [hooker]() {
        hooker->RequestExclusiveHook<Functions::_ZN12CNWSCreature10CanUseItemEP8CNWSItemi, int32_t, CNWSCreature*, CNWSItem*, int32_t>(&CanUseItemHook);
        m_CanUseItemHook =  hooker->FindHookByAddress(Functions::_ZN12CNWSCreature10CanUseItemEP8CNWSItemi);
        hooker->RequestExclusiveHook<Functions::_ZN12CNWSCreature14CanEquipWeaponEP8CNWSItemPjiiP10CNWSPlayer, unsigned char, CNWSCreature*, CNWSItem*, int32_t*, int32_t, int32_t, CNWSPlayer*>(&CanEquipWeaponHook);
        m_CanEquipWeaponHook =  hooker->FindHookByAddress(Functions::_ZN12CNWSCreature14CanEquipWeaponEP8CNWSItemPjiiP10CNWSPlayer);
        hooker->RequestExclusiveHook<Functions::_ZN12CNWSCreature16CanUnEquipWeaponEP8CNWSItem, unsigned char, CNWSCreature*, CNWSItem*>(&CanUnEquipWeaponHook);
        m_CanUnEquipWeaponHook =  hooker->FindHookByAddress(Functions::_ZN12CNWSCreature16CanUnEquipWeaponEP8CNWSItem);
        hooker->RequestExclusiveHook<Functions::_ZN21CNWSEffectListHandler13OnApplyDisarmEP10CNWSObjectP11CGameEffecti, int32_t,CNWSEffectListHandler*, CNWSObject*, CGameEffect*, int32_t>(&OnApplyDisarmHook);
        m_OnApplyDisarmHook =  hooker->FindHookByAddress(Functions::_ZN21CNWSEffectListHandler13OnApplyDisarmEP10CNWSObjectP11CGameEffecti);
    });
}

int32_t ArelithEvents::CanUseItemHook( CNWSCreature *pCreature, CNWSItem *pItem, int32_t bIgnoreIdentifiedFlag)
{
    int32_t retVal = m_CanUseItemHook->CallOriginal<int32_t>(pCreature, pItem, bIgnoreIdentifiedFlag);

    std::string sResult = "";
    if (pCreature->m_bPlayerCharacter)
    {
        Arelith::PushEventData("ITEM_OBJECT_ID", Utils::ObjectIDToString(pItem->m_idSelf)); //oidItem
        Arelith::PushEventData("CANUSEITEM_RESULT", std::to_string(retVal)); //original result

        Arelith::SignalEvent("NWNX_ARELITH_CANUSEITEM", pCreature->m_idSelf, &sResult);
    }

    return (sResult == "") ? retVal : atoi(sResult.c_str());
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




int32_t ArelithEvents::OnApplyDisarmHook(CNWSEffectListHandler*, CNWSObject *pObject, CGameEffect *pEffect, int32_t bLoadingGame)
{
	CNWSCreature *pCreature = Utils::AsNWSCreature(pObject);
	CNWSCreature *pDisarmingCreature;
    
	if ( pCreature )
	{

		if ( !pCreature->m_bDisarmable ||
		        pCreature->GetArea() == NULL )
		{
			return 1;
		}

		if ( pObject->GetDead() && !bLoadingGame )
		{
			return 1;
		}
        //do we need the disarmer?
		pDisarmingCreature = API::Globals::AppManager()->m_pServerExoApp->GetCreatureByGameObjectID(pEffect->m_oidCreator);
		
        Arelith::PushEventData("TARGET_OBJECT_ID", Utils::ObjectIDToString(pCreature->m_idSelf)); //oidDisarmee
        Arelith::PushEventData("DISARMER_OBJECT_ID", Utils::ObjectIDToString((pDisarmingCreature) ? pDisarmingCreature->m_idSelf : API::Constants::OBJECT_INVALID)); //oidDisarmer

        Arelith::SignalEvent("NWNX_ARELITH_ONDISARM", pCreature->m_idSelf, NULL);
	 }

	return 1;
}


}
