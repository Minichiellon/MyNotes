#ifndef __SORT_H
#define __SORT_H

#define MAXSIZE 100
typedef int DataType;

typedef struct{
    DataType data[MAXSIZE + 1];
    int size;
}array;

void InsertSort(array *ar);     //直接插入排序
void BinarySort(array *ar);     //折半插入排序
void ShellSort(array *ar);      //希尔插入排序
void ShellInsert(array *ar, int dk);

void BubbleSort(array *ar);     //冒泡排序
void QuickSort(array *ar, int low, int high);     //快速排序
int Partition(array *ar, int low, int high);

void SelectSort(array *ar);     //选择排序
void HeapSort(array *ar);       //堆排序

void MergeSort(array *ar);      //归并排序

void BaseSort(array *ar);       //基数排序


void printArray(array *ar);
void GenerateArray(array *ar);
#endif