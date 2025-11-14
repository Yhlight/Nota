import os
import subprocess
import sys

def run_command(command):
    """Runs a command and exits if it fails."""
    try:
        subprocess.run(command, check=True, shell=True)
    except subprocess.CalledProcessError as e:
        print(f"Error running command: {e}", file=sys.stderr)
        sys.exit(1)

def main():
    """Builds and runs the Nota project."""
    # Create a build directory if it doesn't exist
    if not os.path.exists("build"):
        os.makedirs("build")

    # Change to the build directory
    os.chdir("build")

    # Configure the project with CMake
    run_command("cmake ..")

    # Build the project
    run_command("cmake --build .")

    # Run the executable
    print("\n--- Running Nota ---")
    run_command("./nota")
    print("--- Nota finished ---\n")

    # Run the tests
    print("\n--- Running Tests ---")
    run_command("ctest")
    print("--- Tests finished ---\n")

if __name__ == "__main__":
    main()
