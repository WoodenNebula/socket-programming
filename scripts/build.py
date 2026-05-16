#!/usr/bin/env python3
import os
from os.path import isdir
import sys
import subprocess
from pathlib import Path

PROJECT_NAME = "TCP-Messaging"
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

    exe_path = f"build/bin/{PROJECT_NAME}/{exe_name}"

    if not exe_path or not os.path.exists(exe_path):
        print(f"Executable {exe_path} not found")
        print("Project Not Compiled")
        print("==== ====")
        return
    print (f"Running executable at {exe_path}")
    run_cmd(exe_path)
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
