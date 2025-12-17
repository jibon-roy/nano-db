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
    "create table <name>",
    "list db",
    "list table",
    "use <name>",
    "insert into <table> set ...",
    "get <table>",
    "get <table> <field:value>",
    "update <table> <where> <set>",
    "delete <table> <field:value>",
    "delete table <name>",
    "delete db <name>",
    "drop table <name>",
    "drop db <name>",
    "clear",
    "cls",
    "help",
    "version",
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

// Update specific records in a table based on where clause and set clause
void update_record_in_table(const char *table_name, const char *db_name, const char *where_clause, const char *set_clause)
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

    // Parse the where clause to extract field and value
    char where_field[100], where_value[200];
    if (sscanf(where_clause, "%99[^:]:%199s", where_field, where_value) != 2)
    {
        printf("Error: Invalid where clause format. Use 'field:value' (e.g., id:1)\n");
        return;
    }

    // Parse the set clause to extract field and value
    char set_field[100], set_value[200];
    if (sscanf(set_clause, "%99[^:]:%199s", set_field, set_value) != 2)
    {
        printf("Error: Invalid set clause format. Use 'field:value' (e.g., name:NewName)\n");
        return;
    }

    // Create temporary file
    char temp_path[300];
#ifndef _WIN32
    snprintf(temp_path, sizeof(temp_path), "db/%s/%s_temp.txt", db_name, table_name);
#else
    snprintf(temp_path, sizeof(temp_path), "db\\%s\\%s_temp.txt", db_name, table_name);
#endif

    FILE *file = fopen(table_path, "r");
    FILE *temp_file = fopen(temp_path, "w");

    if (!file || !temp_file)
    {
        printf("Error: Failed to open files for update.\n");
        if (file)
            fclose(file);
        if (temp_file)
            fclose(temp_file);
        return;
    }

    char line[512];
    int updated_count = 0;

    // Read each line and update if it matches the where clause
    while (fgets(line, sizeof(line), file))
    {
        // Remove trailing newline for processing
        size_t len = strlen(line);
        bool had_newline = false;
        if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
            had_newline = true;
        }

        bool should_update = false;

        // Check if the line contains the where_field=where_value pattern
        char search_pattern[300];
        snprintf(search_pattern, sizeof(search_pattern), "%s=%s", where_field, where_value);

        char search_pattern_quoted[300];
        snprintf(search_pattern_quoted, sizeof(search_pattern_quoted), "%s=\"%s\"", where_field, where_value);

        if (strstr(line, search_pattern) != NULL || strstr(line, search_pattern_quoted) != NULL)
        {
            should_update = true;
            updated_count++;

            // Find and replace the field value in the line
            char updated_line[512];
            char *field_pos = NULL;

            // Try to find the field to update
            char old_pattern[300];
            snprintf(old_pattern, sizeof(old_pattern), "%s=", set_field);
            field_pos = strstr(line, old_pattern);

            if (field_pos)
            {
                // Copy everything before the field
                size_t prefix_len = field_pos - line + strlen(old_pattern);
                strncpy(updated_line, line, prefix_len);
                updated_line[prefix_len] = '\0';

                // Skip the old value (quoted or unquoted)
                char *value_start = field_pos + strlen(old_pattern);
                char *value_end = value_start;

                if (*value_start == '"')
                {
                    // Quoted value
                    value_end = strchr(value_start + 1, '"');
                    if (value_end)
                        value_end++;
                }
                else
                {
                    // Unquoted value - find comma or end of string
                    while (*value_end && *value_end != ',' && *value_end != ' ')
                        value_end++;
                }

                if (!value_end)
                    value_end = value_start + strlen(value_start);

                // Add the new value (with quotes if original had quotes)
                if (*value_start == '"')
                {
                    snprintf(updated_line + strlen(updated_line),
                             sizeof(updated_line) - strlen(updated_line),
                             "\"%s\"%s", set_value, value_end);
                }
                else
                {
                    snprintf(updated_line + strlen(updated_line),
                             sizeof(updated_line) - strlen(updated_line),
                             "%s%s", set_value, value_end);
                }

                // Write updated line
                fprintf(temp_file, "%s\n", updated_line);
            }
            else
            {
                // Field not found, write original line
                fprintf(temp_file, "%s%s", line, had_newline ? "\n" : "");
            }
        }
        else
        {
            // No match, write original line
            fprintf(temp_file, "%s%s", line, had_newline ? "\n" : "");
        }
    }

    fclose(file);
    fclose(temp_file);

    // Replace original file with temp file
    remove(table_path);
    rename(temp_path, table_path);

    if (updated_count > 0)
    {
        printf("Updated %d record(s) in table '%s' where %s=%s, set %s=%s.\n",
               updated_count, table_name, where_field, where_value, set_field, set_value);
    }
    else
    {
        printf("No records found matching the where clause.\n");
    }
}

// Delete specific records from a table based on query
void delete_record_from_table(const char *table_name, const char *db_name, const char *query)
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

    // Parse the query to extract field and value
    char field[100], value[200];
    if (sscanf(query, "%99[^:]:%199s", field, value) != 2)
    {
        printf("Error: Invalid query format. Use 'field:value' (e.g., id:1 or name:Hello)\n");
        return;
    }

    // Create temporary file
    char temp_path[300];
#ifndef _WIN32
    snprintf(temp_path, sizeof(temp_path), "db/%s/%s_temp.txt", db_name, table_name);
#else
    snprintf(temp_path, sizeof(temp_path), "db\\%s\\%s_temp.txt", db_name, table_name);
#endif

    FILE *file = fopen(table_path, "r");
    FILE *temp_file = fopen(temp_path, "w");

    if (!file || !temp_file)
    {
        printf("Error: Failed to open files for deletion.\n");
        if (file)
            fclose(file);
        if (temp_file)
            fclose(temp_file);
        return;
    }

    char line[512];
    int deleted_count = 0;

    // Read each line and write to temp file if it doesn't match
    while (fgets(line, sizeof(line), file))
    {
        bool should_delete = false;

        // Check if the line contains the field=value pattern
        char search_pattern[300];
        snprintf(search_pattern, sizeof(search_pattern), "%s=%s", field, value);

        char search_pattern_quoted[300];
        snprintf(search_pattern_quoted, sizeof(search_pattern_quoted), "%s=\"%s\"", field, value);

        if (strstr(line, search_pattern) != NULL || strstr(line, search_pattern_quoted) != NULL)
        {
            should_delete = true;
            deleted_count++;
        }

        // Write to temp file if not deleting
        if (!should_delete)
        {
            fputs(line, temp_file);
        }
    }

    fclose(file);
    fclose(temp_file);

    // Replace original file with temp file
    remove(table_path);
    rename(temp_path, table_path);

    if (deleted_count > 0)
    {
        printf("Deleted %d record(s) from table '%s' where %s=%s.\n", deleted_count, table_name, field, value);
    }
    else
    {
        printf("No records found matching the query.\n");
    }
}

// Delete entire table
void delete_table(const char *table_name, const char *db_name)
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

    if (remove(table_path) == 0)
    {
        printf("Table '%s' deleted successfully from database '%s'.\n", table_name, db_name);
    }
    else
    {
        printf("Error: Failed to delete table '%s'.\n", table_name);
    }
}

// Delete entire database (recursive deletion)
void delete_database(const char *db_name)
{
    if (!check_db_exists(db_name))
    {
        printf("Error: Database '%s' does not exist.\n", db_name);
        return;
    }

    char db_path[300];
#ifndef _WIN32
    snprintf(db_path, sizeof(db_path), "db/%s", db_name);
#else
    snprintf(db_path, sizeof(db_path), "db\\%s", db_name);
#endif

    // First, delete all files in the database directory
#ifdef _WIN32
    struct _finddata_t data;
    intptr_t handle;
    char search_path[300];
    snprintf(search_path, sizeof(search_path), "db\\%s\\*.*", db_name);

    handle = _findfirst(search_path, &data);
    if (handle != -1)
    {
        do
        {
            if (strcmp(data.name, ".") != 0 && strcmp(data.name, "..") != 0)
            {
                char file_path[300];
                snprintf(file_path, sizeof(file_path), "db\\%s\\%s", db_name, data.name);
                remove(file_path);
            }
        } while (_findnext(handle, &data) == 0);
        _findclose(handle);
    }
#else
    DIR *dir = opendir(db_path);
    if (dir)
    {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                char file_path[300];
                snprintf(file_path, sizeof(file_path), "db/%s/%s", db_name, entry->d_name);
                remove(file_path);
            }
        }
        closedir(dir);
    }
#endif

    // Now remove the directory itself
#ifdef _WIN32
    if (_rmdir(db_path) == 0)
#else
    if (rmdir(db_path) == 0)
#endif
    {
        printf("Database '%s' deleted successfully.\n", db_name);
    }
    else
    {
        printf("Error: Failed to delete database '%s'.\n", db_name);
    }
}

// Get all data from a table
void get_all_data(const char *table_name, const char *db_name)
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

    FILE *file = fopen(table_path, "r");
    if (!file)
    {
        printf("Error: Failed to open table file.\n");
        return;
    }

    char line[512];
    int count = 0;

    printf("Data from table '%s':\n", table_name);
    printf("-----------------------------------\n");

    while (fgets(line, sizeof(line), file))
    {
        // Remove trailing newline
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';

        if (strlen(line) > 0)
        {
            printf("%s\n", line);
            count++;
        }
    }

    printf("-----------------------------------\n");
    printf("Total records: %d\n", count);

    fclose(file);
}

// Get filtered data from a table based on query (e.g., id=1 or name=Hello)
void get_filtered_data(const char *table_name, const char *db_name, const char *query)
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

    FILE *file = fopen(table_path, "r");
    if (!file)
    {
        printf("Error: Failed to open table file.\n");
        return;
    }

    // Parse the query to extract field and value (e.g., "id:1" or "name:Hello")
    char field[100], value[200];
    if (sscanf(query, "%99[^:]:%199s", field, value) != 2)
    {
        printf("Error: Invalid query format. Use 'field:value' (e.g., id:1 or name:Hello)\n");
        fclose(file);
        return;
    }

    char line[512];
    int count = 0;
    bool found = false;

    printf("Filtered data from table '%s' where %s=%s:\n", table_name, field, value);
    printf("-----------------------------------\n");

    while (fgets(line, sizeof(line), file))
    {
        // Remove trailing newline
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';

        if (strlen(line) > 0)
        {
            // Check if the line contains the field=value pattern
            char search_pattern[300];
            snprintf(search_pattern, sizeof(search_pattern), "%s=%s", field, value);

            // Also check for quoted values
            char search_pattern_quoted[300];
            snprintf(search_pattern_quoted, sizeof(search_pattern_quoted), "%s=\"%s\"", field, value);

            if (strstr(line, search_pattern) != NULL || strstr(line, search_pattern_quoted) != NULL)
            {
                printf("%s\n", line);
                count++;
                found = true;
            }
        }
    }

    printf("-----------------------------------\n");
    if (found)
    {
        printf("Total matching records: %d\n", count);
    }
    else
    {
        printf("No records found matching the query.\n");
    }

    fclose(file);
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

    // get <table> or get <table> <query>
    if (parts >= 2 && strcmp(cmd, "get") == 0)
    {
        char table_name[100];
        char query[200] = {0};

        // Try to parse: get <table> <query>
        int scan_result = sscanf(input, "get %99s %199s", table_name, query);

        if (scan_result == 1)
        {
            // Only table name provided - get all data
            get_all_data(table_name, DB);
        }
        else if (scan_result == 2)
        {
            // Table name and query provided - filter data
            get_filtered_data(table_name, DB, query);
        }
        else
        {
            printf("Invalid get syntax. Use 'get <table>' or 'get <table> <query>'\n");
        }
        return;
    }

    // update <table> <where_clause> <set_clause>
    if (parts >= 2 && strcmp(cmd, "update") == 0)
    {
        char table_name[100];
        char where_clause[200];
        char set_clause[200];

        // Try to parse: update <table> <field:value> <field:value>
        int scan_result = sscanf(input, "update %99s %199s %199s", table_name, where_clause, set_clause);

        if (scan_result == 3)
        {
            update_record_in_table(table_name, DB, where_clause, set_clause);
        }
        else
        {
            printf("Invalid update syntax. Use 'update <table> <where_field:value> <set_field:value>'\n");
            printf("Example: update Users id:1 name:NewName\n");
        }
        return;
    }

    // delete <table> <field:value> - delete specific records
    // delete table <name> - delete entire table
    // delete db <name> - delete entire database
    if (parts >= 2 && strcmp(cmd, "delete") == 0)
    {
        // delete db <name>
        if (parts == 3 && strcmp(type, "db") == 0)
        {
            delete_database(name);
            return;
        }

        // delete table <name>
        if (parts == 3 && strcmp(type, "table") == 0)
        {
            delete_table(name, DB);
            return;
        }

        // delete <table> <field:value>
        if (parts == 3)
        {
            char table_name[100];
            char query[200];

            if (sscanf(input, "delete %99s %199s", table_name, query) == 2)
            {
                delete_record_from_table(table_name, DB, query);
            }
            else
            {
                printf("Invalid delete syntax.\n");
            }
            return;
        }

        printf("Invalid delete syntax. Use:\n");
        printf(" - delete <table> <field:value>\n");
        printf(" - delete table <name>\n");
        printf(" - delete db <name>\n");
        return;
    }

    // drop table <name> (alias for delete table)
    if (parts == 3 && strcmp(cmd, "drop") == 0 && strcmp(type, "table") == 0)
    {
        delete_table(name, DB);
        return;
    }

    // If we reach here, command was not recognized
    printf("Error: Unrecognized command '%s'. Type 'help' to see available commands.\n", input);
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
