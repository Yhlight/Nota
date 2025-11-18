import os
import sys
import subprocess

def run_command(command):
    """Executes a command and exits if it fails."""
    try:
        subprocess.run(command, check=True, shell=True)
    except subprocess.CalledProcessError as e:
        print(f"Error executing command: {e}")
        sys.exit(1)

def build():
    """Configures and builds the project."""
    print("--- Configuring and Building Project ---")
    os.makedirs("build", exist_ok=True)
    run_command("cmake -S . -B build")
    run_command("cmake --build build")
    print("--- Build successful ---")

def run():
    """Runs the main executable."""
    print("--- Running Application ---")
    executable_path = os.path.join("build", "app", "nota")
    if not os.path.exists(executable_path):
        print(f"Executable not found at {executable_path}. Please build the project first.")
        sys.exit(1)
    run_command(executable_path)
    print("--- Application finished ---")


def test():
    """Runs the tests."""
    print("--- Running Tests ---")
    test_executable_path = os.path.join("build", "tests", "nota_tests")
    if not os.path.exists(test_executable_path):
        print(f"Test executable not found at {test_executable_path}. Please build the project first.")
        sys.exit(1)
    run_command(test_executable_path)
    print("--- Tests finished ---")

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 build.py [build|run|test]")
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
        print("Usage: python3 build.py [build|run|test]")
        sys.exit(1)

if __name__ == "__main__":
    main()
