#ifndef QUEUE_H
#define QUEUE_H
#include "../tree/tree.h"
#define MAXQSIZE 50
#define OK 1
#define ERROR 0
#define CONST_0 0

typedef char TElemType;

typedef BiNode QElemType;

typedef struct {
    QElemType *base;  //保存队列元素,base指向数组首地址
    int front;        //头指针
    int rear;        //尾指针
    int QueueSize;   //队列大小
}SqQueue;

Bool InitQueue(SqQueue*);
Bool EnQueue(SqQueue *Q, QElemType e);
Bool DeQueue(SqQueue *Q);
int QueueLength(const SqQueue* Q);
QElemType* GetHead(const SqQueue* Q);
Bool IsQueueEmpty(const SqQueue* Q);
Bool IsQueueFull(const SqQueue* Q);
Bool ClearQueue(SqQueue* qu);
void printQueue(const SqQueue* Q);
#endif