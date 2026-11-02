@page sql Readme
@ingroup sql

General data access, storage and manipulation of persistent data in a database.

## Migration
@ref sql_migrate "Migration from Legacy NWNX"

## Environment Variables

### NWNX_SQL_TYPE

Controls the target database type used at runtime.

Possible values (case insensitive):

* ``MYSQL``
* ``POSTGRESQL``
* ``SQLITE``

__Example__

```
export NWNX_SQL_TYPE=POSTGRESQL
```

### NWNX_SQL_HOST

Host name where the database resides.  Typically ``localhost`` but can be any valid host name or IP address.

Not used if using SQLite.

__Example__

```
export NWNX_SQL_HOST=localhost
```

### NWNX_SQL_PORT

The port for the database connection. If left unset it will use the default port.

Only used by MySQL.

__Example__

```
export NWNX_SQL_PORT=3307
```

### NWNX_SQL_USERNAME

Database username used for the connection.

Not used if using SQLite.

__Example__

```
export NWNX_SQL_USERNAME=nwn_db
```

### NWNX_SQL_PASSWORD

Password for the database connection.

Not used if using SQLite.

__Example__

```
export NWNX_SQL_PASSWORD=lathander_rulez
```

### NWNX_SQL_DATABASE

The database to connect to.

The filename of the database when using SQLite, it is stored in the UserDirectory/database folder.

__Example__

```
export NWNX_SQL_DATABASE=mymodulename
```

### NWNX_SQL_QUERY_METRICS

Export query execution metrics.

The Metrics_InfluxDB plugin and a visualizer like Grafana are required to view these metrics.

__Example__

```
export NWNX_SQL_QUERY_METRICS=true
```

### NWNX_SQL_USE_UTF8

Convert all strings going between the database and game to/from UTF8

This takes into account the core locale as well.

__Example__

```
export NWNX_SQL_USE_UTF8=true
```

### NWNX_SQL_CHARACTER_SET

Set the connection's character set to be used.

Only supported on mysql and pgsql. For sqlite this can be achieved with a query.

__Examples (MySQL)__

```
export NWNX_SQL_CHARACTER_SET=utf8
export NWNX_SQL_CHARACTER_SET=cp1251
```

__Examples (PostgreSQL)__

```
export NWNX_SQL_CHARACTER_SET=WIN1252
```

(see https://www.postgresql.org/docs/current/multibyte.html for list)

### NWNX_SQL_ASYNC_DISABLE

Kill switch for async queries. When `true`, no worker thread is started and `NWNX_SQL_ExecuteAsync()` runs each
query synchronously, at the call, on a separate connection of its own (so a query prepared through the sync API
and `NWNX_SQL_GetLastError()` are left alone). The callback still runs on the next tick, so scripts behave the same.
`NWNX_SQL_AsyncFlush()` then returns `TRUE` at once. Default `false`.

Async queries also run this way when `NWNX_SQL_TYPE` isn't `MYSQL`.

```
export NWNX_SQL_ASYNC_DISABLE=true
```

### NWNX_SQL_ASYNC_QUEUE_LIMIT

The most async jobs that can wait in the queue. Past it, `NWNX_SQL_ExecuteAsync()` refuses the job: it returns 0,
the job never runs and no callback follows. An error is logged (at most once a second, with a count). The job isn't
run synchronously instead, because that would put it ahead of older queued jobs. Default `10000`.

### NWNX_SQL_ASYNC_SHUTDOWN_TIMEOUT

Seconds to spend running the remaining async jobs when the server shuts down. Jobs still queued after that are
logged and dropped; a job still running has its connection killed so the thread can stop. Default `10`.

### NWNX_SQL_ASYNC_CONNECT_TIMEOUT, NWNX_SQL_ASYNC_READ_TIMEOUT, NWNX_SQL_ASYNC_WRITE_TIMEOUT

Network timeouts in seconds for the async worker's connection only (MySQL `MYSQL_OPT_CONNECT_TIMEOUT`,
`MYSQL_OPT_READ_TIMEOUT`, `MYSQL_OPT_WRITE_TIMEOUT`). `0` means the client library's default. Defaults `5`, `30`
and `30`. The read timeout also limits how long a single async query may run before it fails with "lost connection"
(2013, not retried), so it must be longer than any legitimate async query. The main connection is not affected.

## Async queries

`NWNX_SQL_ExecuteAsync()` queues a query for a worker thread that has its own database connection, so a slow query
doesn't stall the server. The worker is part of this plugin; it doesn't use the core's shared async thread.

* **One worker per server, first in first out.** One server's async statements run in the order they were queued.
  Nothing ever runs ahead of a queued job: when the queue is full, new jobs are refused (`ExecuteAsync` returns 0).
* **Bound parameters.** Push one parameter per `?` with `NWNX_SQL_PushAsyncParam()` (string),
  `NWNX_SQL_PushAsyncParamInt()`, `NWNX_SQL_PushAsyncParamFloat()` or `NWNX_SQL_PushAsyncParamNULL()`, in order,
  then call `NWNX_SQL_ExecuteAsync()`. Values are bound by the client library, not spliced into the SQL. A job whose
  parameter count doesn't match the query fails.
* **Callbacks.** Finished jobs are picked up once per main loop tick. If a callback script is given, it runs then,
  on the main thread, with `oCallbackTarget` as `OBJECT_SELF` (`OBJECT_INVALID` = the module); never inside
  `NWNX_SQL_ExecuteAsync()` or `NWNX_SQL_AsyncFlush()`. A job that finishes while callbacks are running (for
  example one a callback queued) waits for the next tick, so a callback that retries can't hold the main thread. If
  the target object no longer exists, the callback is skipped (logged at debug level). Inside the callback,
  `NWNX_SQL_GetAsyncJobId()`, `NWNX_SQL_GetAsyncSuccess()`, `NWNX_SQL_GetAsyncAffectedRows()`,
  `NWNX_SQL_GetAsyncLastError()` and `NWNX_SQL_AsyncReadyToReadNextRow()` / `NWNX_SQL_AsyncReadNextRow()` /
  `NWNX_SQL_AsyncReadDataInActiveRow()` describe that job. Each job has its own result rows; they never mix with
  the synchronous `NWNX_SQL_ReadNextRow()` results.
* **Reconnects** happen on the worker thread only, with a backoff of up to 5 s while the database is unreachable.
  The job at the head of the queue waits (it isn't failed), and the queue fills up to the queue limit. The retry
  rules match the synchronous path: a statement is re-prepared after a reconnect, run again once only after
  "server has gone away" (2006), and never after "lost connection during query" (2013), because it may already
  have run.
* **Flush.** `NWNX_SQL_AsyncFlush(fTimeout)` blocks until the queue is empty and the running job is done, or the
  timeout passes. While the worker can't reach the database it returns `FALSE` straight away (or as soon as a
  connection attempt fails), so flushes during an outage don't freeze the server for the whole timeout.
* **Shutdown.** When the plugin unloads, new jobs are refused, the queue is drained for up to
  `NWNX_SQL_ASYNC_SHUTDOWN_TIMEOUT` seconds, and whatever is left is logged as NOT run. Jobs that ran during the
  drain don't get callbacks; any that failed are logged as errors with their query. A crash loses the queued jobs.
* **Metrics.** With `NWNX_SQL_QUERY_METRICS=true`, each finished job pushes `SQLAsync` (`wait_ns`, `exec_ns`,
  `depth`), resampled to the per-second maximum. There are no per-query tags. At log level debug, every job also logs
  its wait, run time and the queue depth.

### Consistency rules

1. One server's async writes happen in order.
2. A synchronous read doesn't see async writes that are still queued. Read the value from locals cached on the
   object, or keep that write synchronous.
3. Call `NWNX_SQL_AsyncFlush()` before a player leaves the server (portal transfer, OnClientLeave, before a
   restart), so the next server sees the saved state. Check its result: `FALSE` means some writes haven't landed yet.
4. Keep these synchronous: gold, bank, items and containers; insert-then-read-ID; ban, VPN and login checks.
5. Check `NWNX_SQL_ExecuteAsync()`'s result for writes that matter: 0 means the job was refused (queue full or
   shutting down) and will never run.
