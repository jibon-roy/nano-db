# nanoDB

A lightweight, file-backed database shell written in C with full CRUD operations support.

This project implements a minimal interactive shell for creating and managing databases, tables, and records. All data is stored as text files under a `db/` directory for easy inspection and portability.

## Features

- ✅ **Cross-platform support** — Works on Windows, Linux, and macOS
- ✅ **Database management** — Create, list, use, and delete databases
- ✅ **Table operations** — Create tables, list tables, delete tables
- ✅ **CRUD operations** — Insert, retrieve (get), update, and delete records
- ✅ **Query filtering** — Search records by field:value
- ✅ **Auto-increment IDs** — Each record gets an automatic ID
- ✅ **Simple text storage** — All data stored as human-readable text files

**Key points**

- Default database: `nano`
- Databases are stored as folders under the `db/` directory
- Tables are stored as `.txt` files inside database folders
- Records are stored with comma-separated key=value pairs
- Authentication: Username `admin`, Password `admin123`

## Build

Compile with GCC:

```bash
gcc -o main main.c
```

Or on Windows:

```bash
gcc -o main.exe main.c
```

## Run

Start the shell:

```bash
./main
```

You will be prompted for login credentials:

```
Enter username : admin
Enter admin password: admin123
```

After successful login, you will see a prompt like `nano~$: ` (or the name of the currently selected database).

---

## Complete Command Reference

### Database Management Commands

#### `create db <name>`

Creates a new database folder.

**Usage:**

```
nano~$: create db myapp
Database 'myapp' created at db/myapp
```

#### `list db`

Lists all existing databases.

**Usage:**

```
nano~$: list db
Databases:
 - myapp
 - testdb
```

#### `use <name>`

Switches to an existing database. Changes the prompt to show the current database.

**Usage:**

```
nano~$: use myapp
Switched to database 'myapp'
myapp~$:
```

#### `delete db <name>`

Deletes an entire database with all its tables and data. **Cannot be undone.**

**Usage:**

```
myapp~$: delete db myapp
Database 'myapp' deleted successfully.
```

#### `drop db <name>`

Alias for `delete db`. Removes a database folder (must be empty).

**Usage:**

```
nano~$: drop db myapp
Database 'myapp' deleted from db/myapp
```

---

### Table Management Commands

#### `create table <name>`

Creates a new table in the current database.

**Usage:**

```
myapp~$: create table users
Table 'users' created successfully inside database 'myapp'.
```

#### `list table`

Lists all tables in the current database.

**Usage:**

```
myapp~$: list table
Tables in database 'myapp':
 - users
 - products
```

#### `delete table <name>`

Deletes an entire table and all its records. **Cannot be undone.**

**Usage:**

```
myapp~$: delete table users
Table 'users' deleted successfully from database 'myapp'.
```

#### `drop table <name>`

Alias for `delete table`. Removes a table from the current database.

**Usage:**

```
myapp~$: drop table users
Table 'users' deleted successfully from database 'myapp'.
```

---

### Data Manipulation Commands

#### `insert into <table> set <field:value> [, <field:value> ...]`

Inserts a new record into a table. The record automatically receives an auto-incremented ID.

**Usage:**

```
myapp~$: insert into users set name:John, email:john@example.com, age:30
Inserted record with ID 1 into table 'users'.

myapp~$: insert into users set name:Jane, email:jane@example.com, age:28
Inserted record with ID 2 into table 'users'.
```

**Note:** Records are stored with automatic `id=` prefix and comma-separated fields.

#### `get <table>`

Retrieves and displays **all records** from a table.

**Usage:**

```
myapp~$: get users
Data from table 'users':
-----------------------------------
id=1, name:John, email:john@example.com, age:30
id=2, name:Jane, email:jane@example.com, age:28
-----------------------------------
Total records: 2
```

#### `get <table> <field:value>`

Retrieves and displays **filtered records** matching the query.

**Usage:**

```
myapp~$: get users name:John
Filtered data from table 'users' where name=John:
-----------------------------------
id=1, name:John, email:john@example.com, age:30
-----------------------------------
Total matching records: 1
```

**Examples:**

```
myapp~$: get users id:1
myapp~$: get users email:jane@example.com
myapp~$: get users age:30
```

#### `update <table> <where_field:value> <set_field:value>`

Updates records matching a WHERE condition with a new value.

**Usage:**

```
myapp~$: update users id:1 name:Jonathan
Updated 1 record(s) in table 'users' where id=1, set name=Jonathan.
```

**More Examples:**

```
myapp~$: update users name:John age:31
myapp~$: update users email:jane@example.com name:Jane_Smith
myapp~$: update users id:2 email:jane.doe@example.com
```

#### `delete <table> <field:value>`

Deletes records matching the query.

**Usage:**

```
myapp~$: delete users id:1
Deleted 1 record(s) from table 'users' where id=1.
```

**More Examples:**

```
myapp~$: delete users name:John
myapp~$: delete users email:old@example.com
myapp~$: delete users age:30
```

---

### Utility Commands

#### `help`

Displays all available commands.

**Usage:**

```
myapp~$: help
Available commands:
 - create db <name>
 - create table <name>
 - list db
 - list table
 - use <name>
 - insert into <table> set ...
 - get <table>
 - get <table> <field:value>
 - update <table> <where> <set>
 - delete <table> <field:value>
 - delete table <name>
 - delete db <name>
 - drop table <name>
 - drop db <name>
 - clear
 - cls
 - help
 - version
 - exit
 - quit
```

#### `version`

Displays the nanoDB version.

**Usage:**

```
myapp~$: version
nanoDB version 0.1.0
```

#### `clear` / `cls`

Clears the terminal screen.

**Usage:**

```
myapp~$: clear
```

#### `exit` / `quit`

Exits the application (or returns to default `nano` database if in another database).

**Usage:**

```
myapp~$: exit
nano~$: exit
Logout.
```

---

## Complete Example Session

```bash
$ ./main
Enter username : admin
Enter admin password: admin123

nano~$: create db store
Database 'store' created at db/store

nano~$: use store
Switched to database 'store'

store~$: create table products
Table 'products' created successfully inside database 'store'.

store~$: create table orders
Table 'orders' created successfully inside database 'store'.

store~$: list table
Tables in database 'store':
 - products
 - orders

store~$: insert into products set name:Laptop, price:999, stock:5
Inserted record with ID 1 into table 'products'.

store~$: insert into products set name:Mouse, price:25, stock:50
Inserted record with ID 2 into table 'products'.

store~$: insert into products set name:Keyboard, price:75, stock:20
Inserted record with ID 3 into table 'products'.

store~$: get products
Data from table 'products':
-----------------------------------
id=1, name:Laptop, price:999, stock:5
id=2, name:Mouse, price:25, stock:50
id=3, name:Keyboard, price:75, stock:20
-----------------------------------
Total records: 3

store~$: get products price:25
Filtered data from table 'products' where price=25:
-----------------------------------
id=2, name:Mouse, price:25, stock:50
-----------------------------------
Total matching records: 1

store~$: update products id:1 stock:10
Updated 1 record(s) in table 'products' where id=1, set stock=10.

store~$: delete products id:3
Deleted 1 record(s) from table 'products' where id=3.

store~$: get products
Data from table 'products':
-----------------------------------
id=1, name:Laptop, price:999, stock:10
id=2, name:Mouse, price:25, stock:50
-----------------------------------
Total records: 2

store~$: exit
nano~$: exit
Logout.
```

---

## Data Storage Format

Each record is stored as a text line in the table file:

```
id=1, name:John, email:john@example.com, age:30
id=2, name:Jane, email:jane@example.com, age:28
```

- Each field is separated by commas
- Fields use `key:value` format
- IDs are automatically assigned and incremented
- All data is human-readable and easily inspectable

Example directory structure:

```
db/
├── store/
│   ├── products.txt
│   └── orders.txt
└── myapp/
    └── users.txt
```

---

## Error Handling

The application provides clear error messages for invalid operations:

```
nano~$: get nonexistent
Error: Table 'nonexistent' does not exist in database 'nano'.

nano~$: invalid_command
Error: Unrecognized command 'invalid_command'. Type 'help' to see available commands.

nano~$: update users name:John
Error: Invalid update syntax. Use 'update <table> <where_field:value> <set_field:value>'
Example: update Users id:1 name:NewName
```

---

## Cross-Platform Compatibility

- ✅ **Windows** — Full support with MinGW GCC
- ✅ **Linux** — Full support with GCC/Clang
- ✅ **macOS** — Full support with GCC/Clang
- All path separators are handled automatically
- All file operations work across platforms

---

## Notes & Limitations

- This is a learning/demonstration project, not for production use
- No transaction support
- No table schemas or data types
- Single-threaded (no concurrent access support)
- Data is not encrypted or backed up automatically
- All data stored as plain text files for simplicity

---

## Future Enhancements

- [ ] Table schemas with data types
- [ ] Backup and restore functionality
- [ ] Export to CSV/JSON
- [ ] Advanced query filtering (AND/OR conditions)
- [ ] Indexes for faster searching
- [ ] User roles and permissions
