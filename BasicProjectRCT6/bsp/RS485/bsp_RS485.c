#include "bsp_RS485.h"

xRS485_TypeDef  xRS485;



#if 1
static void delay_ms(uint32_t ms)                     // ����һ��ms��ʱ������������ֲʱ���ⲿ�ļ�����;
{
    ms = ms * 10286;                                  // ע�⣺�� "Options --> C/C++ ---> One ELF Section per Functionѡ��"
    for (uint32_t i = 0; i < ms; i++);                // 72MHzϵͳʱ���£���Լ���ٸ���ѭ����ʱ1ms
}
#endif


// ��RS485ģ�鷢��AT����
// ����: char* cmdString :     AT�����ַ�����ַ
//       char* answerString :  �ȴ����ص�����ȷ���ַ�����ַ
//       uint32_t waitTimesMS: ���������ȴ���ʱ�䣬���루��׼ȷʱ��ֵ��
uint8_t RS485_CMD(char *cmdString, char *answerString, uint32_t waitTimesMS)
{
    uint16_t CNT = 0;                   // �ȴ���ʱ����
    RS485_CleanReceivedFlag();          // ���ձ�ʾ��0
    RS485_SendString(cmdString);        // ����ATָ��
    while (1)                           // �ȴ�ָ���ִ�����
    {
        if (RS485_CheckReceivedNum())
        {
            if (strstr((char *)xRS485.ReceivedData, answerString)) // �յ�����ȷ��
                return 1;
        }
        delay_ms(1);
        if (++CNT > waitTimesMS)        // ��ʱδ�յ���ȷ���ݣ�
        {
            return 0;                   // ���ش���ֵ��0
        }
    }
}


// RS485��ʼ��, ��Ҫ�ǳ�ʼ����ص�UART
uint8_t RS485_Init(USART_TypeDef *USARTx, uint32_t baudrate)
{
    xRS485.FlagInit = 0;           // ��ʼ��״̬
    xRS485.USARTx   = USARTx;      // ��¼���ô��ڶ˿�
    xRS485.Baudrate = baudrate;    // ��¼���õĲ�����

    if (USARTx == USART1)       USART1_Init(baudrate);
    if (USARTx == USART2)       USART2_Init(baudrate);
    if (USARTx == USART3)       USART3_Init(baudrate);
#ifdef STM32F10X_HD
    if (USARTx == UART4)        UART4_Init(baudrate);
    if (USARTx == UART5)        UART5_Init(baudrate);
#endif
    xRS485.FlagInit = 1;
    return xRS485.FlagInit;       // ���س�ʼ��״̬��0_ʧ�ܣ�1_����
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

// ����Ƿ��յ�RS485�����������ݣ���־
// ���ؽ��յ����ֽ�����
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


// ����RS485�Ľ��ջ��棬�������ճ��ȱ��������ݴ�Ż���
void RS485_CleanReceivedFlag(void)
{
    if (xRS485.USARTx == USART1)       xUSART.USART1ReceivedNum = 0;
    if (xRS485.USARTx == USART2)       xUSART.USART2ReceivedNum = 0;
    if (xRS485.USARTx == USART3)       xUSART.USART3ReceivedNum = 0;
#ifdef STM32F10X_HD
    if (xRS485.USARTx == UART4)        xUSART.UART4ReceivedNum = 0;
    if (xRS485.USARTx == UART5)        xUSART.UART5ReceivedNum = 0;
#endif
    xRS485.ReceivedNum = 0;                                       // ��0�����ճ���
    memset(xRS485.ReceivedData, 0, RS485_RX_BUF_SIZE);          // ���㣬���ջ���
}







