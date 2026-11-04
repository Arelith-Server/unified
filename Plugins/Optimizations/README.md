@addtogroup optimizations Optimizations
@page optimizations Readme
@ingroup optimizations

Game optimizations. Improves performance of various game elements.

## Environment Variables

| Variable Name | Value | Notes |
| -------------   | :----: | ------------------------------------ |
| `NWNX_OPTIMIZATIONS_ASYNC_LOG_FLUSH` | true/false | Flushes the game log on an async thread, potentially improving performance |
| `NWNX_OPTIMIZATIONS_GAME_OBJECT_LOOKUP` | true/false | Optimizes object lookup code, improving performance |
| `NWNX_OPTIMIZATIONS_PLAYER_LOOKUP` | true/false | Optimizes Player client lookup from object IDs, improving performance |
| `NWNX_OPTIMIZATIONS_LUO_LOOKUP` | true/false | Optimizes LastUpdateObject lookup code, improving performance |
| `NWNX_OPTIMIZATIONS_ALTERNATE_GAME_OBJECT_UPDATE` | true/false | Uses an experimental alternative update mechanism. Requires `LUO_LOOKUP`. **WARNING**: Will break all of NWNX_Appearance and the following NWNX_Player functions: SetObjectVisualTransformOverride, ApplyLoopingVisualEffectToObject, SetPlaceableNameOverride, SetCreatureNameOverride, SetObjectMouseCursorOverride and SetObjectHiliteColorOverride. Forcing objects to be always visible with NWNX_Visibility will also break. |
| `NWNX_OPTIMIZATIONS_CACHE_SCRIPT_CHUNKS` | true/false | Caches all script chunks, improving performance |
| `NWNX_OPTIMIZATIONS_CACHE_DEBUGGER_INSTANCES` | true/false | Caches all nwscript debugger instances, improving GetScriptBacktrace() performance |
| `NWNX_OPTIMIZATIONS_CACHE_SCRIPTS` | true/false | Caches all scripts, improving performance |
| `NWNX_OPTIMIZATIONS_CHARACTER_SAVE_LABEL_CACHE` | true/false | During character saves (and other creature saves) GFF field labels are looked up through a hash index instead of a linear scan of the label table. The saved file is byte-for-byte the same. |
| `NWNX_OPTIMIZATIONS_COALESCE_CHARACTER_EXPORTS` | true/false | An `ExportSingleCharacter()` request for a player who is already queued for export in the same server tick is not queued again. The engine would write the same file twice in a row. |
