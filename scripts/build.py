#!/usr/bin/env python3
import os
import sys
import subprocess
from pathlib import Path
import time

PROJECT_NAME = "SocketProgramming"
CONFIG = "Debug"

# Detect platform
IS_WINDOWS = os.name == "nt"

def run_cmd(cmd, cwd=None):
    """Run a shell command and print output live"""
    try:
        subprocess.run(cmd, cwd=cwd, check=True, shell=True)
    except subprocess.CalledProcessError as e:
        print(f"Command failed: {e}")
        sys.exit(1)

def config_project():
    premake = Path("vendor/premake/premake5.exe" if IS_WINDOWS else "vendor/premake/premake5")

    if not premake.exists():
        print(f"Error: Premake not found at {premake}")
        sys.exit(1)

    run_cmd(f"{premake} clean")
    print(f"==== Generating ({CONFIG}) Build Files ====")

    if IS_WINDOWS:
        run_cmd(f"{premake} vs2026")   # Use Visual Studio on Windows
    else:
        run_cmd(f"{premake} gmake")   # Use GNU Make on Linux

    print("==== Build Files Generation Complete ====\n")

def build_project():
    print(f"==== BUILDING PROJECT {PROJECT_NAME} ====")

    project_dir = Path(PROJECT_NAME)
    if not project_dir.exists():
        print(f"Error: Project directory {project_dir} not found")
        sys.exit(1)

    if IS_WINDOWS:
        print("Build manually using visual studio")
    else:
        # Linux build with make
        run_cmd("bear -- make")

    print("==== BUILD FINISHED ====\n")

def run_project():
    print(f"==== Running the executable: {PROJECT_NAME} ====")

    if IS_WINDOWS:
        exe_name = f"{PROJECT_NAME}.exe"
    else:
        exe_name = f"{PROJECT_NAME}.out"

    def find(name, path):
        for root, dirs, files in os.walk(path):
            if name in files:
                return os.path.join(root, name)

    exe_path = find(exe_name, str(Path("build") / "bin"))
    if not exe_path or not os.path.exists(exe_path):
        print(f"Executable {exe_path} not found")
        print("Project Not Compiled")
        print("==== ====")
        return

    print(f"Running executable at {exe_path}")

    def launch_in_new_terminal(command, title):
        # Launch each role in its own terminal window so they can run concurrently.
        if IS_WINDOWS:
            terminal_cmd = f'start "{title}" cmd /k "{command}"'
        else:
            terminal_cmd = f'x-terminal-emulator -T "{title}" -e {command}'

        try:
            subprocess.Popen(terminal_cmd, shell=True)
        except Exception as e:
            print(f"Failed to launch {title}: {e}")
            sys.exit(1)

    server_cmd = f'"{exe_path}" server'
    client_cmd = f'"{exe_path}" client'

    launch_in_new_terminal(server_cmd, f"{PROJECT_NAME} Server")
    print("Server launched. Waiting 3 seconds before starting client...")
    time.sleep(3)
    launch_in_new_terminal(client_cmd, f"{PROJECT_NAME} Client")
    print("Client launched.")
    print("========\n")

def main():
    if len(sys.argv) == 1:
        config_project()
        if not IS_WINDOWS:
            build_project()
            run_project()
    elif sys.argv[1] == "-b":
        build_project()
    elif sys.argv[1] == "-c":
        config_project()
    elif sys.argv[1] == "-r":
        run_project()
    else:
        print("Usage: build.py [-b | -c | -r]")

if __name__ == "__main__":
    main()
