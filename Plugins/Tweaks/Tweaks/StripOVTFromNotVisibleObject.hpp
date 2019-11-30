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
    static uint32_t HookComputeUpdateRequired(CNWSMessage* thisMessage, CNWSPlayer* pPlayer, CNWSObject* pGameObject, CLastUpdateObject* pLastUpdateObject, int32_t bPlayerObject);
    
    static bool CheckObjectVisibility(CNWSPlayer* pPlayer, CNWSObject* pGameObject);
};

}
