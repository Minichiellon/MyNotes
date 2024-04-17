#include <stdio.h>
#include "sort.h"
#include <time.h>

int main(void)
{
    char* SortMethod[] ={"InsertSort","BinarySort","ShellSort","BubbleSort","SelectSort","HeapSort","QuickSort"};

    typedef void (*p_SortFun)();
    p_SortFun p_fun[8] = {InsertSort,BinarySort,ShellSort,BubbleSort,SelectSort,HeapSort};

    clock_t t[9];
    array ar;

    printArray(&ar);
    printf("\n\n\n");

    for(int i = 0 ; i < 6 ; ++i)
    {
        GenerateArray(&ar);
        t[i] = clock();
        p_fun[i](&ar);
        t[i] = clock() - t[i];
    }
    
    GenerateArray(&ar);
    t[6] = clock();
    QuickSort(&ar, 1, ar.size);
    t[6] = clock() - t[6];
    printArray(&ar);

    for(int i = 0 ; i < 7 ; ++i)
    {
        printf("%s:%f seconds\n",SortMethod[i],((float)t[i])/CLOCKS_PER_SEC);
    }
    return 0;
}