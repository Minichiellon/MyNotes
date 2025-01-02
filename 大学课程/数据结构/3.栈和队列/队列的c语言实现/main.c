#include "queue.h"
#include <stdio.h>
#include <ctype.h>  //toupper
int main(void){
    SqQueue que;
    SqQueue* qu = &que;
    InitQueue(qu);

    printf("Press below char:\n\
    E/e to Enqueue,D/d to Dequeue,C/c to Clear Queue,L/l to get Queue Length,\n\
    F/f to get Front element,P/p to print Queue,Q/q to quit\n");
    char ch,EleToEnqueue,FrontEle;

    while((ch = getchar()) != 'q'){
        while((getchar()) != '\n') continue;
        if('\n' == ch) continue;
        switch(toupper(ch)){
            case 'E':
                printf("Please enter a character:\n");
                scanf("%c",&EleToEnqueue);
                while((getchar()) != '\n') continue;
                if(EnQueue(qu, EleToEnqueue)){
                    printf("\033[0m\033[1;32m EnQueue element: %c\033[0m\n",EleToEnqueue);
                }
                else{
                    printf("\033[0m\033[1;31m EnQueue Failed! \033[0m\n");
                }
                break;
            case 'D':
                FrontEle = GetHead(qu);
                if(DeQueue(qu)){
                    printf("\033[0m\033[1;32m \"%c\" has been DeQueued \033[0m\n",FrontEle);
                }else{
                    printf("\033[0m\033[1;31m Dequeue Failed! \033[0m\n");
                }
                break;
            case 'C':
                if(ClearQueue(qu)){
                    printf("\033[0m\033[1;32m Queue has been Cleared \033[0m\n");
                }else{
                    printf("\033[0m\033[1;31m Clear Queue Failed! \033[0m\n");
                }
                break;
            case 'L':
                printf("\033[0m\033[1;32m The Length of Queue is %d \033[0m\n",QueueLength(qu));
                break;
            case 'F':
                if(GetHead(qu) == '\0'){
                    printf("\033[0m\033[1;31m The queue is empty \033[0m\n");
                }else{
                    printf("\033[0m\033[1;32m The Front Element is %c \033[0m\n",GetHead(qu));
                }
                break;
            case 'P':
                printQueue(qu);
                printf("\n");
                break;
            case 'Q':
                break;
            default:
                continue;
        }
        printf("Press below char:\n\
    E to Enqueue,D to Dequeue,C to Clear Queue,L to get Queue Length,\n\
    F to get Front element,p to print Queue,q to quit\n");  
    }
    return 0;
}
