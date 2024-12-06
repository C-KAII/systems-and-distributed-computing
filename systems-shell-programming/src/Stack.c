#include "Stack.h"

#include <stdio.h>
#include <stdlib.h>

void initStack(Stack *s) { s->top = -1; }

int isFull(Stack *s) { return s->top == MAX_STACK_SIZE - 1; }

int isEmpty(Stack *s) { return s->top == -1; }

void push(Stack *s, double value) {
  if (isFull(s)) {
    printf(">>> Stack overflow\n");
    exit(1);
  }
  s->data[++s->top] = value;
}

double pop(Stack *s) {
  if (isEmpty(s)) {
    printf(">>> Stack underflow\n");
    exit(1);
  }
  return s->data[s->top--];
}
