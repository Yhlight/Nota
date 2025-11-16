import os
import sys
import subprocess

def run_command(command, cwd=None):
    """Runs a command and checks for errors."""
    try:
        subprocess.run(command, check=True, cwd=cwd, shell=True)
    except subprocess.CalledProcessError as e:
        print(f"Error running command: {e}")
        sys.exit(1)
    except FileNotFoundError:
        print(f"Error: Command '{command.split()[0]}' not found. Make sure it's in your PATH.")
        sys.exit(1)

def build():
    """Configures and builds the project using CMake."""
    if not os.path.exists("build"):
        os.makedirs("build")

    print("Configuring project...")
    run_command("cmake -S . -B build",)

    print("Building project...")
    run_command("cmake --build build")

def run():
    """Runs the compiled executable."""
    executable_path = os.path.join("build", "Nota")
    if sys.platform == "win32":
        executable_path += ".exe"

    if not os.path.exists(executable_path):
        print(f"Error: Executable not found at '{executable_path}'.")
        print("Please build the project first using 'python build.py build'.")
        sys.exit(1)

    print(f"Running {executable_path}...")
    run_command(executable_path)

def test():
    """Runs the tests."""
    print("Running tests...")
    run_command("ctest --test-dir build --output-on-failure")

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
