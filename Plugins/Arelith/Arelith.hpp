#pragma once

#include "Plugin.hpp"
#include "Services/Events/Events.hpp"
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

using ArgumentStack = NWNXLib::Services::Events::ArgumentStack;
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
    Arelith(const Plugin::CreateParams& params);
    virtual ~Arelith();

    // Pushes event data to the stack - won't do anything until SignalEvent is called.
    static void PushEventData(const std::string tag, const std::string data);

    // Get event data
    static std::string GetEventData(const std::string tag);

    // Returns true if the event can proceed, or false if the event has been skipped.
    static bool SignalEvent(const std::string& eventName, const NWNXLib::API::Types::ObjectID target, std::string *result=nullptr);

    static void InitOnFirstSubscribe(const std::string& eventName, std::function<void(void)> init);

private: // Structures
    using EventMapType = std::unordered_map<std::string, std::vector<std::string>>;

private:
    NWNXLib::Services::Events::ArgumentStack OnSubscribeEvent(NWNXLib::Services::Events::ArgumentStack&& args);
    NWNXLib::Services::Events::ArgumentStack OnPushEventData(NWNXLib::Services::Events::ArgumentStack&& args);
    NWNXLib::Services::Events::ArgumentStack OnSignalEvent(NWNXLib::Services::Events::ArgumentStack&& args);
    NWNXLib::Services::Events::ArgumentStack OnGetEventData(NWNXLib::Services::Events::ArgumentStack&& args);
    NWNXLib::Services::Events::ArgumentStack OnSkipEvent(NWNXLib::Services::Events::ArgumentStack&& args);
    NWNXLib::Services::Events::ArgumentStack OnEventResult(NWNXLib::Services::Events::ArgumentStack&& args);
    NWNXLib::Services::Events::ArgumentStack OnGetCurrentEvent(NWNXLib::Services::Events::ArgumentStack&& args);

    // Pushes a brand new event data onto the event data stack, set up with the correct defaults.
    // Only does it if needed though, based on the current event depth!
    void CreateNewEventDataIfNeeded();

    void RunEventInit(const std::string& eventName);

    EventMapType m_eventMap; // Event name -> subscribers.
    std::stack<EventParams> m_eventData; // Data tag -> data for currently executing event.
    uint8_t m_eventDepth;

    std::unordered_map<std::string, std::function<void(void)>> m_initList;

    std::unique_ptr<ArelithEvents> m_arelithEvents;

};

}