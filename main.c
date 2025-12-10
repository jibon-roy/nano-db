#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#endif

#define MAX_INPUT_SIZE 256
#define DB_DIR "db"
#define VERSION "0.1.0"
#define CMD_COUNT 20
#define DEFAULT_DB "nano"

#ifdef _WIN32
#include <io.h>
#else
#include <dirent.h>
#endif

char DB[50] = DEFAULT_DB;

char cmd_list[CMD_COUNT][50] = {
    "create db <name>",
    "list db",
    "version",
    "create table <name>",
    "list table",
    "insert into <table> values (...)",
    "select * from <table>",
    "update <table> set ... where ...",
    "use <name>",
    "delete from <table> where ...",
    "drop table <name>",
    "drop db <name>",
    "clear",
    "cls",
    "help",
    "exit",
    "quit",
};

// take input
void get_input(char *buffer, size_t size)
{
    if (fgets(buffer, size, stdin) == NULL)
    {
        buffer[0] = '\0';
        return;
    }
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n')
        buffer[len - 1] = '\0';
}

// Clear
void clear_screen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Cross-platform mkdir
int make_dir(const char *path)
{
    if (path == NULL)
        return -1;

#ifdef _WIN32
    return _mkdir(path);
#else
    return mkdir(path, 0755);
#endif
}

// Cross-platform rmdir
int rmdir(const char *path)
{
    if (path == NULL)
        return -1;

    return remove(path);
}

// Initialize DB directory
void initialize()
{
    make_dir(DB_DIR);
}

// Create DB folder
void create_db(const char *name)
{
    if (name == NULL || strlen(name) == 0)
    {
        printf("Invalid database name.\n");
        return;
    }

    char path[300] = {0};
    snprintf(path, sizeof(path), "db/%s", name);

    int result = make_dir(path);

    if (result == 0)
    {
        printf("Database '%s' created at %s\n", name, path);
    }
    else
    {
        printf("Failed to create database. It may already exist.\n");
    }
}

// List DB folders
void list_dbs()
{

#ifdef _WIN32
    struct _finddata_t data;
    intptr_t handle;

    handle = _findfirst("db\\*", &data);
    if (handle == -1)
    {
        printf("No databases found.\n");
        return;
    }

    bool found = false;

    do
    {
        if (data.attrib & _A_SUBDIR)
        {
            if (strcmp(data.name, ".") == 0 || strcmp(data.name, "..") == 0)
                continue;

            if (!found)
            {
                printf("Databases:\n");
                found = true;
            }

            printf(" - %s\n", data.name);
        }
    } while (_findnext(handle, &data) == 0);

    _findclose(handle);

    if (!found)
        printf("No databases found.\n");

#else
    DIR *dir = opendir("db");
    if (!dir)
    {
        printf("No databases found.\n");
        return;
    }

    struct dirent *entry;
    bool found = false;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Check if it's a directory using stat
        char path[512];
        snprintf(path, sizeof(path), "db/%s", entry->d_name);

        struct stat statbuf;
        if (stat(path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode))
        {
            if (!found)
            {
                printf("Databases:\n");
                found = true;
            }

            printf(" - %s\n", entry->d_name);
        }
    }

    closedir(dir);

    if (!found)
        printf("No databases found.\n");
#endif
}

// void check DB exists -- to be implemented

bool check_db_exists(const char *name)
{
    if (name == NULL || name[0] == '\0')
    {
        printf("Invalid database name.\n");
        return false;
    }

    char path[300] = {0};

#ifdef _WIN32
    snprintf(path, sizeof(path), "db\\%s", name);
    return (_access(path, 0) == 0);

#else
    snprintf(path, sizeof(path), "db/%s", name);
    return (access(path, F_OK) == 0);

#endif
}

// create Table (Text file) -- to be implemented
void create_table(const char *name, const char *db_name)
{
    // Check valid table name & db name
    if (!name || name[0] == '\0' || !db_name || db_name[0] == '\0')
    {
        printf("Invalid table or database name.\n");
        return;
    }

    // Check if database folder exists
    if (!check_db_exists(db_name))
    {
        printf("Error: Database '%s' not found. Please create it first or use an existing database.\n", db_name);
        return;
    }

    // Build full table file path
    char table_path[300] = {0};
    snprintf(table_path, sizeof(table_path), "db/%s/%s.txt", db_name, name);

#ifdef _WIN32
    snprintf(table_path, sizeof(table_path), "db\\%s\\%s.txt", db_name, name);
#endif
    // Create the table file
    FILE *file = fopen(table_path, "w");
    if (!file)
    {
        printf("Failed to create table file.\n");
        return;
    }

    printf("Table '%s' created successfully inside database '%s'.\n",
           name, db_name);

    fclose(file);
}

// Get next auto-increment ID for a table
int get_next_id(const char *db_name, const char *table)
{
    char table_path[300] = {0};

#ifndef _WIN32
    snprintf(table_path, sizeof(table_path), "db/%s/%s.txt", db_name, table);
#else
    snprintf(table_path, sizeof(table_path), "db\\%s\\%s.txt", db_name, table);
#endif

    FILE *file = fopen(table_path, "r");
    if (!file)
    {
        // File doesn't exist yet, start with ID 1
        return 1;
    }

    int last_id = 0;
    char line[512];

    // Read all lines to find the highest ID
    while (fgets(line, sizeof(line), file))
    {
        // Parse id= from the line
        char *id_ptr = strstr(line, "id=");
        if (id_ptr)
        {
            int current_id;
            if (sscanf(id_ptr, "id=%d", &current_id) == 1)
            {
                if (current_id > last_id)
                {
                    last_id = current_id;
                }
            }
        }
    }

    fclose(file);
    return last_id + 1;
}

// Check if table exists in a database
bool check_table_exists(const char *db_name, const char *table_name)
{
    if (!db_name || db_name[0] == '\0' || !table_name || table_name[0] == '\0')
        return false;

    if (!check_db_exists(db_name))
    {
        printf("Database '%s' does not exist.\n", db_name);
        return false;
    }

    char table_path[300] = {0};

#ifndef _WIN32
    snprintf(table_path, sizeof(table_path), "db/%s/%s.txt", db_name, table_name);
#else
    snprintf(table_path, sizeof(table_path), "db\\%s\\%s.txt", db_name, table_name);
#endif

#ifdef _WIN32
    return (_access(table_path, 0) == 0);
#else
    return (access(table_path, F_OK) == 0);
#endif
}

// insert into table with attributes
void insert_table_with_attributes(const char *table_name, const char *db_name, const char *attributes)
{
    if (!check_table_exists(db_name, table_name))
    {
        printf("Error: Table '%s' does not exist in database '%s'.\n", table_name, db_name);
        return;
    }
    char table_path[300] = {0};
#ifndef _WIN32
    snprintf(table_path, sizeof(table_path), "db/%s/%s.txt", db_name, table_name);
#else
    snprintf(table_path, sizeof(table_path), "db\\%s\\%s.txt", db_name, table_name);
#endif

    FILE *file = fopen(table_path, "a");
    if (!file)
    {
        printf("Error: Failed to open table file for appending.\n");
        return;
    }

    int next_id = get_next_id(db_name, table_name);

    // Write the new record
    fprintf(file, "id=%d, %s\n", next_id, attributes);
    fflush(file); // Ensure data is written to disk

    printf("Inserted record with ID %d into table '%s'.\n", next_id, table_name);

    fclose(file);
}
// List all tables in a given database
void list_tables(const char *db_name)
{
    if (!db_name || db_name[0] == '\0')
    {
        printf("Invalid database name.\n");
        return;
    }

    if (!check_db_exists(db_name))
    {
        printf("Database '%s' does not exist.\n", db_name);
        return;
    }

    printf("Tables in database '%s':\n", db_name);

#ifndef _WIN32
    char path[300];
    snprintf(path, sizeof(path), "db/%s", db_name);
    DIR *dir = opendir(path);

    if (!dir)
    {
        printf("No tables found.\n");
        return;
    }

    struct dirent *entry;
    bool found = false;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Only show .txt files (tables)
        size_t len = strlen(entry->d_name);
        if (len > 4 && strcmp(entry->d_name + len - 4, ".txt") == 0)
        {
            if (!found)
                found = true;
            // Remove .txt extension when printing
            char table_name[100];
            strncpy(table_name, entry->d_name, len - 4);
            table_name[len - 4] = '\0';
            printf(" - %s\n", table_name);
        }
    }

    closedir(dir);

    if (!found)
        printf("No tables found.\n");

#else
    struct _finddata_t data;
    intptr_t handle;
    char search_path[300];
    snprintf(search_path, sizeof(search_path), "db\\%s\\*.txt", db_name);

    handle = _findfirst(search_path, &data);
    if (handle == -1)
    {
        printf("No tables found.\n");
        return;
    }

    bool found = false;

    do
    {
        char table_name[100];
        size_t len = strlen(data.name);
        if (len > 4 && strcmp(data.name + len - 4, ".txt") == 0)
        {
            if (!found)
                found = true;
            strncpy(table_name, data.name, len - 4);
            table_name[len - 4] = '\0';
            printf(" - %s\n", table_name);
        }
    } while (_findnext(handle, &data) == 0);

    _findclose(handle);

    if (!found)
        printf("No tables found.\n");

#endif
}

// drop DB folder
void drop_db(const char *name)
{
    if (name == NULL || strlen(name) == 0)
    {
        printf("Invalid database name.\n");
        return;
    }
    char path[300] = {0};
    snprintf(path, sizeof(path), "db/%s", name);
    int result = rmdir(path);

    if (result == 0)
    {
        printf("Database '%s' deleted from %s\n", name, path);
    }
    else
    {
        printf("Failed to delete database. It may not exist or is not empty.\n");
    }
}

// Command run
void process_command(const char *input)
{
    char cmd[50], type[50], name[100];

    int parts = sscanf(input, "%49s %49s %99s", cmd, type, name);

    // create db <name>
    if (parts == 3 && strcmp(cmd, "create") == 0 && strcmp(type, "db") == 0)
    {
        create_db(name);
        return;
    }

    // drop db <name>
    if (parts == 3 && strcmp(cmd, "drop") == 0 && strcmp(type, "db") == 0)
    {
        drop_db(name);
        return;
    }

    // list db
    if (parts == 2 && strcmp(cmd, "list") == 0 && strcmp(type, "db") == 0)
    {
        list_dbs();
        return;
    }

    if (parts == 2 && strcmp(cmd, "use") == 0)
    {
        const char *dbname = type; // use `type` as the DB name

        // Build path to database folder
        char path[300];
        snprintf(path, sizeof(path), "%s/%s", DB_DIR, dbname);

// Check if folder exists
#ifdef _WIN32
        if (_access(path, 0) != 0)
        {
            printf("Database '%s' does not exist.\n", dbname);
            return;
        }
#else
        if (access(path, F_OK) != 0)
        {
            printf("Database '%s' does not exist.\n", dbname);
            return;
        }
#endif
        // Copy database name safely
        strncpy(DB, dbname, sizeof(DB) - 1);
        DB[sizeof(DB) - 1] = '\0';

        printf("Switched to database '%s'\n", DB);
        return;
    }

    // help
    if (strcmp(input, "help") == 0)
    {
        printf("Available commands:\n");
        for (int i = 0; i <= CMD_COUNT; i++)
        {
            printf(" - %s\n", cmd_list[i]);
        }
        return;
    }

    // version
    if (strcmp(input, "version") == 0)
    {
        printf("nanoDB version %s\n", VERSION);
        return;
    }

    // create table <name>
    if (parts == 3 && strcmp(cmd, "create") == 0 && strcmp(type, "table") == 0)
    {
        create_table(name, DB);
        return;
    }

    // list table
    if (parts == 2 && strcmp(cmd, "list") == 0 && strcmp(type, "table") == 0)
    {
        list_tables(DB);
        return;
    }

    // insert into <table> set name="jibon", roll=12, ...
    if (parts >= 3 && strcmp(cmd, "insert") == 0 && strcmp(type, "into") == 0)
    {
        char table_name[100];
        char rest[300];

        // Get table name and rest of the command
        if (sscanf(input, "insert into %99s %299[^\n]", table_name, rest) != 2)
        {
            printf("Invalid insert syntax.\n");
            return;
        }

        // Check if command uses 'set'
        char *attributes_ptr = strstr(rest, "set ");

        if (attributes_ptr)
        {
            // 'set' keyword found, move pointer after 'set '
            attributes_ptr += 4;
        }
        else
        {
            // No 'set' keyword, treat entire rest as attributes
            attributes_ptr = rest;
        }

        // Trim leading spaces
        while (*attributes_ptr == ' ')
            attributes_ptr++;

        // Verify we have attributes to insert
        if (*attributes_ptr == '\0')
        {
            printf("Error: No attributes provided for insert.\n");
            return;
        }

        // Insert into table by treating the attributes part as CSV-style data
        insert_table_with_attributes(table_name, DB, attributes_ptr);
        return;
    }
}
int main()
{
    const char *USERNAME_ADMIN = "admin";
    const char *PASSWORD_ADMIN = "admin123";

    // Admin login
    char admin_password[100];
    char admin_username[100];
    printf("Enter username : ");
    fgets(admin_username, sizeof(admin_username), stdin);
    // Remove newline character if present
    admin_username[strcspn(admin_username, "\n")] = 0;
    if (strcmp(admin_username, USERNAME_ADMIN) != 0)
    {
        printf("Incorrect username. Exiting.\n");
        return 1;
    }

    printf("Enter admin password: ");
    fgets(admin_password, sizeof(admin_password), stdin);
    // Remove newline character if present
    admin_password[strcspn(admin_password, "\n")] = 0;

    if (strcmp(admin_password, PASSWORD_ADMIN) != 0)
    {
        printf("Incorrect password. Exiting.\n");
        return 1;
    }

    initialize();

    char buffer[MAX_INPUT_SIZE] = {0};

    while (true)
    {
        printf("%s~$: ", DB);

        get_input(buffer, MAX_INPUT_SIZE);

        // exit
        if (strcmp(buffer, "exit") == 0 || strcmp(buffer, "quit") == 0)
        {
            if (strcmp(DB, "nano") == 0)
            {

                printf("Logout.\n");
                break;
            }
            else
            {
                strncpy(DB, DEFAULT_DB, sizeof(DB) - 1);
                DB[sizeof(DB) - 1] = '\0';
            }
        }
        // fallback
        if (strlen(buffer) == 0)
            continue;

        // clear cmd line
        if (strcmp(buffer, "clear") == 0 || strcmp(buffer, "cls") == 0)
        {
            clear_screen();
            continue;
        }

        if (strcmp(buffer, "^[[A") == 0 || strcmp(buffer, "^[[B") == 0 || strcmp(buffer, "^[[C") == 0 || strcmp(buffer, "^[[D") == 0)
        {
            continue;
        }

        process_command(buffer);
    }

    return 0;
}
