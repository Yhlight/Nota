import os
import subprocess

def build():
    """
    Builds the Nota project using CMake.
    """
    if not os.path.exists("build"):
        os.makedirs("build")

    cmake_command = ["cmake", "-S", ".", "-B", "build"]
    build_command = ["cmake", "--build", "build"]

    try:
        subprocess.run(cmake_command, check=True)
        subprocess.run(build_command, check=True)
        print("Build successful!")
    except subprocess.CalledProcessError as e:
        print(f"Build failed: {e}")
        return

    run()

def run():
    """
    Runs the Nota executable.
    """
    executable_path = os.path.join("build", "nota")
    if os.path.exists(executable_path):
        try:
            subprocess.run([executable_path], check=True)
        except subprocess.CalledProcessError as e:
            print(f"Execution failed: {e}")
    else:
        print("Executable not found. Please build the project first.")

if __name__ == "__main__":
    build()
