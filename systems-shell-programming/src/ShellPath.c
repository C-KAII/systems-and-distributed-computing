#include "ShellPath.h"

#include <stdio.h>
#include <unistd.h>

void printPath() {
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf(">>> Current working directory: %s\n", cwd);
  } else {
    perror(">>> getcwd");
  }
}