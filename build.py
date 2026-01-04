import argparse
import os
import subprocess
import sys

def run_command(command, cwd):
    """Runs a command in a specified directory and checks for errors."""
    print(f"Executing: {' '.join(command)} in {cwd}")
    result = subprocess.run(command, cwd=cwd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error executing command: {' '.join(command)}")
        print(f"STDOUT:\n{result.stdout}")
        print(f"STDERR:\n{result.stderr}")
        sys.exit(1)
    return result

def main():
    parser = argparse.ArgumentParser(description="Nota build script")
    parser.add_argument("command", choices=["build", "test"], help="The command to execute")
    args = parser.parse_args()

    project_dir = os.path.dirname(os.path.abspath(__file__))
    build_dir = os.path.join(project_dir, "build")

    if args.command == "build" or args.command == "test":
        os.makedirs(build_dir, exist_ok=True)

        # Configure
        run_command(["cmake", ".."], cwd=build_dir)

        # Build
        run_command(["cmake", "--build", "."], cwd=build_dir)

    if args.command == "test":
        # Run tests
        run_command(["ctest", "--output-on-failure"], cwd=build_dir)

if __name__ == "__main__":
    main()
