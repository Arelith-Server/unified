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

//material = the damage reduction penalty
//all other properties are for the property which bypasses it, fields with -1 are ignored
//reverese changes it from a vulnerability to a resistance
//you may set up multiple properties to bypass a material
void NWNX_Arelith_SetDamageReductionBypass(int material, int propertyType, int subType=-1, int costValue=-1, int paramValue=-1, int reverse=FALSE);

//bypass -> true bypasses all (effect level) immunities
//false stops bypassing immunities
void NWNX_Arelith_SetEffectImmunityBypass(int bypass);

int NWNX_Arelith_GetTrueEffectCount(object oObject);


//-1 if defender has no immunity, 2 if the defender is immune
//Should only be called in spellscripts
int NWNX_Arelith_DoSpellImmunity(object oDefender, object oCaster);

//Should only be called in spell scripts
//-1 if defender has no absorption/immunity Returns 2 if defender is immune via absorption effect
// Returns 3 if the defender is immune and the absorption effect has a limit.
int NWNX_Arelith_DoSpellLevelAbsorption(object oDefender, object oCaster);

//the id can be gained from unpacking the effect
//nay only work on spell-based/effects applied through ApplyEffectToObject, test other instances throughly.
int NWNX_Arelith_RemoveEffectById(object oObject,  string sID);

// Replaces effect at array with new effect struct e on object oObject
void NWNX_Arelith_ReplaceEffect(object oObject, int array, struct  NWNX_EffectUnpackedAre e);

// Disables monk abilities under polymorph.
void NWNX_Arelith_SetDisableMonkAbilitiesPolymorph(int nPolymorphType);


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

    string sItemPropId;

    string sTag; ///< @todo Describe
};

//Gets the active property from the index
struct NWNX_RAWIP NWNX_Arelith_GetActiveProperty(object item, int index);

//Gets the true effect at array spot effectNumbr, will loop through item proeprties and effects.
struct NWNX_EffectUnpackedAre NWNX_Arelith_GetTrueEffect(object oObject, int effectNumber);

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

void NWNX_Arelith_SetEffectImmunityBypass(int bypass)
{
    string sFunc = "SetEffectImmunityBypass";
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, bypass);
    NWNX_CallFunction(ARELITH_PLUGIN, sFunc);
}

int NWNX_Arelith_GetTrueEffectCount(object oObject)
{
    string sFunc = "GetTrueEffectCount";
    NWNX_PushArgumentObject(ARELITH_PLUGIN, sFunc, oObject);
    NWNX_CallFunction(ARELITH_PLUGIN, sFunc);

    return  NWNX_GetReturnValueInt(ARELITH_PLUGIN,sFunc);
}

struct NWNX_EffectUnpackedAre NWNX_Arelith_GetTrueEffect(object oObject, int effectNumber)
{
    string sFunc = "GetTrueEffect";
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, effectNumber);
    NWNX_PushArgumentObject(ARELITH_PLUGIN, sFunc, oObject);
    NWNX_CallFunction(ARELITH_PLUGIN, sFunc);

    struct NWNX_EffectUnpackedAre n;
    n.sTag = NWNX_GetReturnValueString(ARELITH_PLUGIN, sFunc);

    n.sItemPropId = NWNX_GetReturnValueString(ARELITH_PLUGIN, sFunc);

    n.oParam3 = NWNX_GetReturnValueObject(ARELITH_PLUGIN, sFunc);
    n.oParam2 = NWNX_GetReturnValueObject(ARELITH_PLUGIN, sFunc);
    n.oParam1 = NWNX_GetReturnValueObject(ARELITH_PLUGIN, sFunc);
    n.oParam0 = NWNX_GetReturnValueObject(ARELITH_PLUGIN, sFunc);
    n.sParam5 = NWNX_GetReturnValueString(ARELITH_PLUGIN, sFunc);
    n.sParam4 = NWNX_GetReturnValueString(ARELITH_PLUGIN, sFunc);
    n.sParam3 = NWNX_GetReturnValueString(ARELITH_PLUGIN, sFunc);
    n.sParam2 = NWNX_GetReturnValueString(ARELITH_PLUGIN, sFunc);
    n.sParam1 = NWNX_GetReturnValueString(ARELITH_PLUGIN, sFunc);
    n.sParam0 = NWNX_GetReturnValueString(ARELITH_PLUGIN, sFunc);
    n.fParam3 = NWNX_GetReturnValueFloat(ARELITH_PLUGIN, sFunc);
    n.fParam2 = NWNX_GetReturnValueFloat(ARELITH_PLUGIN, sFunc);
    n.fParam1 = NWNX_GetReturnValueFloat(ARELITH_PLUGIN, sFunc);
    n.fParam0 = NWNX_GetReturnValueFloat(ARELITH_PLUGIN, sFunc);
    n.nParam7 = NWNX_GetReturnValueInt(ARELITH_PLUGIN, sFunc);
    n.nParam6 = NWNX_GetReturnValueInt(ARELITH_PLUGIN, sFunc);
    n.nParam5 = NWNX_GetReturnValueInt(ARELITH_PLUGIN, sFunc);
    n.nParam4 = NWNX_GetReturnValueInt(ARELITH_PLUGIN, sFunc);
    n.nParam3 = NWNX_GetReturnValueInt(ARELITH_PLUGIN, sFunc);
    n.nParam2 = NWNX_GetReturnValueInt(ARELITH_PLUGIN, sFunc);
    n.nParam1 = NWNX_GetReturnValueInt(ARELITH_PLUGIN, sFunc);
    n.nParam0 = NWNX_GetReturnValueInt(ARELITH_PLUGIN, sFunc);
    n.nNumIntegers = NWNX_GetReturnValueInt(ARELITH_PLUGIN, sFunc);


    n.nCasterLevel = NWNX_GetReturnValueInt(ARELITH_PLUGIN, sFunc);
    n.bShowIcon = NWNX_GetReturnValueInt(ARELITH_PLUGIN, sFunc);
    n.bExpose = NWNX_GetReturnValueInt(ARELITH_PLUGIN, sFunc);
    n.nSpellId = NWNX_GetReturnValueInt(ARELITH_PLUGIN, sFunc);
    n.oCreator = NWNX_GetReturnValueObject(ARELITH_PLUGIN, sFunc);

    n.nExpiryTimeOfDay = NWNX_GetReturnValueInt(ARELITH_PLUGIN, sFunc);
    n.nExpiryCalendarDay = NWNX_GetReturnValueInt(ARELITH_PLUGIN, sFunc);
    n.fDuration = NWNX_GetReturnValueFloat(ARELITH_PLUGIN, sFunc);

    n.nSubType = NWNX_GetReturnValueInt(ARELITH_PLUGIN, sFunc);
    n.nType = NWNX_GetReturnValueInt(ARELITH_PLUGIN, sFunc);
    n.sID = NWNX_GetReturnValueString(ARELITH_PLUGIN, sFunc);
    return n;
}

int NWNX_Arelith_DoSpellImmunity(object oDefender, object oCaster)
{
    string sFunc = "DoSpellImmunity";
    NWNX_PushArgumentObject(ARELITH_PLUGIN, sFunc, oCaster);
    NWNX_PushArgumentObject(ARELITH_PLUGIN, sFunc, oDefender);
    NWNX_CallFunction(ARELITH_PLUGIN, sFunc);

    return  NWNX_GetReturnValueInt(ARELITH_PLUGIN,sFunc);
}

int NWNX_Arelith_DoSpellLevelAbsorption(object oDefender, object oCaster)
{
    string sFunc = "DoSpellLevelAbsorption";
    NWNX_PushArgumentObject(ARELITH_PLUGIN, sFunc, oCaster);
    NWNX_PushArgumentObject(ARELITH_PLUGIN, sFunc, oDefender);
    NWNX_CallFunction(ARELITH_PLUGIN, sFunc);

    return  NWNX_GetReturnValueInt(ARELITH_PLUGIN,sFunc);
}

int NWNX_Arelith_RemoveEffectById(object oObject,  string sID)
{
    string sFunc = "RemoveEffectById";
    NWNX_PushArgumentString(ARELITH_PLUGIN, sFunc, sID);
    NWNX_PushArgumentObject(ARELITH_PLUGIN, sFunc, oObject);
    NWNX_CallFunction(ARELITH_PLUGIN, sFunc);

    return  NWNX_GetReturnValueInt(ARELITH_PLUGIN,sFunc);
}

void NWNX_Arelith_ReplaceEffect(object oObject, int array, struct  NWNX_EffectUnpackedAre e)
{
    string sFunc = "ReplaceEffect";
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, e.nType);
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, e.nSubType);

    NWNX_PushArgumentFloat(ARELITH_PLUGIN, sFunc, e.fDuration);
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, e.nExpiryCalendarDay);
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, e.nExpiryTimeOfDay);

    NWNX_PushArgumentObject(ARELITH_PLUGIN, sFunc, e.oCreator);
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, e.nSpellId);
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, e.bExpose);
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, e.bShowIcon);
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, e.nCasterLevel);

    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, e.nParam0);
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, e.nParam1);
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, e.nParam2);
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, e.nParam3);
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, e.nParam4);
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, e.nParam5);
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, e.nParam6);
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, e.nParam7);
    NWNX_PushArgumentFloat(ARELITH_PLUGIN, sFunc, e.fParam0);
    NWNX_PushArgumentFloat(ARELITH_PLUGIN, sFunc, e.fParam1);
    NWNX_PushArgumentFloat(ARELITH_PLUGIN, sFunc, e.fParam2);
    NWNX_PushArgumentFloat(ARELITH_PLUGIN, sFunc, e.fParam3);
    NWNX_PushArgumentString(ARELITH_PLUGIN, sFunc, e.sParam0);
    NWNX_PushArgumentString(ARELITH_PLUGIN, sFunc, e.sParam1);
    NWNX_PushArgumentString(ARELITH_PLUGIN, sFunc, e.sParam2);
    NWNX_PushArgumentString(ARELITH_PLUGIN, sFunc, e.sParam3);
    NWNX_PushArgumentString(ARELITH_PLUGIN, sFunc, e.sParam4);
    NWNX_PushArgumentString(ARELITH_PLUGIN, sFunc, e.sParam5);
    NWNX_PushArgumentObject(ARELITH_PLUGIN, sFunc, e.oParam0);
    NWNX_PushArgumentObject(ARELITH_PLUGIN, sFunc, e.oParam1);
    NWNX_PushArgumentObject(ARELITH_PLUGIN, sFunc, e.oParam2);
    NWNX_PushArgumentObject(ARELITH_PLUGIN, sFunc, e.oParam3);

    NWNX_PushArgumentString(ARELITH_PLUGIN, sFunc, e.sTag);

    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, array);
    NWNX_PushArgumentObject(ARELITH_PLUGIN, sFunc, oObject);
    NWNX_CallFunction(ARELITH_PLUGIN, sFunc);

}

void NWNX_Arelith_SetDisableMonkAbilitiesPolymorph(int nPolymorphType)
{
    string sFunc = "SetDisableMonkAbilitiesPolymorph";
    NWNX_PushArgumentInt(ARELITH_PLUGIN, sFunc, nPolymorphType);
    NWNX_CallFunction(ARELITH_PLUGIN, sFunc);
}