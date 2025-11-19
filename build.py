import os
import subprocess
import sys
import platform

def run_command(command, cwd):
    """Runs a command in a specified directory and checks for errors."""
    print(f"Running command: {' '.join(command)} in {cwd}")
    result = subprocess.run(command, cwd=cwd, capture_output=True, text=True, check=False)
    if result.returncode != 0:
        print(f"Error running command: {' '.join(command)}")
        print(f"stdout:\n{result.stdout}")
        print(f"stderr:\n{result.stderr}")
        sys.exit(1)
    print(result.stdout)
    return result

def main():
    """
    Main function to build the Nota project using CMake.
    """
    build_dir = "build"
    project_root = os.path.dirname(os.path.abspath(__file__))
    build_path = os.path.join(project_root, build_dir)


    # Create build directory if it doesn't exist
    if not os.path.exists(build_path):
        os.makedirs(build_path)

    # Configure the project with CMake
    cmake_command = ["cmake", ".."]
    run_command(cmake_command, cwd=build_path)

    # Build the project
    build_command = ["cmake", "--build", "."]
    run_command(build_command, cwd=build_path)

    # Find the executable
    executable_name = "nota"
    if platform.system() == "Windows":
        executable_name += ".exe"

    executable_path = os.path.join(build_path, executable_name)

    if os.path.exists(executable_path):
        print(f"\nBuild successful! Executable at: {os.path.relpath(executable_path)}")
    else:
        print("Error: Executable not found!")
        # Let's search for it, maybe it is in a subdirectory
        for root, dirs, files in os.walk(build_path):
            if executable_name in files:
                executable_path = os.path.join(root, executable_name)
                print(f"\nBuild successful! Executable at: {os.path.relpath(executable_path)}")
                sys.exit(0)
        sys.exit(1)


if __name__ == "__main__":
    main()
