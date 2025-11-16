import os
import subprocess
import sys
import shutil

BUILD_DIR = "build"
EXECUTABLE_NAME = "nota"

def run_command(command, cwd=None):
    """Runs a command and checks for errors."""
    try:
        subprocess.run(command, check=True, shell=True, cwd=cwd)
    except subprocess.CalledProcessError as e:
        print(f"Error running command: {' '.join(command)}", file=sys.stderr)
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)
    except FileNotFoundError:
        print(f"Error: Command '{command[0]}' not found. Make sure it's in your PATH.", file=sys.stderr)
        sys.exit(1)


def build_project():
    """Configures and builds the project using CMake."""
    print("--- Configuring and Building Project ---")
    if not os.path.exists(BUILD_DIR):
        os.makedirs(BUILD_DIR)

    cmake_configure_command = "cmake -S . -B " + BUILD_DIR
    cmake_build_command = "cmake --build " + BUILD_DIR

    run_command(cmake_configure_command)
    run_command(cmake_build_command)
    print("--- Build Succeeded ---")


def run_executable(args):
    """Runs the compiled executable."""
    print("--- Running Executable ---")
    executable_path = os.path.join(BUILD_DIR, EXECUTABLE_NAME)

    if sys.platform == "win32":
        executable_path += ".exe"

    if not os.path.exists(executable_path):
        print(f"Error: Executable not found at '{executable_path}'. Did you build the project?", file=sys.stderr)
        sys.exit(1)

    run_command([executable_path] + args)


def clean_build():
    """Removes the build directory."""
    print("--- Cleaning Build Directory ---")
    if os.path.exists(BUILD_DIR):
        shutil.rmtree(BUILD_DIR)
        print(f"Removed directory: {BUILD_DIR}")
    else:
        print("Build directory not found, nothing to clean.")

def print_usage():
    """Prints the usage instructions."""
    print("Usage: python build.py [command]")
    print("Commands:")
    print("  build      - Builds the project.")
    print("  run [args] - Runs the executable with optional arguments.")
    print("  clean      - Removes the build directory.")
    print("  help       - Shows this help message.")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print_usage()
        sys.exit(1)

    command = sys.argv[1]

    if command == "build":
        build_project()
    elif command == "run":
        run_executable(sys.argv[2:])
    elif command == "clean":
        clean_build()
    elif command == "help":
        print_usage()
    else:
        print(f"Unknown command: {command}")
        print_usage()
        sys.exit(1)
