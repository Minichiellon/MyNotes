#ifndef QUEUE_H
#define QUEUE_H
#define MAXQSIZE 10
#define OK 1
#define ERROR 0
#define CONST_0 0
typedef char QElemType;
typedef enum { FALSE , TRUE } Bool;
typedef struct {
    QElemType *base;  //保存队列元素,base指向数组首地址
    int front;        //头指针
    int rear;        //尾指针
    int QueueSize;   //队列大小
}SqQueue;
Bool InitQueue(SqQueue*);               //初始化队列
Bool EnQueue(SqQueue *Q, QElemType e);  //入队
Bool DeQueue(SqQueue *Q);               //出队
int QueueLength(const SqQueue* Q);      //求队列长度
QElemType GetHead(const SqQueue* Q);    //获取队头元素
Bool IsQueueEmpty(const SqQueue* Q);    //队列是否为空
Bool IsQueueFull(const SqQueue* Q);     //队列是否已满
Bool ClearQueue(SqQueue* qu);           //清空队列
void printQueue(const SqQueue* Q);      //打印队列
#endif