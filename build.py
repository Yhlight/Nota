import os
import subprocess
import sys
import argparse

def run_command(command):
    """Runs a command and prints its output."""
    try:
        subprocess.run(command, check=True, text=True)
    except subprocess.CalledProcessError as e:
        print(f"Error executing command: {' '.join(command)}")
        sys.exit(1)

def main():
    """Main function to build the Nota project."""
    parser = argparse.ArgumentParser(description="Build and run the Nota project.")
    parser.add_argument("--run", action="store_true", help="Run the Nota executable after building.")
    parser.add_argument("--test", action="store_true", help="Run the tests after building.")
    args = parser.parse_args()

    build_dir = "build"

    # Create build directory if it doesn't exist
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    # Configure the project
    cmake_configure_command = ["cmake", "-S", ".", "-B", build_dir]
    run_command(cmake_configure_command)

    # Build the project
    cmake_build_command = ["cmake", "--build", build_dir]
    run_command(cmake_build_command)

    if args.run:
        print("\n--- Running Nota ---")
        run_command([os.path.join(build_dir, "src", "nota")])

    if args.test:
        print("\n--- Running Tests ---")
        run_command([os.path.join(build_dir, "tests", "nota_tests")])

if __name__ == "__main__":
    main()
