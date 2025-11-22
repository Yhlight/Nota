# Nota

Nota is a general-purpose, interpreted programming language with a focus on simplicity and ease of use. It is written in C++17/C++20 and features a bytecode VM, garbage collection, and a simple syntax.

## Building

To build the Nota interpreter, you will need a C++17 (or newer) compiler and CMake. A Python script is provided to simplify the build process.

```bash
python3 build.py
```

This will create a `build` directory and compile the `nota` executable.

## Usage

The Nota CLI can be run in two modes: file mode and interactive mode (REPL).

### File Mode

To execute a `.nota` script file, pass the file path as an argument to the `nota` executable:

```bash
python3 build.py --run /path/to/your/script.nota
```

### Interactive Mode (REPL)

To start an interactive session, run the `nota` executable without any arguments:

```bash
python3 build.py --run
```

This will start a REPL where you can enter and execute Nota code line by line.
