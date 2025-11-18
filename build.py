import os
import subprocess
import sys

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

def run():
    build()
    run_command(["build/nota"])

def test():
    build()
    run_command(["build/nota_tests"])

def main():
    if len(sys.argv) < 2:
        print("Usage: python build.py [build|run|test]")
        sys.exit(1)

    command = sys.argv[1]
    if command == "build":
        build()
    elif command == "run":
        run()
    elif command == "test":
        test()
    else:
        print(f"Unknown command: {command}")
        sys.exit(1)

if __name__ == "__main__":
    main()
