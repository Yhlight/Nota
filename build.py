import os
import subprocess

def run_command(command, working_dir):
    """Runs a command in a specified working directory."""
    print(f"Running command: {' '.join(command)} in {working_dir}")
    process = subprocess.Popen(command, cwd=working_dir, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    if process.returncode != 0:
        print(f"Error running command: {' '.join(command)}")
        print(stderr.decode())
        exit(1)
    print(stdout.decode())

def main():
    """Main function to build the project."""
    build_dir = "build"
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    run_command(["cmake", ".."], build_dir)
    run_command(["make"], build_dir)

if __name__ == "__main__":
    main()
