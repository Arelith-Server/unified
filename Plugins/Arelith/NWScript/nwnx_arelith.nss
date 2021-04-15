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

// Gets attacker's weapon power against versus. AKA the AB/enhancement on the weapon.
int NWNX_Arelith_GetWeaponPower(object attacker, object versus, int offHand=FALSE);

//Sets host and path for webhook events
void NWNX_Arelith_SetWebhook(string host, string path, string addendum="");

// Handles concealment and miss chance resolution
// 0/FALSE if the attack hits 1/TRUE if the attack misses
int NWNX_Arelith_ResolveDefensiveEffects(object attacker, object versus, int attackHit=TRUE);

//material = the damage reduction penalty
//all other properties are for the property which bypasses it, fields with -1 are ignored
//reverese changes it from a vulnerability to a resistance
//you may set up multiple properties to bypass a material
void NWNX_Arelith_SetDamageReductionBypass(int material, int propertyType, int subType=-1, int costValue=-1, int paramValue=-1, int reverse=FALSE);

// Disables monk abilities under polymorph.
void NWNX_Arelith_SetDisableMonkAbilitiesPolymorph(int nPolymorphType);

void NWNX_Arelith_SetDispelResistanceModifier(object oCreature, int nClass, int nModifier, int bPersist = FALSE);

struct NWNX_EffectUnpackedAre
{
    string sID;
    int nType; ///< @todo Describe
    int nSubType; ///< @todo Describe

    float fDuration; ///< @todo Describe
    int nExpiryCalendarDay; ///< @todo Describe
    int nExpiryTimeOfDay; ///< @todo Describe

    object oCreator; ///< @todo Describe
    int nSpellId; ///< @todo Describe
    int bExpose; ///< @todo Describe
    int bShowIcon; ///< @todo Describe
    int nCasterLevel; ///< @todo Describe

    int nNumIntegers; ///< @todo Describe
    int nParam0; ///< @todo Describe
    int nParam1; ///< @todo Describe
    int nParam2; ///< @todo Describe
    int nParam3; ///< @todo Describe
    int nParam4; ///< @todo Describe
    int nParam5; ///< @todo Describe
    int nParam6; ///< @todo Describe
    int nParam7; ///< @todo Describe
    float fParam0; ///< @todo Describe
    float fParam1; ///< @todo Describe
    float fParam2; ///< @todo Describe
    float fParam3; ///< @todo Describe
    string sParam0; ///< @todo Describe
    string sParam1; ///< @todo Describe
    string sParam2; ///< @todo Describe
    string sParam3; ///< @todo Describe
    string sParam4; ///< @todo Describe
    string sParam5; ///< @todo Describe
    object oParam0; ///< @todo Describe
    object oParam1; ///< @todo Describe
    object oParam2; ///< @todo Describe
    object oParam3; ///< @todo Describe
    vector vParam0; ///< @todo Describe
    vector vParam1; ///< @todo Describe

    string sItemPropId;

    string sTag; ///< @todo Describe
};

//Gets the true effect at array spot effectNumbr, will loop through item proeprties and effects.
struct NWNX_EffectUnpackedAre NWNX_Arelith_GetTrueEffect(object oObject, int effectNumber);

const string ARELITH_PLUGIN = "NWNX_Arelith";

void NWNX_Arelith_SubscribeEvent(string evt, string script)
{
    string sFunc = "OnSubscribeEvent";
    NWNX_PushArgumentString(script);
    NWNX_PushArgumentString(evt);
    NWNX_CallFunction(ARELITH_PLUGIN, sFunc);
}

void NWNX_Arelith_PushEventData(string tag, string data)
{
    string sFunc = "OnPushEventData";
    NWNX_PushArgumentString(data);
    NWNX_PushArgumentString(tag);
    NWNX_CallFunction(ARELITH_PLUGIN, sFunc);
}

int NWNX_Arelith_SignalEvent(string evt, object target)
{
    string sFunc = "OnSignalEvent";
    NWNX_PushArgumentObject(target);
    NWNX_PushArgumentString(evt);
    NWNX_CallFunction("NWNX_Arelith", sFunc);
    return NWNX_GetReturnValueInt();
}

string NWNX_Arelith_GetEventData(string tag)
{
    string sFunc = "OnGetEventData";
    NWNX_PushArgumentString(tag);
    NWNX_CallFunction("NWNX_Arelith", sFunc);
    return NWNX_GetReturnValueString();
}

void NWNX_Arelith_SkipEvent()
{
    NWNX_CallFunction("NWNX_Arelith", "OnSkipEvent");
}

void NWNX_Arelith_SetEventResult(string data)
{
    NWNX_PushArgumentString(data);
    NWNX_CallFunction("NWNX_Arelith", "OnEventResult");
}

string NWNX_Arelith_GetCurrentEvent()
{
    NWNX_CallFunction("NWNX_Arelith", "OnGetCurrentEvent");
    return NWNX_GetReturnValueString();
}

int NWNX_Arelith_GetAttackModifierVersus(object attacker, object versus=OBJECT_INVALID)
{
    NWNX_PushArgumentObject(versus);
    NWNX_PushArgumentObject(attacker);
    NWNX_CallFunction("NWNX_Arelith", "GetAttackModifierVersus");

    return NWNX_GetReturnValueInt();
}

int NWNX_Arelith_GetWeaponPower(object attacker, object versus, int offHand=FALSE)
{
    NWNX_PushArgumentInt(offHand);
    NWNX_PushArgumentObject(versus);
    NWNX_PushArgumentObject(attacker);
    NWNX_CallFunction("NWNX_Arelith", "GetWeaponPower");

    return NWNX_GetReturnValueInt();
}

int NWNX_Arelith_ResolveDefensiveEffects(object attacker, object versus, int attackHit=TRUE)
{
    NWNX_PushArgumentInt(attackHit);
    NWNX_PushArgumentObject(versus);
    NWNX_PushArgumentObject(attacker);
    NWNX_CallFunction("NWNX_Arelith", "ResolveDefensiveEffects");

    return NWNX_GetReturnValueInt();
}

void NWNX_Arelith_SetWebhook(string host, string path, string addendum="")
{
    string sFunc = "SetWebhook";
    NWNX_PushArgumentString(path);
    NWNX_PushArgumentString(host);
    NWNX_PushArgumentString(addendum);
    NWNX_CallFunction(ARELITH_PLUGIN, sFunc);
}

void NWNX_Arelith_SetDamageReductionBypass(int material, int propertyType, int subType=-1, int costValue=-1, int paramValue=-1, int reverse=FALSE)
{
    string sFunc = "SetDamageReductionBypass";
    NWNX_PushArgumentInt(reverse);
    NWNX_PushArgumentInt(paramValue);
    NWNX_PushArgumentInt(costValue);
    NWNX_PushArgumentInt(subType);
    NWNX_PushArgumentInt(propertyType);
    NWNX_PushArgumentInt(material);

    NWNX_CallFunction(ARELITH_PLUGIN, sFunc);
}


void NWNX_Arelith_SetDisableMonkAbilitiesPolymorph(int nPolymorphType)
{
    string sFunc = "SetDisableMonkAbilitiesPolymorph";
    NWNX_PushArgumentInt(nPolymorphType);
    NWNX_CallFunction(ARELITH_PLUGIN, sFunc);
}

void NWNX_Arelith_SetDispelResistanceModifier(object oCreature, int nClass, int nModifier, int bPersist = FALSE)
{
    string sFunc = "SetDispelResistanceModifier";

    NWNX_PushArgumentInt(bPersist);
    NWNX_PushArgumentInt(nModifier);
    NWNX_PushArgumentInt(nClass);
    NWNX_PushArgumentObject(oCreature);

    NWNX_CallFunction(ARELITH_PLUGIN, sFunc);
}
