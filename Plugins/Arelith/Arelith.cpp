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
//#include "Services/Config/Config.hpp"
//#include "Services/Messaging/Messaging.hpp"
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
//#include "Services/Tasks/Tasks.hpp"
//#include "Services/Messaging/Messaging.hpp"
//#include "Encoding.hpp"
#include "API/CNWItemProperty.hpp"
#include <cmath>
#include <sstream>
#include <iomanip>
#include "External/httplib.h"
#include "API/CNWSInventory.hpp"
#include "API/CNWSAreaOfEffectObject.hpp"
#include "API/CNWSpellArray.hpp"

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
static Hooks::Hook s_GetEffectImmunityHook;
static Hooks::Hook s_GetUseMonkAbilitiesHook;
static Hooks::Hook s_WriteToLogFileHook;
static Hooks::Hook s_ReportErrorHook;
static Hooks::Hook s_OnApplyEffectImmunityHook;
static Hooks::Hook s_OnItemPropertyAppliedHook;
static Hooks::Hook s_SetCreatorHook;

Arelith::Arelith(Services::ProxyServiceList* services)
    : Plugin(services), m_eventDepth(0)
{

    if (g_plugin == nullptr) // :(
        g_plugin = this;

#define REGISTER(func) \
    Events::RegisterEvent(PLUGIN_NAME, #func, \
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
    //REGISTER(SetDamageReductionBypass);
    REGISTER(SetDisableMonkAbilitiesPolymorph);
#undef REGISTER

    MessageBus::Subscribe("NWNX_ARELITH_SIGNAL_EVENT",
        [](const std::vector<std::string> message)
        {
            ASSERT(message.size() == 2);
            SignalEvent(message[0], std::strtoul(message[1].c_str(), nullptr, 16));
        });

    MessageBus::Subscribe("NWNX_ARELITH_PUSH_EVENT_DATA",
        [](const std::vector<std::string> message)
        {
            ASSERT(message.size() == 2);
            PushEventData(message[0], message[1]);
        });


    m_arelithEvents   = std::make_unique<ArelithEvents>();

    s_ReportErrorHook = Hooks::HookFunction(Functions::_ZN25CNWVirtualMachineCommands11ReportErrorER10CExoStringi,
                                                        (void*)&ReportErrorHook, Hooks::Order::VeryEarly);

    s_WriteToLogFileHook = Hooks::HookFunction(Functions::_ZN17CExoDebugInternal14WriteToLogFileERK10CExoString,
                                                        (void*)&WriteToLogFileHook, Hooks::Order::VeryEarly);

    s_GetEffectImmunityHook = Hooks::HookFunction(Functions::_ZN17CNWSCreatureStats17GetEffectImmunityEhP12CNWSCreaturei,
        (void*)&GetEffectImmunityHook, Hooks::Order::Latest);

    s_OnItemPropertyAppliedHook = Hooks::HookFunction(Functions::_ZN15CServerAIMaster21OnItemPropertyAppliedEP8CNWSItemP15CNWItemPropertyP12CNWSCreatureji,
        (void*)&OnItemPropertyAppliedHook, Hooks::Order::Early);

    s_OnApplyEffectImmunityHook = Hooks::HookFunction(Functions::_ZN21CNWSEffectListHandler21OnApplyEffectImmunityEP10CNWSObjectP11CGameEffecti,
        (void*)&OnApplyEffectImmunityHook, Hooks::Order::Early);

    s_SetCreatorHook =
            Hooks::HookFunction(Functions::_ZN11CGameEffect10SetCreatorEj,
        (void*)&SetCreatorHook, Hooks::Order::Earliest);
   /* if(GetServices()->m_config->Get<bool>("DMG_RED", false))
    {
        GetServices()->m_hooks->RequestSharedHook<Functions::_ZN21CNWSEffectListHandler22OnApplyDamageReductionEP10CNWSObjectP11CGameEffecti, bool, CNWSEffectListHandler*, CNWSObject*, CGameEffect*, BOOL>(&OnApplyDamageReductionHook);
        GetServices()->m_hooks->RequestSharedHook<Functions::_ZN10CNWSObject17DoDamageReductionEP12CNWSCreatureihii, bool, CNWSObject*, CNWSCreature*, int32_t, uint8_t, BOOL, BOOL>(&DoDamageReductionHook);
    }*/

    if (Config::Get<bool>("POLYMORPH", false))
    {
        s_GetUseMonkAbilitiesHook = Hooks::HookFunction(Functions::_ZN12CNWSCreature19GetUseMonkAbilitiesEv,
        (void*)&CNWSCreature__GetUseMonkAbilities_hook, Hooks::Order::Early);
    }
    s_sHost = Config::Get<std::string>("HOST", "");
    s_sOrigPath = Config::Get<std::string>("PATH", "");
    s_sAdden = Config::Get<std::string>("ROLE", "");


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


    MessageBus::Broadcast("NWNX_ARELITH_SIGNAL_EVENT_RESULT",  { eventName, result ? *result : ""});
    MessageBus::Broadcast("NWNX_ARELITH_SIGNAL_EVENT_SKIPPED", { eventName, skipped ? "1" : "0"});
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

NWNX_EXPORT ArgumentStack Arelith::OnSubscribeEvent(ArgumentStack&& args)
{
    const auto event = Events::ExtractArgument<std::string>(args);
    auto script = Events::ExtractArgument<std::string>(args);

    RunEventInit(event);
    auto& eventVector = m_eventMap[event];

    if (std::find(std::begin(eventVector), std::end(eventVector), script) != std::end(eventVector))
    {
        throw std::runtime_error("Attempted to subscribe to an event with a script that already subscribed!");
    }

    LOG_INFO("Script '%s' subscribed to event '%s'.", script, event);
    eventVector.emplace_back(std::move(script));

    return {};
}

NWNX_EXPORT ArgumentStack Arelith::OnPushEventData(ArgumentStack&& args)
{
    const auto tag = Events::ExtractArgument<std::string>(args);
    const auto data = Events::ExtractArgument<std::string>(args);
    PushEventData(tag, data);
    return {};
}

NWNX_EXPORT ArgumentStack Arelith::OnSignalEvent(ArgumentStack&& args)
{
    const auto event = Events::ExtractArgument<std::string>(args);
    const auto object = Events::ExtractArgument<ObjectID>(args);
    bool signalled = SignalEvent(event, object);
    return Events::Argument(signalled ? 1 : 0);
}

NWNX_EXPORT ArgumentStack Arelith::OnGetEventData(ArgumentStack&& args)
{
    std::string data = GetEventData(Events::ExtractArgument<std::string>(args));
    return Events::Argument(data);
}

NWNX_EXPORT ArgumentStack Arelith::OnSkipEvent(ArgumentStack&&)
{
    if (m_eventDepth == 0 || m_eventData.empty())
    {
        throw std::runtime_error("Attempted to skip event in an invalid context.");
    }
    m_eventData.top().m_Skipped = true;

    LOG_DEBUG("Skipping last event.");

    return {};
}

NWNX_EXPORT ArgumentStack Arelith::OnEventResult(ArgumentStack&& args)
{
    if (m_eventDepth == 0 || m_eventData.empty())
    {
        throw std::runtime_error("Attempted to skip event in an invalid context.");
    }
    const auto data = Events::ExtractArgument<std::string>(args);

    m_eventData.top().m_Result = data;

    LOG_DEBUG("Received event result '%s'.", data);

    return {};
}

NWNX_EXPORT ArgumentStack Arelith::OnGetCurrentEvent(ArgumentStack&&)
{
    if (m_eventDepth == 0 || m_eventData.empty())
    {
        throw std::runtime_error("Attempted to get the current event in an invalid context.");
    }



    std::string eventName = g_plugin->m_eventData.top().m_EventName;
    return Events::Argument(eventName);
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
    const auto objectId = Events::ExtractArgument<ObjectID>(args);

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
    const auto creatureId = Events::ExtractArgument<ObjectID>(args);

    if (creatureId == Constants::OBJECT_INVALID)
    {
        LOG_NOTICE("NWNX_Creature function called on OBJECT_INVALID");
        return nullptr;
    }

    return Globals::AppManager()->m_pServerExoApp->GetCreatureByGameObjectID(creatureId);
}
NWNX_EXPORT ArgumentStack Arelith::GetWeaponPower(ArgumentStack&& args)
{
    int32_t retVal = -1;
    if (auto *pCreature = creature(args))
    {

        if (auto *versus = object(args))
        {
            const auto isOffhand = Events::ExtractArgument<int32_t>(args);
            retVal = pCreature->GetWeaponPower(versus, isOffhand);
        }


    }
    return Events::Argument(retVal);
}

NWNX_EXPORT ArgumentStack Arelith::GetAttackModifierVersus(ArgumentStack&& args)
{
    int32_t retVal = -1;
    if (auto *pCreature = creature(args))
    {
        if(auto *versus = creature(args))
            retVal = pCreature->m_pStats->GetAttackModifierVersus(versus);

    }
    return Events::Argument(retVal);
}
NWNX_EXPORT ArgumentStack Arelith::ResolveDefensiveEffects(ArgumentStack&& args)
{
    int32_t retVal = 0;
    if (auto *pCreature = creature(args))
    {
        if (auto *versus = object(args))
        {
            const auto isAttackHit = Events::ExtractArgument<int32_t>(args);
            retVal = pCreature->ResolveDefensiveEffects(versus, isAttackHit);
        }
    }
    return Events::Argument(retVal);
}


void Arelith::ReportErrorHook(CNWVirtualMachineCommands *pVirtualMachineCommands, CExoString *message, int32_t error)
{
    s_ReportErrorHook->CallOriginal<void>(pVirtualMachineCommands, message, error);
    if(s_sHost.empty() || s_sOrigPath.empty())
    {
        LOG_INFO("Host or path was empty.");
        return;
    }
    s_bSendError=true;
}

void Arelith::WriteToLogFileHook(CExoDebugInternal* pExoDebugInternal, CExoString* message)
{

    if(s_bSendError)
    {
        SendWebHookHTTPS(message->CStr());
        s_bSendError=false;
    }

    s_WriteToLogFileHook->CallOriginal<void>(pExoDebugInternal, message);
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
    message = String::Trim(message);
    message = message + " " + s_sAdden;
    message = R"({"text": ")" + message + "\"";
    message = message +  R"(, "mrkdwn": false)";
    message = message + "}";

    // For Discord, will wait for a response
    auto path = origPath + "?wait=true";

    message = String::ToUTF8(message);
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
        Tasks::QueueOnAsyncThread([cli, message, host, path, origPath]()
        {
            auto res = cli->second->post(path.c_str(), message, "application/json");
            Tasks::QueueOnMainThread([message, host, path, origPath, res]()
            {
                if (Core::g_CoreShuttingDown)
                    return;

              ;
                auto moduleOid = NWNXLib::Utils::ObjectIDToString(Utils::GetModule()->m_idSelf);

                if (res)
                {
                    MessageBus::Broadcast("NWNX_EVENT_PUSH_EVENT_DATA", {"STATUS", std::to_string(res->status)});
                    MessageBus::Broadcast("NWNX_EVENT_PUSH_EVENT_DATA", {"MESSAGE", message});
                    MessageBus::Broadcast("NWNX_EVENT_PUSH_EVENT_DATA", {"HOST", host});
                    MessageBus::Broadcast("NWNX_EVENT_PUSH_EVENT_DATA", {"PATH", origPath});
                    if (res->status == 200 || res->status == 201 || res->status == 204 || res->status == 429)
                    {
                        // Discord sends your rate limit information even on success so you can stagger calls if you want
                        // This header also lets us know it's Discord not Slack, important because Discord sends RETRY_AFTER
                        // in milliseconds and Slack sends it as seconds.
                        if (!res->get_header_value("X-RateLimit-Limit").empty())
                        {
                            MessageBus::Broadcast("NWNX_EVENT_PUSH_EVENT_DATA", {"RATELIMIT_LIMIT", res->get_header_value("X-RateLimit-Limit")});
                            MessageBus::Broadcast("NWNX_EVENT_PUSH_EVENT_DATA", {"RATELIMIT_REMAINING", res->get_header_value("X-RateLimit-Remaining")});
                            MessageBus::Broadcast("NWNX_EVENT_PUSH_EVENT_DATA", {"RATELIMIT_RESET", res->get_header_value("X-RateLimit-Reset")});
                            if (!res->get_header_value("Retry-After").empty())
                                MessageBus::Broadcast("NWNX_EVENT_PUSH_EVENT_DATA", {"RETRY_AFTER", res->get_header_value("Retry-After")});
                        }
                            // Slack rate limited
                        else if (!res->get_header_value("Retry-After").empty())
                        {
                            float fSlackRetry = stof(res->get_header_value("Retry-After")) * 1000.0f;
                            MessageBus::Broadcast("NWNX_EVENT_PUSH_EVENT_DATA", {"RETRY_AFTER", std::to_string(fSlackRetry)});
                        }
                        if (res->status != 429)
                        {
                            MessageBus::Broadcast("NWNX_EVENT_SIGNAL_EVENT", {"NWNX_ON_ARE_WEBHOOK_SUCCESS", moduleOid});
                            LOG_INFO("Sent webhook '%s' to '%s%s'.", message, host, path);
                        }
                        else
                        {
                            MessageBus::Broadcast("NWNX_EVENT_SIGNAL_EVENT", {"NWNX_ON_ARE_WEBHOOK_FAILED", moduleOid});
                            LOG_WARNING("Failed to send WebHook (HTTPS) message '%s' to '%s%s'. Rate Limited.", message, host, path);
                        }
                    }
                    else
                    {
                        MessageBus::Broadcast("NWNX_EVENT_PUSH_EVENT_DATA", {"FAIL_INFO", res->body});
                        MessageBus::Broadcast("NWNX_EVENT_SIGNAL_EVENT", {"NWNX_ON_ARE_WEBHOOK_FAILED", moduleOid});
                        LOG_WARNING("Failed to send WebHook (HTTPS) message '%s' to '%s%s', status code '%d'.", message, host, path, res->status);
                    }
                }
                else
                {
                    MessageBus::Broadcast("NWNX_EVENT_PUSH_EVENT_DATA", {"FAIL_INFO", "Failed to post to server. Is the url correct?"});
                    MessageBus::Broadcast("NWNX_EVENT_SIGNAL_EVENT", {"NWNX_ON_ARE_WEBHOOK_FAILED", moduleOid});
                    LOG_WARNING("Failed to send WebHook (HTTPS) to '%s%s'.", host, path);
                }
            });
        });
    }

}
NWNX_EXPORT ArgumentStack Arelith::SetWebhook(ArgumentStack&& args)
{
    s_sAdden = Events::ExtractArgument<std::string>(args);
    s_sHost = Events::ExtractArgument<std::string>(args);
    s_sOrigPath = Events::ExtractArgument<std::string>(args);
    return {};
}

int32_t Arelith::OnItemPropertyAppliedHook(CServerAIMaster* pServerAIMaster, CNWSItem* pItem, CNWItemProperty *pItemProperty, CNWSCreature* pCreature, uint32_t slot, BOOL bLoadingGame)
{
    if(pItemProperty->m_nParam1Value > 0 && pItemProperty->m_nParam1Value != 255)
    {
        if(pItemProperty->m_nPropertyName==Constants::ItemProperty::DamageReduction || pItemProperty->m_nPropertyName==Constants::ItemProperty::ImmunityMiscellaneous)
          s_iMaterial=pItemProperty->m_nParam1Value;

    }

   return s_OnItemPropertyAppliedHook->CallOriginal<int32_t>(pServerAIMaster, pItem, pItemProperty, pCreature, slot, bLoadingGame);
}
/*void Arelith::OnApplyDamageReductionHook(bool before, CNWSEffectListHandler*, CNWSObject*, CGameEffect* pEffect, BOOL)
{
    if(before && s_iMaterial > 0)
    {
        pEffect->SetInteger(3, s_iMaterial);
        s_iMaterial=0;
    }
}*/
int32_t Arelith::OnApplyEffectImmunityHook(CNWSEffectListHandler* pEffectListHandler, CNWSObject *pObject, CGameEffect * pEffect, BOOL bLoadingGame = false)
{
    if(s_iMaterial > 0)
    {
        pEffect->SetInteger(4, s_iMaterial);
        s_iMaterial=0;
    }
    return s_OnApplyEffectImmunityHook->CallOriginal<int32_t>(pEffectListHandler, pObject, pEffect, bLoadingGame);
}
/*void Arelith::DoDamageReductionHook(bool before, CNWSObject *pObject, CNWSCreature *pCreature, int32_t, uint8_t, BOOL, BOOL)
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
    auto material = Events::ExtractArgument<int32_t>(args);
    auto propType = Events::ExtractArgument<int32_t>(args);
    auto subType =  Events::ExtractArgument<int32_t>(args);
    auto costValue = Events::ExtractArgument<int32_t>(args);
    auto param1Value =  Events::ExtractArgument<int32_t>(args);
    auto reverse =  Events::ExtractArgument<int32_t>(args);
    bypassRed ip;
    ip.m_nPropertyName=propType;
    ip.m_nSubType=subType;
    ip.m_nParam1Value=param1Value;
    ip.m_nCostTableValue=costValue;
    ip.bReverse=reverse;
    m_bypass.insert(std::make_pair(material, ip));
    return Events::Argument();
}*/

NWNX_EXPORT ArgumentStack Arelith::SetDisableMonkAbilitiesPolymorph(ArgumentStack&& args)
{
    g_plugin->polymorph.push_back(Events::ExtractArgument<int32_t>(args));
    return {};
}
BOOL Arelith::GetEffectImmunityHook(CNWSCreatureStats *pStats, uint8_t nType, CNWSCreature * pVersus, BOOL bConsiderFeats)
{

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
    return s_GetEffectImmunityHook->CallOriginal<BOOL>(pStats, nType, pVersus, bConsiderFeats);
}
int32_t Arelith::CNWSCreature__GetUseMonkAbilities_hook(CNWSCreature* pThis)
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

    return s_GetUseMonkAbilitiesHook->CallOriginal<int32_t>(pThis);
}

void Arelith::SetCreatorHook(CGameEffect *pThis, ObjectID oidCreator)
{
    auto* pCreator = Utils::GetGameObject(oidCreator);
    pThis->m_oidCreator = oidCreator;

    if (auto* pCreatorObject = Utils::AsNWSObject(pCreator))
    {
        pThis->m_nSpellId = pCreatorObject->GetEffectSpellId();

        if (auto* pAoE = Utils::AsNWSAreaOfEffectObject(pCreatorObject))
        {
            pThis->m_oidCreator = pAoE->m_oidCreator;
        }
        else if (auto* pCreature = Utils::AsNWSCreature(pCreatorObject))
        {
            if (pCreature->m_bLastSpellCast)
            {
                if (auto* pSpell = Globals::Rules()->m_pSpellArray->GetSpell(pCreature->m_nLastSpellId))
                {
                    if (pCreature->m_bLastItemCastSpell)
                    {
                        pThis->m_nCasterLevel = 150+pCreature->m_nLastItemCastSpellLevel;
                    }
                    else if (pCreature->m_nLastSpellCastMulticlass == Constants::ClassType::MAX)
                    {
                        pThis->m_nCasterLevel = pCreature->m_pStats->GetSpellLikeAbilityCasterLevel(pCreature->m_nLastSpellId);
                    }
                    else
                    {
                        pThis->m_nCasterLevel = pCreature->m_pStats->GetClassLevel(pCreature->m_nLastSpellCastMulticlass, 0);
                    }
                }
            }
        }
    }
}

}