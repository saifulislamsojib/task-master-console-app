# Task Master Console App

A complete C project setup for Visual Studio Code using the clangd language server for superior code intelligence, with build, run, and debug configurations.

## Project Structure

```
c-project-setup/
├── .vscode/
│   ├── extensions.json         # Recommended extensions
│   ├── launch.json              # Debug configuration
│   ├── settings.json            # VS Code and clangd settings
│   └── tasks.json               # Build tasks
├── src/
│   └── main.c                   # Source files go here
├── bin/                         # Compiled executable (created on build)
├── obj/                         # Object files (created on build)
├── compile_commands.json        # Compilation database for clangd
├── Makefile                     # Build system (generates compile_commands.json)
├── .gitignore                   # Git ignore rules
└── README.md                    # This file
```

## Prerequisites

### Required Software

1. **Visual Studio Code**: Download from [code.visualstudio.com](https://code.visualstudio.com/)

2. **clangd Extension**: Install the official clangd extension
   - Open VS Code
   - Press `Ctrl+Shift+X` (or `Cmd+Shift+X` on Mac)
   - Search for "clangd" by LLVM
   - Click Install
   - **Important**: If you have the Microsoft C/C++ extension installed, disable its IntelliSense or uninstall it to avoid conflicts

3. **GCC Compiler**:
   - **Linux**: Usually pre-installed
     ```bash
     sudo apt update
     sudo apt install build-essential gdb
     ```
   - **macOS**: Install Xcode Command Line Tools
     ```bash
     xcode-select --install
     ```
   - **Windows**: Install MinGW-w64 or use WSL

- **Option 2 - WSL**: Install Windows Subsystem for Linux and use Linux commands

## How to Use

### Initial Setup

1. **Open the project folder** in VS Code
   - File → Open Folder → Select `c-project-setup`

2. **Install clangd extension** (if not already installed)
   - VS Code will prompt you to install recommended extensions
   - Click "Install" on the clangd extension

3. **Generate compilation database** (first time only)
   ```bash
   make compile_commands.json
   ```
   This creates `compile_commands.json` which tells clangd how to compile your project.
   The Makefile will automatically regenerate this when you build.

### Method 1: Using VS Code Tasks (Recommended)

1. **Build the project**:
   - Press `Ctrl+Shift+B` (or `Cmd+Shift+B` on Mac)
   - Or: Terminal → Run Build Task
   - This also regenerates `compile_commands.json` automatically

2. **Run the program**:
   - Press `Ctrl+Shift+P` (or `Cmd+Shift+P`)
   - Type "Run Task"
   - Select "Run Program"

3. **Debug the program**:
   - Press `F5`
   - Or: Run → Start Debugging
   - Set breakpoints by clicking left of line numbers

### Method 2: Using Makefile

```bash
# Build the project (also generates compile_commands.json)
make

# Build and run
make run

# Clean build artifacts
make clean

# Regenerate compile_commands.json only
make compile_commands.json
```

### Method 3: Using Terminal Commands

```bash
# Create bin directory if it doesn't exist
mkdir -p bin

# Compile
gcc -g src/main.c -o bin/program

# Run
./bin/program
```

## Adding New Files

1. Create new `.c` files in the `src/` directory
2. Create header files (`.h`) in the `src/` directory
3. Run `make` to rebuild and regenerate `compile_commands.json`
4. clangd will automatically pick up the new files

After adding new files, run `make` to update `compile_commands.json`.

## clangd Features

### Code Intelligence

- **Smart completion**: Context-aware suggestions as you type
- **Inlay hints**: Shows parameter names and deduced types inline
- **Go to definition**: `F12` or `Ctrl+Click` on any symbol
- **Find references**: `Shift+F12` to find all usages
- **Hover documentation**: Hover over functions to see signatures

### Code Quality

- **Real-time diagnostics**: Errors and warnings as you type
- **clang-tidy checks**: Built-in linting for common issues
- **Unused includes**: Detects and can remove unnecessary headers
- **Code formatting**: Right-click → Format Document (uses clang-format)

### Refactoring

- **Rename symbol**: `F2` to rename across the project
- **Extract function**: Select code → Code Action (`Ctrl+.`)
- **Include what you use**: Automatically manages #include directives

## Keyboard Shortcuts

- `Ctrl+Shift+B` - Build project
- `F5` - Start debugging
- `Shift+F5` - Stop debugging
- `F9` - Toggle breakpoint
- `F10` - Step over
- `F11` - Step into
- `Shift+F11` - Step out

## Troubleshooting

### clangd not working

- Install the clangd extension from LLVM
- Make sure `compile_commands.json` exists (run `make compile_commands.json`)
- Check VS Code output panel → clangd for errors
- Verify clangd is installed: run `clangd --version` in terminal

### "clangd could not be found"

- Install clangd binary (see Prerequisites)
- Or let VS Code extension download it automatically
- Set custom path in settings: `"clangd.path": "/path/to/clangd"`

### IntelliSense conflicts

- Disable Microsoft C/C++ extension IntelliSense
- Already done in `.vscode/settings.json`
- Or uninstall `ms-vscode.cpptools` extension entirely

### compile_commands.json outdated

- Run `make` to regenerate after adding/removing files
- The Makefile automatically updates it on each build

### Code completion not working

- Make sure you've built the project at least once
- Check that `compile_commands.json` includes your file
- Reload VS Code window: `Ctrl+Shift+P` → "Reload Window"

### "gcc: command not found"

- Install GCC compiler (see Prerequisites)
- Make sure GCC is in your PATH

### Program doesn't run after build

- Check that `bin/` directory exists
- Verify executable permissions: `chmod +x bin/program`

### Debugging not working

- Install `gdb`: `sudo apt install gdb` (Linux)
- Check `launch.json` has correct debugger path

## Customization

### Compiler Flags

Edit `Makefile` to add compiler flags:

- `-Wall` - Enable all warnings
- `-Wextra` - Extra warnings
- `-O2` - Optimization level 2
- `-std=c17` - Use C17 standard

### C Standard

Change in `Makefile`:

```makefile
CFLAGS = -Wall -Wextra -g -std=c11  # or c99, c17, c23
```

## Next Steps

- Add more source files in `src/`
- Create header files for modular code
- Use the debugger to step through code
- Learn about compiler optimization flags
- Explore static analysis tools
