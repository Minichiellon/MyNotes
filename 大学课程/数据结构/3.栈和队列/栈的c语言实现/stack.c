#include<stdio.h>
#include <stdlib.h>     //malloc
#include "stack.h"
Bool InitStack(Stack* ps){
    ps->data = (ElemType*)malloc(sizeof(MAX_STACK_SIZE*sizeof(ElemType)));
    if(ps->data == NULL) return FALSE;
    ps->top = STACK_EMPTY;
    return TRUE;
}
Bool Push(Stack *ps,ElemType e){
    if(ps->top == MAX_STACK_SIZE-1) return FALSE;
    ps->data[ps->top] = e; 
    ps->top++;
    return TRUE;
}
Bool Pop(Stack *ps){
    if(ps->top == STACK_EMPTY) return FALSE;
    //free(ps->data[ps->top]);
    ps->top--;
    return TRUE;
}
Bool IsEmpty(const Stack *ps){
    return ps->top == STACK_EMPTY ? TRUE:FALSE;
}
ElemType GetTop(const Stack* ps){
    if(ps->top == STACK_EMPTY)
        return '\0';
    return ps->data[ps->top];
}
int StackSize(const Stack *ps){
    return ps->top + 1;
}
void printStack(const Stack *ps){
    char prt[MAX_STACK_SIZE];
    if(ps == NULL || ps->top == STACK_EMPTY)
        printf("\033[0m\033[1;31m Stack is Empty!\033[0m \n");
    for(int i = 0;i <= ps->top;i++){
        printf("%c\n",(ps->data)[i]);
    }
}
void printTips(void){
    printf("Press below char:\n\
    I/i to Init Stack,P/p to push,O/o to pop,T/t to get top element,\n\
    S/s to get stack size,R/r to print stack,Q/q to quit\n");
}
