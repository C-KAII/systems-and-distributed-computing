#ifndef STACK_H
#define STACK_H

#define MAX_STACK_SIZE 100

typedef struct {
  double data[MAX_STACK_SIZE];
  int top;
} Stack;

void initStack(Stack *s);
int isFull(Stack *s);
int isEmpty(Stack *s);
void push(Stack *s, double value);
double pop(Stack *s);

#endif  // STACK_H