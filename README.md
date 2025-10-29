# Custom System Monitor & Process Manager

## Phase 1: Basic Process Lister

A command-line tool to list all running processes with their details.

### Features
- List all running processes with PID, Name, State, and Memory usage
- Read process information from Linux `/proc` filesystem
- Clean tabular display

### Build Instructions
```bash
# Compile the project
make

# Run the program
./task_manager

# Clean build files
make clean
