@addtogroup dormantobjects DormantObjects
@page dormantobjects Readme
@ingroup dormantobjects

**EXPERIMENTAL.** Takes placeables and items (and, opt-in, doors, triggers and containers) that cannot change on their own out of the engine's per-tick AI list
walk (`CServerAIMaster::UpdateState`), and puts them back as soon as something gives them work. Off by default.

An object is dormant when it is:
- a placeable with no inventory, no trap, no heartbeat script, no queued actions, not dead, and only effects that never
  expire (permanent, equipped or innate), or none;
- an item with no queued actions and only effects that never expire (item properties are permanent effects).
- with `CONTAINERS`, also a placeable with an inventory or a trap that is otherwise idle as above, unless it is a body
  bag (`DieWhenEmpty` with an inventory): the engine destroys an emptied body bag from its AI update, so those always
  stay in the walk;
- with `DOORS`, a door with no heartbeat script, no queued actions, not dead and only effects that never expire;
- with `TRIGGERS`, a trigger with no heartbeat script and no queued actions.

None of these is dormant while it is in a conversation (as its owner or a partner): the AI list walk also calls
`UpdateDialog`, which plays a conversation's delayed entries.

Door, trigger and container events (OnOpen, OnClose, OnEnter, OnExit, OnDisturbed, OnFailToOpen, traps springing,
locking, inventory changes) are driven by the creature or script acting on the object and the event queue, not by the
object's own AI update, so they work the same while it is dormant.

An object with a RunScript or Regenerate effect is never dormant, whatever the effect's duration: the engine runs the
interval script, or regenerates, from the AI update.

A dormant object goes back into its AI list when an effect is applied to it, or when an action is queued on it
(`AddAction`, `AddActionToFront`, `AddActionAfterFront`; `PlayAnimation` and `ActionDoDoorAction` use the latter two).
A script changing one of its event scripts (`SetEventScript`) wakes it at once if that gives it work (a heartbeat).
Joining a conversation (`CNWSObject::SetDialogOwner`) wakes it at once. So does NWNX_Effect editing one of its applied
effects in place (`NWNX_Effect_ReplaceEffect`, when the Effect plugin broadcasts `NWNX_EFFECT_APPLIED_EFFECT_EDITED`)
if the edit gives it an effect that can expire. A bounded slice of dormant objects is also re-checked every tick, which
catches anything else.

If a hook the AI lists depend on fails to install, the plugin logs an error and stays inactive. If only the
`SetEventScript` or `SetDialogOwner` hook fails, it logs a warning and runs without it: those objects wake at their
next re-check instead.

## Environment Variables

| Variable Name | Value | Default | Notes |
| ------------- | :---: | :-----: | ----- |
| `NWNX_DORMANTOBJECTS_ENABLE` | true/false | false | Nothing is hooked unless this is true. |
| `NWNX_DORMANTOBJECTS_SWEEP_SECONDS` | int | 10 | How often the AI lists are swept for dormant objects. Must be above 0. |
| `NWNX_DORMANTOBJECTS_RECHECK_PER_TICK` | int | 1000 | Dormant objects re-checked per tick. Must be above 0. |
| `NWNX_DORMANTOBJECTS_PLACEABLES` | true/false | true | Consider placeables. |
| `NWNX_DORMANTOBJECTS_ITEMS` | true/false | true | Consider items. |
| `NWNX_DORMANTOBJECTS_CONTAINERS` | true/false | false | Also consider placeables with an inventory or a trap (the engine's placeable update ignores both). |
| `NWNX_DORMANTOBJECTS_DOORS` | true/false | false | Consider doors: no heartbeat script, no actions, not dead, only permanent effects. |
| `NWNX_DORMANTOBJECTS_TRIGGERS` | true/false | false | Consider triggers: no heartbeat script, no actions. |
| `NWNX_DORMANTOBJECTS_RECHECK_SECONDS` | int | 0 | Re-check every dormant object once in about this many seconds, at most `RECHECK_PER_TICK` per tick. 0 always uses `RECHECK_PER_TICK`. |
| `NWNX_DORMANTOBJECTS_DEBUG_SIMULATE_HOOK_FAILURE` | true/false | false | Testing only: behave as if a hook failed to install. |

## NWScript

`nwnx_dormant.nss`:
- `NWNX_DormantObjects_Disable()` wakes every dormant object and stops sweeping, at runtime. Returns how many were woken.
- `NWNX_DormantObjects_Enable()` starts sweeping again. Returns TRUE if the plugin was enabled at startup with every
  hook installed.
- `NWNX_DormantObjects_GetDormantCount()` returns the number of dormant objects, or -1 when inactive.
- `NWNX_DormantObjects_GetIsDormant(oObject)` returns TRUE if that object is dormant.
- `NWNX_DormantObjects_LogCensus()` logs what the AI lists still hold and why each placeable, door, trigger and item
  there isn't dormant (`hb`, `inventory`, `trap`, `die-when-empty`, `actions`, `dialog`, `dead`, `fx-expiring`).
