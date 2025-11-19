import os
import subprocess
import argparse
import sys

def run_command(command, cwd):
    """Runs a command in a specified directory and checks for errors."""
    print(f"Executing: {' '.join(command)} in {cwd}")
    result = subprocess.run(command, cwd=cwd)
    if result.returncode != 0:
        print(f"Error executing command: {' '.join(command)}")
        sys.exit(1)

def main():
    parser = argparse.ArgumentParser(description="CMake build script for Nota.")
    parser.add_argument("--clean", action="store_true", help="Clean the build directory before building.")
    parser.add_argument("--config", default="Debug", help="Build configuration (e.g., Debug, Release).")
    parser.add_argument("--run", action="store_true", help="Run the main executable after building.")
    parser.add_argument("--test", action="store_true", help="Run tests after building.")
    args = parser.parse_args()

    project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
    build_dir = os.path.join(project_root, "build")

    if args.clean and os.path.exists(build_dir):
        print(f"Cleaning build directory: {build_dir}")
        import shutil
        shutil.rmtree(build_dir)

    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    cmake_command = ["cmake", f"-DCMAKE_BUILD_TYPE={args.config}", ".."]
    build_command = ["cmake", "--build", "."]

    run_command(cmake_command, cwd=build_dir)
    run_command(build_command, cwd=build_dir)

    if args.run:
        executable_path = os.path.join(build_dir, "src", "Nota")
        run_command([executable_path], cwd=project_root)

    if args.test:
        test_executable_path = os.path.join(build_dir, "NotaTests")
        run_command([test_executable_path], cwd=build_dir)

if __name__ == "__main__":
    main()
