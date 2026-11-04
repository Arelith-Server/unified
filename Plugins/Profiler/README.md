@page profiler Readme
@ingroup profiler 

Acquires shared hooks to expose various useful metrics.

## Environment Variables

| Variable Name                  |   Type   | Default Value |
| -------------                  | :------: | ------------- |
| NWNX_PROFILER_ENABLE_OVERHEAD_COMPENSATION   | bool     | true    |
| NWNX_PROFILER_OVERHEAD_COMPENSATION_FORCE     | int64_t  | _none_  |
| NWNX_PROFILER_OVERHEAD_COMPENSATION_RUNS     | size_t   | 500     |
| NWNX_PROFILER_OVERHEAD_COMPENSATION_RECALIBRATE   | bool   | false   |
| NWNX_PROFILER_OVERHEAD_COMPENSATION_RECALIBRATION_PERIOD | uint32_t | 1000 |
| NWNX_PROFILER_ENABLE_AI_MASTER_UPDATES       | bool     | true    |
| NWNX_PROFILER_AI_MASTER_UPDATES_OVERKILL     | bool     | false   |
| NWNX_PROFILER_ENABLE_MAIN_LOOP               | bool     | true    |
| NWNX_PROFILER_ENABLE_NET_LAYER               | bool     | true    |
| NWNX_PROFILER_ENABLE_NET_MESSAGES            | bool     | true    |
| NWNX_PROFILER_ENABLE_OBJECT_AI_UPDATES       | bool     | false   |
| NWNX_PROFILER_ENABLE_OBJECT_EVENT_HANDLERS   | bool     | false   |
| NWNX_PROFILER_ENABLE_PATHING                 | bool     | true    |
| NWNX_PROFILER_PATHING_MOVER_TAGS             | bool     | false   |
| NWNX_PROFILER_PATHING_SLOW_LOG               | bool     | false   |
| NWNX_PROFILER_PATHING_SLOW_LOG_MS            | uint32_t | 100     |
| NWNX_PROFILER_PATHING_SLOW_LOG_PER_MINUTE    | uint32_t | 10      |
| NWNX_PROFILER_ENABLE_SCRIPTS                 | bool     | true    |
| NWNX_PROFILER_SCRIPTS_AREA_TIMINGS           | bool     | true    |
| NWNX_PROFILER_SCRIPTS_TYPE_TIMINGS           | bool     | true    |
| NWNX_PROFILER_ENABLE_TICKRATE                | bool     | true    |

## Pathing extras

Both are off by default, so the PlotPath series and the log are unchanged unless they are turned on.

- `NWNX_PROFILER_PATHING_MOVER_TAGS=true` adds two tags to the PlotPath timing:
  - `Mover`: `pc-click` (a player character moving because the client clicked), `pc-script` (a player character moving
    for any other reason, such as following or a scripted move), `dm` (a DM avatar), `copy` (no template resref, such as
    CopyObject clones or creatures made from JSON), or `npc`.
  - `HasTarget`: `yes` when the path is to an object (following, chasing, moving to an object), `no` for a point.

  These add at most 10 series per ResRef and Area pair; the cost is a few field reads per call.
- `NWNX_PROFILER_PATHING_SLOW_LOG=true` logs one line for each PlotPath call slower than
  `NWNX_PROFILER_PATHING_SLOW_LOG_MS`. The line has the creature (character name for player characters, tag otherwise),
  its object ID and Mover value, the area resref, the start and end points, the tag of the object it moves to, the
  duration and the engine's call counter for the current path. At most `NWNX_PROFILER_PATHING_SLOW_LOG_PER_MINUTE`
  lines are written each minute; the rest are counted and reported as "suppressed" in the next minute.
