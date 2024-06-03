#include <stdio.h>
#include "sort.h"
#include <time.h>
#include <string.h>
const p_SortFun p_fun[] = {InsertSort,BinarySort,ShellSort,BubbleSort,
                            SelectSort,HeapSort,BaseSort,BaseSort};
const char* SortMethod[] ={"InsertSort","BinarySort","ShellSort","BubbleSort",
                            "SelectSort","HeapSort","MergeSort","BaseSort","QuickSort"};
const char ShowArray[] = "BaseSort";

int main(void)
{
    clock_t t[9];
    array ar;
    for(int i = 0 ; i < sizeof(p_fun)/sizeof(p_fun[0]) ; ++i)
    {
        GenerateArray(&ar);
        if(strcmp(SortMethod[i], ShowArray) == 0) {printArray(&ar);}
        t[i] = clock();
        p_fun[i](&ar);
        t[i] = clock() - t[i];
        if(strcmp(SortMethod[i], ShowArray) == 0) {printArray(&ar);}
    }
    
    GenerateArray(&ar);
    t[8] = clock();
    QuickSort(&ar, 1, ar.size);
    t[8] = clock() - t[8];

    for(int i = 0 ; i < sizeof(SortMethod)/sizeof(SortMethod[0]) ; ++i)
    {
        printf("%s:%f seconds\n",SortMethod[i],((float)t[i])/CLOCKS_PER_SEC);
    }
    return 0;
}
