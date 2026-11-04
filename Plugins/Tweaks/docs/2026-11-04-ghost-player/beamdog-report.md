# Server hangs forever on shutdown after a second connect request on the same connection (ghost CNWSPlayer)

**Build:** NWN:EE dedicated server 8193.37-17 (Linux x64), with NWNX.
**Impact:** a "ghost" `CNWSPlayer` stays in the server player list for the rest of the run. At shutdown, `CServerExoAppInternal::Shutdown` never finishes: the main thread spins at 100% CPU until the process is killed. Ours ran for 21 minutes and had to be killed by hand.

## What happened

1. **18:49:26 UTC:** a player's client opens a new connection (one IP:port, one `CNetConnectionId`). It is given window 24, joins as Player 27 and, in the same second, "Left as a Player" (normal handover).
2. **18:50:14 UTC:** a connect request on the **same connection** gets **window 10** and joins as **Player 25**.
3. **18:50:32 UTC:** another connect request on the **same connection** gets **window 10 again** and joins as **Player 26**. No "Left as a Player", no "Evicting old connection" line, and no new connection was created for that client between 18:49:26 and its "Cleared connection" at 19:10:02.
4. **19:07:50 UTC:** the player logs out. One "Left as a Player" is logged. The other `CNWSPlayer` with player ID 10 stays in the player list.
5. **Whole run (about 23 h):** 1028 "Joined as Player" lines, 1027 "Left as a Player" lines.
6. **16:47:50 UTC next day:** `StopServices` logs "Server shutting down". Every real player leaves within one second; the last line is "Left as a Player (1 players left)". Then the main thread spins until the process is killed at about 17:09:16 UTC.

```
CNetLayerInternal::PlayerIdToSlidingWindow / CNetLayerInternal::SendBNDPMessage
  < CNetLayerInternal::DisconnectPlayer
  < CServerExoAppInternal::Shutdown
  < CServerExoAppInternal::StopServices
  < CAppManager::DestroyServer
  < (NWNX) Core::NWNXCore::DestroyServerHandler < main
```

## Root cause (from the 8193.37-17 Linux disassembly)

1. `CNetLayerInternal::HandleBNCSMessage` (server path) calls `SetSlidingWindow(connectionId, &window)`. If the connection already owns a window, `SetSlidingWindow` returns **that same window** instead of a new one.
2. `HandleBNCSMessage` then logs "Connection Attempt made by ..." and calls `CNetLayerPlayerInfo::Initialize()` on that window's player slot (player ID = window index). This clears `m_bPlayerInUse`, `m_bPlayerPrivileges`, the CD key, and so on. It does **not** call `PlayerListChange(id, FALSE)`, so the `CNWSPlayer` of the earlier login stays in `m_lstPlayerList`.
3. The new login is validated and `PlayerListChange(id, TRUE)` builds a second `CNWSPlayer(id)` and inserts it at index 0. Nothing checks for an existing player with the same ID.
4. On the next leave, `PlayerListChange(id, FALSE)` removes only the first match (the newest object), then `DisconnectPlayer` resets the slot again. The old object is now an orphan.
5. `PlayerListChange(id, FALSE)` removes a player only if `GetPlayerInfo(id)` still has `m_bPlayerPrivileges` (or `m_bGameMasterPrivileges`). An orphan's slot has neither, so it can never be removed.
6. `Shutdown` loops `while (m_lstPlayerList.num) DisconnectPlayer(m_lstPlayerList[0]->m_nPlayerID)`. For the orphan, `DisconnectPlayer` finds no window, `PlayerListChange` removes nothing, and the loop never ends.

The engine already protects one variant: `HandleBNK1Message` evicts an existing window for the connection on a crypto re-key ("Evicting old connection for crypto re-key (aborted connection attempt or player timeout?)"). A second `BNCS` on an already-keyed connection skips that check, which is what happened here: there was no re-key and no eviction line.

## Repro

- Engine level, with no client: on a free slot, mark the slot as a validated player and call `PlayerListChange(id, TRUE)`. Then call `CNetLayerPlayerInfo::Initialize()` (what the second BNCS does), mark the slot validated again, and call `PlayerListChange(id, TRUE)`. Then call `CNetLayer::DisconnectPlayer(id)`. One `CNWSPlayer` with that ID stays in the list, and the next shutdown spins forever. Our test plugin is attached (`repro/GhostRepro.cpp`).
- With a client: send a second connect request on the same UDP source port before the first login finishes loading (we saw 18 s between the two).

## Attached (sanitised: CD keys, IPs and player names replaced)

- **`engine-log-excerpt.txt`:** every engine-log line about the affected account, the connection lines for the windows it used, the join/leave counts, and the shutdown tail.
- **`lagwatch-freeze-stacks.txt`:** main-thread samples during the hang (perf, 199 Hz).
- **`repro/GhostRepro.cpp`:** the engine-level repro (NWNX test plugin).

## Requests

1. **Root cause:** when `HandleBNCSMessage` reuses a window that already has a validated player, remove that player first (the same leave as `DisconnectPlayer`), or reject the second request.
2. **Safety net:** have `PlayerListChange(id, FALSE)` remove a `CNWSPlayer` whose netlayer slot is no longer in use, and have `Shutdown` give up on such a player instead of looping forever.
