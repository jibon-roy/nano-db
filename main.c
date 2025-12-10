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

    // default fallback
    printf("Command not recognized: %s\n", input);
}

int main()
{

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
