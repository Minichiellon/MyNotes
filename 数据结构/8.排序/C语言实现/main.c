#include <stdio.h>
#include "sort.h"
#include <time.h>
#include <string.h>
const p_SortFun p_fun[] = {InsertSort,BinarySort,ShellSort,BubbleSort,
                            SelectSort,HeapSort,MergeSort,BaseSort};
const char* SortMethod[] ={"InsertSort","BinarySort","ShellSort","BubbleSort",
                            "SelectSort","HeapSort","MergeSort","BaseSort","QuickSort"};
const char ShowArray[] = "QuickSort";

int main(void)
{
    
    clock_t t[9];
    array ar;
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
    t[8] = clock();
    //BaseSort(&ar);
    iterativeQuickSort(&ar, 1, ar.size);
    t[8] = clock() - t[8];//printArray(&ar);
    /*
    for(int i = 0 ; i < sizeof(SortMethod)/sizeof(SortMethod[0]) ; ++i)
    {
        printf("%s:%f seconds\n",SortMethod[i],((float)t[i])/CLOCKS_PER_SEC);
    }
    */
   /*
   // 打开文件用于写入 ("w" 模式表示写入，文件不存在时创建，存在时覆盖)
    FILE *fp = fopen("output.txt", "a");
    
    // 检查文件是否成功打开
    if (fp == NULL) {
        printf("无法打开文件!\n");
        return 1;
    }
    
    fprintf(fp,"%d:%f seconds\n",MAXSIZE,((float)t[8])/CLOCKS_PER_SEC);
    // 关闭文件
    fclose(fp);
    */
    printf("%s:%f seconds\n",SortMethod[8],((float)t[8])/CLOCKS_PER_SEC);
    return 0;
}
