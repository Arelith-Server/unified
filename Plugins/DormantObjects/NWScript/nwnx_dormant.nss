/// @addtogroup dormantobjects DormantObjects
/// @brief Runtime control of the experimental DormantObjects plugin (NWNX_DORMANTOBJECTS_ENABLE=true).
/// @{
/// @file nwnx_dormant.nss

const string NWNX_DormantObjects = "NWNX_DormantObjects"; ///< @private

/// @brief Wakes every dormant object (puts it back in its AI list) and stops making objects dormant.
/// @return The number of objects woken.
int NWNX_DormantObjects_Disable();

/// @brief Starts making objects dormant again after NWNX_DormantObjects_Disable().
/// @return TRUE if the plugin is configured (enabled at startup with every hook installed) and now active.
int NWNX_DormantObjects_Enable();

/// @brief The number of objects currently dormant.
/// @return The count, or -1 if the plugin isn't active.
int NWNX_DormantObjects_GetDormantCount();

/// @brief Whether oObject is currently dormant (out of the engine's per-tick AI list walk).
/// @param oObject The object.
/// @return TRUE if dormant.
int NWNX_DormantObjects_GetIsDormant(object oObject);

/// @brief Logs (at INFO) what the engine's AI lists still hold: objects by type per list, why placeables, doors,
/// triggers and items aren't dormant, dormant objects by type, and creature effect counts. Read-only.
/// @return The number of objects left in the AI lists.
int NWNX_DormantObjects_LogCensus();

/// @}

int NWNX_DormantObjects_Disable()
{
    NWNXCall(NWNX_DormantObjects, "Disable");
    return NWNXPopInt();
}

int NWNX_DormantObjects_Enable()
{
    NWNXCall(NWNX_DormantObjects, "Enable");
    return NWNXPopInt();
}

int NWNX_DormantObjects_GetDormantCount()
{
    NWNXCall(NWNX_DormantObjects, "GetDormantCount");
    return NWNXPopInt();
}

int NWNX_DormantObjects_GetIsDormant(object oObject)
{
    NWNXPushObject(oObject);
    NWNXCall(NWNX_DormantObjects, "GetIsDormant");
    return NWNXPopInt();
}

int NWNX_DormantObjects_LogCensus()
{
    NWNXCall(NWNX_DormantObjects, "LogCensus");
    return NWNXPopInt();
}
