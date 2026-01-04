import argparse
import os
import subprocess
import sys

def run_command(command, cwd):
    """Runs a command in a specified directory and checks for errors."""
    print(f"Executing: {' '.join(command)} in {cwd}")
    result = subprocess.run(command, cwd=cwd)
    if result.returncode != 0:
        print(f"Error executing command: {' '.join(command)}")
        sys.exit(1)

def main():
    parser = argparse.ArgumentParser(description="Nota build script")
    parser.add_argument("command", choices=["build"], help="The command to execute")
    args = parser.parse_args()

    project_dir = os.path.dirname(os.path.abspath(__file__))
    build_dir = os.path.join(project_dir, "build")

    if args.command == "build":
        os.makedirs(build_dir, exist_ok=True)

        # Configure
        run_command(["cmake", ".."], cwd=build_dir)

        # Build
        run_command(["cmake", "--build", "."], cwd=build_dir)

if __name__ == "__main__":
    main()
