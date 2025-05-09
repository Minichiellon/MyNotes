#include "bsp_RS485.h"

xRS485_TypeDef  xRS485;



#if 1
static void delay_ms(uint32_t ms)                     // 定义一个ms延时函数，减少移植时对外部文件依赖;
{
    ms = ms * 10286;                                  // 注意：打勾 "Options --> C/C++ ---> One ELF Section per Function选项"
    for (uint32_t i = 0; i < ms; i++);                // 72MHz系统时钟下，大约多少个空循环耗时1ms
}
#endif


// 向RS485模块发送AT命令
// 参数: char* cmdString :     AT命令字符串地址
//       char* answerString :  等待返回的命令确认字符串地址
//       uint32_t waitTimesMS: 发送命令后等待的时间，毫秒（非准确时间值）
uint8_t RS485_CMD(char *cmdString, char *answerString, uint32_t waitTimesMS)
{
    uint16_t CNT = 0;                   // 等待超时计数
    RS485_CleanReceivedFlag();          // 接收标示置0
    RS485_SendString(cmdString);        // 发送AT指令
    while (1)                           // 等待指令返回执行情况
    {
        if (RS485_CheckReceivedNum())
        {
            if (strstr((char *)xRS485.ReceivedData, answerString)) // 收到命令确认
                return 1;
        }
        delay_ms(1);
        if (++CNT > waitTimesMS)        // 超时未收到正确数据，
        {
            return 0;                   // 返回错误值：0
        }
    }
}


// RS485初始化, 主要是初始化相关的UART
uint8_t RS485_Init(USART_TypeDef *USARTx, uint32_t baudrate)
{
    xRS485.FlagInit = 0;           // 初始化状态
    xRS485.USARTx   = USARTx;      // 记录所用串口端口
    xRS485.Baudrate = baudrate;    // 记录所用的波特率

    if (USARTx == USART1)       USART1_Init(baudrate);
    if (USARTx == USART2)       USART2_Init(baudrate);
    if (USARTx == USART3)       USART3_Init(baudrate);
#ifdef STM32F10X_HD
    if (USARTx == UART4)        UART4_Init(baudrate);
    if (USARTx == UART5)        UART5_Init(baudrate);
#endif
    xRS485.FlagInit = 1;
    return xRS485.FlagInit;       // 返回初始化状态，0_失败，1_正常
}


void RS485_SendData(uint8_t *_pBuf, uint16_t _usLen)
{
    if (xRS485.USARTx == USART1)       USART1_SendData(_pBuf, _usLen);
    if (xRS485.USARTx == USART2)       USART2_SendData(_pBuf, _usLen);
    if (xRS485.USARTx == USART3)       USART3_SendData(_pBuf, _usLen);
#ifdef STM32F10X_HD
    if (xRS485.USARTx == UART4)        UART4_SendData(_pBuf, _usLen);
    if (xRS485.USARTx == UART5)        UART5_SendData(_pBuf, _usLen);

#endif
}

void RS485_SendString(char *str)
{
    if (xRS485.USARTx == USART1)       USART1_SendString(str);
    if (xRS485.USARTx == USART2)       USART2_SendString(str);
    if (xRS485.USARTx == USART3)       USART3_SendString(str);
#ifdef STM32F10X_HD
    if (xRS485.USARTx == UART4)        UART4_SendString(str);
    if (xRS485.USARTx == UART5)        UART5_SendString(str);

#endif
}

// 检查是否收到RS485发回来的数据，标志
// 返回接收到的字节数量
uint16_t RS485_CheckReceivedNum(void)
{
    if ((xRS485.USARTx == USART1) && (xUSART.USART1ReceivedNum))
    {
        xRS485.ReceivedNum = xUSART.USART1ReceivedNum;
        memcpy(xRS485.ReceivedData, xUSART.USART1ReceivedData, xRS485.ReceivedNum);
        xUSART.USART1ReceivedNum = 0;
    }
    if ((xRS485.USARTx == USART2) && (xUSART.USART2ReceivedNum))
    {
        xRS485.ReceivedNum = xUSART.USART2ReceivedNum;
        memcpy(xRS485.ReceivedData, xUSART.USART2ReceivedData, xRS485.ReceivedNum);
        xUSART.USART2ReceivedNum = 0;
    }
    if ((xRS485.USARTx == USART3) && (xUSART.USART3ReceivedNum))
    {
        xRS485.ReceivedNum = xUSART.USART3ReceivedNum;
        memcpy(xRS485.ReceivedData, xUSART.USART3ReceivedData, xRS485.ReceivedNum);
        xUSART.USART3ReceivedNum = 0;
    }
#ifdef STM32F10X_HD
    if ((xRS485.USARTx == UART4)  && (xUSART.UART4ReceivedNum))
    {
        xRS485.ReceivedNum = xUSART.UART4ReceivedNum;
        memcpy(xRS485.ReceivedData, xUSART.UART4ReceivedData, xRS485.ReceivedNum);
        xUSART.UART4ReceivedNum = 0;
    }
    if ((xRS485.USARTx == UART5)  && (xUSART.UART5ReceivedNum))
    {
        xRS485.ReceivedNum = xUSART.UART5ReceivedNum;
        memcpy(xRS485.ReceivedData, xUSART.UART5ReceivedData, xRS485.ReceivedNum);
        xUSART.UART5ReceivedNum = 0;
    }
#endif
    return xRS485.ReceivedNum;
}


// 清理RS485的接收缓存，包括接收长度变量和数据存放缓存
void RS485_CleanReceivedFlag(void)
{
    if (xRS485.USARTx == USART1)       xUSART.USART1ReceivedNum = 0;
    if (xRS485.USARTx == USART2)       xUSART.USART2ReceivedNum = 0;
    if (xRS485.USARTx == USART3)       xUSART.USART3ReceivedNum = 0;
#ifdef STM32F10X_HD
    if (xRS485.USARTx == UART4)        xUSART.UART4ReceivedNum = 0;
    if (xRS485.USARTx == UART5)        xUSART.UART5ReceivedNum = 0;
#endif
    xRS485.ReceivedNum = 0;                                       // 置0，接收长度
    memset(xRS485.ReceivedData, 0, RS485_RX_BUF_SIZE);          // 清零，接收缓存
}







