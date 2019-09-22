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

};

}
