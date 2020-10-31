#include "Arelith.hpp"
#include "API/CAppManager.hpp"
#include "API/CServerExoApp.hpp"
#include "API/CExoString.hpp"
#include "API/CVirtualMachine.hpp"
#include "API/CNWSCreature.hpp"
#include "API/CNWSItem.hpp"
#include "API/CNWSCreatureStats.hpp"
#include "API/Globals.hpp"
#include "Source/ArelithEvents.hpp"
#include "Services/Config/Config.hpp"
#include "Services/Messaging/Messaging.hpp"
#include "API/Constants.hpp"
#include "API/Globals.hpp"
#include "API/CNWRules.hpp"
#include "API/CNWCCMessageData.hpp"
#include "API/CNWSCombatRound.hpp"
#include "API/CNWSPlayer.hpp"
//#include "ViewPtr.hpp"
#include "API/Functions.hpp"
#include <algorithm>
#include <regex>
#include <cstring>
#include "API/CNWSModule.hpp"
#include "Services/Tasks/Tasks.hpp"
#include "Services/Messaging/Messaging.hpp"
#include "Encoding.hpp"
#include "API/CNWItemProperty.hpp"
#include <cmath>
#include <sstream>
#include <iomanip>
#include "External/httplib.h"
#include "API/CNWSInventory.hpp"

using namespace NWNXLib;
using namespace NWNXLib::API;
using namespace NWNXLib::API::Constants;

static Arelith::Arelith* g_plugin;

NWNX_PLUGIN_ENTRY Plugin* PluginLoad(Services::ProxyServiceList* services)
{
    g_plugin = new Arelith::Arelith(services);
    return g_plugin;
}

namespace Core {
extern bool g_CoreShuttingDown;
}

namespace Arelith {

bool Arelith::s_bSendError = false;
std::string Arelith::s_sHost;
std::string Arelith::s_sOrigPath;
std::string Arelith::s_sAdden;
uint8_t Arelith::s_iMaterial;
std::unordered_multimap<int32_t, bypassRed> Arelith::m_bypass;

Arelith::Arelith(Services::ProxyServiceList* services)
    : Plugin(services), m_eventDepth(0)
{
    if (g_plugin == nullptr) // :(
        g_plugin = this;

#define REGISTER(func) \
    GetServices()->m_events->RegisterEvent(#func, \
        [this](ArgumentStack&& args){ return func(std::move(args)); })
    REGISTER(OnSubscribeEvent);
    REGISTER(OnPushEventData);
    REGISTER(OnSignalEvent);
    REGISTER(OnGetEventData);
    REGISTER(OnSkipEvent);
    REGISTER(OnEventResult);
    REGISTER(OnGetCurrentEvent);
    REGISTER(GetWeaponPower);
    REGISTER(GetAttackModifierVersus);
    REGISTER(ResolveDefensiveEffects);
    REGISTER(SetWebhook);
    REGISTER(SetDamageReductionBypass);
    REGISTER(SetEffectImmunityBypass);
    REGISTER(GetTrueEffectCount);
    REGISTER(GetTrueEffect);
    REGISTER(RemoveEffectById);
    REGISTER(ReplaceEffect);
    REGISTER(SetDisableMonkAbilitiesPolymorph);
#undef REGISTER

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
    auto hooker = GetServices()->m_hooks.get();

    m_arelithEvents   = std::make_unique<ArelithEvents>(hooker);


    GetServices()->m_hooks->RequestSharedHook<Functions::_ZN25CNWVirtualMachineCommands11ReportErrorER10CExoStringi, int32_t>(&ReportErrorHook);

    GetServices()->m_hooks->RequestSharedHook<Functions::_ZN17CExoDebugInternal14WriteToLogFileERK10CExoString,
        void, CExoDebugInternal*, CExoString*>(&WriteToLogFileHook);

    m_GetEffectImmunityHook = GetServices()->m_hooks->RequestExclusiveHook<Functions::_ZN17CNWSCreatureStats17GetEffectImmunityEhP12CNWSCreaturei>(&GetEffectImmunityHook);

    GetServices()->m_hooks->RequestSharedHook<Functions::_ZN15CServerAIMaster21OnItemPropertyAppliedEP8CNWSItemP15CNWItemPropertyP12CNWSCreatureji, bool, CServerAIMaster*, CNWSItem*, CNWItemProperty*, CNWSCreature*, uint32_t, BOOL>(&OnItemPropertyAppliedHook);

    GetServices()->m_hooks->RequestSharedHook<Functions::_ZN21CNWSEffectListHandler21OnApplyEffectImmunityEP10CNWSObjectP11CGameEffecti, bool, CNWSEffectListHandler*, CNWSObject*, CGameEffect*, BOOL>(&OnApplyEffectImmunityHook);

    if(GetServices()->m_config->Get<bool>("DMG_RED", false))
    {
        GetServices()->m_hooks->RequestSharedHook<Functions::_ZN21CNWSEffectListHandler22OnApplyDamageReductionEP10CNWSObjectP11CGameEffecti, bool, CNWSEffectListHandler*, CNWSObject*, CGameEffect*, BOOL>(&OnApplyDamageReductionHook);
        GetServices()->m_hooks->RequestSharedHook<Functions::_ZN10CNWSObject17DoDamageReductionEP12CNWSCreatureihii, bool, CNWSObject*, CNWSCreature*, int32_t, uint8_t, BOOL, BOOL>(&DoDamageReductionHook);
    }

    if(GetServices()->m_config->Get<bool>("POLYMORPH", false))
    {
        pGetUseMonkAbilities_hook = GetServices()->m_hooks->RequestExclusiveHook<Functions::_ZN12CNWSCreature19GetUseMonkAbilitiesEv>(&CNWSCreature__GetUseMonkAbilities_hook);
    }
    s_sHost = GetServices()->m_config->Get<std::string>("HOST", "");
    s_sOrigPath = GetServices()->m_config->Get<std::string>("PATH", "");
    s_sAdden = GetServices()->m_config->Get<std::string>("ROLE", "");

    g_plugin->bypassEffectImm=0;
}

Arelith::~Arelith()
{
}

void Arelith::PushEventData(const std::string& tag, const std::string& data)
{
    LOG_DEBUG("Pushing event data: '%s' -> '%s'.", tag, data);
    g_plugin->CreateNewEventDataIfNeeded();
    g_plugin->m_eventData.top().m_EventDataMap[tag] = std::move(data);
}


std::string Arelith::GetEventData(const std::string& tag)
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
    LOG_DEBUG("Getting event data: '%s' -> '%s'.", tag, retVal);
    return retVal;
}

bool Arelith::SignalEvent(const std::string& eventName, const ObjectID target, std::string *result)
{
    bool skipped = false;

    g_plugin->CreateNewEventDataIfNeeded();

    g_plugin->m_eventData.top().m_EventName = eventName;

    for (const auto& script : g_plugin->m_eventMap[eventName])
    {
       LOG_DEBUG("Dispatching notification for event '%s' to script '%s'.", eventName, script);
       CExoString scriptExoStr = script.c_str();

        ++g_plugin->m_eventDepth;
        API::Globals::VirtualMachine()->RunScript(&scriptExoStr, target, 1);

        skipped |= g_plugin->m_eventData.top().m_Skipped;

        if (result)
        {
            *result = g_plugin->m_eventData.top().m_Result;
        }

        --g_plugin->m_eventDepth;
    }


    g_plugin->GetServices()->m_messaging->BroadcastMessage("NWNX_ARELITH_SIGNAL_EVENT_RESULT",  { eventName, result ? *result : ""});
    g_plugin->GetServices()->m_messaging->BroadcastMessage("NWNX_ARELITH_SIGNAL_EVENT_SKIPPED", { eventName, skipped ? "1" : "0"});
    g_plugin->m_eventData.pop();
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
                        it.first, eventName);
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

    LOG_INFO("Script '%s' subscribed to event '%s'.", script, event);
    eventVector.emplace_back(std::move(script));

    return Services::Events::Arguments();
}

Services::Events::ArgumentStack Arelith::OnPushEventData(Services::Events::ArgumentStack&& args)
{
    const auto tag = Services::Events::ExtractArgument<std::string>(args);
    const auto data = Services::Events::ExtractArgument<std::string>(args);
    PushEventData(tag, data);
    return Services::Events::Arguments();
}

Services::Events::ArgumentStack Arelith::OnSignalEvent(Services::Events::ArgumentStack&& args)
{
    const auto event = Services::Events::ExtractArgument<std::string>(args);
    const auto object = Services::Events::ExtractArgument<ObjectID>(args);
    bool signalled = SignalEvent(event, object);
    return Services::Events::Arguments(signalled ? 1 : 0);
}

Services::Events::ArgumentStack Arelith::OnGetEventData(Services::Events::ArgumentStack&& args)
{
    std::string data = GetEventData(Services::Events::ExtractArgument<std::string>(args));
    return Services::Events::Arguments(data);
}

Services::Events::ArgumentStack Arelith::OnSkipEvent(Services::Events::ArgumentStack&&)
{
    if (m_eventDepth == 0 || m_eventData.empty())
    {
        throw std::runtime_error("Attempted to skip event in an invalid context.");
    }
    m_eventData.top().m_Skipped = true;

    LOG_DEBUG("Skipping last event.");

    return Services::Events::Arguments();
}

Services::Events::ArgumentStack Arelith::OnEventResult(Services::Events::ArgumentStack&& args)
{
    if (m_eventDepth == 0 || m_eventData.empty())
    {
        throw std::runtime_error("Attempted to skip event in an invalid context.");
    }
    const auto data = Services::Events::ExtractArgument<std::string>(args);

    m_eventData.top().m_Result = data;

    LOG_DEBUG("Received event result '%s'.", data);

    return Services::Events::Arguments();
}

Services::Events::ArgumentStack Arelith::OnGetCurrentEvent(Services::Events::ArgumentStack&&)
{
    if (m_eventDepth == 0 || m_eventData.empty())
    {
        throw std::runtime_error("Attempted to get the current event in an invalid context.");
    }



    std::string eventName = g_plugin->m_eventData.top().m_EventName;
    return Services::Events::Arguments(eventName);
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

CNWSObject *Arelith::object(ArgumentStack& args)
{
    const auto objectId = Services::Events::ExtractArgument<ObjectID>(args);

    if (objectId == Constants::OBJECT_INVALID)
    {
        LOG_NOTICE("NWNX_Object function called on OBJECT_INVALID");
        return nullptr;
    }

    auto *pGameObject = Globals::AppManager()->m_pServerExoApp->GetGameObject(objectId);
    return Utils::AsNWSObject(pGameObject);
}
CNWSCreature *Arelith::creature(ArgumentStack& args)
{
    const auto creatureId = Services::Events::ExtractArgument<ObjectID>(args);

    if (creatureId == Constants::OBJECT_INVALID)
    {
        LOG_NOTICE("NWNX_Creature function called on OBJECT_INVALID");
        return nullptr;
    }

    return Globals::AppManager()->m_pServerExoApp->GetCreatureByGameObjectID(creatureId);
}
ArgumentStack Arelith::GetWeaponPower(ArgumentStack&& args)
{
    int32_t retVal = -1;
    if (auto *pCreature = creature(args))
    {

        if (auto *versus = object(args))
        {
            const auto isOffhand = Services::Events::ExtractArgument<int32_t>(args);
            retVal = pCreature->GetWeaponPower(versus, isOffhand);
        }


    }
    return Services::Events::Arguments(retVal);
}

ArgumentStack Arelith::GetAttackModifierVersus(ArgumentStack&& args)
{
    int32_t retVal = -1;
    if (auto *pCreature = creature(args))
    {
        if(auto *versus = creature(args))
            retVal = pCreature->m_pStats->GetAttackModifierVersus(versus);

    }
    return Services::Events::Arguments(retVal);
}
ArgumentStack Arelith::ResolveDefensiveEffects(ArgumentStack&& args)
{
    int32_t retVal = 0;
    if (auto *pCreature = creature(args))
    {
        if (auto *versus = object(args))
        {
            const auto isAttackHit = Services::Events::ExtractArgument<int32_t>(args);
            retVal = pCreature->ResolveDefensiveEffects(versus, isAttackHit);
        }
    }
    return Services::Events::Arguments(retVal);
}


void Arelith::ReportErrorHook(bool before, CNWVirtualMachineCommands*, CExoString, int32_t)
{
    if(s_sHost.empty() || s_sOrigPath.empty())
    {
        LOG_INFO("Host or path was empty.");
        return;
    }
    if(before) s_bSendError=true;
}

void Arelith::WriteToLogFileHook(bool before, CExoDebugInternal*, CExoString* message)
{

    if(before && s_bSendError)
    {
        SendWebHookHTTPS(message->CStr());
        s_bSendError=false;
    }
}

std::string escape_json(const std::string &s) {
    std::ostringstream o;
    for (auto c = s.cbegin(); c != s.cend(); c++) {
        if (*c == '"' || *c == '\\' || ('\x00' <= *c && *c <= '\x1f')) {
            o << "\\u"
              << std::hex << std::setw(4) << std::setfill('0') << (int)*c;
        } else {
            o << *c;
        }
    }
    return o.str();
}

void Arelith::SendWebHookHTTPS(const char* messagec)
{

    std::string host = s_sHost;
    std::string origPath = s_sOrigPath;
    std::string message(messagec);
    message = Utils::trim(message);
    message = message + " " + s_sAdden;
    message = R"({"text": ")" + message + "\"";
    message = message +  R"(, "mrkdwn": false)";
    message = message + "}";

    // For Discord, will wait for a response
    auto path = origPath + "?wait=true";

    message = Encoding::ToUTF8(message);
    escape_json(message);

    static std::unordered_map<std::string, std::unique_ptr<httplib::SSLClient>> s_ClientCache;
    auto cli = s_ClientCache.find(host);

    if (cli == std::end(s_ClientCache))
    {
        LOG_DEBUG("Creating new SSL client for host %s.", host);
        cli = s_ClientCache.insert(std::make_pair(host, std::make_unique<httplib::SSLClient>(host.c_str(), 443))).first;
    }
    if (Core::g_CoreShuttingDown)
    {
        auto res = cli->second->post(path.c_str(), message, "application/json");

        if (res->status == 200)
        {
            LOG_INFO("Sent webhook '%s' to '%s%s'.", message, host, path);
        }
        else
        {
            LOG_WARNING("Failed to send WebHook (HTTPS) message '%s' to '%s%s', status code '%d'.",
                        message.c_str(), host.c_str(), path.c_str(), res->status);
        }
    }
    else
    {
        g_plugin->GetServices()->m_tasks->QueueOnAsyncThread([cli, message, host, path, origPath]()
        {
            auto res = cli->second->post(path.c_str(), message, "application/json");
            g_plugin->GetServices()->m_tasks->QueueOnMainThread([message, host, path, origPath, res]()
            {
                if (Core::g_CoreShuttingDown)
                    return;

                auto messaging = g_plugin->GetServices()->m_messaging.get();
                auto moduleOid = NWNXLib::Utils::ObjectIDToString(Utils::GetModule()->m_idSelf);

                if (res)
                {
                    messaging->BroadcastMessage("NWNX_EVENT_PUSH_EVENT_DATA", {"STATUS", std::to_string(res->status)});
                    messaging->BroadcastMessage("NWNX_EVENT_PUSH_EVENT_DATA", {"MESSAGE", message});
                    messaging->BroadcastMessage("NWNX_EVENT_PUSH_EVENT_DATA", {"HOST", host});
                    messaging->BroadcastMessage("NWNX_EVENT_PUSH_EVENT_DATA", {"PATH", origPath});
                    if (res->status == 200 || res->status == 201 || res->status == 204 || res->status == 429)
                    {
                        // Discord sends your rate limit information even on success so you can stagger calls if you want
                        // This header also lets us know it's Discord not Slack, important because Discord sends RETRY_AFTER
                        // in milliseconds and Slack sends it as seconds.
                        if (!res->get_header_value("X-RateLimit-Limit").empty())
                        {
                            messaging->BroadcastMessage("NWNX_EVENT_PUSH_EVENT_DATA", {"RATELIMIT_LIMIT", res->get_header_value("X-RateLimit-Limit")});
                            messaging->BroadcastMessage("NWNX_EVENT_PUSH_EVENT_DATA", {"RATELIMIT_REMAINING", res->get_header_value("X-RateLimit-Remaining")});
                            messaging->BroadcastMessage("NWNX_EVENT_PUSH_EVENT_DATA", {"RATELIMIT_RESET", res->get_header_value("X-RateLimit-Reset")});
                            if (!res->get_header_value("Retry-After").empty())
                                messaging->BroadcastMessage("NWNX_EVENT_PUSH_EVENT_DATA", {"RETRY_AFTER", res->get_header_value("Retry-After")});
                        }
                            // Slack rate limited
                        else if (!res->get_header_value("Retry-After").empty())
                        {
                            float fSlackRetry = stof(res->get_header_value("Retry-After")) * 1000.0f;
                            messaging->BroadcastMessage("NWNX_EVENT_PUSH_EVENT_DATA", {"RETRY_AFTER", std::to_string(fSlackRetry)});
                        }
                        if (res->status != 429)
                        {
                            messaging->BroadcastMessage("NWNX_EVENT_SIGNAL_EVENT", {"NWNX_ON_ARE_WEBHOOK_SUCCESS", moduleOid});
                            LOG_INFO("Sent webhook '%s' to '%s%s'.", message, host, path);
                        }
                        else
                        {
                            messaging->BroadcastMessage("NWNX_EVENT_SIGNAL_EVENT", {"NWNX_ON_ARE_WEBHOOK_FAILED", moduleOid});
                            LOG_WARNING("Failed to send WebHook (HTTPS) message '%s' to '%s%s'. Rate Limited.", message, host, path);
                        }
                    }
                    else
                    {
                        messaging->BroadcastMessage("NWNX_EVENT_PUSH_EVENT_DATA", {"FAIL_INFO", res->body});
                        messaging->BroadcastMessage("NWNX_EVENT_SIGNAL_EVENT", {"NWNX_ON_ARE_WEBHOOK_FAILED", moduleOid});
                        LOG_WARNING("Failed to send WebHook (HTTPS) message '%s' to '%s%s', status code '%d'.", message, host, path, res->status);
                    }
                }
                else
                {
                    messaging->BroadcastMessage("NWNX_EVENT_PUSH_EVENT_DATA", {"FAIL_INFO", "Failed to post to server. Is the url correct?"});
                    messaging->BroadcastMessage("NWNX_EVENT_SIGNAL_EVENT", {"NWNX_ON_ARE_WEBHOOK_FAILED", moduleOid});
                    LOG_WARNING("Failed to send WebHook (HTTPS) to '%s%s'.", host, path);
                }
            });
        });
    }

}
ArgumentStack Arelith::SetWebhook(ArgumentStack&& args)
{
    s_sAdden = Services::Events::ExtractArgument<std::string>(args);
    s_sHost = Services::Events::ExtractArgument<std::string>(args);
    s_sOrigPath = Services::Events::ExtractArgument<std::string>(args);
    return Services::Events::Arguments();
}

void Arelith::OnItemPropertyAppliedHook(bool before, CServerAIMaster*, CNWSItem*, CNWItemProperty *pItemProperty, CNWSCreature*, uint32_t, BOOL)
{
   if(before)
   {
       if(pItemProperty->m_nParam1Value > 0 && pItemProperty->m_nParam1Value != 255)
       {
        if(pItemProperty->m_nPropertyName==Constants::ItemProperty::DamageReduction || pItemProperty->m_nPropertyName==Constants::ItemProperty::ImmunityMiscellaneous)
        {
          s_iMaterial=pItemProperty->m_nParam1Value;
        }
       }
   }
}
void Arelith::OnApplyDamageReductionHook(bool before, CNWSEffectListHandler*, CNWSObject*, CGameEffect* pEffect, BOOL)
{
    if(before && s_iMaterial > 0)
    {
        pEffect->SetInteger(3, s_iMaterial);
        s_iMaterial=0;
    }
}
void Arelith::OnApplyEffectImmunityHook(bool before, CNWSEffectListHandler*, CNWSObject *, CGameEffect * pEffect, BOOL)
{
    if(before && s_iMaterial > 0)
    {
        pEffect->SetInteger(4, s_iMaterial);
        s_iMaterial=0;
    }
}
void Arelith::DoDamageReductionHook(bool before, CNWSObject *pObject, CNWSCreature *pCreature, int32_t, uint8_t, BOOL, BOOL)
{
    static std::unordered_map<uint64_t, int32_t> s_mEffects;

    if(pCreature == nullptr)
      return;

    if(before)
    {
        CNWSItem* pWeapon = nullptr;
        pWeapon = pCreature->m_pcCombatRound->GetCurrentAttackWeapon();
        if(pWeapon == nullptr)
            return; //no need to continue as there is no material type

        if(pWeapon->m_nBaseItem == Constants::BaseItem::HeavyCrossbow || pWeapon->m_nBaseItem == Constants::BaseItem::LightCrossbow)
            pWeapon = pCreature->m_pInventory->GetItemInSlot(Constants::EquipmentSlot::Bolts);
        else if(pWeapon->m_nBaseItem == Constants::BaseItem::Longbow || pWeapon->m_nBaseItem == Constants::BaseItem::Shortbow)
            pWeapon = pCreature->m_pInventory->GetItemInSlot(Constants::EquipmentSlot::Arrows);
        else if(pWeapon->m_nBaseItem == Constants::BaseItem::Sling)
            pWeapon = pCreature->m_pInventory->GetItemInSlot(Constants::EquipmentSlot::Bullets);
        if(pWeapon == nullptr)
            return;
        bool bRemoveDR;
        for (int i = 0; i < pObject->m_appliedEffects.num; i++)
        {
                auto *eff = pObject->m_appliedEffects.element[i];
                bRemoveDR=false;
                if(eff->m_nType==Constants::EffectTrueType::DamageReduction && eff->m_nParamInteger[3] > 0)
                {
                    auto redType = eff->m_nParamInteger[3];
                    auto range = m_bypass.equal_range(redType);
                    for (auto it= range.first; it!= range.second; ++it)
                    {
                        auto bypass = it->second;
                        for (int i = 0; i < pWeapon->m_lstPassiveProperties.num; i++)
                        {
                            auto property = pWeapon->GetPassiveProperty(i);
                            if (property->m_nPropertyName == bypass.m_nPropertyName &&
                                (property->m_nCostTableValue == bypass.m_nCostTableValue || bypass.m_nCostTableValue==-1) &&
                                (property->m_nSubType == bypass.m_nSubType || bypass.m_nSubType==-1)  &&
                                (property->m_nParam1Value == bypass.m_nParam1Value || bypass.m_nParam1Value==-1))
                            {
                                if(!bypass.bReverse)
                                {
                                   bRemoveDR=true;
                                }
                                break; //as long as we found a property, break
                            }
                            if(bypass.bReverse && i==pWeapon->m_lstPassiveProperties.num-1) //last property and we still didn't find it, so remove DR
                                bRemoveDR=true;
                        }
                        if(bRemoveDR) break; // no reason to kep checking
                    }

                }

                if(bRemoveDR)
                {
                    s_mEffects[eff->m_nID] = eff->m_nParamInteger[1];
                    eff->m_nParamInteger[1]=0;
                }
        }
    }
    else
    {
        for (int i = 0; i < pObject->m_appliedEffects.num; i++)
        {
                auto *eff = pObject->m_appliedEffects.element[i];

                if(eff->m_nType==Constants::EffectTrueType::DamageReduction)
                {
                    auto original = s_mEffects.find(eff->m_nID);
                    if (original != std::end(s_mEffects))
                    {
                        eff->m_nParamInteger[1]=original->second;
                        s_mEffects.erase(eff->m_nID);
                    }
                }
        }
    }
}
ArgumentStack Arelith::SetDamageReductionBypass(ArgumentStack&& args)
{
    auto material = Services::Events::ExtractArgument<int32_t>(args);
    auto propType = Services::Events::ExtractArgument<int32_t>(args);
    auto subType =  Services::Events::ExtractArgument<int32_t>(args);
    auto costValue = Services::Events::ExtractArgument<int32_t>(args);
    auto param1Value =  Services::Events::ExtractArgument<int32_t>(args);
    auto reverse =  Services::Events::ExtractArgument<int32_t>(args);
    bypassRed ip;
    ip.m_nPropertyName=propType;
    ip.m_nSubType=subType;
    ip.m_nParam1Value=param1Value;
    ip.m_nCostTableValue=costValue;
    ip.bReverse=reverse;
    m_bypass.insert(std::make_pair(material, ip));
    return Services::Events::Arguments();
}
BOOL Arelith::GetEffectImmunityHook(CNWSCreatureStats *pStats, uint8_t nType, CNWSCreature * pVersus, BOOL bConsiderFeats)
{
    if(g_plugin->bypassEffectImm > 0)
        return false;

    if(nType == Constants::ImmunityType::CriticalHit || nType == Constants::ImmunityType::SneakAttack)
    {
        if(bConsiderFeats && pStats->HasFeat(Constants::Feat::DeathlessMastery))
            return true;

        auto effectList = pStats->m_pBaseCreature->m_appliedEffects;

        int32_t highest = 0;

        for (int32_t i = 0; i < effectList.num; i++)
        {
            auto *eff = effectList.element[i];

            if(eff->m_nType==Constants::EffectTrueType::Immunity && eff->m_nParamInteger[0]==nType)
            {
                if((eff->m_nParamInteger[1] == Constants::RacialType::All || (pVersus != nullptr && eff->m_nParamInteger[1] == pVersus->m_pStats->m_nRace)) && //race check
                   (eff->m_nParamInteger[2] == Constants::Alignment::All || (pVersus != nullptr && eff->m_nParamInteger[2] == pVersus->m_pStats->m_nAlignmentLawChaos)) &&
                   (eff->m_nParamInteger[3] == Constants::Alignment::All || (pVersus != nullptr && eff->m_nParamInteger[3] == pVersus->m_pStats->m_nAlignmentGoodEvil)))
                {

                    if(eff->m_nParamInteger[4] <= 0 || eff->m_nParamInteger[4] >= 100)
                        return true;

                    if(eff->m_nParamInteger[4] > highest)
                        highest = eff->m_nParamInteger[4];
                }

            }

        }

        if(highest > 0 && Globals::Rules()->RollDice(1, 100) <= highest)
            return true;

        return false;
    }
    return g_plugin->m_GetEffectImmunityHook->CallOriginal<BOOL>(pStats, nType, pVersus, bConsiderFeats);
}
ArgumentStack Arelith::SetEffectImmunityBypass(ArgumentStack&& args)
{
    g_plugin->bypassEffectImm = Services::Events::ExtractArgument<int32_t>(args);
    return Services::Events::Arguments();
}

ArgumentStack Arelith::GetTrueEffectCount(ArgumentStack&& args)
{
    int32_t ret = 0;
    if (auto *pObject = object(args))
    {
        ret = pObject->m_appliedEffects.num;
    }
    return Services::Events::Arguments(ret);
}
ArgumentStack Arelith::GetTrueEffect(ArgumentStack&& args)
{
    ArgumentStack stack;
    CGameEffect *eff;
    auto *pObject = object(args);
      ASSERT_OR_THROW(pObject);
    auto it = Services::Events::ExtractArgument<int32_t>(args);
      ASSERT_OR_THROW(it >= 0);
      ASSERT_OR_THROW(it < pObject->m_appliedEffects.num);
    eff = pObject->m_appliedEffects.element[it];


    Services::Events::InsertArgument(stack, std::to_string(eff->m_nID));
    Services::Events::InsertArgument(stack, (int32_t)eff->m_nType);
    Services::Events::InsertArgument(stack, (int32_t)eff->m_nSubType);
    Services::Events::InsertArgument(stack, (float)eff->m_fDuration);
    Services::Events::InsertArgument(stack, (int32_t)eff->m_nExpiryCalendarDay);
    Services::Events::InsertArgument(stack, (int32_t)eff->m_nExpiryTimeOfDay);
    Services::Events::InsertArgument(stack, (ObjectID)eff->m_oidCreator);
    Services::Events::InsertArgument(stack, (int32_t)eff->m_nSpellId);
    Services::Events::InsertArgument(stack, (int32_t)eff->m_bExpose);
    Services::Events::InsertArgument(stack, (int32_t)eff->m_bShowIcon);
    Services::Events::InsertArgument(stack, (int32_t)eff->m_nCasterLevel);

    // The DestroyGameEffect at the end of this function will delete any linked effects
    // as well so we make a copy of the linked effects and send those for unpacking
   /* CGameEffect *leftLinkEff = nullptr;
    if (eff->m_pLinkLeft != nullptr)
    {
        leftLinkEff = new CGameEffect(true);
        leftLinkEff->CopyEffect(eff->m_pLinkLeft, 0);
    }
    Services::Events::InsertArgument(stack, leftLinkEff);
    Services::Events::InsertArgument(stack, eff->m_pLinkLeft != nullptr);

    CGameEffect *rightLinkEff = nullptr;
    if (eff->m_pLinkRight != nullptr)
    {
        rightLinkEff = new CGameEffect(true);
        rightLinkEff->CopyEffect(eff->m_pLinkRight, 0);
    }*/
    Services::Events::InsertArgument(stack, (int32_t)eff->m_nNumIntegers);
    Services::Events::InsertArgument(stack, (int32_t)(eff->m_nNumIntegers > 0 ? eff->m_nParamInteger[0] : -1));
    Services::Events::InsertArgument(stack, (int32_t)(eff->m_nNumIntegers > 1 ? eff->m_nParamInteger[1] : -1));
    Services::Events::InsertArgument(stack, (int32_t)(eff->m_nNumIntegers > 2 ? eff->m_nParamInteger[2] : -1));
    Services::Events::InsertArgument(stack, (int32_t)(eff->m_nNumIntegers > 3 ? eff->m_nParamInteger[3] : -1));
    Services::Events::InsertArgument(stack, (int32_t)(eff->m_nNumIntegers > 4 ? eff->m_nParamInteger[4] : -1));
    Services::Events::InsertArgument(stack, (int32_t)(eff->m_nNumIntegers > 5 ? eff->m_nParamInteger[5] : -1));
    Services::Events::InsertArgument(stack, (int32_t)(eff->m_nNumIntegers > 6 ? eff->m_nParamInteger[6] : -1));
    Services::Events::InsertArgument(stack, (int32_t)(eff->m_nNumIntegers > 7 ? eff->m_nParamInteger[7] : -1));

    Services::Events::InsertArgument(stack, (float)eff->m_nParamFloat[0]);
    Services::Events::InsertArgument(stack, (float)eff->m_nParamFloat[1]);
    Services::Events::InsertArgument(stack, (float)eff->m_nParamFloat[2]);
    Services::Events::InsertArgument(stack, (float)eff->m_nParamFloat[3]);

    Services::Events::InsertArgument(stack, std::string(eff->m_sParamString[0].CStr()));
    Services::Events::InsertArgument(stack, std::string(eff->m_sParamString[1].CStr()));
    Services::Events::InsertArgument(stack, std::string(eff->m_sParamString[2].CStr()));
    Services::Events::InsertArgument(stack, std::string(eff->m_sParamString[3].CStr()));
    Services::Events::InsertArgument(stack, std::string(eff->m_sParamString[4].CStr()));
    Services::Events::InsertArgument(stack, std::string(eff->m_sParamString[5].CStr()));

    Services::Events::InsertArgument(stack, (ObjectID)eff->m_oidParamObjectID[0]);
    Services::Events::InsertArgument(stack, (ObjectID)eff->m_oidParamObjectID[1]);
    Services::Events::InsertArgument(stack, (ObjectID)eff->m_oidParamObjectID[2]);
    Services::Events::InsertArgument(stack, (ObjectID)eff->m_oidParamObjectID[3]);

    Services::Events::InsertArgument(stack, std::to_string(eff->m_nItemPropertySourceId));

    Services::Events::InsertArgument(stack, std::string(eff->m_sCustomTag.CStr()));

    return stack;
}

ArgumentStack Arelith::RemoveEffectById(ArgumentStack&& args)
{
   int32_t ret = 0;
   if (auto *pObject = object(args))
   {
        auto id = Services::Events::ExtractArgument<std::string>(args);
        ret = pObject->RemoveEffectById(std::stoi(id));
   }

   return Services::Events::Arguments(ret);
}

ArgumentStack Arelith::ReplaceEffect(ArgumentStack&& args)
{
   // auto eff = Services::Events::ExtractArgument<CGameEffect*>(args);
    if(auto *pObject = object(args))
    {
        auto element = Services::Events::ExtractArgument<int32_t>(args);
          ASSERT_OR_THROW(element >= 0);
          ASSERT_OR_THROW(element < pObject->m_appliedEffects.num);
        auto eff = pObject->m_appliedEffects.element[element];

        eff->m_sCustomTag = Services::Events::ExtractArgument<std::string>(args).c_str();

        auto vector1z = Services::Events::ExtractArgument<float>(args);
        auto vector1y = Services::Events::ExtractArgument<float>(args);
        auto vector1x = Services::Events::ExtractArgument<float>(args);
        eff->m_vParamVector[1] = {vector1x, vector1y, vector1z};

        auto vector0z = Services::Events::ExtractArgument<float>(args);
        auto vector0y = Services::Events::ExtractArgument<float>(args);
        auto vector0x = Services::Events::ExtractArgument<float>(args);
        eff->m_vParamVector[0] = {vector0x, vector0y, vector0z};

        eff->m_oidParamObjectID[3] = Services::Events::ExtractArgument<ObjectID>(args);
        eff->m_oidParamObjectID[2] = Services::Events::ExtractArgument<ObjectID>(args);
        eff->m_oidParamObjectID[1] = Services::Events::ExtractArgument<ObjectID>(args);
        eff->m_oidParamObjectID[0] = Services::Events::ExtractArgument<ObjectID>(args);

        eff->m_sParamString[5] = Services::Events::ExtractArgument<std::string>(args).c_str();
        eff->m_sParamString[4] = Services::Events::ExtractArgument<std::string>(args).c_str();
        eff->m_sParamString[3] = Services::Events::ExtractArgument<std::string>(args).c_str();
        eff->m_sParamString[2] = Services::Events::ExtractArgument<std::string>(args).c_str();
        eff->m_sParamString[1] = Services::Events::ExtractArgument<std::string>(args).c_str();
        eff->m_sParamString[0] = Services::Events::ExtractArgument<std::string>(args).c_str();

        eff->m_nParamFloat[3] = Services::Events::ExtractArgument<float>(args);
        eff->m_nParamFloat[2] = Services::Events::ExtractArgument<float>(args);
        eff->m_nParamFloat[1] = Services::Events::ExtractArgument<float>(args);
        eff->m_nParamFloat[0] = Services::Events::ExtractArgument<float>(args);

        eff->m_nParamInteger[7] = Services::Events::ExtractArgument<int32_t>(args);
        eff->m_nParamInteger[6] = Services::Events::ExtractArgument<int32_t>(args);
        eff->m_nParamInteger[5] = Services::Events::ExtractArgument<int32_t>(args);
        eff->m_nParamInteger[4] = Services::Events::ExtractArgument<int32_t>(args);
        eff->m_nParamInteger[3] = Services::Events::ExtractArgument<int32_t>(args);
        eff->m_nParamInteger[2] = Services::Events::ExtractArgument<int32_t>(args);
        eff->m_nParamInteger[1] = Services::Events::ExtractArgument<int32_t>(args);
        eff->m_nParamInteger[0] = Services::Events::ExtractArgument<int32_t>(args);

        eff->m_nCasterLevel       = Services::Events::ExtractArgument<int32_t>(args);
        eff->m_bShowIcon          = Services::Events::ExtractArgument<int32_t>(args);
        eff->m_bExpose            = Services::Events::ExtractArgument<int32_t>(args);
        eff->m_nSpellId           = Services::Events::ExtractArgument<int32_t>(args);
        eff->m_oidCreator         = Services::Events::ExtractArgument<ObjectID>(args);
        eff->m_nExpiryTimeOfDay   = Services::Events::ExtractArgument<int32_t>(args);
        eff->m_nExpiryCalendarDay = Services::Events::ExtractArgument<int32_t>(args);
        eff->m_fDuration          = Services::Events::ExtractArgument<float>(args);
        eff->m_nSubType           = Services::Events::ExtractArgument<int32_t>(args);
    }


    return Services::Events::Arguments();
}

int32_t Arelith::CNWSCreature__GetUseMonkAbilities_hook(CNWSCreature *pThis)
{
    if ( pThis->m_bIsPolymorphed)
    {
        if(g_plugin->polymorph.empty())
            return false;
        else
        {
            auto effectList = pThis->m_appliedEffects;
            for (int32_t i = 0; i < effectList.num; i++)
            {
                auto *eff = effectList.element[i];
                if(eff->m_nType == Constants::EffectTrueType::Polymorph)
                {
                    auto it = std::find (g_plugin->polymorph.begin(), g_plugin->polymorph.end(), eff->m_nParamInteger[0]);
                    if(it != g_plugin->polymorph.end())
                        return false;
                }

            }
        }
    }
    return g_plugin->pGetUseMonkAbilities_hook->CallOriginal<int32_t>(pThis);
}
ArgumentStack Arelith::SetDisableMonkAbilitiesPolymorph(ArgumentStack&& args)
{
    g_plugin->polymorph.push_back(Services::Events::ExtractArgument<int32_t>(args));
    return Services::Events::Arguments();
}
}