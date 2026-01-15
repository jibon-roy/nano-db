# nanoDB Documentation (Student Notes)

## 1. What is nanoDB

nanoDB is a tiny, file-based database project we built for practicing systems thinking. Everything lives in plain text files inside a `db/` folder. There is no heavy server; commands run directly in the terminal and write to `.txt` tables.

## 2. Why this nanoDB

I wanted something simpler than MySQL or Postgres to understand how data gets stored. With nanoDB I can see the files, open them, and know exactly what changed. It is also light enough to run on low-end laptops in our campus labs.

## 3. What we research

- How schemas are defined using a single header line.
- How CRUD operations translate into string writes and reads.
- How to handle validation (field checking) with only standard C functions.
- Cross-platform directory handling (Linux/Windows) without external libs.

## 4. How to use this DB system

1. Run the program: `./main` (or the Windows build) and log in with the admin credentials.
2. Create a database: `create db mydb` then switch: `use mydb`.
3. Create a table: `create table students` and type columns like `name, dept, roll` when prompted.
4. Insert data: `insert into students set name:Arif, dept:CSE, roll:2022334`.
5. View data: `get students` or filter: `get students roll:2022334`.
6. Update: `update students roll:2022334 name:Arif_Hasan`.
7. Delete: `delete students roll:2022334`.
8. List tables: `list table`; list DBs: `list db`; show schema: `schema students`.

## 5. Which field it is best

Best for small classroom projects, quick prototypes, and learning exercises where we need zero setup. Good fit for:

- Teaching basic DB concepts without installing big servers.
- Offline demos on shared lab PCs.
- Simple logging or config storage in text form.

## 6. Tips from my experience

- Keep table names and column names short to avoid typos.
- Always check `schema <table>` before inserting or updating.
- Use `version` to confirm the build, and `help` to see all commands.
- Back up the `db/` folder before experimenting with delete or drop commands.

## 7. Limitations we noticed

- No concurrent access handling; one user at a time is safest.
- Data is plain text, so there is no encryption by default.
- Complex queries (joins, sorting) are manual; we focus on simple filters only.

## 8. Future ideas we want to try

- Add export/import to CSV so we can move data to Excel or bigger DBs.
- Add basic indexing on columns like `id` or `roll` to speed up lookups.
- Write unit tests around the command parser and file I/O.
- Maybe add a simple CLI history so typing feels smoother.

## 9. Quick command reference

- `help` — show the command menu.
- `create db <name>` — make a new database folder.
- `use <name>` — switch current database.
- `create table <name>` — add a table (prompts for columns).
- `insert into <table> set ...` — add a row.
- `get <table> [field:value]` — view rows (all or filtered).
- `update <table> <where> <set>` — modify rows.
- `delete <table> <field:value>` — remove rows.
- `delete db <name>` / `drop db <name>` — remove database (careful!).
- `exit` / `quit` — leave the app.

## 10. Closing note

This project feels like a small lab partner. It is simple, but it helped me see how databases actually touch the filesystem. If something breaks, I can open the `.txt` files and fix them by hand. That makes learning less scary and more transparent.
