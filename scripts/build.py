import os
import subprocess
import sys

def run_command(command):
    """Runs a command and checks for errors."""
    try:
        subprocess.run(command, check=True, shell=True)
    except subprocess.CalledProcessError as e:
        print(f"Error running command: {e}", file=sys.stderr)
        sys.exit(1)

def main():
    """Main function for the build script."""
    # Create build directory if it doesn't exist
    if not os.path.exists("build"):
        os.makedirs("build")

    # Change to the build directory
    os.chdir("build")

    # Run CMake
    run_command("cmake ..")

    # Build the project
    run_command("cmake --build .")

    # Run the executable or tests (optional)
    if len(sys.argv) > 1:
        if sys.argv[1] == "run":
            print("\\n--- Running Nota ---")
            if sys.platform == "win32":
                run_command(".\\Debug\\nota.exe")
            else:
                run_command("./nota")
        elif sys.argv[1] == "test":
            print("\\n--- Running Tests ---")
            run_command("ctest")

if __name__ == "__main__":
    main()
