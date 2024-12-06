#include "ShellPut.h"

#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static void copyFile(const char *src, const char *dest);

void putFiles(char *dirname, char *filenames[], int force) {
  struct stat st = {0};

  if (stat(dirname, &st) == -1) {
    if (mkdir(dirname, 0700) != 0) {
      perror(">>> Error creating directory");
      return;
    }
  } else {
    if (!force) {
      printf(">>> Directory %s already exists. Use -f to overwrite.\n",
             dirname);
      return;
    } else {
      char command[256];
      snprintf(command, sizeof(command), "rm -rf %s", dirname);
      system(command);
      if (mkdir(dirname, 0700) != 0) {
        perror(">>> Error creating directory");
        return;
      }
    }
  }

  for (int i = 0; filenames[i] != NULL; i++) {
    char dest_path[512];
    char *filename_copy = strdup(filenames[i]);
    snprintf(dest_path, sizeof(dest_path), "%s/%s", dirname,
             basename(filename_copy));
    free(filename_copy);

    if (access(filenames[i], F_OK) != 0) {
      printf(">>> File not found: %s\n", filenames[i]);
      continue;
    }

    copyFile(filenames[i], dest_path);
  }
}

static void copyFile(const char *src, const char *dest) {
  FILE *src_file = fopen(src, "r");
  if (src_file == NULL) {
    perror(">>> Error opening source file");
    return;
  }

  FILE *dest_file = fopen(dest, "w");
  if (dest_file == NULL) {
    perror(">>> Error opening destination file");
    fclose(src_file);
    return;
  }

  char buffer[BUFSIZ];
  size_t n;
  while ((n = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
    if (fwrite(buffer, 1, n, dest_file) != n) {
      perror(">>> Error writing to destination file");
    }
  }

  printf(">>> File %s copied into %s\n", src, dest);
  fclose(src_file);
  fclose(dest_file);
}