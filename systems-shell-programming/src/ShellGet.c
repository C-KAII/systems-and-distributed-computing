#include "ShellGet.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define LINES_PER_PAGE 40

static void disableEcho(struct termios *oldt);
static void restoreTerminal(struct termios *oldt);

void getFile(char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror(">>> Error opening file");
    return;
  }

  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  int line_count = 0;

  struct termios oldt;
  disableEcho(&oldt);

  while ((read = getline(&line, &len, file)) != -1) {
    printf("%s", line);
    ++line_count;

    if (line_count % LINES_PER_PAGE == 0) {
      printf("\n---Press any key to continue, 'q' to quit---\n");
      int ch = getchar();
      if (ch == 'q' || ch == 'Q') {
        break;
      }
    }
  }
  printf("\n");

  free(line);
  fclose(file);
  restoreTerminal(&oldt);
}

static void disableEcho(struct termios *oldt) {
  struct termios newt;
  tcgetattr(STDIN_FILENO, oldt);
  newt = *oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

static void restoreTerminal(struct termios *oldt) {
  tcsetattr(STDIN_FILENO, TCSANOW, oldt);
}