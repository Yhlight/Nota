import os
import subprocess

def run_command(command):
    """Runs a command and prints its output."""
    print(f"Running command: {' '.join(command)}")
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    if stdout:
        print(stdout.decode('utf-8'))
    if stderr:
        print(stderr.decode('utf-8'))
    if process.returncode != 0:
        print(f"Error running command: {' '.join(command)}")
        exit(1)

def main():
    """Builds the project using CMake."""
    # Create a build directory if it doesn't exist
    if not os.path.exists('build'):
        os.makedirs('build')

    # Configure the project
    run_command(['cmake', '-S', '.', '-B', 'build'])

    # Build the project
    run_command(['cmake', '--build', 'build'])

if __name__ == '__main__':
    main()
