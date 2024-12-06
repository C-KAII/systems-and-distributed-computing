#include "Run.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "ShellCalc.h"
#include "ShellGet.h"
#include "ShellHelp.h"
#include "ShellPath.h"
#include "ShellPut.h"
#include "ShellSys.h"
#include "ShellTime.h"

#define MAX_ARGS 10

static int tokeniseInput(char *line, char *argv[]);
static void executeCommand(char *cmd, int argc, char *argv[]);

void run() {
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  int argc;
  char *argv[MAX_ARGS];

  printf(
      "---Welcome to the shell---\n"
      ">>> help (-h) to list commands.\n\n");

  while (1) {
    printf("> ");
    read = getline(&line, &len, stdin);
    if (read == -1) {
      perror(">>> getline");
      exit(EXIT_FAILURE);
    }

    line[strcspn(line, "\n")] = 0;  // remove newline
    argc = tokeniseInput(line, argv);
    if (argc > 0) {
      executeCommand(argv[0], argc, argv);
    }
  }
  free(line);
}

static int tokeniseInput(char *line, char *argv[]) {
  int argc = 0;
  char *token = strtok(line, " ");
  while (token != NULL && argc < MAX_ARGS - 1) {
    argv[argc++] = token;
    token = strtok(NULL, " ");
  }
  argv[argc] = NULL;
  return argc;
}

static void executeCommand(char *cmd, int argc, char *argv[]) {
  if (strcmp(cmd, "calc") == 0 || strcmp(cmd, "-c") == 0) {
    calculateExpression(argc, argv);
  } else if (strcmp(cmd, "time") == 0 || strcmp(cmd, "-t") == 0) {
    printTime();
  } else if (strcmp(cmd, "path") == 0 || strcmp(cmd, "-pa") == 0) {
    printPath();
  } else if (strcmp(cmd, "sys") == 0 || strcmp(cmd, "-s") == 0) {
    printSys();
  } else if (strcmp(cmd, "put") == 0 || strcmp(cmd, "-p") == 0) {
    int force = 0;
    if (argc > 2 && strcmp(argv[argc - 1], "-f") == 0) {
      force = 1;
      argv[argc - 1] = NULL;
    }
    putFiles(argv[1], &argv[2], force);
  } else if (strcmp(cmd, "get") == 0 || strcmp(cmd, "-g") == 0) {
    if (argc > 1) {
      getFile(argv[1]);
    } else {
      printf(">>> Usage: get filename\n");
    }
  } else if (strcmp(cmd, "help") == 0 || strcmp(cmd, "-h") == 0) {
    if (argc == 1) {
      help();
    } else {
      for (int i = 1; i < argc; i++) {
        moreHelp(argv[i]);
      }
    }
  } else if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "-q") == 0) {
    printf(">>> Terminating shell...\n");
    // Could change this to return 1 to manually unwind stack
    exit(0);
  } else {
    printf(">>> Unknown command: '%s', use 'help' to list commands.\n", cmd);
  }
}