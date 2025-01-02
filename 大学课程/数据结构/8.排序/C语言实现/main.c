#include <stdio.h>
#include "sort.h"
#include <time.h>
#include <string.h>
#include<limits.h>
const p_SortFun p_fun[] = {InsertSort,BinarySort,ShellSort,BubbleSort,iterativeQuickSort,
                            SelectSort,HeapSort,MergeSort,BaseSort,countingSort};
const char* SortMethod[] ={"InsertSort","BinarySort","ShellSort","BubbleSort","iterativeQuickSort",
                            "SelectSort","HeapSort","MergeSort","BaseSort","countingSort"};
const char ShowArray[] = "QuickSort";

array ar;
int main(void)
{
    
    clock_t t[sizeof(p_fun)/sizeof(p_fun[0])];
    printf("%d",sizeof(p_fun)/sizeof(p_fun[0]));
/*
    for(int i = 0 ; i < sizeof(p_fun)/sizeof(p_fun[0]) ; ++i)
    {
        GenerateArray(&ar);
        //if(strcmp(SortMethod[i], ShowArray) == 0) {printArray(&ar);}
        t[i] = clock();
        p_fun[i](&ar);
        t[i] = clock() - t[i];
        //if(strcmp(SortMethod[i], ShowArray) == 0) {printArray(&ar);}
    }
*/

    GenerateArray(&ar);//printArray(&ar);
    //WriteFile("output1",&ar);
    t[8] = clock();
    //ShellSort(&ar);
    countingSort(&ar);
    t[8] = clock() - t[8];//printArray(&ar);
    //WriteFile("output2",&ar); 
/*
    for(int i = 0 ; i < sizeof(SortMethod)/sizeof(SortMethod[0]) ; ++i)
    {
        printf("%s:%f seconds\n",SortMethod[i],((float)t[i])/CLOCKS_PER_SEC);
    }
*/
   
    printf("%s:%f seconds\n",SortMethod[8],((float)t[8])/CLOCKS_PER_SEC);
    return 0;
}
