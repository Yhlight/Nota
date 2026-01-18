import sys
import os
import subprocess

def run_command(command):
    print(f"Running: {command}")
    result = subprocess.run(command, shell=True)
    if result.returncode != 0:
        print(f"Command failed with return code {result.returncode}")
        sys.exit(result.returncode)

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 build.py [build|test]")
        sys.exit(1)

    action = sys.argv[1]

    if action == "build":
        if not os.path.exists("build"):
            os.makedirs("build")
        os.chdir("build")
        run_command("cmake ..")
        run_command("cmake --build .")
    elif action == "test":
        if not os.path.exists("build"):
            print("Build directory does not exist. Run build first.")
            sys.exit(1)
        os.chdir("build")
        run_command("ctest --output-on-failure")
    else:
        print(f"Unknown action: {action}")
        sys.exit(1)

if __name__ == "__main__":
    main()
