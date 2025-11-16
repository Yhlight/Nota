import os
import subprocess
import sys

def run_command(args):
    """Runs a command and prints its output."""
    print(f"Running command: {' '.join(args)}")
    subprocess.run(args, check=True)

def build():
    """Configures and builds the project."""
    if not os.path.exists("build"):
        os.makedirs("build")

    # Configure
    run_command(["cmake", "-S", ".", "-B", "build"])

    # Build
    run_command(["cmake", "--build", "build"])

def run():
    """Runs the main executable."""
    run_command(["build/nota"])

def test():
    """Runs the tests."""
    run_command(["build/tests/nota_tests"])


if __name__ == "__main__":
    if len(sys.argv) > 1:
        if sys.argv[1] == "build":
            build()
        elif sys.argv[1] == "run":
            run()
        elif sys.argv[1] == "test":
            test()
        else:
            print(f"Unknown command: {sys.argv[1]}")
    else:
        print("Usage: python3 build.py [build|run|test]")
