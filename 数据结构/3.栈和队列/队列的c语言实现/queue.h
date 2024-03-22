#ifndef QUEUE_H
#define QUEUE_H
#define MAXQSIZE 10
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
bool InitQueue(SqQueue*);               //初始化队列
bool EnQueue(SqQueue *Q, QElemType e);  //入队
bool DeQueue(SqQueue *Q);               //出队
int QueueLength(const SqQueue* Q);      //求队列长度
QElemType GetHead(const SqQueue* Q);    //获取队头元素
bool IsQueueEmpty(const SqQueue* Q);    //队列是否为空
bool IsQueueFull(const SqQueue* Q);     //队列是否已满
bool ClearQueue(SqQueue* qu);           //清空队列
void printQueue(const SqQueue* Q);      //打印队列
#endif
