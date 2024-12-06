#include "ShellHelp.h"

#include <stdio.h>
#include <string.h>

void help() {
  printf(">>> ---Command---\t\t\t---Info---\n");
  printf(
      ">>> calc (-c) expr.\t\t\t- prints out the result of the mathematical "
      "prefix expression that comes after the command.\n"
      ">>> time (-t)\t\t\t\t- prints out the current local time and date.\n"
      ">>> path (-pa)\t\t\t\t- prints out the current working directory.\n"
      ">>> sys (-s)\t\t\t\t- prints the name and version of the OS and CPU "
      "type.\n"
      ">>> put (-p) dirname filename(s) [-f]\t- put files 'filenames' in the "
      "directory 'dirname'\n"
      ">>> get (-g) filename\t\t\t- prints the content of the file 'filename' "
      "to the screen.\n"
      ">>> help (-h) [command]\t\t\t- prints all commands -- optional "
      "[command] provides further information for the given command arg.\n"
      ">>> quit (-q)\t\t\t\t- ends the program.\n\n");
}

void moreHelp(char *cmd) {
  if (strcmp(cmd, "calc") == 0 || strcmp(cmd, "-c") == 0) {
    printf(
        ">>> calc (-c) expr.\n"
        ">>> Prints out the result of the mathematical prefix expression that "
        "comes after the command.\n"
        ">>> Supports '+', '-', 'x', and '/'. Separate signs and numbers with "
        "a space character.\n"
        ">>> Example usage: calc + 2 5\n\n");
  } else if (strcmp(cmd, "time") == 0 || strcmp(cmd, "-t") == 0) {
    printf(
        ">>> time (-t)\n"
        ">>> Prints out the current local time and date.\n\n");
  } else if (strcmp(cmd, "path") == 0 || strcmp(cmd, "-pa") == 0) {
    printf(
        ">>> path (-pa)\n"
        ">>> Prints out the current working directory.\n\n");
  } else if (strcmp(cmd, "sys") == 0 || strcmp(cmd, "-s") == 0) {
    printf(
        ">>> sys (-s)\n"
        ">>> Prints the name and version of the OS and CPU type.\n\n");
  } else if (strcmp(cmd, "put") == 0 || strcmp(cmd, "-p") == 0) {
    printf(
        ">>> put (-p) dirname filename(s) [-f]\n"
        ">>> Put files 'filenames' in the directory 'dirname'\n"
        ">>> The put command will create a new directory called dirname and "
        "copy the file (or files) listed in the command, in this directory.\n"
        ">>> If the directory exists, nothing will happen, unless -f has been "
        "specified,\n"
        ">>> in which case the directory will be completely overwritten (old "
        "content is deleted).\n"
        ">>> Example usage: put exampleDir examplefile1.txt "
        "examplefile2.txt\n\n");
  } else if (strcmp(cmd, "get") == 0 || strcmp(cmd, "-g") == 0) {
    printf(
        ">>> get (-g) filename\n"
        ">>> Prints the content of the file 'filename' to the screen.\n"
        ">>> The get command will dump the file contents to the screen 40 "
        "lines at a time and pause,\n"
        ">>> waiting for a key to be pressed before displaying the next 40 "
        "lines etc.\n"
        ">>> If 'q' key is pressed, file dump will be cancelled.\n\n");
  } else if (strcmp(cmd, "help") == 0 || strcmp(cmd, "-h") == 0) {
    printf(
        ">>> help (-h) [command]\n"
        ">>> Prints all commands -- optional [command] provides further "
        "information for the given command arg.\n"
        ">>> Include additional commands separated by a space character to get "
        "help on each.\n\n");
  } else if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "-q") == 0) {
    printf(
        ">>> quit (-q)\n"
        ">>> Ends the program.\n\n");
  }
}