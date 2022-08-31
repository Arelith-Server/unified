#include "nwnx.hpp"

#include "API/CNWSItem.hpp"
#include "API/CNWSStore.hpp"

namespace Arelith {

using namespace NWNXLib;
using namespace NWNXLib::API;
using namespace NWNXLib::API::Constants;

static Hooks::Hook s_getItemAppearanceHook;

int32_t CalculateItemBuyPrice(CNWSStore* thisPtr, CNWSItem* pItem, OBJECT_ID oidSeller)
{
    if ( pItem->m_bPlotObject )
    {
        return 0;
    }

    int32_t nItemPrice = pItem->GetCost();

    if (nItemPrice == 0)
    {
        return 0;
    }

    double dItemPrice = (float)nItemPrice * (int32_t)(thisPtr->GetCustomerBuyRate( oidSeller, pItem->m_bStolen ))/100.0;
    nItemPrice = (int32_t)dItemPrice;

    if(thisPtr->m_iMaxBuyPrice != -1)
    {
        nItemPrice = std::min(nItemPrice, thisPtr->m_iMaxBuyPrice * std::max(1, pItem->m_nStackSize));
    }

    return std::max(nItemPrice,1);
}

void MaxStorePricePerStack() __attribute__((constructor));

void MaxStorePricePerStack()
{
    static Hooks::Hook _0 = Hooks::HookFunction(
        API::Functions::_ZN9CNWSStore21CalculateItemBuyPriceEP8CNWSItemj,
        (void*)&CalculateItemBuyPrice,
        Hooks::Order::Earliest);
}

}