#pragma once

#include "API/Types.hpp"
#include "API/Vector.hpp"
#include "Common.hpp"
#include "Services/Hooks/Hooks.hpp"
#include "ViewPtr.hpp"

namespace Arelith {

class ArelithEvents
{
public:
    ArelithEvents(NWNXLib::ViewPtr<NWNXLib::Services::HooksProxy> hooker);

private:
    static int32_t CanUseItemHook( NWNXLib::API::CNWSCreature *pCreature, NWNXLib::API::CNWSItem *pItem, int32_t bIgnoreIdentifiedFlag);
    static unsigned char CanEquipWeaponHook( NWNXLib::API::CNWSCreature *pCreature, NWNXLib::API::CNWSItem *pItem, int32_t *nEquipToSlot, int32_t bEquipping, int32_t, NWNXLib::API::CNWSPlayer *pFeedbackPlayer);
    static unsigned char CanUnEquipWeaponHook( NWNXLib::API::CNWSCreature *pCreature, NWNXLib::API::CNWSItem *pItem);
    static int32_t OnApplyDisarmHook( NWNXLib::API::CNWSObject *pObject, NWNXLib::API::CGameEffect *pEffect, int32_t bLoadingGame);
};

}
