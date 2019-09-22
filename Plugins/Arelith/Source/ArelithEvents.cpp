#include "Source/ArelithEvents.hpp"
#include "API/CNWSCreature.hpp"
#include "API/CNWSItem.hpp"
#include "API/Functions.hpp"
#include "API/CNWSObject.hpp"
#include "Plugin.hpp"
#include "Arelith.hpp"
#include "Utils.hpp"

namespace Arelith {

using namespace NWNXLib;

static NWNXLib::Hooking::FunctionHook* m_CanUseItemHook=nullptr;

ArelithEvents::ArelithEvents(ViewPtr<Services::HooksProxy> hooker)
{
    Arelith::InitOnFirstSubscribe("NWNX_ARELITH_*", [hooker]() {
        hooker->RequestExclusiveHook<API::Functions::CNWSCreature__CanUseItem, int32_t, API::CNWSCreature*, API::CNWSItem*, int32_t>(&CanUseItemHook);
        m_CanUseItemHook =  hooker->FindHookByAddress(API::Functions::CNWSCreature__CanUseItem);
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

        Arelith::SignalEvent("NWNX_ARELITH_CANUSEITEM", pCreature->m_idSelf, &sResult);
    }

    return (sResult == "") ? retVal : atoi(sResult.c_str());
}

}
