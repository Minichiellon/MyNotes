#include "sort.h"
#include<time.h>      //time
#include<stdlib.h>    //malloc,free,srand,rand,RAND_MAX
#include<stdio.h>
#include<limits.h>    //INT_MAX

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

int min(int x, int y) {
    return x < y ? x : y;
}
void MergeSort(array* ar)
{
    DataType* p_ar = &ar->data[1];
    DataType* tmp = (DataType*)malloc(ar->size * sizeof(DataType));     //申请和ar的数组一样大的临时空间

    for (int seg = 1; seg < ar->size; seg *= 2)
    {
        for (int start = 0; start < ar->size; start += seg + seg)
        {
            int low = start, mid = min(start + seg, ar->size), high = min(start + seg + seg, ar->size);
            int k = low;
            int start1 = low, end1 = mid;       //分段一的开始结束位置
            int start2 = mid, end2 = high;      //分段二的开始结束位置

            while (start1 < end1 && start2 < end2)  //将分段一和分段二当前start指针指向的较小值放入tmp
                tmp[k++] = p_ar[start1] < p_ar[start2] ? p_ar[start1++] : p_ar[start2++];

            while (start1 < end1)   //将分段一剩余元素放入tmp
                tmp[k++] = p_ar[start1++];

            while (start2 < end2)   //将分段二剩余元素放入tmp
                tmp[k++] = p_ar[start2++];
        }
        DataType* temp = p_ar;
        p_ar = tmp;
        tmp = temp;
    }

    //如果p_ar不指向原始数组(即p_ar指向排好序的临时空间,tmp指向原始数组)
    if (p_ar != &ar->data[1]) 
    {
        int i;
        for (i = 0; i < ar->size; i++)
            tmp[i] = p_ar[i];        //将排好序的数组复制至原始数组
        tmp = p_ar;                  //令tmp指向临时空间
    }
    free(tmp);      //释放临时空间
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
