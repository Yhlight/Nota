import os
import subprocess
import sys

def run_command(command, cwd):
    """Runs a command in a specified directory and checks for errors."""
    print(f"Running command: {' '.join(command)} in {cwd}")
    process = subprocess.Popen(command, cwd=cwd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    if process.returncode != 0:
        print(f"Error running command: {' '.join(command)}")
        print(stderr.decode())
        sys.exit(1)
    print(stdout.decode())

def main():
    """Main function to build the project."""
    build_dir = "build"
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    # Configure the project
    run_command(["cmake", ".."], cwd=build_dir)

    # Build the project
    run_command(["cmake", "--build", "."], cwd=build_dir)

    print("Build successful!")

if __name__ == "__main__":
    main()
