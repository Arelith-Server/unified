#include "Arelith.hpp"
#include "API/CAppManager.hpp"
#include "API/CServerExoApp.hpp"
#include "API/CExoString.hpp"
#include "API/CVirtualMachine.hpp"
#include "API/CNWSCreature.hpp"
#include "API/CNWSCreatureStats.hpp"
#include "API/Globals.hpp"
#include "API/Version.hpp"
#include "Source/ArelithEvents.hpp"
#include "Services/Config/Config.hpp"
#include "Services/Messaging/Messaging.hpp"
#include "API/Constants.hpp"
#include "API/Globals.hpp"
#include "ViewPtr.hpp"
#include <algorithm>
#include <regex>
#include <cstring>

using namespace NWNXLib;

static ViewPtr<Arelith::Arelith> g_plugin;

NWNX_PLUGIN_ENTRY Plugin::Info* PluginInfo()
{
    return new Plugin::Info
    {
        "Arelith",
        "Plugin dedicated to assorted Arelith-specific functionality, including implementation dependent functions and custom events.",
        "Silvard",
        "jusenkyo@gmail.com",
        1,
        true
    };
}

NWNX_PLUGIN_ENTRY Plugin* PluginLoad(Plugin::CreateParams params)
{
    g_plugin = new Arelith::Arelith(params);
    return g_plugin;
}

namespace Arelith {

Arelith::Arelith(const Plugin::CreateParams& params)
    : Plugin(params), m_eventDepth(0)
{
    if (g_plugin == nullptr) // :(
        g_plugin = this;

    GetServices()->m_events->RegisterEvent("SUBSCRIBE_EVENT", std::bind(&Arelith::OnSubscribeEvent, this, std::placeholders::_1));
    GetServices()->m_events->RegisterEvent("PUSH_EVENT_DATA", std::bind(&Arelith::OnPushEventData, this, std::placeholders::_1));
    GetServices()->m_events->RegisterEvent("SIGNAL_EVENT", std::bind(&Arelith::OnSignalEvent, this, std::placeholders::_1));
    GetServices()->m_events->RegisterEvent("GET_EVENT_DATA", std::bind(&Arelith::OnGetEventData, this, std::placeholders::_1));
    GetServices()->m_events->RegisterEvent("SKIP_EVENT", std::bind(&Arelith::OnSkipEvent, this, std::placeholders::_1));
    GetServices()->m_events->RegisterEvent("EVENT_RESULT", std::bind(&Arelith::OnEventResult, this, std::placeholders::_1));
    GetServices()->m_events->RegisterEvent("GET_CURRENT_EVENT", std::bind(&Arelith::OnGetCurrentEvent, this, std::placeholders::_1));


    GetServices()->m_messaging->SubscribeMessage("NWNX_ARELITH_SIGNAL_EVENT",
        [](const std::vector<std::string> message)
        {
            ASSERT(message.size() == 2);
            SignalEvent(message[0], std::strtoul(message[1].c_str(), nullptr, 16));
        });

    GetServices()->m_messaging->SubscribeMessage("NWNX_ARELITH_PUSH_EVENT_DATA",
        [](const std::vector<std::string> message)
        {
            ASSERT(message.size() == 2);
            PushEventData(message[0], message[1]);
        });

    m_arelithEvents   = std::make_unique<ArelithEvents>(GetServices()->m_hooks);
}

Arelith::~Arelith()
{
}

void Arelith::PushEventData(const std::string tag, const std::string data)
{
    LOG_DEBUG("Pushing event data: '%s' -> '%s'.", tag.c_str(), data.c_str());
    g_plugin->CreateNewEventDataIfNeeded();
    g_plugin->m_eventData.top().m_EventDataMap[tag] = std::move(data);
}


std::string Arelith::GetEventData(const std::string tag)
{
    std::string retVal;
    if (g_plugin->m_eventDepth == 0 || g_plugin->m_eventData.empty())
    {
        LOG_ERROR("Attempted to access invalid event data or in an invalid context.");
        return retVal;
    }

    auto& eventData = g_plugin->m_eventData.top();
    auto data = eventData.m_EventDataMap.find(tag);

    if (data == std::end(eventData.m_EventDataMap))
    {
        LOG_ERROR("Tried to access event data with invalid tag.");
        return retVal;
    }

    retVal=data->second;
    LOG_DEBUG("Getting event data: '%s' -> '%s'.", tag.c_str(), retVal.c_str());
    return retVal;
}

bool Arelith::SignalEvent(const std::string& eventName, const API::Types::ObjectID target, std::string *result)
{
    bool skipped = false;

    g_plugin->CreateNewEventDataIfNeeded();

    g_plugin->m_eventData.top().m_EventName = eventName;

    for (const auto& script : g_plugin->m_eventMap[eventName])
    {
        LOG_DEBUG("Dispatching notification for event '%s' to script '%s'.", eventName.c_str(), script.c_str());
        API::CExoString scriptExoStr = script.c_str();

        ++g_plugin->m_eventDepth;
        API::Globals::VirtualMachine()->RunScript(&scriptExoStr, target, 1);

        skipped |= g_plugin->m_eventData.top().m_Skipped;

        if (result)
        {
            *result = g_plugin->m_eventData.top().m_Result;
        }

        --g_plugin->m_eventDepth;
    }

    g_plugin->m_eventData.pop();

    g_plugin->GetServices()->m_messaging->BroadcastMessage("NWNX_ARELITH_SIGNAL_EVENT_RESULT",  { eventName, result ? *result : ""});
    g_plugin->GetServices()->m_messaging->BroadcastMessage("NWNX_ARELITH_SIGNAL_EVENT_SKIPPED", { eventName, skipped ? "1" : "0"});

    return !skipped;
}

void Arelith::InitOnFirstSubscribe(const std::string& eventName, std::function<void(void)> init)
{
    g_plugin->m_initList[eventName] = init;
}

void Arelith::RunEventInit(const std::string& eventName)
{
    std::vector<std::string> erase;
    for (auto it: m_initList)
    {
        if (std::regex_search(eventName, std::regex(it.first)))
        {
            LOG_DEBUG("Running init function for events '%s' (requested by event '%s')",
                        it.first.c_str(), eventName.c_str());
            it.second();
            erase.push_back(it.first);
        }
    }
    for (auto e: erase)
    {
        m_initList.erase(e);
    }

}

Services::Events::ArgumentStack Arelith::OnSubscribeEvent(Services::Events::ArgumentStack&& args)
{
    const auto event = Services::Events::ExtractArgument<std::string>(args);
    auto script = Services::Events::ExtractArgument<std::string>(args);

    RunEventInit(event);
    auto& eventVector = m_eventMap[event];

    if (std::find(std::begin(eventVector), std::end(eventVector), script) != std::end(eventVector))
    {
        throw std::runtime_error("Attempted to subscribe to an event with a script that already subscribed!");
    }

    LOG_INFO("Script '%s' subscribed to event '%s'.", script.c_str(), event.c_str());
    eventVector.emplace_back(std::move(script));

    return Services::Events::ArgumentStack();
}

Services::Events::ArgumentStack Arelith::OnPushEventData(Services::Events::ArgumentStack&& args)
{
    const auto tag = Services::Events::ExtractArgument<std::string>(args);
    const auto data = Services::Events::ExtractArgument<std::string>(args);
    PushEventData(tag, data);
    return Services::Events::ArgumentStack();
}

Services::Events::ArgumentStack Arelith::OnSignalEvent(Services::Events::ArgumentStack&& args)
{
    const auto event = Services::Events::ExtractArgument<std::string>(args);
    const auto object = Services::Events::ExtractArgument<API::Types::ObjectID>(args);
    bool signalled = SignalEvent(event, object);
    Services::Events::ArgumentStack stack;
    Services::Events::InsertArgument(stack, signalled ? 1 : 0);
    return stack;
}

Services::Events::ArgumentStack Arelith::OnGetEventData(Services::Events::ArgumentStack&& args)
{
    std::string data = GetEventData(Services::Events::ExtractArgument<std::string>(args));
    Services::Events::ArgumentStack stack;
    Services::Events::InsertArgument(stack, data);
    return stack;
}

Services::Events::ArgumentStack Arelith::OnSkipEvent(Services::Events::ArgumentStack&&)
{
    if (m_eventDepth == 0 || m_eventData.empty())
    {
        throw std::runtime_error("Attempted to skip event in an invalid context.");
    }
    m_eventData.top().m_Skipped = true;

    LOG_DEBUG("Skipping last event.");

    return Services::Events::ArgumentStack();
}

Services::Events::ArgumentStack Arelith::OnEventResult(Services::Events::ArgumentStack&& args)
{
    if (m_eventDepth == 0 || m_eventData.empty())
    {
        throw std::runtime_error("Attempted to skip event in an invalid context.");
    }
    const auto data = Services::Events::ExtractArgument<std::string>(args);

    m_eventData.top().m_Result = data;

    LOG_DEBUG("Received event result '%s'.", data.c_str());

    return Services::Events::ArgumentStack();
}

Services::Events::ArgumentStack Arelith::OnGetCurrentEvent(Services::Events::ArgumentStack&&)
{
    if (m_eventDepth == 0 || m_eventData.empty())
    {
        throw std::runtime_error("Attempted to get the current event in an invalid context.");
    }

    std::string eventName = g_plugin->m_eventData.top().m_EventName;

    Services::Events::ArgumentStack stack;
    Services::Events::InsertArgument(stack, eventName);
    return stack;
}

void Arelith::CreateNewEventDataIfNeeded()
{
    if (m_eventData.size() <= m_eventDepth)
    {
        EventParams params;
        params.m_Skipped = false;
        m_eventData.emplace(std::move(params));
    }
}

}
