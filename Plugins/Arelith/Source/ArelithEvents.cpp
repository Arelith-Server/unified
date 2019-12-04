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

namespace Arelith {

using namespace NWNXLib;
using namespace NWNXLib::API::Constants;

static NWNXLib::Hooking::FunctionHook* m_CanUseItemHook=nullptr;
static NWNXLib::Hooking::FunctionHook* m_CanEquipWeaponHook=nullptr;
static NWNXLib::Hooking::FunctionHook* m_CanUnEquipWeaponHook=nullptr;
static NWNXLib::Hooking::FunctionHook* m_OnApplyDisarmHook=nullptr;
static NWNXLib::Hooking::FunctionHook* m_OnEffectAppliedHook=nullptr;


ArelithEvents::ArelithEvents(ViewPtr<Services::HooksProxy> hooker)
{
    Arelith::InitOnFirstSubscribe("NWNX_ARELITH_.*", [hooker]() {
        hooker->RequestExclusiveHook<API::Functions::CNWSCreature__CanUseItem, int32_t, API::CNWSCreature*, API::CNWSItem*, int32_t>(&CanUseItemHook);
        m_CanUseItemHook =  hooker->FindHookByAddress(API::Functions::CNWSCreature__CanUseItem);
        hooker->RequestExclusiveHook<API::Functions::CNWSCreature__CanEquipWeapon, unsigned char, API::CNWSCreature*, API::CNWSItem*, int32_t*, int32_t, int32_t, API::CNWSPlayer*>(&CanEquipWeaponHook);
        m_CanEquipWeaponHook =  hooker->FindHookByAddress(API::Functions::CNWSCreature__CanEquipWeapon);
        hooker->RequestExclusiveHook<API::Functions::CNWSCreature__CanUnEquipWeapon, unsigned char, API::CNWSCreature*, API::CNWSItem*>(&CanUnEquipWeaponHook);
        m_CanUnEquipWeaponHook =  hooker->FindHookByAddress(API::Functions::CNWSCreature__CanUnEquipWeapon);
        hooker->RequestExclusiveHook<API::Functions::CNWSEffectListHandler__OnApplyDisarm, int32_t,API::CNWSEffectListHandler*, API::CNWSObject*, API::CGameEffect*, int32_t>(&OnApplyDisarmHook);
        m_OnApplyDisarmHook =  hooker->FindHookByAddress(API::Functions::CNWSEffectListHandler__OnApplyDisarm);
        hooker->RequestExclusiveHook<API::Functions::CNWSEffectListHandler__OnEffectApplied, int32_t,API::CNWSEffectListHandler*, API::CNWSObject*, API::CGameEffect*, int32_t>(&OnEffectAppliedHook);
        m_OnEffectAppliedHook =  hooker->FindHookByAddress(API::Functions::CNWSEffectListHandler__OnEffectApplied);
    });
}

int32_t ArelithEvents::CanUseItemHook( NWNXLib::API::CNWSCreature *pCreature, NWNXLib::API::CNWSItem *pItem, int32_t bIgnoreIdentifiedFlag)
{
    int32_t retVal = m_CanUseItemHook->CallOriginal<int32_t>(pCreature, pItem, bIgnoreIdentifiedFlag);

    std::string sResult = "";
    if (pCreature->m_bPlayerCharacter)
    {
        Arelith::PushEventData("ITEM_OBJECT_ID", Utils::ObjectIDToString(pItem->m_idSelf)); //oidItem
        Arelith::PushEventData("CANUSEITEM_RESULT", std::to_string(retVal)); //original result

        Arelith::SignalEvent("NWNX_ARELITH_CAN_USE_ITEM", pCreature->m_idSelf, &sResult);
    }

    return (sResult == "") ? retVal : atoi(sResult.c_str());
}

unsigned char ArelithEvents::CanEquipWeaponHook( NWNXLib::API::CNWSCreature *pCreature, NWNXLib::API::CNWSItem *pItem, int32_t *nEquipToSlot, int32_t bEquipping, int32_t bDisplayFeedback, NWNXLib::API::CNWSPlayer *pFeedbackPlayer)
{
    unsigned char retVal = m_CanEquipWeaponHook->CallOriginal<unsigned char>(pCreature, pItem, nEquipToSlot, bEquipping, 0, pFeedbackPlayer);

    std::string sResult = "";
    if (pCreature->m_bPlayerCharacter)
    {
        Arelith::PushEventData("WEAPON_OBJECT_ID", Utils::ObjectIDToString(pItem->m_idSelf)); //oidWeapon
        Arelith::PushEventData("CANEQUIPWEAPON_RESULT", std::to_string((unsigned char)retVal)); //original result

        Arelith::SignalEvent("NWNX_ARELITH_CAN_EQUIP_WEAPON", pCreature->m_idSelf, &sResult);
    }
    retVal = (sResult == "") ? retVal : (unsigned char)atoi(sResult.c_str());
    if (retVal) m_CanEquipWeaponHook->CallOriginal<unsigned char>(pCreature, pItem, nEquipToSlot, bEquipping, bDisplayFeedback, pFeedbackPlayer);
    return retVal;
}

unsigned char ArelithEvents::CanUnEquipWeaponHook( NWNXLib::API::CNWSCreature *pCreature, NWNXLib::API::CNWSItem *pItem)
{
    unsigned char retVal = m_CanUnEquipWeaponHook->CallOriginal<unsigned char>(pCreature, pItem);

    std::string sResult = "";
    if (pCreature->m_bPlayerCharacter)
    {
        Arelith::PushEventData("WEAPON_OBJECT_ID", Utils::ObjectIDToString(pItem->m_idSelf)); //oidWeapon
        Arelith::PushEventData("CANUNEQUIPWEAPON_RESULT", std::to_string((unsigned char)retVal)); //original result
 
        Arelith::SignalEvent("NWNX_ARELITH_CAN_UNEQUIP_WEAPON", pCreature->m_idSelf, &sResult);
    }

    return (sResult == "") ? retVal : (unsigned char)atoi(sResult.c_str());
}




int32_t ArelithEvents::OnApplyDisarmHook(NWNXLib::API::CNWSEffectListHandler*, NWNXLib::API::CNWSObject *pObject, NWNXLib::API::CGameEffect *pEffect, int32_t bLoadingGame)
{
	NWNXLib::API::CNWSCreature *pCreature = Utils::AsNWSCreature(pObject);
	NWNXLib::API::CNWSCreature *pDisarmingCreature;
    
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

        Arelith::SignalEvent("NWNX_ARELITH_ON_DISARM", pCreature->m_idSelf, NULL);
	 }

	return 1;
}



int32_t ArelithEvents::OnEffectAppliedHook(NWNXLib::API::CNWSEffectListHandler *pEffectListHandler, NWNXLib::API::CNWSObject *pObject, NWNXLib::API::CGameEffect *pEffect, int32_t bLoadingGame)
{
	if (pEffect->m_nType == EffectTrueType::ItemProperty || !Utils::AsNWSCreature(pObject)) 
    {
        return m_OnEffectAppliedHook->CallOriginal<int32_t>(pEffectListHandler, pObject, pEffect, bLoadingGame);
    }

    Arelith::PushEventData("UNIQUE_ID", std::to_string(pEffect->m_nID));
    Arelith::PushEventData("CREATOR", Utils::ObjectIDToString(pEffect->m_oidCreator));
    Arelith::PushEventData("TYPE", std::to_string(pEffect->m_nType));
    Arelith::PushEventData("SUB_TYPE", std::to_string(pEffect->m_nSubType & EffectSubType::MASK));
    Arelith::PushEventData("DURATION_TYPE", std::to_string(pEffect->m_nSubType & EffectDurationType::MASK));
    Arelith::PushEventData("DURATION", std::to_string(pEffect->m_fDuration));
    Arelith::PushEventData("SPELL_ID", std::to_string(pEffect->m_nSpellId));
    Arelith::PushEventData("CASTER_LEVEL", std::to_string(pEffect->m_nCasterLevel));
    Arelith::PushEventData("CUSTOM_TAG", pEffect->m_sCustomTag.CStr());

    for (int i = 0; i < pEffect->m_nNumIntegers; i++)
    {// Int Params
        Arelith::PushEventData("INT_PARAM_" + std::to_string(i + 1), std::to_string(pEffect->m_nParamInteger[i]));
    }

    for(int i = 0; i < 4; i++)
    {// Float Params
        Arelith::PushEventData("FLOAT_PARAM_" + std::to_string(i + 1), std::to_string(pEffect->m_nParamFloat[i]));
    }

    for(int i = 0; i < 6; i++)
    {// String Params
        Arelith::PushEventData("STRING_PARAM_" + std::to_string(i + 1), pEffect->m_sParamString[i].CStr());
    }

    for(int i = 0; i < 4; i++)
    {// Object Params
        Arelith::PushEventData("OBJECT_PARAM_" + std::to_string(i + 1), Utils::ObjectIDToString(pEffect->m_oidParamObjectID[i]));
    }

    if (Arelith::SignalEvent("NWNX_ARELITH_ON_EFFECT_APPLIED", pObject->m_idSelf))
    {
        return m_OnEffectAppliedHook->CallOriginal<int32_t>(pEffectListHandler, pObject, pEffect, bLoadingGame);
    }

	return 0;
}

}