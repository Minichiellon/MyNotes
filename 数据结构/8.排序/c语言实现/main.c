#include <stdio.h>
#include "sort.h"
#include <time.h>

const p_SortFun p_fun[8] = {InsertSort,BinarySort,ShellSort,BubbleSort,SelectSort,HeapSort,MergeSort};
const char* SortMethod[] ={"InsertSort","BinarySort","ShellSort","BubbleSort",
                            "SelectSort","HeapSort","MergeSort","QuickSort"};

int main(void)
{
    clock_t t[9];
    array ar;

    for(int i = 0 ; i < 7 ; ++i)
    {
        GenerateArray(&ar);
        if(i == 6) {printArray(&ar);printf("\n");}
        t[i] = clock();
        p_fun[i](&ar);
        t[i] = clock() - t[i];
        if(i == 6) {printArray(&ar);printf("\n");}
    }
    
    GenerateArray(&ar);
    t[7] = clock();
    QuickSort(&ar, 1, ar.size);
    t[7] = clock() - t[7];

    for(int i = 0 ; i < 8 ; ++i)
    {
        printf("%s:%f seconds\n",SortMethod[i],((float)t[i])/CLOCKS_PER_SEC);
    }
    return 0;
}
