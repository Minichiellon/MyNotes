#ifndef __SORT_H
#define __SORT_H

#define ARRAY_MAXSIZE 100000000
#define NUM_THREADS 5
typedef int DataType;
typedef void (*p_SortFun)();
typedef struct{
    DataType data[ARRAY_MAXSIZE + 1];
    int size;
}array;

#include<pthread.h>

void InsertSort(array *ar);     //直接插入排序
void BinarySort(array *ar);     //折半插入排序
void ShellSort(array *ar);      //希尔插入排序
void ShellInsert(array *ar, int dk);

void BubbleSort(array *ar);     //冒泡排序
void QuickSort(array *ar, int low, int high);     //快速排序
void iterativeQuickSort(array *ar);   //非递归的快速排序
int Partition(array *ar, int low, int high);

void SelectSort(array *ar);     //选择排序
void HeapSort(array *ar);       //堆排序
void HeapAdjust(array *ar, int NodeIndex, int size);

void MergeSort(array* ar);      //归并排序
int min(int x, int y);
void BaseSort(array *ar);       //基数排序
void countingSort(array* ar);

void printArray(array *ar);
void GenerateArray(array *ar);
void swap(DataType* a, DataType* b);
void WriteFile(char* FileName, array* ar);
#endif
