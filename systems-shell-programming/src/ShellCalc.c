#include "ShellCalc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Stack.h"

double evaluatePrefix(char *expression);

void calculateExpression(int argc, char *argv[]) {
  if (argc < 4) {
    printf(">>> Usage: > calc <expression>\n");
    return;
  }

  // Concatenate all arguments into a single string
  char expression[256] = {0};
  for (int i = 1; i < argc; i++) {
    strcat(expression, argv[i]);
    if (i < argc - 1) {
      strcat(expression, " ");
    }
  }

  double result = evaluatePrefix(expression);
  printf(">>> Result: %f\n", result);
}

double evaluatePrefix(char *expression) {
  Stack stack;
  initStack(&stack);

  char *token = strtok(expression, " ");
  char *tokens[MAX_STACK_SIZE];
  int count = 0;

  while (token != NULL) {
    tokens[count++] = token;
    token = strtok(NULL, " ");
  }

  for (int i = count - 1; i >= 0; i--) {
    if (tokens[i][0] == '+' || tokens[i][0] == '-') {
      double operand1 = pop(&stack);
      double operand2 = pop(&stack);
      if (tokens[i][0] == '+') {
        push(&stack, operand1 + operand2);
      } else if (tokens[i][0] == '-') {
        push(&stack, operand1 - operand2);
      }
    } else {
      push(&stack, atof(tokens[i]));
    }
  }

  return pop(&stack);
}