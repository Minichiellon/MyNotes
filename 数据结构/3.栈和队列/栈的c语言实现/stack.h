#ifndef STACK_H
#define STACK_H

#define MAX_STACK_SIZE 50
#define STACK_EMPTY -1
typedef enum{FALSE = 0,TRUE} Bool;
typedef char ElemType;
typedef struct Stack{
    ElemType* data;
    int top;
}Stack;
Bool InitStack(Stack*);
Bool Push(Stack *ps,ElemType e);
Bool Pop(Stack *ps);
Bool IsEmpty(const Stack *ps);
ElemType GetTop(const Stack* ps);
int StackSize(const Stack *ps);
void printStack(const Stack *ps);
void printTips(void);
#endif
