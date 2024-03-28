#include<stdio.h>
#include<ctype.h>   //toupper
#include "stack.h"
int main(void){
    Stack st;
    Stack* ps = &st;
    printTips();
    char ch,EleToPush,TopEle;
    while((ch = getchar()) != 'q' ){
        scanf("%*[^\n]"); scanf("%*c");
        if('\n' == ch){
            printTips();
            continue;
        }
        switch(toupper(ch)){
            case 'I':
                if(!InitStack(ps)) printf("\033[0m\033[1;31m Initialized Stack Failed!\033[0m \n");
                else printf("\033[0m\033[1;32m A Stack has been Initialized!\033[0m \n");
                break;
            case 'P':
                printf("Please Enter the character to push:\n");
                EleToPush = getchar();
                scanf("%*[^\n]"); scanf("%*c");
                if(!Push(ps,EleToPush)) printf("\033[0m\033[1;31m Push Element Failed!\033[0m \n");
                else printf("\033[0m\033[1;32m Push Element Succeed!\033[0m \n");
                break;
            case 'O':
                if(!Pop(ps)) printf("\033[0m\033[1;31m Pop Element Failed!\033[0m \n");
                else printf("\033[0m\033[1;32m Pop Element Succeed!\033[0m \n");
                break;
            case 'T':
                TopEle = GetTop(ps);
                if(TopEle == '\0') printf("\033[0m\033[1;31m Stack already Empty!\033[0m \n");
                else printf("\033[0m\033[1;32m Top Element is %c!\033[0m \n",TopEle);
                break;
            case 'S':
                printf("\033[0m\033[1;32m Stack size is %d!\033[0m \n",StackSize(ps));
                break;
            case 'R':
                printStack(ps);
                printf("\n");
                break;
            case 'Q':
                break;
            default:
                //printTips();
                break;
        }
        printTips();
    }

    return 0;
}
