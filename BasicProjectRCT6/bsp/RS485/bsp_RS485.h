#ifndef __RS485__H
#define __RS485__H



#include <stm32f10x.h>
#include "stdio.h"
#include "bsp_usart.h"





/*****************************************************************************
 ** 全局变量
*****************************************************************************/
#define RS485_RX_BUF_SIZE       1024                    // 数据接收缓冲区大小，大部份情况下都不用修改; USART中断里的缓存大小

typedef struct
{
    uint8_t         FlagInit;                           // 状态标记; 0=未初始化或异常, 1=正常
    uint16_t        ReceivedNum;                        // 接收到多少个字节数据; 0-无数据，非0_接收到的字节数
    uint8_t         ReceivedData[RS485_RX_BUF_SIZE];  // 接收到数据的缓存;
    uint32_t        Baudrate;                           // 记录所用的串口波特率
    USART_TypeDef  *USARTx;                             // 记录所用的端口
} xRS485_TypeDef;
extern xRS485_TypeDef  xRS485;                          // 声明为全局变量,方便记录信息、状态




/*****************************************************************************
 ** 声明全局函数
 ** 说明：封装的函数比较多，但实际操作时，只调用约四个函数，即可建立通信;
 **       已对常用操作，做了简单高效的流程优化，具体操作，请参考main文件;
****************************************************************************/
// RS485部分
uint8_t   RS485_Init(USART_TypeDef *USARTx, uint32_t baudrate);  // 初始化
void      RS485_SendString(char *str);                           // 发送任意长度字符串， 注意：长度应小于256字节
void      RS485_SendData(uint8_t *_pBuf, uint16_t _usLen);       // 发送指定数据
uint16_t  RS485_CheckReceivedNum(void);                          // 检查是否收到新数据，返回接收到的字节长度
void      RS485_CleanReceivedFlag(void);                         // 清理RS485的接收缓存，包括接收长度变量和数据存放缓存

#endif


