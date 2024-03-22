#include "queue.h"
#include <stdio.h>
int main(void){
    char mem[10];
    
    SqQueue que;
    SqQueue* qu = &que;
    InitQueue(qu);

    EnQueue(qu, 'a');
    printQueue(qu);
    printf("\n");

    EnQueue(qu, 'b');
    printQueue(qu);
    printf("\n");

    EnQueue(qu, 'c');
    printQueue(qu);
    printf("\n");

    EnQueue(qu, 'd');
    printQueue(qu);
    printf("\n");

    ClearQueue(qu);
    printQueue(qu);
    printf("\n");

    DeQueue(qu);
    printQueue(qu);
    printf("\n");

    DeQueue(qu);
    printQueue(qu);
    printf("\n");

    EnQueue(qu, 'f');
    printQueue(qu);
    printf("\n");
    
    return 0;
}
