# Ghost player and endless shutdown (8193.37-17), 2026-11-04

The Distant Shores shutdown on 2026-11-04 hung for 21 minutes, until staff killed the process. The cause is a stale `CNWSPlayer` (a "ghost") left in the player list since 2026-11-03 18:50 UTC. `NWNX_TWEAKS_FIX_GHOST_PLAYERS=true` (`../../FixGhostPlayers.cpp`) prevents it.

## Files

| File | What it is |
| ---- | ---------- |
| `engine-log-excerpt.txt` | Sanitised engine-log lines for the affected account, join/leave counts and the shutdown tail |
| `lagwatch-freeze-stacks.txt` | Main-thread perf samples during the hang |
| `beamdog-report.md` | Draft engine bug report for Beamdog |
| `repro/GhostRepro.cpp` | Test-only NWNX plugin that reproduces the bug at engine level (not built by default) |
| `repro/results/*.txt` | Server log lines from the AreDev runs below |

## Root cause (from the 8193.37-17 Linux disassembly)

1. `CNetLayerInternal::HandleBNCSMessage` (a connect request) calls `SetSlidingWindow(connectionId)`. If that connection already owns a window, it returns the **same** window.
2. `HandleBNCSMessage` then calls `CNetLayerPlayerInfo::Initialize()` on that slot (player ID = window index). This clears `m_bPlayerInUse`, `m_bPlayerPrivileges` and the CD key, but it never calls `PlayerListChange(id, FALSE)`.
3. The new login is validated, and `PlayerListChange(id, TRUE)` inserts a second `CNWSPlayer(id)` at index 0. The engine does not check for an existing one.
4. On the next leave, `PlayerListChange(id, FALSE)` removes the first match (the newest object) and the slot is reset again. The older object is now an orphan.
5. `PlayerListChange(id, FALSE)` removes a player only if the slot still has player or DM privileges, so the orphan can never be removed.
6. `CServerExoAppInternal::Shutdown` loops `DisconnectPlayer(list[0]->m_nPlayerID)` until the list is empty. With the orphan it never ends: `DisconnectPlayer` → `SendBNDPMessage` / `PlayerIdToSlidingWindow` find no window → `PlayerListChange` does nothing → `Initialize`, and round again.

The engine's own guard, "Evicting old connection for crypto re-key", lives in `HandleBNK1Message` and only fires when the client redoes the key exchange. A second BNCS on an already-keyed connection skips it.

The live log fits this exactly. All three logins of the account (18:49:26, 18:50:14, 18:50:32) came over one connection (one IP:port). That connection was created at 18:49:26, and the engine logged no new connection for that address until the "Cleared connection" at 19:10:02. The 18:50:32 connect request reused window 10, still held by the 18:50:14 login, which was 18 s into loading.

## Fix

`FixGhostPlayers.cpp` hooks `CNetLayerPlayerInfo::Initialize()`. Every slot reset goes through that function: the second BNCS, both reject paths in `HandleBNVSMessage`, and `DisconnectPlayer`. Before the slot is reset, the hook checks for any `CNWSPlayer` that still uses the slot's player ID:

- **Slot still has player/DM privileges** (the earlier login is being replaced): call the engine's own leave, `CServerExoApp::PlayerListChange(id, FALSE)`. This saves the character, runs OnClientLeave, updates player lists and logs "Left as a Player", as for any disconnect. The new login has not loaded its character yet, so this save cannot overwrite a newer `.bic`.
- **Slot already free** (an orphan, which the engine can never remove): do what the engine's leave does by hand (RemovePCFromWorld, remove from list, player-list updates, delete). The character save is suppressed, because the `.bic` on disk may come from a newer session. This is also what stops the shutdown loop if an orphan ever exists.

In the normal case `DisconnectPlayer` has already removed the player before it calls `Initialize()`, so the hook finds nothing and does nothing. The cost is one scan of the player list per slot reset.

## Repro and results (AreDev pool slot, 8193.37-17 image)

`repro/GhostRepro.cpp` runs on slot 200 with no client. It drives the same engine calls a real duplicate login makes: a validated slot and `PlayerListChange(id, TRUE)`; then `CNetLayerPlayerInfo::Initialize()` as in step 2; then a validated slot and `PlayerListChange(id, TRUE)` again; then `CNetLayer::DisconnectPlayer(id)`. After that the server is shut down with the console `exit` command.

| Run | Log | Result |
| --- | --- | ------ |
| Stock plugins | `results/baseline.txt` | Joined 1, Joined 2, one "Left (1 players left)"; `GHOST PRESENT`. After `exit`: "Server shutting down", then the server spins at ~100% CPU and never exits (killed after 4 min). gdb on the hung test server: `PlayerIdToSlidingWindow+0x31` ← `DisconnectPlayer+0x78`, the same stack as on Distant Shores. |
| `NWNX_TWEAKS_FIX_GHOST_PLAYERS=true` | `results/fixed.txt` | The slot reset removes the first player with the normal leave; one object at a time; `NO GHOST`. `exit` → clean exit (code 0) in 16 s. |
| Fix plus `NWNX_GHOSTREPRO_ORPHAN=true` | `results/orphan.txt` | An orphan is planted whose slot is already free. On `exit`, the safety net removes it without saving and the server exits cleanly. |

Not verified: a duplicate connect from a real game client (the client was busy with other tests), and the `.bic` contents with a real character in the world. The engine-level repro players never load a creature, so the save rules above are argued from the code, not measured.
