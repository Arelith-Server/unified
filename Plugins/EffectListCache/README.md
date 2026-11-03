@addtogroup effectlistcache EffectListCache
@page effectlistcache Readme
@ingroup effectlistcache

**EXPERIMENTAL.** Skips `CNWSObject::UpdateEffectList` on objects with nothing on their effect list due yet. Off by
default.

Every AI update walks the object's whole applied-effects list. On a creature most of the list is equipped and innate
item-property effects that never expire, so the walk usually does nothing. This plugin keeps, per object, the earliest
time anything on the list can be due and skips the call until then.

What `UpdateEffectList` does (8193.37 disassembly), per effect:
- Regenerate, Poison, Disease, Wounding, Swarm: periodic work every call. An object with any of these is never
  skipped.
- RunScript with an interval script: runs it on its interval. Never skipped. Without one it is an ordinary effect.
- Icon, on a creature, temporary, not yet flagged: sets the "about to expire" flag once now + 5 s reaches the expiry.
- Any temporary effect: removed once now is past its expiry.

Nothing else in the function does work, so a skipped call is one that would have done nothing.

The cached due time is dropped when an effect is applied to or removed from the object
(`CServerAIMaster::OnEffectApplied` / `OnEffectRemoved`), when `NWNX_Effect_ReplaceEffect` or
`NWNX_Effect_ReplaceEffectByIndex` edits one of its effects in place (the Effect plugin broadcasts
`NWNX_EFFECT_APPLIED_EFFECT_EDITED`), when the list's length changes, and every `REFRESH_SECONDS` of world time as a
backstop. Other code that edits applied effects in place must broadcast that message too.

If any hook fails to install, the plugin logs an error and stays inactive.

## Environment Variables

| Variable Name | Value | Default | Notes |
| ------------- | :---: | :-----: | ----- |
| `NWNX_EFFECTLISTCACHE_ENABLE` | true/false | false | Nothing is hooked unless this is true. |
| `NWNX_EFFECTLISTCACHE_REFRESH_SECONDS` | int | 6 | Rebuild each cached entry at least this often (world time). 0 = never. |
| `NWNX_EFFECTLISTCACHE_ALL_OBJECTS` | true/false | false | Also cache placeables, doors and items, not only creatures. |
| `NWNX_EFFECTLISTCACHE_LOG_SECONDS` | int | 60 | How often to log counters and drop entries for destroyed objects. |
| `NWNX_EFFECTLISTCACHE_VERIFY` | true/false | false | Testing: on every skip, rebuild the entry from the list and log an error if something was due. |
| `NWNX_EFFECTLISTCACHE_DEBUG_SIMULATE_HOOK_FAILURE` | true/false | false | Testing only: behave as if a hook failed to install. |

## NWScript

`nwnx_effcache.nss`:
- `NWNX_EffectListCache_Disable()` stops skipping and drops the cache, at runtime.
- `NWNX_EffectListCache_Enable()` starts again. Returns TRUE if the plugin was enabled at startup with every hook
  installed.
- `NWNX_EffectListCache_GetStat(nStat)` returns a counter (see the `NWNX_EFFECTLISTCACHE_STAT_*` constants), or -1
  when not configured.
