#include <stdio.h>
#include "sort.h"
#include <time.h>

int main(void)
{
    clock_t t[9];
    array ar;
    GenerateArray(&ar);

    printArray(&ar);
    printf("\n\n\n");

    t[0] = clock();
    //InsertSort(&ar);
    //BinarySort(&ar);
    //ShellSort(&ar);
    //BubbleSort(&ar);
    //QuickSort(&ar, 1, MAXSIZE);
    SelectSort(&ar);
    t[0] = clock() - t[0];
    printArray(&ar);

    printf("%f seconds",((float)t[0])/CLOCKS_PER_SEC);
    return 0;
}