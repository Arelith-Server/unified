#pragma once

#include "nwnx.hpp"
//#include "ViewPtr.hpp"

namespace Arelith {

class ArelithEvents
{
public:
    ArelithEvents();

private:
    static unsigned char CanEquipWeaponHook( CNWSCreature *pCreature, CNWSItem *pItem, int32_t *nEquipToSlot, int32_t bEquipping, int32_t, CNWSPlayer *pFeedbackPlayer);
    static unsigned char CanUnEquipWeaponHook( CNWSCreature *pCreature, CNWSItem *pItem);
};

}
