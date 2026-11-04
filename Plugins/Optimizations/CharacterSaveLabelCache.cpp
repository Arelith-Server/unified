#include "nwnx.hpp"

#include "API/CNWSPlayer.hpp"
#include "API/CNWSCreature.hpp"
#include "API/CResGFF.hpp"
#include "API/CResGFFFileHeader.hpp"
#include "API/CResGFFLabel.hpp"

#include <cstring>
#include <unordered_map>

namespace Optimizations {

using namespace NWNXLib;
using namespace NWNXLib::API;

// Every field written to a GFF goes through CResGFF::AddLabel, which looks the field label up with a linear
// scan of the GFF's label table. A character file has a few hundred distinct labels and tens of thousands of
// fields (item local variables and item properties), so the scans are a large share of SaveServerCharacter.
//
// While a creature is being saved (CNWSPlayer::SaveServerCharacter, CNWSCreature::SaveCreature, including
// anything that runs inside them), this keeps a hash index of the label table of:
//  - every GFF created with CreateGFFFile during the save (the character file, and any GFF that something
//    called from inside the save creates, e.g. a nested object serialisation), and
//  - the GFF passed to SaveCreature.
// AddLabel on one of those GFFs is answered from its index; AddLabel on any other GFF goes to the engine.
// The index is filled in table order, so for a label that appears more than once it holds the highest index,
// the one the engine's backwards scan returns; a hit is also checked against the table entry. Labels not in the
// index go through the engine's AddLabel, which scans and appends them exactly as before, so the label table,
// and with it the saved file, is byte-for-byte unchanged.
//
// An index is dropped when its GFF gets a new label table: CreateGFFFile (which starts a fresh, empty index),
// InitializeForWriting, or loading data with GetDataFromPointer (GetDataFromFile calls it). A GFF obtained from a
// resource demand is read-only; the engine only writes to it after InitializeForWriting. So a GFF freed during the
// save and a new one at the same address can never share an index. All indexes are dropped when the outermost save
// returns. (OnResourceServiced, which every load ends in, cannot be hooked: its first instructions cannot be
// relocated by the hook library.)
//
// Main thread only: the engine saves characters and creatures on the main thread, and this state is not
// synchronised.

namespace {

struct LabelKey
{
    uint64_t a, b;
    bool operator==(const LabelKey& o) const { return a == o.a && b == o.b; }
};
struct LabelKeyHash
{
    size_t operator()(const LabelKey& k) const { return std::hash<uint64_t>()(k.a * 0x9E3779B97F4A7C15ull ^ k.b); }
};
struct LabelIndex
{
    uint32_t nIndexed = 0;   // label table entries [0, nIndexed) are in the map
    std::unordered_map<LabelKey, uint32_t, LabelKeyHash> map;
};

}

static Hooks::Hook s_SaveServerCharacterHook;
static Hooks::Hook s_SaveCreatureHook;
static Hooks::Hook s_CreateGFFFileHook;
static Hooks::Hook s_InitializeForWritingHook;
static Hooks::Hook s_GetDataFromPointerHook;
static Hooks::Hook s_AddLabelHook;

static int s_nSaveDepth;
// GFFs indexed during the current save, normally one or two. Node-based, so a LabelIndex* stays valid until
// that entry is erased. s_pLast/s_pLastIndex cache the most recent lookup, which is nearly every AddLabel call.
static std::unordered_map<CResGFF*, LabelIndex> s_indexes;
static CResGFF *s_pLast;
static LabelIndex *s_pLastIndex;

static LabelIndex* FindIndex(CResGFF *pGff)
{
    if (pGff == s_pLast)
        return s_pLastIndex;
    auto it = s_indexes.find(pGff);
    s_pLast = pGff;
    s_pLastIndex = it == s_indexes.end() ? nullptr : &it->second;
    return s_pLastIndex;
}

static void DropIndex(CResGFF *pGff)
{
    s_indexes.erase(pGff);
    if (pGff == s_pLast)
        s_pLastIndex = nullptr;
}

static LabelIndex* StartIndex(CResGFF *pGff)
{
    auto& idx = s_indexes[pGff];
    idx.nIndexed = 0;
    idx.map.clear();
    if (idx.map.bucket_count() < 512)
        idx.map.reserve(512);
    s_pLast = pGff;
    s_pLastIndex = &idx;
    return &idx;
}

static void LeaveSave()
{
    if (--s_nSaveDepth == 0)
    {
        s_indexes.clear();
        s_pLast = nullptr;
        s_pLastIndex = nullptr;
    }
}

static BOOL SaveServerCharacter(CNWSPlayer *pThis, BOOL bBackupPlayer)
{
    s_nSaveDepth++;
    auto ret = s_SaveServerCharacterHook->CallOriginal<BOOL>(pThis, bBackupPlayer);
    LeaveSave();
    return ret;
}

static BOOL SaveCreature(CNWSCreature *pThis, CResGFF *pRes, CResStruct *pStruct, BOOL bStoreAssociateList,
                         BOOL bUseDesiredAreaInfo, BOOL bExportingChar, BOOL bSaveOIDs)
{
    s_nSaveDepth++;
    if (pRes && !FindIndex(pRes))
        StartIndex(pRes);   // filled from the existing table on the first AddLabel
    auto ret = s_SaveCreatureHook->CallOriginal<BOOL>(pThis, pRes, pStruct, bStoreAssociateList, bUseDesiredAreaInfo,
                                                      bExportingChar, bSaveOIDs);
    LeaveSave();
    return ret;
}

static BOOL CreateGFFFile(CResGFF *pThis, CResStruct *pStruct, const CExoString& sFileType, const CExoString& sVersion)
{
    auto ret = s_CreateGFFFileHook->CallOriginal<BOOL>(pThis, pStruct, sFileType, sVersion);
    if (s_nSaveDepth > 0)
        StartIndex(pThis);
    return ret;
}

static void InitializeForWriting(CResGFF *pThis)
{
    if (!s_indexes.empty())
        DropIndex(pThis);
    s_InitializeForWritingHook->CallOriginal<void>(pThis);
}

static BOOL GetDataFromPointer(CResGFF *pThis, void *pData, int32_t nDataSize, bool bTakeOwnership)
{
    if (!s_indexes.empty())
        DropIndex(pThis);
    return s_GetDataFromPointerHook->CallOriginal<BOOL>(pThis, pData, nDataSize, bTakeOwnership);
}

static uint32_t AddLabel(CResGFF *pThis, const char *szFieldID)
{
    LabelIndex *idx;
    if (s_nSaveDepth == 0 || !pThis->m_pFileHeader || !pThis->m_pLabel || !(idx = FindIndex(pThis)))
        return s_AddLabelHook->CallOriginal<uint32_t>(pThis, szFieldID);

    const uint32_t nCount = pThis->m_pFileHeader->m_nLabelCount;
    if (nCount < idx->nIndexed)
    {
        idx->map.clear();
        idx->nIndexed = 0;
    }
    for (uint32_t i = idx->nIndexed; i < nCount; i++)
    {
        LabelKey k;
        std::memcpy(&k, pThis->m_pLabel[i].m_pLabel, sizeof(k));
        idx->map[k] = i;   // ascending, so a duplicate keeps the highest index, as the engine's backwards scan finds
    }
    idx->nIndexed = nCount;

    LabelKey key = {0, 0};
    // Same as the engine's strncpy into a zeroed 16-byte buffer: up to 16 characters, no terminator needed.
    auto *pKey = reinterpret_cast<char*>(&key);
    for (size_t i = 0; i < sizeof(key) && szFieldID[i]; i++)
        pKey[i] = szFieldID[i];
    auto it = idx->map.find(key);
    if (it != idx->map.end() && it->second < nCount &&
        std::memcmp(pThis->m_pLabel[it->second].m_pLabel, &key, sizeof(key)) == 0)
        return it->second;

    const uint32_t ret = s_AddLabelHook->CallOriginal<uint32_t>(pThis, szFieldID);
    if (ret != 0xFFFFFFFF && pThis->m_pFileHeader)
    {
        idx->map[key] = ret;
        if (ret == idx->nIndexed && pThis->m_pFileHeader->m_nLabelCount == ret + 1)
            idx->nIndexed = ret + 1;
    }
    return ret;
}

void CharacterSaveLabelCache() __attribute__((constructor));
void CharacterSaveLabelCache()
{
    if (Config::Get<bool>("CHARACTER_SAVE_LABEL_CACHE", false))
    {
        LOG_INFO("Creature and character saves will look up GFF field labels through a hash index");
        s_SaveServerCharacterHook = Hooks::HookFunction(&CNWSPlayer::SaveServerCharacter, &SaveServerCharacter, Hooks::Order::Earliest);
        s_SaveCreatureHook = Hooks::HookFunction(&CNWSCreature::SaveCreature, &SaveCreature, Hooks::Order::Earliest);
        s_CreateGFFFileHook = Hooks::HookFunction(&CResGFF::CreateGFFFile, &CreateGFFFile, Hooks::Order::Earliest);
        s_InitializeForWritingHook = Hooks::HookFunction(&CResGFF::InitializeForWriting, &InitializeForWriting, Hooks::Order::Earliest);
        s_GetDataFromPointerHook = Hooks::HookFunction(&CResGFF::GetDataFromPointer, &GetDataFromPointer, Hooks::Order::Earliest);
        s_AddLabelHook = Hooks::HookFunction(&CResGFF::AddLabel, &AddLabel, Hooks::Order::Earliest);
    }
}

}
