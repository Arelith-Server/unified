#include "nwnx.hpp"
#include "API/CNWSCreature.hpp"
#include "API/CItemRepository.hpp"
#include "API/CNWSItem.hpp"

namespace Arelith {

using namespace NWNXLib;
using namespace NWNXLib::API;

NWNX_EXPORT ArgumentStack GetItemPosition(ArgumentStack&& args)
{    
    ArgumentStack stack;

    if (auto *pItem= Utils::PopItem(args))
    {
        Events::InsertArgument(stack, int32_t(pItem->m_nRepositoryPositionY));
        Events::InsertArgument(stack, int32_t(pItem->m_nRepositoryPositionX));

        return stack;
    }
    Events::InsertArgument(stack, int32_t(-1));
    Events::InsertArgument(stack, int32_t(-1));
    return stack;
}

NWNX_EXPORT ArgumentStack SetItemPosition(ArgumentStack&& args)
{
    if (auto *pCreature = Utils::PopCreature(args))
    {
        CItemRepository *pRepo;
        pRepo = pCreature->m_pcItemRepository;

        auto *pItem = Utils::PopItem(args);

        const auto nXPos = args.extract<int32_t>();
        const auto nYPos = args.extract<int32_t>();

        if (pRepo == nullptr || pItem == nullptr)
        {
            LOG_ERROR("Item or Object Repository not found.");
            return -1;
        }

        return pRepo->MoveItem(pItem, uint8_t(nXPos), uint8_t(nYPos));
    }
    return -1;
}

}