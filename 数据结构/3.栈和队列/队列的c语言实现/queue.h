#ifndef QUEUE_H
#define QUEUE_H
#define MAXQSIZE 50
#define OK 1
#define ERROR 0
#define CONST_0 0
typedef char QElemType;
typedef enum { false = 0, true } bool;
typedef struct {
    QElemType *base;  //保存队列元素,base指向数组首地址
    int front;        //头指针
    int rear;        //尾指针
    int QueueSize;   //队列大小
}SqQueue;
bool InitQueue(SqQueue*);
bool EnQueue(SqQueue *Q, QElemType e);
bool DeQueue(SqQueue *Q,QElemType *e);
int QueueLength(const SqQueue* Q);
QElemType GetHead(const SqQueue* Q);
bool IsQueueEmpty(const SqQueue* Q);
bool IsQueueFull(const SqQueue* Q);
bool ClearQueue(SqQueue* qu);
void printQueue(const SqQueue* Q);
#endif
