#include "bsp_ESP8266.h"

xESP8266_TypeDef  xESP8266;  


// 本地US粗略延时函数，减少移植时对外部文件依赖;
#if 0
static void delayUS( __IO u32 times)
{
    times=times*7;      //  10us内用7;
    while(--times)         
        __nop();  
}
#endif 

#if 1
// ms延时函数，减少移植时对外部文件依赖;
static void delayMS(u32 ms)
{
    ms=ms*6500;                  
    for(u32 i=0; i<ms; i++);      // 72MHz系统时钟下，多少个空循环约耗时1ms
}
#endif 


//

uint8_t ESP8266_CMD(char* cmdString, char* answerString, uint32_t waitTimesMS)
{
    uint16_t CNT=0;                     // 等待超时计数
    ESP8266_CleanReceivedFlag();        // 接收标示置0   
    ESP8266_SendString(cmdString);      // 发送AT指令        
    while(1)                            // 等待指令返回执行情况
    {
        if(ESP8266_CheckReceivedNum())  
        {
            if(strstr((char*)xESP8266.ReceivedBuffer, answerString)!=0)  
                return 1;
        }
        delayMS(1);
        if(++CNT > waitTimesMS)    return 0;  // 超时未收到正确数据，返回错误值：0  
    }
}



uint8_t ESP8266_Init(USART_TypeDef* USARTx, uint32_t baudrate)
{      
    printf("\r\nESP8266 开始配置及测试......\r\n");              
    delayMS(300);                                    // 重要，上电后，必须稍延时以等待8266稳定方可工作
        
    if(USARTx == USART1)  { USART1_Init(baudrate); }   
    if(USARTx == USART2)  { USART2_Init(baudrate); }   
    if(USARTx == USART3)  { USART3_Init(baudrate); }   
    #ifdef STM32F10X_HD
    if(USARTx == UART4)   { UART4_Init(baudrate);  }   
    if(USARTx == UART5)   { UART5_Init(baudrate);  }   
    #endif   
    
    xESP8266.FlagOkay = 0;           // 初始化状态
    xESP8266.USARTx   = USARTx;      // 记录所用串口端口
    xESP8266.Baudrate = baudrate;    // 记录所用的波特率                
    
    ESP8266_CMD("AT\r\n",   "OK", 1000)  ? (xESP8266.FlagOkay =1) : (xESP8266.FlagOkay =0) ;   // 测试是否能连接ESP8266
    xESP8266.FlagOkay ? printf("模块连接测试:  成功\r\n") : printf("模块连接测试:  失败\r\n");   // 输出连接状况
    
    return xESP8266.FlagOkay;      // 返回初始化状态，0_失败，1_正常
}


void ESP8266_SendString(char* str)
{
    if(xESP8266.USARTx== USART1) { USART1_SendString(str); }
    if(xESP8266.USARTx== USART2) { USART2_SendString(str); }
    if(xESP8266.USARTx== USART3) { USART3_SendString(str); }
    #ifdef STM32F10X_HD
    if(xESP8266.USARTx== UART4)  { UART4_SendString(str);  }
    if(xESP8266.USARTx== UART5)  { UART5_SendString(str);  }
    #endif          
}

// 检查是否收到ESP8266发回来的数据，标志
// 返回接收到的字节数量
uint16_t ESP8266_CheckReceivedNum(void)
{
    if((xESP8266.USARTx== USART1) && (xUSART.USART1ReceivedNum))   {xESP8266.ReceivedNum = xUSART.USART1ReceivedNum;  memcpy(xESP8266.ReceivedBuffer, xUSART.USART1ReceivedBuffer, xESP8266.ReceivedNum);  xUSART.USART1ReceivedNum=0;}
    if((xESP8266.USARTx== USART2) && (xUSART.USART2ReceivedNum))   {xESP8266.ReceivedNum = xUSART.USART2ReceivedNum;  memcpy(xESP8266.ReceivedBuffer, xUSART.USART2ReceivedBuffer, xESP8266.ReceivedNum);  xUSART.USART2ReceivedNum=0;}
    if((xESP8266.USARTx== USART3) && (xUSART.USART3ReceivedNum))   {xESP8266.ReceivedNum = xUSART.USART3ReceivedNum;  memcpy(xESP8266.ReceivedBuffer, xUSART.USART3ReceivedBuffer, xESP8266.ReceivedNum);  xUSART.USART3ReceivedNum=0;}
    #ifdef STM32F10X_HD
    if((xESP8266.USARTx== UART4)  && (xUSART.UART4ReceivedNum))    {xESP8266.ReceivedNum = xUSART.UART4ReceivedNum;   memcpy(xESP8266.ReceivedBuffer, xUSART.UART4ReceivedBuffer, xESP8266.ReceivedNum);   xUSART.UART4ReceivedNum=0;}
    if((xESP8266.USARTx== UART5)  && (xUSART.UART5ReceivedNum))    {xESP8266.ReceivedNum = xUSART.UART5ReceivedNum;   memcpy(xESP8266.ReceivedBuffer, xUSART.UART5ReceivedBuffer, xESP8266.ReceivedNum);   xUSART.UART5ReceivedNum=0;}
    #endif   
        
    return xESP8266.ReceivedNum;        
}


// 清理ESP8266的接收缓存，包括接收长度变量和数据存放缓存
void ESP8266_CleanReceivedFlag(void)
{
    if(xESP8266.USARTx== USART1)  { xUSART.USART1ReceivedNum =0;}
    if(xESP8266.USARTx== USART2)  { xUSART.USART2ReceivedNum =0;}
    if(xESP8266.USARTx== USART3)  { xUSART.USART3ReceivedNum =0;}
    #ifdef STM32F10X_HD
    if(xESP8266.USARTx== UART4)   { xUSART.UART4ReceivedNum =0; }
    if(xESP8266.USARTx== UART5)   { xUSART.UART5ReceivedNum =0; }
    #endif      
    
    xESP8266.ReceivedNum =0;                                          // 置0，接收长度
    memset(xESP8266.ReceivedBuffer ,0, ESP8266_RX_BUF_SIZE);          // 清零，接收缓存   
}


/******************************************************************************
 * 函  数： ESP8266_GetLinkStatus
 * 功  能： 初始化USART的GPIO、通信参数配置、中断优先级 
 *          (8位数据、无校验、1个停止位)
 * 参  数： uint32_t baudrate  通信波特率
 * 返回值:  0_获取状态失败
 *          2_获得ip
 *          3_建立连接
 *          4_失去连接
 ******************************************************************************/  
uint8_t ESP8266_GetLinkStatus(void)
{
    if (ESP8266_CMD("AT+CIPSTATUS\r\n", "OK", 10000))
    {
        if (strstr((char*)xESP8266.ReceivedBuffer , "STATUS:2"))   return 2;
        if (strstr((char*)xESP8266.ReceivedBuffer , "STATUS:3"))   return 3;
        if (strstr((char*)xESP8266.ReceivedBuffer , "STATUS:4"))   return 4; 
        if (strstr((char*)xESP8266.ReceivedBuffer , "STATUS:5"))   return 5;             
    }
    return 0;
}



/******************************************************************************
 * 函  数： ESP8266_JoinAP
 * 功  能： 连接AP
 * 参  数： char* SSID       WiFi名称
 *          char* passWord   WiFi密码
 * 返 回 值: 0_连接失败
 *           1_连接成功
 ******************************************************************************/  
uint8_t ESP8266_JoinAP (char* SSID, char* passWord)
{
    char strTemp[60];
    uint8_t linkStatus=0;
    
    printf("准备连接SSID：%s, %s\r\n", SSID, passWord ); 
    // 把ESP8266重新配置成SAT模式    
    ESP8266_CMD("AT+RESTORE\r\n" , "ready", 1000)  ? printf("恢复出厂设置:  成功\r\n") : printf("恢复出厂设置: 失败\r\n");   // 恢复模块的出厂设置
    ESP8266_CMD("AT+CWMODE=1\r\n",    "OK", 3000)  ? printf("配置 STA模式:  成功\r\n") : printf("配置 STA模式: 失败\r\n");   // 工作模式：1_STA, 2_AP, 3_STA+AP
    ESP8266_CMD("AT+RST\r\n",      "ready", 3000)  ? printf("重启 ESP8266:  成功\r\n") : printf("重启 ESP8266: 失败\r\n");   // 重启模块: 设置工作模式后，需重启才生效
    ESP8266_CMD("AT+CIPMUX=0\r\n",    "OK", 3000)  ? printf("单连接模式  :  成功\r\n") : printf("单连接模式  : 失败\r\n");   // 多 连 接: 0_单连接，1_多连接 
    //连接至指定WiFi热点
    sprintf(strTemp, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, passWord );
    printf("开始连接AP ... ");
    ESP8266_CMD(strTemp,        "OK\r\n", 20000)  ? printf("成功\r\n") : printf("失败\r\n");
    // 检测连接状态
    printf("获取连接状态：");
    linkStatus = ESP8266_GetLinkStatus();
    ESP8266_CleanReceivedFlag();  
        
    if(linkStatus==0)  {printf("失败，原因：获取失败！\r\n");           return 0;}
    if(linkStatus==2)  {printf("成功，已获得IP\r\n");                   return 1;}
    if(linkStatus==3)  {printf("失败，原因：已连接，但未获得IP！\r\n"); return 0;}
    if(linkStatus==4)  {printf("失败，原因：失去连接！\r\n");           return 0;}
    if(linkStatus==5)  {printf("失败，原因：没有连接\r\n");             return 0;}
         
    return 0;
    
}

 // 把模块设置成AP模式 
uint8_t ESP8266_SetAP(char* SSID, char* passWord)
{
    char strTemp[60];
        
    printf("准备建立SSID：%s, %s\r\n", SSID, passWord ); 
    // 把ESP8266重新配置成AP模式    
    ESP8266_CMD("AT+RESTORE\r\n" ,"ready", 1000)  ? printf("恢复出厂设置: 成功\r\n") : printf("恢复出厂设置: 失败\r\n");   // 恢复模块的出厂设置
    ESP8266_CMD("AT+CWMODE=2\r\n",   "OK", 3000)  ? printf("配置为AP模式: 成功\r\n") : printf("配置 STA模式: 失败\r\n");   // 工作模式：1_STA, 2_AP, 3_STA+AP
    ESP8266_CMD("AT+RST\r\n",     "ready", 3000)  ? printf("重启 ESP8266: 成功\r\n") : printf("重启 ESP8266: 失败\r\n");   // 重启模块: 设置工作模式后，需重启才生效
    // 配置WiFi热点
    sprintf(strTemp, "AT+CWSAP=\"%s\",\"%s\",11,0\r\n", SSID, passWord );
    printf("开始建立AP... ");
    if(ESP8266_CMD(strTemp,        "OK\r\n", 10000)) 
    {
        printf("成功\r\n");
        return 1;
    }
    else
    {
        printf("失败\r\n");   
        return 0;
    }
} 


uint8_t ESP8266_TCPConnect(char* IP, uint16_t port)
{
    char strTemp[100];
    uint8_t flag=0;    
    
    printf("建立TCP通信... ");    
    sprintf(strTemp, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", IP, port );
    ESP8266_CMD(strTemp, "OK\r\n", 20000)  ? (flag=1) : (flag=0) ;
    flag ? printf("成功\r\n") : printf("失败, 请检查手机端IP、端口号，并断开之前的连接状态\r\n");
    ESP8266_CleanReceivedFlag();   
    return flag;        
}

// 设置：多连接，0_单连接，1_多连接;备注：透传必须AT+CIPMUX=0;
uint8_t   ESP8266_SetCIPMux(uint8_t value)    
{
    uint8_t flag=0;
    char strTemp[50];
    sprintf(strTemp, "AT+CIPMUX=%d\r\n", value);
    ESP8266_CMD(strTemp, "OK", 3000)  ?  (flag=1) : (flag=0) ;
    flag ? printf("设置连接方式:  成功\r\n") : printf("设置连接方式: 失败\r\n");   // 多 连 接: 0_单连接，1_多连接 
    ESP8266_CleanReceivedFlag();
    return flag;        
}


  // 设置：传输方式，0_普通传输，1_透明传输
uint8_t   ESP8266_SetCIPMode(uint8_t value)   
{
    uint8_t flag=0;
    char strTemp[50];
    sprintf(strTemp, "AT+CIPMODE=%d\r\n", value);
    ESP8266_CMD(strTemp, "OK", 3000)  ?  (flag=1) : (flag=0) ;
    flag ? printf("设置传输方式:  成功\r\n") : printf("设置传输方式: 失败\r\n");   // 传输方式，0_普通传输，1_透明传输
    ESP8266_CleanReceivedFlag();
    return flag;         
}

// 开启透传
uint8_t ESP8266_EnablePassThrough(void)
{
    uint8_t status=1;
    status &= ESP8266_CMD("AT+CIPMODE=1\r\n", "OK", 3000);   // 传输方式，0_普通传输，1_透明传输
    status &= ESP8266_CMD("AT+CIPSEND\r\n",    ">", 3000);   // 数据传输，当已设置透传时，无需参数
     
    ESP8266_CleanReceivedFlag();                             // 清除接收标记、缓存
    
    status ? printf("开始透明传输: 成功\r\n") : printf("开始透明传输: 失败\r\n");   
    return status;            
}

// 停止透传
uint8_t ESP8266_DisablePassThrough(void)
{
    
        
 
    return 0;
}
