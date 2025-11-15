#!/usr/bin/env python3

import os
import subprocess
import argparse
import shutil
import sys

def get_script_path():
    return os.path.dirname(os.path.realpath(sys.argv[0]))

def run_command(command, cwd):
    """Runs a command in a specified directory and checks for errors."""
    print(f"Running command: {' '.join(command)} in {cwd}")
    result = subprocess.run(command, cwd=cwd)
    if result.returncode != 0:
        print(f"Error: Command failed with exit code {result.returncode}")
        sys.exit(result.returncode)

def main():
    parser = argparse.ArgumentParser(description="CMake build script for Nota.")
    parser.add_argument("action", choices=["build", "run", "test", "clean"], help="The action to perform.")
    parser.add_argument("--args", help="Arguments to pass to the executable when using 'run' action.")

    args = parser.parse_args()

    root_dir = get_script_path()
    build_dir = os.path.join(root_dir, "build")
    nota_executable = os.path.join(build_dir, "src", "nota")
    test_executable = os.path.join(build_dir, "tests", "nota_tests")

    if args.action == "build":
        os.makedirs(build_dir, exist_ok=True)
        run_command(["cmake", ".."], cwd=build_dir)
        run_command(["make"], cwd=build_dir)
        print("Build complete.")

    elif args.action == "run":
        if not os.path.exists(nota_executable):
            print("Executable not found. Please run the 'build' action first.")
            sys.exit(1)

        run_args = [nota_executable]
        if args.args:
            run_args.extend(args.args.split())

        run_command(run_args, cwd=root_dir)

    elif args.action == "test":
        if not os.path.exists(test_executable):
            print("Test executable not found. Please run the 'build' action first.")
            sys.exit(1)
        run_command([test_executable], cwd=root_dir)

    elif args.action == "clean":
        if os.path.exists(build_dir):
            print(f"Removing build directory: {build_dir}")
            shutil.rmtree(build_dir)
            print("Clean complete.")
        else:
            print("Build directory not found. Nothing to clean.")

if __name__ == "__main__":
    main()
