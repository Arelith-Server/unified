@addtogroup dormantobjects DormantObjects
@page dormantobjects Readme
@ingroup dormantobjects

**EXPERIMENTAL.** Takes placeables and items that cannot change on their own out of the engine's per-tick AI list
walk (`CServerAIMaster::UpdateState`), and puts them back as soon as something gives them work. Off by default.

An object is dormant when it is:
- a placeable with no inventory, no trap, no heartbeat script, no queued actions, not dead, and only effects that never
  expire (permanent, equipped or innate), or none;
- an item with no queued actions and only effects that never expire (item properties are permanent effects).

A dormant object goes back into its AI list when an effect is applied to it, when an action is queued on it, or, at
the next sweep, if it no longer qualifies (for example a heartbeat script was set).

## Environment Variables

| Variable Name | Value | Default | Notes |
| ------------- | :---: | :-----: | ----- |
| `NWNX_DORMANTOBJECTS_ENABLE` | true/false | false | Nothing is hooked unless this is true. |
| `NWNX_DORMANTOBJECTS_SWEEP_SECONDS` | int | 10 | How often the AI lists are swept for dormant objects. |
| `NWNX_DORMANTOBJECTS_PLACEABLES` | true/false | true | Consider placeables. |
| `NWNX_DORMANTOBJECTS_ITEMS` | true/false | true | Consider items. |
