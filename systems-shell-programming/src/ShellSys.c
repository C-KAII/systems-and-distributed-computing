#include "ShellSys.h"

#include <stdio.h>
#include <stdlib.h>

void printSys() {
  printf("---System Name and Version---\n");
  system("uname -a");

  printf("\n---CPU Information---\n");
  system("lscpu");

  printf("\n---Memory Information---\n");
  system("free -h");

  printf("\n---Disk Usage Information---\n");
  system("df -h");

  printf("\n---CPU Cores---\n");
  system("nproc --all");
}