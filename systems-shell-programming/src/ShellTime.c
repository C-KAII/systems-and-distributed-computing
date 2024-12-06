#include "ShellTime.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void printTime() {
  time_t t;
  struct tm *tmp;
  char time_str[100];

  t = time(NULL);
  tmp = localtime(&t);
  if (tmp == NULL) {
    perror(">>> localtime");
    return;
  }

  if (strftime(time_str, sizeof(time_str), "%c", tmp) == 0) {
    fprintf(stderr, ">>> strftime returned 0");
    return;
  }

  printf(">>> Current local time and date: %s\n", time_str);
}