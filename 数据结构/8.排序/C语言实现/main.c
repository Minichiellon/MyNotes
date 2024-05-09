#include <stdio.h>
#include "sort.h"
#include <time.h>

const p_SortFun p_fun[] = {InsertSort,BinarySort,ShellSort,BubbleSort,
                            SelectSort,HeapSort,MergeSort,BaseSort};
const char* SortMethod[] ={"InsertSort","BinarySort","ShellSort","BubbleSort",
                            "SelectSort","HeapSort","MergeSort","BaseSort","QuickSort"};

int main(void)
{
    clock_t t[9];
    array ar;

    for(int i = 0 ; i < 8 ; ++i)
    {
        GenerateArray(&ar);
        if(i == 7) {printArray(&ar);}
        t[i] = clock();
        p_fun[i](&ar);
        t[i] = clock() - t[i];
        if(i == 7) {printArray(&ar);}
    }
    
    GenerateArray(&ar);
    t[8] = clock();
    QuickSort(&ar, 1, ar.size);
    t[8] = clock() - t[8];

    for(int i = 0 ; i < 9 ; ++i)
    {
        printf("%s:%f seconds\n",SortMethod[i],((float)t[i])/CLOCKS_PER_SEC);
    }
    return 0;
}
