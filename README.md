# nanoDB

A tiny, file-backed demonstration database shell written in C.

This repository contains a single-source example (`main.c`) that implements a minimal interactive shell for creating and managing simple database folders under a `db/` directory.

**Key points**

- Default database: `nano`
- Databases are stored as folders under the `db/` directory
- This is a learning/demo project — not for production

**Build**

Compile with GCC:

```
gcc main.c -o nano
```

**Run**

Start the shell:

```
./nano
```

You will see a prompt like `nano~$: ` (or the name of the currently selected database).

**Common commands**

- `create db <name>` — create a new database folder under `db/`
- `list db` — list database folders in `db/`
- `use <name>` — switch to an existing database (changes the prompt)
- `drop db <name>` — remove a database folder (may fail if not empty)
- `version` — print the program version
- `help` — list available commands
- `clear` / `cls` — clear the terminal
- `exit` / `quit` — exit the shell (or return to default `nano` DB)

**Example session**

```
nano~$: create db example
Database 'example' created at db/example
nano~$: list db
Databases:
 - example
nano~$: use example
Switched to database 'example'
example~$: version
nanoDB version 0.1.0
example~$: exit
nano~$: exit
Logout.
```

**Notes & limitations**

- The implementation is intentionally simple: databases are directories and there is no table/file format beyond that.
- `drop db` uses `remove()` and will fail on non-empty directories.
- Cross-platform code is partially supported (POSIX/Windows branches) but primarily tested on Linux.

If you'd like, I can expand this README with build targets, a Makefile, or examples for creating tables and inserting data.
