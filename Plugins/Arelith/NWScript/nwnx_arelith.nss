#include "nwnx"

// Scripts can subscribe to events.
// Some events are dispatched via the NWNX plugin (see NWNX_Arelith_EVENT_* constants).
// Others can be signaled via script code (see NWNX_Arelith_SignalEvent).
void NWNX_Arelith_SubscribeEvent(string evt, string script);

// Pushes event data at the provided tag, which subscribers can access with GetEventData.
// This should be called BEFORE SignalEvent.
void NWNX_Arelith_PushEventData(string tag, string data);

// Signals an event. This will dispatch a notification to all subscribed handlers.
// Returns TRUE if anyone was subscribed to the event, FALSE otherwise.
int NWNX_Arelith_SignalEvent(string evt, object target);

// Retrieves the event data for the currently executing script.
// THIS SHOULD ONLY BE CALLED FROM WITHIN AN EVENT HANDLER.
string NWNX_Arelith_GetEventData(string tag);

// Skips execution of the currently executing event.
// If this is a NWNX event, that means that the base function call won't be called.
// This won't impact any other subscribers, nor dispatch for before / after functions.
// For example, if you are subscribing to NWNX_ON_EXAMINE_OBJECT_BEFORE, and you skip ...
// - The other subscribers will still be called.
// - The original function in the base game will be skipped.
// - The matching after event (NWNX_ON_EXAMINE_OBJECT_AFTER) will also be executed.
//
// THIS SHOULD ONLY BE CALLED FROM WITHIN AN EVENT HANDLER.
// ONLY WORKS WITH THE FOLLOWING EVENTS:
void NWNX_Arelith_SkipEvent();

// Set the return value of the event.
//
// THIS SHOULD ONLY BE CALLED FROM WITHIN AN EVENT HANDLER.
// ONLY WORKS WITH THE FOLLOWING EVENTS:
// - CanUseItem Event
void NWNX_Arelith_SetEventResult(string data);

// Returns the current event name
//
// THIS SHOULD ONLY BE CALLED FROM WITHIN AN EVENT HANDLER.
string NWNX_Arelith_GetCurrentEvent();

// Gets the attackers attack modifier versus versus.
// Seems to be 10 + Ability + Feats + Effects + Item Properties.
int NWNX_Arelith_GetAttackModifierVersus(object attacker, object versus=OBJECT_INVALID);

// Gets the Armor classed of attacked against versus
// Touch attack should be true if you want touch attack AC.
int NWNX_Arelith_GetArmorClassVersus(object attacked, int touchAttack=FALSE, object versus=OBJECT_INVALID);

// Gets attacker's weapon power against versus. AKA the AB/enhancement on the weapon.
int NWNX_Arelith_GetWeaponPower(object attacker, object versus, int offHand=FALSE);

//Sets host and path for webhook events
void NWNX_Arelith_SetWebhook(string host, string path, string addendum="");

// Handles concealment and miss chance resolution
// 0/FALSE if the attack hits 1/TRUE if the attack misses
int NWNX_Arelith_ResolveDefensiveEffects(object attacker, object versus, int attackHit=TRUE);

// Gets the caster level of last item used
int NWNX_Arelith_GetLastItemCasterLevel(object creature);

//Sets caster level for the last item used, use in Events spellhook before to set item caster level.
void NWNX_Arelith_SetLastItemCasterLevel(object creature, int casterLvl);

void NWNX_Arelith_SetDamageReductionBypass(int material, int propertyType, int subType=-1, int costValue=-1, int paramValue=-1, int reverse=FALSE);

/// @brief An unpacked itemproperty.
struct NWNX_RAWIP
{
    int nProperty; ///< @todo Describe
    int nSubType; ///< @todo Describe
    int nCostTable; ///< @todo Describe
    int nCostTableValue; ///< @todo Describe
    int nParam1; ///< @todo Describe
    int nParam1Value; ///< @todo Describe
    int nUsesPerDay; ///< @todo Describe
    int nChanceToAppear; ///< @todo Describe
    int bUsable; ///< @todo Describe
    string sTag; ///< @todo Describe
};

struct NWNX_RAWIP NWNX_Arelith_GetActiveProperty(object item, int index);

const string ARELITH_PLUGIN = "NWNX_Arelith";

void NWNX_Arelith_SubscribeEvent(string evt, string script)
{
    string sFunc = "OnSubscribeEvent";
    NWNX_PushArgumentString("NWNX_Arelith", sFunc, script);
    NWNX_PushArgumentString("NWNX_Arelith", sFunc, evt);
    NWNX_CallFunction("NWNX_Arelith", sFunc);
}

void NWNX_Arelith_PushEventData(string tag, string data)
{
    string sFunc = "OnPushEventData";
    NWNX_PushArgumentString("NWNX_Arelith", sFunc, data);
    NWNX_PushArgumentString("NWNX_Arelith", sFunc, tag);
    NWNX_CallFunction("NWNX_Arelith", sFunc);
}

int NWNX_Arelith_SignalEvent(string evt, object target)
{
    string sFunc = "OnSignalEvent";
    NWNX_PushArgumentObject("NWNX_Arelith", sFunc, target);
    NWNX_PushArgumentString("NWNX_Arelith", sFunc, evt);
    NWNX_CallFunction("NWNX_Arelith", sFunc);
    return NWNX_GetReturnValueInt("NWNX_Arelith", sFunc);
}

string NWNX_Arelith_GetEventData(string tag)
{
    string sFunc = "OnGetEventData";
    NWNX_PushArgumentString("NWNX_Arelith", sFunc, tag);
    NWNX_CallFunction("NWNX_Arelith", sFunc);
    return NWNX_GetReturnValueString("NWNX_Arelith", sFunc);
}

void NWNX_Arelith_SkipEvent()
{
    NWNX_CallFunction("NWNX_Arelith", "OnSkipEvent");
}

void NWNX_Arelith_SetEventResult(string data)
{
    NWNX_PushArgumentString("NWNX_Arelith", "OnEventResult", data);
    NWNX_CallFunction("NWNX_Arelith", "OnEventResult");
}

string NWNX_Arelith_GetCurrentEvent()
{
    NWNX_CallFunction("NWNX_Arelith", "OnGetCurrentEvent");
    return NWNX_GetReturnValueString("NWNX_Arelith", "OnGetCurrentEvent");
}

int NWNX_Arelith_GetAttackModifierVersus(object attacker, object versus=OBJECT_INVALID)
{
    NWNX_PushArgumentObject("NWNX_Arelith", "GetAttackModifierVersus", versus);
    NWNX_PushArgumentObject("NWNX_Arelith", "GetAttackModifierVersus", attacker);
    NWNX_CallFunction("NWNX_Arelith", "GetAttackModifierVersus");

    return NWNX_GetReturnValueInt("NWNX_Arelith", "GetAttackModifierVersus");
}

int NWNX_Arelith_GetArmorClassVersus(object attacked, int touchAttack=FALSE, object versus=OBJECT_INVALID)
{
    NWNX_PushArgumentInt("NWNX_Arelith", "GetArmorClassVersus", touchAttack);
    NWNX_PushArgumentObject("NWNX_Arelith", "GetArmorClassVersus", versus);
    NWNX_PushArgumentObject("NWNX_Arelith", "GetArmorClassVersus", attacked);
    NWNX_CallFunction("NWNX_Arelith", "GetArmorClassVersus");

    return NWNX_GetReturnValueInt("NWNX_Arelith", "GetArmorClassVersus");
}

int NWNX_Arelith_GetWeaponPower(object attacker, object versus, int offHand=FALSE)
{
    NWNX_PushArgumentInt("NWNX_Arelith", "GetWeaponPower", offHand);
    NWNX_PushArgumentObject("NWNX_Arelith", "GetWeaponPower", versus);
    NWNX_PushArgumentObject("NWNX_Arelith", "GetWeaponPower", attacker);
    NWNX_CallFunction("NWNX_Arelith", "GetWeaponPower");

    return NWNX_GetReturnValueInt("NWNX_Arelith", "GetWeaponPower");
}

int NWNX_Arelith_ResolveDefensiveEffects(object attacker, object versus, int attackHit=TRUE)
{
    NWNX_PushArgumentInt("NWNX_Arelith", "ResolveDefensiveEffects", attackHit);
    NWNX_PushArgumentObject("NWNX_Arelith", "ResolveDefensiveEffects", versus);
    NWNX_PushArgumentObject("NWNX_Arelith", "ResolveDefensiveEffects", attacker);
    NWNX_CallFunction("NWNX_Arelith", "ResolveDefensiveEffects");

    return NWNX_GetReturnValueInt("NWNX_Arelith", "ResolveDefensiveEffects");
}

void NWNX_Arelith_SetWebhook(string host, string path, string addendum="")
{
    string sFunc = "SetWebhook";
    NWNX_PushArgumentString(ARELITH_PLUGIN, sFunc, path);
    NWNX_PushArgumentString(ARELITH_PLUGIN, sFunc, host);
    NWNX_PushArgumentString(ARELITH_PLUGIN, sFunc, addendum);
    NWNX_CallFunction(ARELITH_PLUGIN, sFunc);
}

int NWNX_Arelith_GetLastItemCasterLevel(object creature)
{
    string sFunc = "GetLastItemCasterLevel";
    NWNX_PushArgumentObject(ARELITH_PLUGIN, sFunc, creature);
    NWNX_CallFunction(ARELITH_PLUGIN, sFunc);

    return NWNX_GetReturnValueInt(ARELITH_PLUGIN, sFunc);
}

void NWNX_Arelith_SetLastItemCasterLevel(object creature, int casterLvl)
{
    string sFunc = "SetLastItemCasterLevel";
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, casterLvl);
    NWNX_PushArgumentObject(ARELITH_PLUGIN, sFunc, creature);
    NWNX_CallFunction(ARELITH_PLUGIN, sFunc);
}

struct NWNX_RAWIP NWNX_Arelith_GetActiveProperty(object item, int index)
{
    string sFunc = "GetActiveProperty";
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, index);
    NWNX_PushArgumentObject(ARELITH_PLUGIN, sFunc, item);
    NWNX_CallFunction(ARELITH_PLUGIN, sFunc);

    struct NWNX_RAWIP n;

    n.sTag=NWNX_GetReturnValueString(ARELITH_PLUGIN, sFunc);
    n.bUsable=NWNX_GetReturnValueInt(ARELITH_PLUGIN,sFunc);
    n.nChanceToAppear=NWNX_GetReturnValueInt(ARELITH_PLUGIN,sFunc);
    n.nUsesPerDay=NWNX_GetReturnValueInt(ARELITH_PLUGIN,sFunc);
    n.nParam1Value=NWNX_GetReturnValueInt(ARELITH_PLUGIN,sFunc);
    n.nParam1=NWNX_GetReturnValueInt(ARELITH_PLUGIN,sFunc);
    n.nCostTableValue=NWNX_GetReturnValueInt(ARELITH_PLUGIN,sFunc);
    n.nCostTable=NWNX_GetReturnValueInt(ARELITH_PLUGIN,sFunc);
    n.nSubType=NWNX_GetReturnValueInt(ARELITH_PLUGIN,sFunc);
    n.nProperty=NWNX_GetReturnValueInt(ARELITH_PLUGIN,sFunc);

    return n;
}

void NWNX_Arelith_SetDamageReductionBypass(int material, int propertyType, int subType=-1, int costValue=-1, int paramValue=-1, int reverse=FALSE)
{
    string sFunc = "SetDamageReductionBypass";
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, reverse);
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, paramValue);
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, costValue);
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, subType);
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, propertyType);
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, material);

    NWNX_CallFunction(ARELITH_PLUGIN, sFunc);
}