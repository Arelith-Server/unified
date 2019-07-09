#pragma once

#include "API/Types.hpp"
#include "Common.hpp"
#include "ViewPtr.hpp"
#include "Services/Hooks/Hooks.hpp"

namespace Tweaks {

class StripOVTFromNotVisibleObject
{
public:
    StripOVTFromNotVisibleObject(NWNXLib::ViewPtr<NWNXLib::Services::HooksProxy> hooker);

private:
    static uint32_t HookComputeUpdateRequired(NWNXLib::API::CNWSMessage* thisMessage, NWNXLib::API::CNWSPlayer* pPlayer, NWNXLib::API::CNWSObject* pGameObject, NWNXLib::API::CLastUpdateObject* pLastUpdateObject, int32_t bPlayerObject);
    
    static NWNXLib::Hooking::FunctionHook* pHookComputeUpdateRequired;
    
    bool CheckObjectVisibility(NWNXLib::API::CNWSPlayer* pPlayer, NWNXLib::API::CNWSObject* pGameObject);
};

}
