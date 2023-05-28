#include "nwnx.hpp"

#include "API/CAppManager.hpp"
#include "API/CVirtualMachine.hpp"
#include "API/CNWSItem.hpp"
#include "API/CNWVirtualMachineCommands.hpp"
#include "API/CServerExoApp.hpp"
#include "API/Globals.hpp"

namespace Arelith {

using namespace NWNXLib;
using namespace NWNXLib::API;
using namespace NWNXLib::API::Constants;

static Hooks::Hook s_getItemAppearanceHook;

// Allows GetItemAppearance to work with per part colours.
int32_t ExecuteCommandGetItemAppearanceHook(CNWVirtualMachineCommands* thisPtr, int32_t nCommandId, int32_t nParameters)
{
    OBJECT_ID oidObject;
    int32_t iType, iIndex;

    if (!Globals::VirtualMachine()->StackPopObject(&oidObject) ||
        !Globals::VirtualMachine()->StackPopInteger(&iType) ||
        !Globals::VirtualMachine()->StackPopInteger(&iIndex))
    {
        return -639;
    }

    if (iType == 4 /* armour colour */ && iIndex >= 6 /* per part */)
    {
        if (CNWSItem* pItem = Globals::AppManager()->m_pServerExoApp->GetItemByGameObjectID(oidObject))
        {
            iIndex -= 6;
            int32_t color = iIndex % 6;
            int32_t part = iIndex / 6;
            return Globals::VirtualMachine()->StackPushInteger(pItem->GetLayeredTextureColorPerPart(color, part)) ? 0 : -638;
        }
    }

    // Push the vars back, then call original.
    Globals::VirtualMachine()->StackPushInteger(iIndex);
    Globals::VirtualMachine()->StackPushInteger(iType);
    Globals::VirtualMachine()->StackPushObject(oidObject);

    return s_getItemAppearanceHook->CallOriginal<int32_t>(thisPtr, nCommandId, nParameters);
}

void GetItemAppearance() __attribute__((constructor));

void GetItemAppearance()
{
    s_getItemAppearanceHook = Hooks::HookFunction(
        API::Functions::_ZN25CNWVirtualMachineCommands31ExecuteCommandGetItemAppearanceEii,
        (void*)&ExecuteCommandGetItemAppearanceHook, 
        Hooks::Order::Earliest);
}

}