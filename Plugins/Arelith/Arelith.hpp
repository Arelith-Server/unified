#pragma once

#include "Plugin.hpp"
#include "Services/Events/Events.hpp"
#include "Services/Hooks/Hooks.hpp"
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

using ArgumentStack = NWNXLib::Services::Events::ArgumentStack;
struct bypassRed
{
    uint16_t m_nPropertyName;
    int32_t m_nSubType;
    int32_t m_nCostTableValue;
    int32_t m_nParam1Value;
    bool bReverse;
};
namespace Arelith {

class ArelithEvents;

class Arelith : public NWNXLib::Plugin
{
public: // Structures
    struct EventParams
    {
        // This maps between event data key -> event data value.
        std::unordered_map<std::string, std::string> m_EventDataMap;

        // This is true if SkipEvent() has been called on this event during its execution.
        bool m_Skipped;

        // The result of the event, if any, is stored here
        std::string m_Result;

        // The current event name
        std::string m_EventName;
    };

public:
    Arelith(NWNXLib::Services::ProxyServiceList* services);
    virtual ~Arelith();

    // Pushes event data to the stack - won't do anything until SignalEvent is called.
    static void PushEventData(const std::string& tag, const std::string& data);

    // Get event data
    static std::string GetEventData(const std::string& tag);

    // Returns true if the event can proceed, or false if the event has been skipped.
    static bool SignalEvent(const std::string& eventName, const ObjectID target, std::string *result=nullptr);

    static void InitOnFirstSubscribe(const std::string& eventName, std::function<void(void)> init);



private: // Structures
    using EventMapType = std::unordered_map<std::string, std::vector<std::string>>;

private:
    NWNXLib::Hooking::FunctionHook* m_GetEffectImmunityHook;
    NWNXLib::Services::Events::ArgumentStack OnSubscribeEvent(NWNXLib::Services::Events::ArgumentStack&& args);
    NWNXLib::Services::Events::ArgumentStack OnPushEventData(NWNXLib::Services::Events::ArgumentStack&& args);
    NWNXLib::Services::Events::ArgumentStack OnSignalEvent(NWNXLib::Services::Events::ArgumentStack&& args);
    NWNXLib::Services::Events::ArgumentStack OnGetEventData(NWNXLib::Services::Events::ArgumentStack&& args);
    NWNXLib::Services::Events::ArgumentStack OnSkipEvent(NWNXLib::Services::Events::ArgumentStack&& args);
    NWNXLib::Services::Events::ArgumentStack OnEventResult(NWNXLib::Services::Events::ArgumentStack&& args);
    NWNXLib::Services::Events::ArgumentStack OnGetCurrentEvent(NWNXLib::Services::Events::ArgumentStack&& args);
    ArgumentStack GetWeaponPower(ArgumentStack&& args);
    ArgumentStack GetAttackModifierVersus(ArgumentStack&& args);
    ArgumentStack GetArmorClassVersus(ArgumentStack&& args);
    ArgumentStack ResolveDefensiveEffects(ArgumentStack&& args);
    ArgumentStack GetActiveProperty(ArgumentStack&& args);
    ArgumentStack SetLastItemCasterLevel(ArgumentStack&& args);
    ArgumentStack GetLastItemCasterLevel(ArgumentStack&& args);
    ArgumentStack SetDamageReductionBypass(ArgumentStack&& args);
    ArgumentStack SetEffectImmunityBypass(ArgumentStack&& args);
    CNWSCreature *creature(ArgumentStack& args);
    static void ReportErrorHook(bool, CNWVirtualMachineCommands*, CExoString, int32_t);
    static void WriteToLogFileHook(bool, CExoDebugInternal*, CExoString*);
    static bool s_bSendError;
    static void SendWebHookHTTPS(const char* message);
    static std::string s_sHost;
    static std::string s_sOrigPath;
    static std::string s_sAdden;
    static uint8_t s_iMaterial;
    ArgumentStack SetWebhook(ArgumentStack&& args);
    static void OnItemPropertyAppliedHook(bool, CServerAIMaster*, CNWSItem*, CNWItemProperty*, CNWSCreature*, uint32_t, BOOL);
    static void OnApplyDamageReductionHook(bool, CNWSEffectListHandler*, CNWSObject*, CGameEffect*, BOOL);
    static void DoDamageReductionHook(bool, CNWSObject*, CNWSCreature*, int32_t, uint8_t, BOOL, BOOL);
    static BOOL GetEffectImmunityHook(CNWSCreatureStats *pStats, uint8_t nType, CNWSCreature * pVersus, BOOL bConsiderFeats = true);
    // Pushes a brand new event data onto the event data stack, set up with the correct defaults.
    // Only does it if needed though, based on the current event depth!
    void CreateNewEventDataIfNeeded();

    void RunEventInit(const std::string& eventName);

    EventMapType m_eventMap; // Event name -> subscribers.
    std::stack<EventParams> m_eventData; // Data tag -> data for currently executing event.
    uint8_t m_eventDepth;
    static std::unordered_multimap<int32_t, bypassRed> m_bypass;
    std::unordered_map<std::string, std::function<void(void)>> m_initList;

    std::unique_ptr<ArelithEvents> m_arelithEvents;
    int32_t bypassEffectImm;

};

}
