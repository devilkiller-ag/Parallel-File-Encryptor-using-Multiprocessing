## Parallel File Encryptor


## Prerequisite to run the project
Create a `.env` file having your `encryption_key` in the project root directory.


### Prerequisite to debug in VS Code

Add following files inside the `.vscode` folder:

1. `launch.json`:
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug encrypt_decrypt",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/encrypt_decrypt",
            "args": [],
            "cwd": "${workspaceFolder}",
            "preLaunchTask": "build"
        },
        {
            "name": "Debug cryption (standalone)",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/cryption",
            "args": ["your_task_data_here"],
            "cwd": "${workspaceFolder}",
            "preLaunchTask": "build"
        }
    ]
}
```
For linux use `"type": "cppdbg"`, for mac use `"type": "lldb"`, and for windows use `"type": "gdb"`.


2. `task.json`:
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "type": "shell",
            "label": "build",
            "command": "make",
            "args": [],
            "options": {
                "cwd": "${workspaceFolder}"
            },
            "problemMatcher": [
                "$gcc"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            }
        }
    ]
}
```
