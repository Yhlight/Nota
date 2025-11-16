import os
import subprocess
import argparse
import shutil

def configure():
    print("Configuring CMake...")
    os.makedirs("build", exist_ok=True)
    subprocess.run(["cmake", "-B", "build", "-S", "."], check=True)

def build():
    print("Building project...")
    subprocess.run(["cmake", "--build", "build"], check=True)

def test():
    print("Running tests...")
    subprocess.run(["./build/tests/run_tests"], check=True)

def run():
    print("Running Nota CLI...")
    subprocess.run(["./build/src/nota_cli"], check=True)

def clean():
    print("Cleaning build directory...")
    if os.path.exists("build"):
        shutil.rmtree("build")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Nota Build Script")
    parser.add_argument("action", choices=["configure", "build", "test", "run", "clean"], help="Action to perform")
    args = parser.parse_args()

    if args.action == "configure":
        configure()
    elif args.action == "build":
        # Ensure configuration is done before building
        if not os.path.exists("build/Makefile"):
            configure()
        build()
    elif args.action == "test":
        if not os.path.exists("build/tests/run_tests"):
            configure()
            build()
        test()
    elif args.action == "run":
        if not os.path.exists("build/src/nota_cli"):
            configure()
            build()
        run()
    elif args.action == "clean":
        clean()
