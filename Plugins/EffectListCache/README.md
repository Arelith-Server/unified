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

## Engine build check

The list above is from the 8193.37-17 disassembly, so the plugin only enables on that build. At startup it checks
the server's build strings (`8193`, `37`, `17`) and a fingerprint of `CNWSObject::UpdateEffectList`'s machine code:
the symbol's size and an FNV-1a 64 hash of its bytes (4696 bytes, `5a7a36d587564aa7` on 8193.37-17). If either
differs it logs an error with the values it found and hooks nothing. A plugin loaded earlier that hooks
`UpdateEffectList` changes the bytes too, so it also makes this check fail.

After an engine upgrade, before changing the expected values in `EffectListCache.cpp`:
1. Disassemble the new `UpdateEffectList` and re-check the per-effect list above, and that nothing else in it does
   work.
2. Check that `CNWSObject::ApplyEffect`, `RemoveEffect` and `RemoveEffectById` are still the only writers of
   `m_appliedEffects` and still call `CServerAIMaster::OnEffectApplied` / `OnEffectRemoved`, and that
   `CNWSPlayer::EatTURD` is still the only place an object ID is reused.
3. Update `ENGINE_BUILD`, `ENGINE_REVISION`, `ENGINE_POSTFIX`, `UPDATE_EFFECT_LIST_SIZE` and
   `UPDATE_EFFECT_LIST_FNV1A`. The refusal logged at startup prints the new values, or run
   `python fingerprint.py nwserver-linux` on the new binary.
4. Run with `VERIFY=true` before plain `ENABLE` on a live server.

The cached due time is dropped when an effect is applied to or removed from the object
(`CServerAIMaster::OnEffectApplied` / `OnEffectRemoved`), when `NWNX_Effect_ReplaceEffect` or
`NWNX_Effect_ReplaceEffectByIndex` edits one of its effects in place (the Effect plugin broadcasts
`NWNX_EFFECT_APPLIED_EFFECT_EDITED`), when the list's length changes, when `CNWSPlayer::EatTURD` gives a logging-in
player's creature its old object ID, and every `REFRESH_SECONDS` of world time as a backstop. Other code that edits
applied effects in place must broadcast that message too.

The plugin and the Effect plugin change together: `NWNX_Effect.so` must come from the same build as
`NWNX_EffectListCache.so`, or `ReplaceEffect` edits go unseen until the next refresh.

If any hook fails to install, the plugin logs an error and stays inactive.

## Environment Variables

| Variable Name | Value | Default | Notes |
| ------------- | :---: | :-----: | ----- |
| `NWNX_EFFECTLISTCACHE_ENABLE` | true/false | false | Nothing is hooked unless this is true. |
| `NWNX_EFFECTLISTCACHE_REFRESH_SECONDS` | int | 6 | Rebuild each cached entry at least this often (world time), 1-3600. Values below 1 are raised to 1 with a warning. |
| `NWNX_EFFECTLISTCACHE_ALL_OBJECTS` | true/false | false | Also cache placeables, doors and items, not only creatures. |
| `NWNX_EFFECTLISTCACHE_LOG_SECONDS` | int | 60 | How often to log counters and drop entries for destroyed objects. |
| `NWNX_EFFECTLISTCACHE_VERIFY` | true/false | false | Testing and first rollout: nothing is skipped. Wherever the plugin would skip, it rebuilds the entry from the list, calls `UpdateEffectList` anyway and logs a `Verify:` error if the entry was stale or the engine changed the effect list, an effect on it, or the object's hit points. Behaves like off; costs about one extra pass over the list per call. The "skipped" counter then counts calls that would have been skipped. |
| `NWNX_EFFECTLISTCACHE_DEBUG_SIMULATE_HOOK_FAILURE` | true/false | false | Testing only: behave as if a hook failed to install. |

## NWScript

`nwnx_effcache.nss`:
- `NWNX_EffectListCache_Disable()` stops skipping and drops the cache, at runtime.
- `NWNX_EffectListCache_Enable()` starts again. Returns TRUE if the plugin was enabled at startup with every hook
  installed.
- `NWNX_EffectListCache_GetStat(nStat)` returns a counter (see the `NWNX_EFFECTLISTCACHE_STAT_*` constants), or -1
  when not configured.
