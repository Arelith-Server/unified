#include "nwnx.hpp"

#include "API/CNWSArea.hpp"

namespace Arelith {

using namespace NWNXLib;
using namespace NWNXLib::API;
using namespace NWNXLib::API::Constants;

static Hooks::Hook s_budgeCreaturesHook;

void BudgeCreaturesHook(CNWSArea* thisPtr, const Vector *vPosition, const Vector *vBBMin, const Vector *vBBMax, OBJECT_ID oidNewObject, BOOL bBumpToActionPoint)
{
    CGameObject* obj = Utils::GetGameObject(oidNewObject);
    if (obj && Utils::AsNWSDoor(obj))
    {
        return; // skip door budges
    }

    return s_budgeCreaturesHook->CallOriginal<void>(thisPtr, vPosition, vBBMin, vBBMax, oidNewObject, bBumpToActionPoint);
}

void DoorsNoBudge() __attribute__((constructor));

void DoorsNoBudge()
{
    s_budgeCreaturesHook = Hooks::HookFunction(&CNWSArea::BudgeCreatures, &BudgeCreaturesHook, Hooks::Order::Earliest);
}

}