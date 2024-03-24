#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

//初始化队列
Bool InitQueue(SqQueue* Q){
    Q->base = (QElemType*)malloc(MAXQSIZE*sizeof(QElemType));//分配数组空间
    if(!Q->base) {
        printf("Sorry!There is no memory!!!");
        return ERROR;//存储分配失败
    }
    Q->front = Q->rear = 0;//头指针尾指针置为0，队列为空
    Q->QueueSize = 0;
    return OK;
}
//求队列长度
int QueueLength(const SqQueue* Q){
    return Q->QueueSize;
    //(Q->rear - Q->front + MAXQSIZE)% MAXQSIZE;
}
//入队
Bool EnQueue(SqQueue *Q, QElemType e){
    if(IsQueueFull(Q)){
        printf("the queue is already full,EnQueue failed!\n");
        return ERROR; //队满
    }
    Q->base[Q->rear] = e;    //新元素加入队尾
    Q->rear = (Q->rear+1)%MAXQSIZE;    //队尾指针+1
    Q->QueueSize++;
    return OK;
}
//出队
Bool DeQueue(SqQueue *Q){
    if(IsQueueEmpty(Q)){
        printf("the queue is already empty,DeQueue failed!\n");
        return ERROR;//队空
    }
    Q->front = (Q->front+1) % MAXQSIZE;    //队头指针+1
    Q->QueueSize--;
    return OK;
}
//获取首元素
QElemType* GetHead(const SqQueue* Q){
    if(Q->front!=Q->rear)//队列不为空
        return &Q->base[Q->front];//返回队头指针元素的值，队头指针不变
    return NULL;
}
//返回队列是否为空标志
Bool IsQueueEmpty(const SqQueue* Q){
    return Q->QueueSize == CONST_0?TRUE:FALSE;
}
//返回队列是否已满标志
Bool IsQueueFull(const SqQueue* Q){
    return Q->QueueSize == MAXQSIZE?TRUE:FALSE;
}

Bool ClearQueue(SqQueue* qu){
    qu->QueueSize = 0;
    qu->front = qu->rear = 0;
    free(qu->base);
    return OK;
}
void printQueue(const SqQueue* Q){
    int ptr = 0;
    if(Q->QueueSize == CONST_0){
        printf("the queue is empty!\n");
    }
    while(ptr != Q->QueueSize){
        printf("%c",Q->base[Q->front+ptr].data);
        ptr++;
    }
}




