import os
import subprocess
import sys

BUILD_DIR = "build"
PROJECT_NAME = "nota"

def run_command(args):
    """Runs a command and exits if it fails."""
    print(f"Running: {' '.join(args)}")
    result = subprocess.run(args)
    if result.returncode != 0:
        print(f"Error: Command failed with exit code {result.returncode}")
        sys.exit(result.returncode)

def build():
    """Configures and builds the project using CMake."""
    os.makedirs(BUILD_DIR, exist_ok=True)
    cmake_args = ["cmake", "-S", ".", "-B", BUILD_DIR]
    run_command(cmake_args)
    build_args = ["cmake", "--build", BUILD_DIR]
    run_command(build_args)

def run():
    """Runs the main executable."""
    executable_path = os.path.join(BUILD_DIR, "app", PROJECT_NAME)
    if not os.path.exists(executable_path):
        print(f"Error: Executable not found at {executable_path}. Run 'build' first.")
        sys.exit(1)
    run_command([executable_path])

def test():
    """Runs the tests."""
    test_executable_path = os.path.join(BUILD_DIR, "tests", f"{PROJECT_NAME}_tests")
    if not os.path.exists(test_executable_path):
        print(f"Error: Test executable not found at {test_executable_path}. Run 'build' first.")
        sys.exit(1)
    run_command([test_executable_path])


def main():
    if len(sys.argv) < 2:
        print("Usage: python build.py [build|run|test]")
        sys.exit(1)

    command = sys.argv[1]

    if command == "build":
        build()
    elif command == "run":
        run()
    elif command == "test":
        test()
    else:
        print(f"Unknown command: {command}")
        sys.exit(1)

if __name__ == "__main__":
    main()
