#pragma once

#include "API/Vector.hpp"
#include "Common.hpp"
#include "Services/Hooks/Hooks.hpp"
//#include "ViewPtr.hpp"

namespace Arelith {

class ArelithEvents
{
public:
    ArelithEvents(NWNXLib::Services::HooksProxy* hooker);

private:
    static unsigned char CanEquipWeaponHook( CNWSCreature *pCreature, CNWSItem *pItem, int32_t *nEquipToSlot, int32_t bEquipping, int32_t, CNWSPlayer *pFeedbackPlayer);
    static unsigned char CanUnEquipWeaponHook( CNWSCreature *pCreature, CNWSItem *pItem);
    static int32_t OnApplyDisarmHook(CNWSEffectListHandler*, CNWSObject *pObject, CGameEffect *pEffect, int32_t bLoadingGame);
};

}
