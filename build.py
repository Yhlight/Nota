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
    os.chdir("build")
    run_command(["cmake", ".."])
    run_command(["cmake", "--build", "."])
    os.chdir("..")

def run():
    executable_path = os.path.join("build", "bin", "nota")
    if not os.path.exists(executable_path):
        print("Executable not found. Please build the project first.")
        sys.exit(1)
    run_command([executable_path])

def test():
    executable_path = os.path.join("build", "bin", "nota_tests")
    if not os.path.exists(executable_path):
        print("Test executable not found. Please build the project first.")
        sys.exit(1)
    run_command([executable_path])

def clean():
    if os.path.exists("build"):
        import shutil
        shutil.rmtree("build")
    print("Build directory cleaned.")

if __name__ == "__main__":
    if len(sys.argv) > 1:
        command = sys.argv[1]
        if command == "build":
            build()
        elif command == "run":
            run()
        elif command == "test":
            test()
        elif command == "clean":
            clean()
        else:
            print(f"Unknown command: {command}")
    else:
        print("Usage: python3 build.py [build|run|test|clean]")
