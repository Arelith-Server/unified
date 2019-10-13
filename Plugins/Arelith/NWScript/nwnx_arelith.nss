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



void NWNX_Arelith_SubscribeEvent(string evt, string script)
{
    NWNX_PushArgumentString("NWNX_Arelith", "SUBSCRIBE_EVENT", script);
    NWNX_PushArgumentString("NWNX_Arelith", "SUBSCRIBE_EVENT", evt);
    NWNX_CallFunction("NWNX_Arelith", "SUBSCRIBE_EVENT");
}

void NWNX_Arelith_PushEventData(string tag, string data)
{
    NWNX_PushArgumentString("NWNX_Arelith", "PUSH_EVENT_DATA", data);
    NWNX_PushArgumentString("NWNX_Arelith", "PUSH_EVENT_DATA", tag);
    NWNX_CallFunction("NWNX_Arelith", "PUSH_EVENT_DATA");
}

int NWNX_Arelith_SignalEvent(string evt, object target)
{
    NWNX_PushArgumentObject("NWNX_Arelith", "SIGNAL_EVENT", target);
    NWNX_PushArgumentString("NWNX_Arelith", "SIGNAL_EVENT", evt);
    NWNX_CallFunction("NWNX_Arelith", "SIGNAL_EVENT");
    return NWNX_GetReturnValueInt("NWNX_Arelith", "SIGNAL_EVENT");
}

string NWNX_Arelith_GetEventData(string tag)
{
    NWNX_PushArgumentString("NWNX_Arelith", "GET_EVENT_DATA", tag);
    NWNX_CallFunction("NWNX_Arelith", "GET_EVENT_DATA");
    return NWNX_GetReturnValueString("NWNX_Arelith", "GET_EVENT_DATA");
}

void NWNX_Arelith_SkipEvent()
{
    NWNX_CallFunction("NWNX_Arelith", "SKIP_EVENT");
}

void NWNX_Arelith_SetEventResult(string data)
{
    NWNX_PushArgumentString("NWNX_Arelith", "EVENT_RESULT", data);
    NWNX_CallFunction("NWNX_Arelith", "EVENT_RESULT");
}

string NWNX_Arelith_GetCurrentEvent()
{
    NWNX_CallFunction("NWNX_Arelith", "GET_CURRENT_EVENT");
    return NWNX_GetReturnValueString("NWNX_Arelith", "GET_CURRENT_EVENT");
}
