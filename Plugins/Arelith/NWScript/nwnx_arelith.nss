#include "nwnx_effect"
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

struct NWNX_Arelith_ItemPosition
{
    int X;
    int Y;
};

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


const string ARELITH_PLUGIN = "NWNX_Arelith";

void NWNX_Arelith_SubscribeEvent(string evt, string script)
{
    string sFunc = "OnSubscribeEvent";
    NWNXPushString(script);
    NWNXPushString(evt);
    NWNXCall(ARELITH_PLUGIN, sFunc);
}

void NWNX_Arelith_PushEventData(string tag, string data)
{
    string sFunc = "OnPushEventData";
    NWNXPushString(data);
    NWNXPushString(tag);
    NWNXCall(ARELITH_PLUGIN, sFunc);
}

int NWNX_Arelith_SignalEvent(string evt, object target)
{
    string sFunc = "OnSignalEvent";
    NWNXPushObject(target);
    NWNXPushString(evt);
    NWNXCall("NWNX_Arelith", sFunc);
    return NWNXPopInt();
}

string NWNX_Arelith_GetEventData(string tag)
{
    string sFunc = "OnGetEventData";
    NWNXPushString(tag);
    NWNXCall("NWNX_Arelith", sFunc);
    return NWNXPopString();
}

void NWNX_Arelith_SkipEvent()
{
    NWNXCall("NWNX_Arelith", "OnSkipEvent");
}

void NWNX_Arelith_SetEventResult(string data)
{
    NWNXPushString(data);
    NWNXCall("NWNX_Arelith", "OnEventResult");
}

string NWNX_Arelith_GetCurrentEvent()
{
    NWNXCall("NWNX_Arelith", "OnGetCurrentEvent");
    return NWNXPopString();
}

struct NWNX_Arelith_ItemPosition NWNX_Arelith_GetItemPosition(object oItem)
{
    NWNXPushObject(oItem);
    NWNXCall(ARELITH_PLUGIN, "GetItemPosition");

    struct NWNX_Arelith_ItemPosition stItemPos;
    stItemPos.X = NWNXPopInt();
    stItemPos.Y = NWNXPopInt();

    return stItemPos;
}

int NWNX_Arelith_SetItemPosition(object oPC, object oItem, int nPositionX, int nPositionY)
{
    NWNXPushInt(nPositionY);
    NWNXPushInt(nPositionX);
    NWNXPushObject(oItem);
    NWNXPushObject(oPC);
    NWNXCall(ARELITH_PLUGIN, "SetItemPosition");

    return NWNXPopInt();
}

int NWNX_Arelith_GetAttackModifierVersus(object attacker, object versus=OBJECT_INVALID)
{
    NWNXPushObject(versus);
    NWNXPushObject(attacker);
    NWNXCall("NWNX_Arelith", "GetAttackModifierVersus");

    return NWNXPopInt();
}

int NWNX_Arelith_GetWeaponPower(object attacker, object versus, int offHand=FALSE)
{
    NWNXPushInt(offHand);
    NWNXPushObject(versus);
    NWNXPushObject(attacker);
    NWNXCall("NWNX_Arelith", "GetWeaponPower");

    return NWNXPopInt();
}

int NWNX_Arelith_ResolveDefensiveEffects(object attacker, object versus, int attackHit=TRUE)
{
    NWNXPushInt(attackHit);
    NWNXPushObject(versus);
    NWNXPushObject(attacker);
    NWNXCall("NWNX_Arelith", "ResolveDefensiveEffects");

    return NWNXPopInt();
}

void NWNX_Arelith_SetWebhook(string host, string path, string addendum="")
{
    string sFunc = "SetWebhook";
    NWNXPushString(path);
    NWNXPushString(host);
    NWNXPushString(addendum);
    NWNXCall(ARELITH_PLUGIN, sFunc);
}

void NWNX_Arelith_SetDamageReductionBypass(int material, int propertyType, int subType=-1, int costValue=-1, int paramValue=-1, int reverse=FALSE)
{
    string sFunc = "SetDamageReductionBypass";
    NWNXPushInt(reverse);
    NWNXPushInt(paramValue);
    NWNXPushInt(costValue);
    NWNXPushInt(subType);
    NWNXPushInt(propertyType);
    NWNXPushInt(material);

    NWNXCall(ARELITH_PLUGIN, sFunc);
}


void NWNX_Arelith_SetDisableMonkAbilitiesPolymorph(int nPolymorphType)
{
    string sFunc = "SetDisableMonkAbilitiesPolymorph";
    NWNXPushInt(nPolymorphType);
    NWNXCall(ARELITH_PLUGIN, sFunc);
}

void NWNX_Arelith_SetDispelResistanceModifier(object oCreature, int nClass, int nModifier, int bPersist = FALSE)
{
    string sFunc = "SetDispelResistanceModifier";

    NWNXPushInt(bPersist);
    NWNXPushInt(nModifier);
    NWNXPushInt(nClass);
    NWNXPushObject(oCreature);

    NWNXCall(ARELITH_PLUGIN, sFunc);
}

void __NWNX_Arelith_ResolvePack(string sFunc, struct NWNX_EffectUnpacked e)
{

    NWNXPushString(e.sID);
    NWNXPushInt(e.nType);

    NWNXPushInt(e.nSubType);

    NWNXPushFloat(e.fDuration);
    NWNXPushInt(e.nExpiryCalendarDay);
    NWNXPushInt(e.nExpiryTimeOfDay);

    NWNXPushObject(e.oCreator);
    NWNXPushInt(e.nSpellId);
    NWNXPushInt(e.bExpose);
    NWNXPushInt(e.bShowIcon);
    NWNXPushInt(e.nCasterLevel);

    NWNXPushEffect(e.eLinkLeft);
    NWNXPushInt(e.bLinkLeftValid);
    NWNXPushEffect(e.eLinkRight);
    NWNXPushInt(e.bLinkRightValid);


    NWNXPushInt(e.nNumIntegers);
    NWNXPushInt(e.nParam0);
    NWNXPushInt(e.nParam1);
    NWNXPushInt(e.nParam2);
    NWNXPushInt(e.nParam3);
    NWNXPushInt(e.nParam4);
    NWNXPushInt(e.nParam5);
    NWNXPushInt(e.nParam6);
    NWNXPushInt(e.nParam7);
    NWNXPushFloat(e.fParam0);
    NWNXPushFloat(e.fParam1);
    NWNXPushFloat(e.fParam2);
    NWNXPushFloat(e.fParam3);
    NWNXPushString(e.sParam0);
    NWNXPushString(e.sParam1);
    NWNXPushString(e.sParam2);
    NWNXPushString(e.sParam3);
    NWNXPushString(e.sParam4);
    NWNXPushString(e.sParam5);
    NWNXPushObject(e.oParam0);
    NWNXPushObject(e.oParam1);
    NWNXPushObject(e.oParam2);
    NWNXPushObject(e.oParam3);

    NWNXPushFloat(e.vParam0x);
    NWNXPushFloat(e.vParam0y);
    NWNXPushFloat(e.vParam0z);

    NWNXPushFloat(e.vParam1x);
    NWNXPushFloat(e.vParam1y);
    NWNXPushFloat(e.vParam1z);

    NWNXPushString(e.sTag);

    NWNXPushString(e.sItemProp);
}

effect NWNX_Arelith_PackEffect(struct NWNX_EffectUnpacked e)
{
    string sFunc = "PackEffect";

    __NWNX_Arelith_ResolvePack(sFunc, e);

    NWNXCall(ARELITH_PLUGIN, sFunc);
    return NWNXPopEffect();
}
