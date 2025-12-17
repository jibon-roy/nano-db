// Include header for standard input/output functions (printf, fgets, etc.)
#include <stdio.h>
// Include header for string functions (strlen, strcmp, strncpy, etc.)
#include <string.h>
// Include header for boolean type (true, false)
#include <stdbool.h>
// Include header for memory allocation (malloc, free, etc.)
#include <stdlib.h>
// Include header for POSIX API functions (access, etc.)
#include <unistd.h>

// Check if compiling on Windows
#ifdef _WIN32
// Include Windows specific header for directory operations
#include <direct.h>
#else
// Include header for file status structures on Unix-like systems
#include <sys/stat.h>
// Include header for system data types on Unix-like systems
#include <sys/types.h>
// Include header for directory operations on Unix-like systems
#include <dirent.h>
#endif

// Define maximum input buffer size for user commands (256 characters)
#define MAX_INPUT_SIZE 256
// Define the database directory path
#define DB_DIR "db"
// Define the application version number
#define VERSION "0.1.0"
// Define total number of available commands
#define CMD_COUNT 20
// Define the default database name
#define DEFAULT_DB "nano"

// Include Windows-specific header for file access functions
#ifdef _WIN32
#include <io.h>
#else
// Include directory operations header for Unix-like systems
#include <dirent.h>
#endif

// Global variable to store the current database name (initially set to "nano")
char DB[50] = DEFAULT_DB;

// Array to store all available command descriptions for help functionality
char cmd_list[CMD_COUNT][50] = {
    "create db <name>",         // Command to create a new database
    "list db",                   // Command to list all databases
    "version",                   // Command to show version
    "create table <name>",       // Command to create a new table
    "insert into <table> values (...)", // Command to insert data into a table
    "select * from <table>",     // Command to retrieve all records from a table
    "update <table> set ... where ...", // Command to update records in a table
    "use <name>",                // Command to switch to a database
    "delete from <table> where ...", // Command to delete records from a table
    "drop table <name>",         // Command to delete a table
    "drop db <name>",            // Command to delete a database
    "clear",                     // Command to clear the screen
    "cls",                       // Command to clear the screen (Windows version)
    "help",                      // Command to display help information
    "exit",                      // Command to exit the program
    "quit",                      // Command to exit the program (alias)
};

// Function to read input from user and store it in a buffer
void get_input(char *buffer, size_t size)
{
    // Read input from stdin, store in buffer with max size limit
    if (fgets(buffer, size, stdin) == NULL)
    {
        // If reading fails, clear the buffer
        buffer[0] = '\0';
        return;
    }
    // Get the length of the string stored in buffer
    size_t len = strlen(buffer);
    // Check if the last character is a newline and remove it
    if (len > 0 && buffer[len - 1] == '\n')
        buffer[len - 1] = '\0';
}

// Function to clear the console screen (cross-platform)
void clear_screen()
{
// Check if compiling on Windows
#ifdef _WIN32
    // Execute Windows command to clear screen
    system("cls");
#else
    // Execute Unix command to clear screen
    system("clear");
#endif
}

// Function to create a directory (cross-platform)
int make_dir(const char *path)
{
    // Validate that path is not NULL
    if (path == NULL)
        return -1;

// Check if compiling on Windows
#ifdef _WIN32
    // Use Windows function to create directory
    return _mkdir(path);
#else
    // Use Unix function to create directory with permissions 0755
    return mkdir(path, 0755);
#endif
}

// Function to remove a directory or file (cross-platform)
int rmdir(const char *path)
{
    // Validate that path is not NULL
    if (path == NULL)
        return -1;

    // Use standard remove function to delete file or directory
    return remove(path);
}

// Function to initialize the database system by creating the db directory
void initialize()
{
    // Create the main database directory
    make_dir(DB_DIR);
}

// Function to create a new database with the specified name
void create_db(const char *name)
{
    // Check if the database name is valid (not NULL and not empty)
    if (name == NULL || strlen(name) == 0)
    {
        // Print error message if name is invalid
        printf("Invalid database name.\n");
        return;
    }

    // Create a buffer to store the full path of the database directory
    char path[300] = {0};
    // Format the path as "db/<name>" using snprintf for safety
    snprintf(path, sizeof(path), "db/%s", name);

    // Attempt to create the directory at the specified path
    int result = make_dir(path);

    // Check if directory creation was successful
    if (result == 0)
    {
        // Print success message with database name and path
        printf("Database '%s' created at %s\n", name, path);
    }
    else
    {
        // Print error message if directory creation failed
        printf("Failed to create database. It may already exist.\n");
    }
}

// Function to list all databases in the db directory (Windows version)
void list_dbs()
{

// Check if compiling on Windows
#ifdef _WIN32
    // Declare structure to store file information
    struct _finddata_t data;
    // Declare handle for file search operations
    intptr_t handle;

    // Search for all items in the db directory
    handle = _findfirst("db\\*", &data);
    // Check if the search was successful
    if (handle == -1)
    {
        // Print message if no databases found
        printf("No databases found.\n");
        return;
    }

    // Flag to track if any database has been found
    bool found = false;

    // Loop through all items in the directory
    do
    {
        // Check if the current item is a directory
        if (data.attrib & _A_SUBDIR)
        {
            // Skip current directory (.) and parent directory (..)
            if (strcmp(data.name, ".") == 0 || strcmp(data.name, "..") == 0)
                continue;

            // Print header only on first database found
            if (!found)
            {
                printf("Databases:\n");
                found = true;
            }

            // Print the database name with a dash prefix
            printf(" - %s\n", data.name);
        }
    } while (_findnext(handle, &data) == 0); // Continue until no more items

    // Close the search handle to free resources
    _findclose(handle);

    // Print message if no databases were found
    if (!found)
        printf("No databases found.\n");

// Unix/Linux version of list_dbs
#else
    // Open the db directory for reading
    DIR *dir = opendir("db");
    // Check if directory opening was successful
    if (!dir)
    {
        // Print message if database directory doesn't exist
        printf("No databases found.\n");
        return;
    }

    // Declare pointer to store directory entries
    struct dirent *entry;
    // Flag to track if any database has been found
    bool found = false;

    // Read each entry in the directory
    while ((entry = readdir(dir)) != NULL)
    {
        // Skip current directory (.) and parent directory (..)
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Check if it's a directory using stat
        // Create full path to the entry
        char path[512];
        snprintf(path, sizeof(path), "db/%s", entry->d_name);

        // Declare structure to store file information
        struct stat statbuf;
        // Get file information and check if it's a directory
        if (stat(path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode))
        {
            // Print header only on first database found
            if (!found)
            {
                printf("Databases:\n");
                found = true;
            }

            // Print the database name with a dash prefix
            printf(" - %s\n", entry->d_name);
        }
    }

    // Close the directory handle to free resources
    closedir(dir);

    // Print message if no databases were found
    if (!found)
        printf("No databases found.\n");
#endif
}

// Function to check if a database with the given name exists
bool check_db_exists(const char *name)
{
    // Validate that database name is not NULL or empty
    if (name == NULL || name[0] == '\0')
    {
        // Print error message for invalid name
        printf("Invalid database name.\n");
        return false;
        }

    // Create a buffer to store the full path
    char path[300] = {0};

// Check if compiling on Windows
#ifdef _WIN32
    // Format Windows path for the database directory
    snprintf(path, sizeof(path), "db\\%s", name);
    // Check if path exists using Windows access function (0 means existence check)
    return (_access(path, 0) == 0);

#else
    // Format Unix path for the database directory
    snprintf(path, sizeof(path), "db/%s", name);
    // Check if path exists using Unix access function (F_OK checks for existence)
    return (access(path, F_OK) == 0);

#endif
}

// Function to create a table file within a specified database
void create_table(const char *name, const char *db_name)
{
    // Check if table name, db name are valid, and not using default database
    if (!name || name[0] == '\0' || !db_name || db_name[0] == '\0' || strcmp(db_name, DEFAULT_DB) == 0)
    {
        // Print error message if parameters are invalid
        printf("Invalid table or database name.\n");
        return;
    }

    // Check if the specified database folder exists
    if (!check_db_exists(db_name))
    {
        // Print error message if database not found
        printf("Error: Database '%s' not found.\n", db_name);
        return;
    }

    // Build full table file path
    char table_path[300] = {0};
    // Format path for Unix/Linux systems
    snprintf(table_path, sizeof(table_path), "db/%s/%s.txt", db_name, name);

// Check if compiling on Windows
#ifdef _WIN32
    // Format path for Windows systems (using backslashes)
    snprintf(table_path, sizeof(table_path), "db\\%s\\%s.txt", db_name, name);
#endif
    // Open the table file in write mode to create it
    FILE *file = fopen(table_path, "w");
    // Check if file creation was successful
    if (!file)
    {
        // Print error message if file creation failed
        printf("Failed to create table file.\n");
        return;
    }

    // Print success message with table and database names
    printf("Table '%s' created successfully inside database '%s'.\n",
           name, db_name);

    // Close the file handle to free resources
    fclose(file);
}

// Function to delete a database folder and all its contents
void drop_db(const char *name)
{
    // Check if the database name is valid (not NULL and not empty)
    if (name == NULL || strlen(name) == 0)
    {
        // Print error message if name is invalid
        printf("Invalid database name.\n");
        return;
    }
    // Create a buffer to store the full path of the database directory
    char path[300] = {0};
    // Format the path as "db/<name>"
    snprintf(path, sizeof(path), "db/%s", name);
    // Attempt to remove the directory and all its contents
    int result = rmdir(path);

    // Check if directory removal was successful
    if (result == 0)
    {
        // Print success message with database name and path
        printf("Database '%s' deleted from %s\n", name, path);
    }
    else
    {
        // Print error message if directory removal failed
        printf("Failed to delete database. It may not exist or is not empty.\n");
    }
}

// Function to parse and execute user commands
void process_command(const char *input)
{
    // Declare variables to store parsed command parts
    char cmd[50], type[50], name[100];

    // Parse input string into up to 3 parts separated by spaces
    int parts = sscanf(input, "%49s %49s %99s", cmd, type, name);

    // Handle "create db <name>" command
    if (parts == 3 && strcmp(cmd, "create") == 0 && strcmp(type, "db") == 0)
    {
        // Call function to create database with the specified name
        create_db(name);
        return;
    }

    // Handle "drop db <name>" command
    if (parts == 3 && strcmp(cmd, "drop") == 0 && strcmp(type, "db") == 0)
    {
        // Call function to delete database with the specified name
        drop_db(name);
        return;
    }

    // Handle "list db" command
    if (parts == 2 && strcmp(cmd, "list") == 0 && strcmp(type, "db") == 0)
    {
        // Call function to list all available databases
        list_dbs();
        return;
    }

    // Handle "use <database_name>" command
    if (parts == 2 && strcmp(cmd, "use") == 0)
    {
        // Extract database name from the parsed type variable
        const char *dbname = type; // use `type` as the DB name

        // Build path to database folder
        char path[300];
        snprintf(path, sizeof(path), "%s/%s", DB_DIR, dbname);

// Check if compiling on Windows
#ifdef _WIN32
        // Check if database folder exists using Windows function
        if (_access(path, 0) != 0)
        {
            // Print error message if database not found
            printf("Database '%s' does not exist.\n", dbname);
            return;
        }
#else
        // Check if database folder exists using Unix function
        if (access(path, F_OK) != 0)
        {
            // Print error message if database not found
            printf("Database '%s' does not exist.\n", dbname);
            return;
        }
#endif
        // Copy database name to global DB variable safely
        strncpy(DB, dbname, sizeof(DB) - 1);
        // Ensure null termination of the string
        DB[sizeof(DB) - 1] = '\0';

        // Print confirmation message with the new database name
        printf("Switched to database '%s'\n", DB);
        return;
    }

    // Handle "help" command
    if (strcmp(input, "help") == 0)
    {
        // Print header for available commands
        printf("Available commands:\n");
        // Loop through and print all commands in the cmd_list array
        for (int i = 0; i <= CMD_COUNT; i++)
        {
            // Print each command with a dash prefix
            printf(" - %s\n", cmd_list[i]);
        }
        return;
    }

    // Handle "version" command
    if (strcmp(input, "version") == 0)
    {
        // Print application version number
        printf("nanoDB version %s\n", VERSION);
        return;
    }

    // Handle "create table <name>" command
    if (parts == 3 && strcmp(cmd, "create") == 0 && strcmp(type, "table") == 0)
    {
        // Call function to create table in the current database
        create_table(name, DB);
        return;
    }

    // Default fallback for unrecognized commands
    // Print error message with the unrecognized command
    printf("Command not recognized: %s\n", input);
}

// Main program entry point
int main()
{
    // Define the admin username constant
    const char *USERNAME_ADMIN = "admin";
    // Define the admin password constant
    const char *PASSWORD_ADMIN = "admin123";

    // Declare buffer for admin password input
    char admin_password[100];
    // Declare buffer for admin username input
    char admin_username[100];
    // Prompt user to enter username
    printf("Enter username : ");
    // Read username from standard input
    fgets(admin_username, sizeof(admin_username), stdin);
    // Remove newline character if present from the username
    admin_username[strcspn(admin_username, "\n")] = 0;
    // Check if entered username matches the admin username
    if (strcmp(admin_username, USERNAME_ADMIN) != 0)
    {
        // Print error message and exit if username is incorrect
        printf("Incorrect username. Exiting.\n");
        return 1;
    }

    // Prompt user to enter password
    printf("Enter admin password: ");
    // Read password from standard input
    fgets(admin_password, sizeof(admin_password), stdin);
    // Remove newline character if present from the password
    admin_password[strcspn(admin_password, "\n")] = 0;

    // Check if entered password matches the admin password
    if (strcmp(admin_password, PASSWORD_ADMIN) != 0)
    {
        // Print error message and exit if password is incorrect
        printf("Incorrect password. Exiting.\n");
        return 1;
    }

    // Initialize the database system by creating necessary directories
    initialize();

    // Declare buffer to store user command input
    char buffer[MAX_INPUT_SIZE] = {0};

    // Main program loop that runs until exit command is issued
    while (true)
    {
        // Print prompt with current database name
        printf("%s~$: ", DB);

        // Read user input into buffer
        get_input(buffer, MAX_INPUT_SIZE);

        // Handle exit and quit commands
        if (strcmp(buffer, "exit") == 0 || strcmp(buffer, "quit") == 0)
        {
            // Check if currently in the default database
            if (strcmp(DB, "nano") == 0)
            {

                // Print logout message and exit the program
                printf("Logout.\n");
                break;
            }
            else
            {
                // Switch back to default database if in another database
                strncpy(DB, DEFAULT_DB, sizeof(DB) - 1);
                // Ensure null termination of the string
                DB[sizeof(DB) - 1] = '\0';
            }
        }
        // Skip processing if input is empty
        if (strlen(buffer) == 0)
            continue;

        // Handle clear screen commands
        if (strcmp(buffer, "clear") == 0 || strcmp(buffer, "cls") == 0)
        {
            // Call function to clear the screen
            clear_screen();
            continue;
        }

        // Skip processing for arrow key input sequences
        if (strcmp(buffer, "^[[A") == 0 || strcmp(buffer, "^[[B") == 0 || strcmp(buffer, "^[[C") == 0 || strcmp(buffer, "^[[D") == 0)
        {
            continue;
        }

        // Process the command entered by the user
        process_command(buffer);
    }

    // Return 0 to indicate successful program termination
    return 0;
}
