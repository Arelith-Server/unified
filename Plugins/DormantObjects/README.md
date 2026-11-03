@addtogroup dormantobjects DormantObjects
@page dormantobjects Readme
@ingroup dormantobjects

**EXPERIMENTAL.** Takes placeables and items that cannot change on their own out of the engine's per-tick AI list
walk (`CServerAIMaster::UpdateState`), and puts them back as soon as something gives them work. Off by default.

An object is dormant when it is:
- a placeable with no inventory, no trap, no heartbeat script, no queued actions, not dead, and only effects that never
  expire (permanent, equipped or innate), or none;
- an item with no queued actions and only effects that never expire (item properties are permanent effects).

An object with a RunScript or Regenerate effect is never dormant, whatever the effect's duration: the engine runs the
interval script, or regenerates, from the AI update.

A dormant object goes back into its AI list when an effect is applied to it, or when an action is queued on it
(`AddAction`, `AddActionToFront`, `AddActionAfterFront`; `PlayAnimation` and `ActionDoDoorAction` use the latter two).
A bounded slice of dormant objects is also re-checked every tick, which catches anything else, for example a heartbeat
script set at runtime.

If any of the plugin's hooks fails to install, it logs an error and stays inactive.

## Environment Variables

| Variable Name | Value | Default | Notes |
| ------------- | :---: | :-----: | ----- |
| `NWNX_DORMANTOBJECTS_ENABLE` | true/false | false | Nothing is hooked unless this is true. |
| `NWNX_DORMANTOBJECTS_SWEEP_SECONDS` | int | 10 | How often the AI lists are swept for dormant objects. Must be above 0. |
| `NWNX_DORMANTOBJECTS_RECHECK_PER_TICK` | int | 1000 | Dormant objects re-checked per tick. Must be above 0. |
| `NWNX_DORMANTOBJECTS_PLACEABLES` | true/false | true | Consider placeables. |
| `NWNX_DORMANTOBJECTS_ITEMS` | true/false | true | Consider items. |
| `NWNX_DORMANTOBJECTS_DEBUG_SIMULATE_HOOK_FAILURE` | true/false | false | Testing only: behave as if a hook failed to install. |

## NWScript

`nwnx_dormant.nss`:
- `NWNX_DormantObjects_Disable()` wakes every dormant object and stops sweeping, at runtime. Returns how many were woken.
- `NWNX_DormantObjects_Enable()` starts sweeping again. Returns TRUE if the plugin was enabled at startup with every
  hook installed.
- `NWNX_DormantObjects_GetDormantCount()` returns the number of dormant objects, or -1 when inactive.
- `NWNX_DormantObjects_GetIsDormant(oObject)` returns TRUE if that object is dormant.
