import os
import sys
import subprocess

def run_command(args):
    try:
        subprocess.run(args, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error executing command: {' '.join(args)}")
        print(e)
        sys.exit(1)

def build():
    if not os.path.exists("build"):
        os.makedirs("build")

    run_command(["cmake", "-S", ".", "-B", "build"])
    run_command(["cmake", "--build", "build"])

def test():
    if not os.path.exists("build"):
        print("Build directory not found. Please run 'build' first.")
        sys.exit(1)

    run_command(["cmake", "--build", "build", "--target", "test"])

def run():
    if not os.path.exists("build/nota"):
        print("Executable not found. Please run 'build' first.")
        sys.exit(1)

    run_command(["build/nota"])

def main():
    if len(sys.argv) < 2:
        print("Usage: python build.py [build|test|run]")
        sys.exit(1)

    command = sys.argv[1]

    if command == "build":
        build()
    elif command == "test":
        test()
    elif command == "run":
        run()
    else:
        print(f"Unknown command: {command}")
        sys.exit(1)

if __name__ == "__main__":
    main()
