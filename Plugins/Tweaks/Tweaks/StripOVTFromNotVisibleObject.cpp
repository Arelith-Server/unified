#include "Tweaks/StripOVTFromNotVisibleObject.hpp"

#include "Services/Hooks/Hooks.hpp"
#include "Utils.hpp"

#include "API/CAppManager.hpp"
#include "API/CServerExoAppInternal.hpp"
#include "API/CServerExoApp.hpp"
#include "API/CExoString.hpp"
#include "API/CNWSCreature.hpp"
#include "API/CNWSCreatureStats.hpp"
#include "API/CNWSPlayer.hpp"
#include "API/Functions.hpp"
#include "API/Globals.hpp"

#define UPDATE_OBJECT_VISUAL_TRANSFORM_FLAG 0x00100000

namespace Tweaks {

using namespace NWNXLib;
using namespace NWNXLib::API;

static NWNXLib::Hooking::FunctionHook* pHookComputeUpdateRequired = nullptr;

StripOVTFromNotVisibleObject::StripOVTFromNotVisibleObject(ViewPtr<Services::HooksProxy> hooker)
{
    hooker->RequestExclusiveHook<Functions::_ZN11CNWSMessage21ComputeUpdateRequiredEP10CNWSPlayerP10CNWSObjectP17CLastUpdateObjecti>
                                    (&HookComputeUpdateRequired);

    pHookComputeUpdateRequired = hooker->FindHookByAddress(Functions::_ZN11CNWSMessage21ComputeUpdateRequiredEP10CNWSPlayerP10CNWSObjectP17CLastUpdateObjecti);
}

uint32_t StripOVTFromNotVisibleObject::HookComputeUpdateRequired(CNWSMessage* thisMessage, CNWSPlayer* pPlayer, CNWSObject* pGameObject, CLastUpdateObject* pLastUpdateObject, int32_t bPlayerObject)
{
    auto CURFlags = pHookComputeUpdateRequired->CallOriginal<uint32_t>(thisMessage, pPlayer, pGameObject, pLastUpdateObject, bPlayerObject);

    if (CURFlags & UPDATE_OBJECT_VISUAL_TRANSFORM_FLAG && pGameObject->m_idSelf != pPlayer->GetGameObject()->m_idSelf && !CheckObjectVisibility(pPlayer, pGameObject))
    {
        CURFlags -= UPDATE_OBJECT_VISUAL_TRANSFORM_FLAG;
    }

    return CURFlags;
}

bool StripOVTFromNotVisibleObject::CheckObjectVisibility(CNWSPlayer* pPlayer, CNWSObject* pGameObject)
{
    if (!pPlayer || !pGameObject)
    {
        return false;
    }
    
    auto pPlayerGameObject = pPlayer->GetGameObject();
    
    if (!pPlayerGameObject)
    {
        return false;
    }
    
    auto pPlayerCreature = pPlayerGameObject->AsNWSCreature();
    
    if (!pPlayerCreature)
    {
        return false;
    }
    
    
    if (pPlayerCreature->GetArea() != pGameObject->GetArea())
    {
        return false;
    }
    
    auto pVisibleElement = pPlayerCreature->GetVisibleListElement(pGameObject->m_idSelf);
    
    if (!pVisibleElement)
    {
        return false;
    }
    
    return true;
}

}
