/// @addtogroup effectlistcache EffectListCache
/// @brief Runtime control of the experimental EffectListCache plugin (NWNX_EFFECTLISTCACHE_ENABLE=true).
/// @{
/// @file nwnx_effcache.nss

const string NWNX_EffectListCache = "NWNX_EffectListCache"; ///< @private

const int NWNX_EFFECTLISTCACHE_STAT_CALLS         = 0; ///< UpdateEffectList calls the plugin looked at
const int NWNX_EFFECTLISTCACHE_STAT_SKIPPED       = 1; ///< Calls skipped: nothing on the list was due
const int NWNX_EFFECTLISTCACHE_STAT_PERIODIC      = 2; ///< Calls passed through: the list has per-tick work
const int NWNX_EFFECTLISTCACHE_STAT_FULL          = 3; ///< Calls passed through and the cache rebuilt after
const int NWNX_EFFECTLISTCACHE_STAT_REBUILT       = 4; ///< Cache entries rebuilt
const int NWNX_EFFECTLISTCACHE_STAT_INVALIDATED   = 5; ///< Cache entries dropped by an apply, remove or edit
const int NWNX_EFFECTLISTCACHE_STAT_VERIFY_MISSES = 6; ///< VERIFY mode: skips that would have missed due work
const int NWNX_EFFECTLISTCACHE_STAT_CACHED        = 7; ///< Objects cached now

/// @brief Stops skipping UpdateEffectList and drops the cache.
/// @return TRUE if it was active.
int NWNX_EffectListCache_Disable();

/// @brief Starts skipping again after NWNX_EffectListCache_Disable().
/// @return TRUE if the plugin is configured (enabled at startup with every hook installed) and now active.
int NWNX_EffectListCache_Enable();

/// @brief A counter since startup, see the NWNX_EFFECTLISTCACHE_STAT_* constants.
/// @param nStat The counter.
/// @return The value (capped at the largest int), or -1 if the plugin isn't configured.
int NWNX_EffectListCache_GetStat(int nStat);

/// @}

int NWNX_EffectListCache_Disable()
{
    NWNXCall(NWNX_EffectListCache, "Disable");
    return NWNXPopInt();
}

int NWNX_EffectListCache_Enable()
{
    NWNXCall(NWNX_EffectListCache, "Enable");
    return NWNXPopInt();
}

int NWNX_EffectListCache_GetStat(int nStat)
{
    NWNXPushInt(nStat);
    NWNXCall(NWNX_EffectListCache, "GetStat");
    return NWNXPopInt();
}
