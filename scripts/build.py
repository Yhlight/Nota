import os
import subprocess
import sys

def run_command(command):
    """Runs a command and prints its output."""
    try:
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        for line in iter(process.stdout.readline, ''):
            sys.stdout.write(line)
        process.stdout.close()
        return_code = process.wait()
        if return_code:
            raise subprocess.CalledProcessError(return_code, command)
    except FileNotFoundError:
        print(f"Error: Command not found: {command[0]}")
        sys.exit(1)

def main():
    """The main build script."""
    # Get the root directory of the project
    root_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    build_dir = os.path.join(root_dir, "build")

    # Create a build directory if it doesn't exist
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    # Change to the build directory
    os.chdir(build_dir)

    # Configure the project
    run_command(["cmake", ".."])

    # Build the project
    run_command(["cmake", "--build", "."])

    # Run the tests
    run_command(["ctest", "--output-on-failure"])

    # Find and run the executable
    executable_path = os.path.join(build_dir, "nota")
    if os.path.exists(executable_path):
        print("\\n--- Running Nota ---")
        run_command([executable_path])
    else:
        print(f"\\nError: Executable not found at {executable_path}")
        sys.exit(1)

if __name__ == "__main__":
    main()
