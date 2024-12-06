# System and Distributed Computing – Systems Shell Programming

## Overview

This assignment involves developing a simple shell program in C that supports basic commands, emulating a command-line interface. The program should run in an endless loop, executing user input commands until the `quit` command is issued. The shell should be designed to work on Linux using Cygwin.

---

## Requirements

### Program Specifications

1. **Program Name**: `Assignment1P1-Yourname`
  - The program waits for a command input, executes it, prints the result to stdout, and repeats this loop until the `quit` command is entered.

2. **Platform Compatibility**: The program must run on Linux using Cygwin.

3. **Supported Commands**:
  - **`calculate expr`**: Evaluates a mathematical prefix expression and prints the result.
    - Example: `calculate + + 5 3 - 4 2` -> `Result: 6`
  - **`time`**: Prints the current local time and date.
  - **`path`**: Prints the current working directory.
  - **`sys`**: Prints the OS name, version, and CPU type.
  - **`put dirname filename(s) [-f]`**: Creates a directory and copies specified files into it.
    - If the directory exists:
      - Print an error unless `-f` is specified, in which case the directory is overwritten.
    - If a file doesn’t exist, print a "file not found" message.
  - **`get filename`**: Displays the file contents 30 lines at a time, pausing for user input between batches.
  - **`quit`**: Ends the program.

---

### Notes

- If the command is not one of the above, you should print an error message and wait for a new command.
- You can assume the expression after the calculate command is a valid prefix expression containing only ‘+’ and ‘–‘ signs. You can also assume a space character separates any two numbers/signs. e.g. “+ + 5 3 - 4 2”.
- Note that if you want to use the prefixadd() function from week 2 workshop you will need to store the expression as a ragged array of strings.
- time – you can use the functions defined in <time.h> (link). Hint: look at time(), localtime() and asctime() functions.
- path - you can use the linux system function getcwd() (link).
- put - The put command will create a new directory called dirname and copy the file (or files) listed in the command, in this directory. If the directory exists you should only print an error message, unless -f has been specified, in which case the directory will be completely overwritten (old content is deleted). If a file(s) doesn’t exist, you will need to print a ‘file not found’ message for that file.
- get - The get command will dump the file contents to the screen 30 lines at a time and pause, waiting for a key to be pressed before displaying the next 30 lines etc.