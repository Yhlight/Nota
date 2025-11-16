import os
import subprocess
import sys

def run_command(command):
    """Runs a command and prints its output."""
    try:
        subprocess.run(command, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error running command: {' '.join(command)}")
        print(e)
        sys.exit(1)

def main():
    """Builds the Nota project using CMake."""
    build_dir = "build"
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    os.chdir(build_dir)

    run_command(["cmake", ".."])
    run_command(["cmake", "--build", "."])

    # Move the executable to the root directory
    executable_name = "nota"
    if sys.platform == "win32":
        executable_name += ".exe"

    executable_path = os.path.join(os.getcwd(), executable_name)
    if os.path.exists(executable_path):
        os.rename(executable_path, os.path.join("..", executable_name))


if __name__ == "__main__":
    main()
