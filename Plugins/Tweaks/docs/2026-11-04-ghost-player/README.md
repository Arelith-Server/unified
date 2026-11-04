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

`FixGhostPlayers.cpp` hooks `CNetLayerPlayerInfo::Initialize()`. Every slot reset goes through that function: the second BNCS, both reject paths in `HandleBNVSMessage`, and `DisconnectPlayer`. Before the slot is reset, the hook looks for any `CNWSPlayer` that still uses the slot's player ID:

- **The slot's own login is being replaced** (the slot still has player/DM privileges, and the engine has not just run a leave for this slot): call the engine's own leave, `CServerExoApp::PlayerListChange(id, FALSE)`. That saves the character, runs OnClientLeave and the TURD drop, updates player lists and logs "Left as a Player", as for any disconnect. The new login has not loaded its character yet, so this save cannot overwrite a newer `.bic`. Only the first match can take this branch.
- **Anything else is an orphan.** That covers a slot that is already free, and any further object found after the engine's leave on the `DisconnectPlayer` path (a hook on `PlayerListChange` records which slot it just ran a leave for). An orphan is removed by hand, mirroring the engine's leave: RemovePCFromWorld, removal from the player list and from `m_lstClientDisconnectPending`, player-list updates, delete. For that one object only, the character save and the TURD drop are skipped: the `.bic` on disk may come from a newer session, and the slot's player name and CD key now belong to nobody, or to another login. OnClientLeave still runs, so the creature is cleaned up. While it runs, the creature has the local int `NWNX_TWEAKS_GHOST_PLAYER_LEAVE` = 1, so a leave script can skip work keyed on the player name or CD key.

Every warning logs the slot's connection address, so live cases can be audited. Docs here keep only the sanitised form.

In the normal case `DisconnectPlayer` has already removed the player before it calls `Initialize()`, so the hook finds nothing and does nothing. The cost is one scan of the player list per slot reset.

`CServerExoAppInternal::RestartNetLayer` resets the netlayer, and with it every slot, without a disconnect. Nothing in the 8193.37 Linux binary calls it directly (`CServerExoApp::RestartNetLayer` only forwards to it). If it ever ran with players online, this hook would remove each of them with the normal leave.

## Repro and results (AreDev pool slot, 8193.37-17 image)

`repro/GhostRepro.cpp` needs no client. In one server run it drives three cases, each on its own unused slot, with the same engine calls as the real sequences. Each case's first (older) player object gets a module NPC as its creature, so its removal runs the full `RemovePCFromWorld` (save, OnClientLeave, TURD). Saves and TURD drops that reach the engine are logged ("GHOSTREPRO save/turd reached the engine") and then skipped, so nothing is written to the vault. Afterwards the server is stopped with the console `exit`.

- **relogin (slot 200):** the Distant Shores sequence. A validated login (with creature), then what a second BNCS on the same connection does (`CNetLayerPlayerInfo::Initialize()` on the slot), then the second login and `CNetLayer::DisconnectPlayer`.
- **double (slot 201):** two objects already share the slot when the player leaves through `DisconnectPlayer`. This is the review case: the engine's leave removes the newest object while the slot still has player privileges, and the older copy must not be saved.
- **orphan (slot 202):** an object whose slot is already free, left until shutdown.

| Run | Log | Result |
| --- | --- | ------ |
| Stock plugins | `results/baseline.txt` | All three cases leave a ghost (`GHOST PRESENT`). After `exit`: "Server shutting down", then ThreadWatchdog LongStall every 15 s. The server never exited and was killed after 240 s. gdb on the hung test server: `PlayerIdToSlidingWindow+0x31` ← `DisconnectPlayer+0x78`, the same stack as on Distant Shores. |
| `NWNX_TWEAKS_FIX_GHOST_PLAYERS=true` | `results/fixed.txt` | **relogin:** the slot reset removes the older object with the normal leave, and its save and TURD reach the engine (correct: the new login hasn't loaded yet). The second session leaves normally; `NO GHOST`. **double:** the engine leave removes the newest object, then the older one (with creature) is removed as an orphan; no save reaches the engine; `NO GHOST`. **orphan:** removed at shutdown, no save. The server exits with code 0. |

Not verified:
- A duplicate connect from a real game client (the client was in use by other tests).
- A real character's `.bic` on disk. The repro checks whether a save reaches the engine and skips writing it.
- Leave scripts reacting to `NWNX_TWEAKS_GHOST_PLAYER_LEAVE` (no Arelith script uses it yet).
