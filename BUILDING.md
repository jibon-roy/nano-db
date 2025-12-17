# Building nanoDB

## Quick Start (Windows)

### Method 1: Using the Build Script (Easiest)

1. Navigate to the project directory
2. Double-click `build.bat`
3. The executable `main.exe` will be created
4. Double-click `run.bat` to start the application

### Method 2: Command Line (PowerShell/CMD)

```powershell
cd "d:\Working file\projects\nano-db"
"C:\MinGW\bin\gcc.exe" -Wall -Wextra -g main.c -o main.exe
```

### Method 3: Using VS Code Build Task

1. Press `Ctrl + Shift + B` to open build tasks
2. Select "C/C++: gcc.exe build active file"
3. Wait for compilation to complete

### Method 4: Manual from Command Prompt

```cmd
cd d:\Working file\projects\nano-db
C:\MinGW\bin\gcc.exe main.c -o main.exe
main.exe
```

---

## Build Options

### Standard Build (Recommended)

```bash
gcc -Wall -Wextra -g main.c -o main.exe
```

- `-Wall` : Enable all warnings
- `-Wextra` : Extra warnings
- `-g` : Include debug info

### Optimized Build

```bash
gcc -O2 -Wall main.c -o main.exe
```

- `-O2` : Optimization level 2 (faster execution)

### Minimal Build (Quick)

```bash
gcc main.c -o main.exe
```

---

## Troubleshooting

### Error: "Permission denied" or "cannot open output file"

**Solution:** The executable is still running.

- Close the running `main.exe` window
- Use the build script (it auto-kills running instances)
- Or manually: `taskkill /F /IM main.exe`

### Error: "gcc.exe not found"

**Solution:** MinGW is not in PATH

- Use full path: `C:\MinGW\bin\gcc.exe main.c -o main.exe`
- Or add MinGW to system PATH: `C:\MinGW\bin`

### Error: "cannot find -lm" or linking errors

**Solution:** This project doesn't require external libraries

- If you see this error, it's likely a compiler issue
- Try updating MinGW or using a different compiler

### Build Succeeds but Program Crashes

**Solution:** Check the following

- You have proper read/write permissions in the project directory
- Create a `db/` folder if it doesn't exist manually
- Run from the project root directory

---

## Running the Application

### Windows Command Prompt

```cmd
cd d:\Working file\projects\nano-db
main.exe
```

### Windows PowerShell

```powershell
cd 'd:\Working file\projects\nano-db'
.\main.exe
```

### From Build Script

Double-click `run.bat`

---

## Project Structure

```
nano-db/
├── main.c              (Source code)
├── README.md           (Documentation)
├── build.bat           (Build script for Windows)
├── run.bat             (Run script)
├── BUILDING.md         (This file)
└── db/                 (Database directory - auto-created)
    └── nano/           (Default database)
        └── (tables as .txt files)
```

---

## Login Credentials

After building and running:

- **Username:** `admin`
- **Password:** `admin123`

---

## Testing the Build

After successful compilation, test with:

```bash
# Windows
main.exe

# Then try these commands in the shell:
help
version
create db test_db
list db
exit
```

Expected output should show the help menu and database operations working.

---

## Building on Linux/macOS

```bash
gcc -Wall -Wextra -g main.c -o main
./main
```

---

## Advanced: Create a Makefile (Optional)

Create a file named `Makefile` in the project directory:

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = main
SOURCES = main.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) $(TARGET).exe

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
```

Then build with:

```bash
make          # Compile
make run      # Compile and run
make clean    # Remove executables
```

---

## Support

If you encounter build issues:

1. Check you have MinGW installed: `gcc --version`
2. Ensure main.c is in the project directory
3. Try the batch scripts first (they handle permissions)
4. Check the README.md for project information
