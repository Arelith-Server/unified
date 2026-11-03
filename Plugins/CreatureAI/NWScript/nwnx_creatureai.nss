/// @addtogroup creatureai CreatureAI
/// @brief Runtime control of the experimental CreatureAI plugin (NWNX_CREATUREAI_HEIGHT_CACHE / NWNX_CREATUREAI_THROTTLE).
/// @{
/// @file nwnx_creatureai.nss

const string NWNX_CreatureAI = "NWNX_CreatureAI"; ///< @private

/// @brief Switches the walkmesh height cache off or on.
/// @param bOn TRUE to switch on, FALSE to switch off.
/// @return TRUE if the cache was enabled at startup with every hook installed, so the switch took effect.
int NWNX_CreatureAI_SetHeightCache(int bOn);

/// @brief Switches the VERY_LOW creature throttle off or on. Switching off makes every throttled creature run on its next visit.
/// @param bOn TRUE to switch on, FALSE to switch off.
/// @return TRUE if the throttle was enabled at startup with every hook installed, so the switch took effect.
int NWNX_CreatureAI_SetThrottle(int bOn);

/// @brief Sets whether a creature whose current action is ActionWait counts as idle for the throttle.
/// @param bOn TRUE to include waiting creatures, FALSE to exclude them.
/// @return TRUE if the throttle was enabled at startup with every hook installed, so the switch took effect.
int NWNX_CreatureAI_SetThrottleWaiting(int bOn);

/// @brief Whether oCreature is currently being throttled.
/// @param oCreature The creature.
/// @return TRUE if throttled.
int NWNX_CreatureAI_GetIsThrottled(object oCreature);

/// @brief Writes the plugin's counters to the NWNX log now and resets them.
void NWNX_CreatureAI_LogStats();

/// @}

int NWNX_CreatureAI_SetHeightCache(int bOn)
{
    NWNXPushInt(bOn);
    NWNXCall(NWNX_CreatureAI, "SetHeightCache");
    return NWNXPopInt();
}

int NWNX_CreatureAI_SetThrottle(int bOn)
{
    NWNXPushInt(bOn);
    NWNXCall(NWNX_CreatureAI, "SetThrottle");
    return NWNXPopInt();
}

int NWNX_CreatureAI_SetThrottleWaiting(int bOn)
{
    NWNXPushInt(bOn);
    NWNXCall(NWNX_CreatureAI, "SetThrottleWaiting");
    return NWNXPopInt();
}

int NWNX_CreatureAI_GetIsThrottled(object oCreature)
{
    NWNXPushObject(oCreature);
    NWNXCall(NWNX_CreatureAI, "GetIsThrottled");
    return NWNXPopInt();
}

void NWNX_CreatureAI_LogStats()
{
    NWNXCall(NWNX_CreatureAI, "LogStats");
    NWNXPopInt();
}
