#include "sort.h"
#include<time.h>
#include<stdlib.h>
#include<stdio.h>
#include<limits.h>

void InsertSort(array *ar)
{
    int i,j;
    for(i = 2 ; i <= ar->size ; ++i)
    {
        if(ar->data[i] < ar->data[i-1])
        {
            ar->data[0] = ar->data[i];

            for(j = i - 1 ; ar->data[j] > ar->data[0] ; --j)
                ar->data[j + 1] = ar->data[j];

            ar->data[j + 1] = ar->data[0];
        }
    }
}

void BinarySort(array *ar)
{
    int i, j, low, high, mid;
    for(i = 2 ; i <= ar->size ; ++i)
    {
        ar->data[0] = ar->data[i];
        low = 1;
        high = i - 1;

        while ( low <= high )
        {
            mid = (low + high)/2;
            if(ar->data[0] < ar->data[mid]) 
                high = mid - 1;
            else
                low = mid + 1;
        }

        for(j = i - 1 ; j >= high + 1; --j)
            ar->data[j + 1] = ar->data[j];

        ar->data[high + 1] = ar->data[0];
    }
}

void ShellSort(array *ar)
{
    int t = 5;
    int dlta[5] = {11,7,5,3,1};
    for(int k = 0 ; k < t ; ++k)
        ShellInsert(ar, dlta[k]);
}

void ShellInsert(array *ar, int dk)
{
    int i, j;
    for(i = dk + 1 ; i <= ar->size ; ++i)
    {
        if(ar->data[i] < ar->data[i-dk])
        {
            ar->data[0] = ar->data[i];

            for(j = i - dk ; j > 0 && ar->data[j] > ar->data[0] ; j = j - dk)
                ar->data[j + dk] = ar->data[j];

            ar->data[j + dk] = ar->data[0];
        }
    }
}
void BubbleSort(array *ar)
{
    int i, j, flag = 1;
    DataType tmp;
     
    for(i = 1 ; i <= ar->size && flag == 1; ++i)
    {
        flag = 0;
        for(j = 1 ; j <= ar->size - i ; ++j)
        {
            if(ar->data[j] > ar->data[j + 1])
            {
                flag = 1;
                tmp = ar->data[j];
                ar->data[j] = ar->data[j + 1];
                ar->data[j + 1] = tmp;
            }
        }
    }
}

void QuickSort(array *ar, int low, int high)
{
    int pivotloc;
    if(low < high)
    {
        pivotloc = Partition(ar, low, high);
        QuickSort(ar, low, pivotloc - 1);
        QuickSort(ar, pivotloc + 1, high);
    }
}

int Partition(array *ar, int low, int high)
{
    DataType pivotData;
    ar->data[0] = ar->data[low];
    pivotData = ar->data[low];

    while(low < high)
    {
        while(low < high && ar->data[high] >= pivotData)
            --high;
        ar->data[low] = ar->data[high];

        while(low < high && ar->data[low] <= pivotData)     //当两个while条件均为<，low和high值相同，则会卡住
            ++low;
        ar->data[high] = ar->data[low];
    }

    ar->data[low] = ar->data[0];
    return low;
}

void SelectSort(array *ar)
{
    int i, j, min;
    DataType tmp;
    for(i = 1 ; i < ar->size ; ++i)
    {
        min = i;
        for(j = i + 1 ; j <= ar->size ; ++j)
        {
            if(ar->data[j] < ar->data[min])
                min = j;
        }

        if(min != i)
        {
            tmp = ar->data[i];
            ar->data[i] = ar->data[min];
            ar->data[min] = tmp;
        }
    }
}

void HeapSort(array *ar)
{
    //根据数组建立大根堆
    for(int i = (ar->size) / 2 ; i >= 1 ; --i)
    {
        HeapAdjust(ar, i, ar->size);
    }
    //取根元素并调整堆
    for(int i = ar->size ; i > 1 ; --i)
    {
        swap(&ar->data[1], &ar->data[i]);
        HeapAdjust(ar, 1, i - 1);
    }
}

void HeapAdjust(array *ar, int NodeIndex, int size)
{
    for(int i = 2 * NodeIndex ; i <= size ; i *= 2)     //从要调整的结点的左孩子开始遍历
    {
        if(i < size && ar->data[i] < ar->data[i+1]) ++i;    //i为左右孩子较大者，注意条件要为i < size,
                                                            //否则当某个结点只有左孩子时,条件i <= size会访问越界

        if(ar->data[NodeIndex] > ar->data[i]) break;        //如果该结点比左右孩子较大者更大，则不需交换，直接跳出该节点的调整
        else
            swap(&ar->data[NodeIndex], &ar->data[i]);       //否则交换这两个结点

        if(2*i > size) break;       //如果该结点调整之后达到叶子结点，则跳出
        else NodeIndex = i;         //否则NodeIndex指向当前节点
    }
}

void MergeSort(array *ar)
{

}

void BaseSort(array *ar)
{

}

void GenerateArray(array *ar)
{
    int i;
    ar->data[0] = 0;
    srand((unsigned)time(0));
    for(i = 1 ; i < MAXSIZE + 1; ++i)
    {
        //ar->data[i] = (INT_MAX/RAND_MAX) * rand();    //线性映射至[0,MAX_INT]
        ar->data[i] = rand();
    }
    ar->size = i - 1;
}

void printArray(array *ar)
{
    for(int i = 1 ; i < ar->size + 1 ; ++i)
    {
        printf("%d\t",ar->data[i]);
        if(i % 10 == 0) printf("\n");
    }
}
void swap(DataType* a, DataType* b)
{
    DataType tmp = *a;
    *a = *b;
    *b = tmp;
}