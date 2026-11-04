@page creatureai Readme
@ingroup creatureai

Experimental. Two independent, opt-in cuts to the fixed per-creature cost of the engine's per-tick AI walk.
Both are off unless enabled, and each stays off if any of its hooks fails to install.

## Height cache

`CNWSCreature::AIUpdate` sets the creature's z from `CNWSArea::ComputeHeight` on every update, moving or not.
In 8193.37 that result depends only on the area's tiles and on x and y (the tile passes z = 0 to the walkmesh, and
nothing on the path writes state apart from a one-off lazy walkmesh load). The plugin caches
(area, tile array, x bits, y bits) → height in an 8192-entry direct-mapped table, so a hit returns exactly what the
engine would.

The table is marked stale when an area is destroyed and when a script changes tiles (`CNWSArea::SetTile`,
`CNWSArea::SetTileset`), and cleared once on the next lookup. `NWNX_Area_RotateArea` rewrites tiles without going
through either, but it replaces the area's tile array, which is part of the key: the first lookup that meets an entry
of that area with the old array clears the table. `NWNX_Tileset` only changes tiles while an area loads, so it's safe.

## VERY_LOW throttle

A creature runs `AIUpdate` on only one visit in `THROTTLE_TICKS` when all of these are true:
- its AI level is VERY_LOW, and its OnSpawn script has run
- its area has no players
- it isn't a PC or a DM, no connected player controls or possesses it, and it isn't dead
- it has nothing queued. With `THROTTLE_WAITING=true`, a creature that is only waiting also counts: an `ActionWait`
  at the head of the queue with nothing behind it but `ActionDoCommand` (Arelith's idle loop). A Wait the engine puts
  in front of other work (pathfinding waits, random-walk pauses, doors, follow, casting) doesn't count.
- it isn't in combat: no combat state, no attack target, and no combat round started

Its `AIUpdate` measures elapsed time from the world time it stored on its last real update, so timers and effect
durations catch up on the next run. At VERY_LOW the engine fires the heartbeat once the 6.0–7.2 s interval has passed
and the creature has then had interval/64 more `AIUpdate` visits, and updates an NPC's visible list (perception) once
4 s have passed and it has then had 100–150 more visits. The plugin counts skipped visits toward both (and steps the
personal-reputation cleanup counter the engine steps alongside the heartbeat count), so each fires at most
`THROTTLE_TICKS - 1` visits late. Any change that ends one of the
conditions above makes it run on its very next visit. A throttled creature also runs once `THROTTLE_MAX_SKIP_MS` has
passed since its last run.

## Environment variables

| Variable | Default | Meaning |
| --- | --- | --- |
| `NWNX_CREATUREAI_HEIGHT_CACHE` | false | Enable the height cache. |
| `NWNX_CREATUREAI_THROTTLE` | false | Enable the VERY_LOW throttle. |
| `NWNX_CREATUREAI_THROTTLE_TICKS` | 4 | Run one visit in this many (2–32). |
| `NWNX_CREATUREAI_THROTTLE_MAX_SKIP_MS` | 500 | Never skip a creature for longer than this (50–6000). |
| `NWNX_CREATUREAI_THROTTLE_WAITING` | false | Count a creature that is only waiting (see above) as idle. |
| `NWNX_CREATUREAI_LOG_SECONDS` | 300 | Log hit and skip counters this often (0 = never). |
| `NWNX_CREATUREAI_DEBUG_SIMULATE_HOOK_FAILURE` | false | Test only: act as if a hook failed, so both parts stay off. |

`nwnx_creatureai.nss` switches a configured part off and on at runtime. It can't enable a part that wasn't configured
at startup.
