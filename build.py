import os
import subprocess
import sys
import shutil

def run_command(command, cwd=None):
    """Runs a command and checks for errors."""
    try:
        subprocess.run(command, check=True, cwd=cwd, shell=True)
    except subprocess.CalledProcessError as e:
        print(f"Error running command: {' '.join(command)}", file=sys.stderr)
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)

def main():
    """Main function for the build script."""
    build_dir = "build"

    # Create build directory if it doesn't exist
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    # Configure the project with CMake
    print("--- Configuring project with CMake ---")
    run_command(f"cmake -S . -B {build_dir}",)

    # Build the project
    print("--- Building project ---")
    run_command(f"cmake --build {build_dir}")

    # Optionally run tests
    if len(sys.argv) > 1 and sys.argv[1] == "test":
        print("--- Running tests ---")
        run_command("./tests/nota_tests", cwd=build_dir)

if __name__ == "__main__":
    main()
