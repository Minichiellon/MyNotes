#ifndef __ESP8266__H
#define __ESP8266__H



#include <stm32f10x.h>  
#include "stdio.h"
#include "bsp_usart.h"





/*****************************************************************************
 ** 全局变量 
****************************************************************************/
#define ESP8266_RX_BUF_SIZE       1200                    // 数据接收缓冲区大小，大部份情况下都不用修改; USART中断里的缓存大小

typedef struct 
{    
    uint8_t         FlagOkay;                             // 状态标记; 0=未初始化或异常, 1=正常
    uint16_t        ReceivedNum;                          // 接收到多少个字节数据; 0-无数据，非0_接收到的字节数
    uint8_t         ReceivedBuffer[ESP8266_RX_BUF_SIZE];  // 接收到数据的缓存; ESP8266在AT模式下，每帧数据最长为1056个字节;
    char*           APName;                               // 当创建或加入AP时的: SSID
    char*           APPassword;                           // 当创建或加入AP时的: 密码
    uint32_t        Baudrate;                             // 记录所用的串口波特率
    USART_TypeDef*  USARTx;                               // 记录所用的端口
}xESP8266_TypeDef;
extern xESP8266_TypeDef  xESP8266;                        // 声明为全局变量,方便记录信息、状态
    



/*****************************************************************************
 ** 声明全局函数
 ** 说明：封装的函数比较多，但实际操作时，只调用约四个函数，即可建立通信;
 **       已对常用操作，做了简单高效的流程优化，具体操作，请参考main文件;
****************************************************************************/
// 下面4个基本函数，可完成AT命令下绝大部分操作
uint8_t   ESP8266_Init(USART_TypeDef* USARTx, uint32_t baudrate);  // 初始化
void      ESP8266_SendString(char* str);                           // 发送任意长度字符串
uint16_t  ESP8266_CheckReceivedNum(void);                          // 检查是否收到新数据，返回接收到的字节长度
void      ESP8266_CleanReceivedFlag(void);                         // 清理ESP8266的接收缓存，包括接收长度变量和数据存放缓存
// 下面4个扩展函数，为方便使用而编写，基于上面4个函数实现
uint8_t   ESP8266_JoinAP (char* SSID, char* passWord);             // 加入某热点，
uint8_t   ESP8266_SetAP(char* SSID, char* passWord);               // 把模块配置成AP模式 
uint8_t   ESP8266_GetLinkStatus(void);                             // 获取连接状态
uint8_t   ESP8266_TCPConnect(char* IP, uint16_t port);             // 以TCP通信方式与目标连接
// 常用AT指令封装
uint8_t   ESP8266_SetCIPMux(uint8_t value);                        // 设置：多连接，0_单连接，1_多连接;备注：透传必须AT+CIPMUX=0;
uint8_t   ESP8266_SetCIPMode(uint8_t value);                       // 设置：传输方式，0_普通传输，1_透明传输
uint8_t   ESP8266_CMD (char* cmdString, char* answerString, uint32_t waitTimesMS); // 发送AT命令，并等待返回
uint8_t   ESP8266_EnablePassThrough(void);
uint8_t   ESP8266_DisablePassThrough(void);
#endif


