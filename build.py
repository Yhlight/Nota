import os
import subprocess
import sys

def run_command(command):
    """Runs a command and prints its output."""
    try:
        subprocess.run(command, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error executing command: {' '.join(command)}")
        sys.exit(1)

def main():
    """Builds and runs the Nota project."""
    # Create build directory if it doesn't exist
    if not os.path.exists("build"):
        os.makedirs("build")

    # Configure CMake
    run_command(["cmake", "-S", ".", "-B", "build"])

    # Build the project
    run_command(["cmake", "--build", "build"])

    # Run the executable
    print("\n--- Running Nota ---")
    run_command(["build/src/nota"])

if __name__ == "__main__":
    main()
