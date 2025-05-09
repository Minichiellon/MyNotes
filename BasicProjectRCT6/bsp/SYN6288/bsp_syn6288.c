#include "bsp_syn6288.h"
#include "stdarg.h"



xSYN6288_TypeDef  xSYN6288;                // 全局变量结构体



static void delay_ms(uint32_t ms)          // 定义一个ms延时函数，减少移植时对外部文件依赖; 本函数仅作粗略延时使用，而并非精准延时;
{
    ms = ms * 11993;                       // 注意：此参考值运行条件：打勾 Options/ c++ / One ELF Section per Function
    for (uint32_t i = 0; i < ms; i++);     // 72MHz系统时钟下，大约多少个空循环耗时1ms
}



//Music:选择背景音乐。0:无背景音乐，1~15：选择背景音乐
// SYN6288_SendFrameInfo(0, "[v10][m1][t5]结算金额 为32.8元");
// 参数： 0~15  :  背景音乐，0_无背景音乐，1~15_背景音乐可选
//       [V0~16]:  文本朗读音量，0_最小，16_最大
//       [m0~16]:  背景音乐音量，0_最小，16_最大
//       [t0~5]:   朗读语速，0_最慢，5_最快
//       其它不常用功能请参考数据手册
static void SYN6288_SendFrameInfo(uint8_t Music, uint8_t *HZdata)
{
    /****************需要发送的文本**********************************/
    unsigned  char  Frame_Info[50];
    unsigned  char  HZ_Length;
    unsigned  char  ecc  = 0;             //定义校验字节
    unsigned  int i = 0;
    HZ_Length = strlen((char *)HZdata);   //需要发送文本的长度

    /*****************帧固定配置信息**************************************/
    Frame_Info[0] = 0xFD ;                //构造帧头FD
    Frame_Info[1] = 0x00 ;                //构造数据区长度的高字节
    Frame_Info[2] = HZ_Length + 3;        //构造数据区长度的低字节
    Frame_Info[3] = 0x01 ;                //构造命令字：合成播放命令
    Frame_Info[4] = 0x01 | Music << 4 ;   //构造命令参数：背景音乐设定

    /*******************校验码计算***************************************/
    for (i = 0; i < 5; i++)               //依次发送构造好的5个帧头字节
    {
        ecc = ecc ^ (Frame_Info[i]);      //对发送的字节进行异或校验
    }

    for (i = 0; i < HZ_Length; i++)       //依次发送待合成的文本数据
    {
        ecc = ecc ^ (HZdata[i]);          //对发送的字节进行异或校验
    }
    /*******************发送帧信息***************************************/
    memcpy(&Frame_Info[5], HZdata, HZ_Length);
    Frame_Info[5 + HZ_Length] = ecc;

    if (xSYN6288.USARTx == USART1)   USART1_SendData(Frame_Info, 5 + HZ_Length + 1);
    if (xSYN6288.USARTx == USART2)   USART2_SendData(Frame_Info, 5 + HZ_Length + 1);
    if (xSYN6288.USARTx == USART3)   USART3_SendData(Frame_Info, 5 + HZ_Length + 1);
    if (xSYN6288.USARTx == UART4)    UART4_SendData(Frame_Info, 5 + HZ_Length + 1);
    if (xSYN6288.USARTx == UART5)    UART5_SendData(Frame_Info, 5 + HZ_Length + 1);
}



/***********************************************************
* 名    称： SYN6288_Set(uint8_t *Info_data)
* 功    能： 主函数 程序入口
* 入口参数： *Info_data:固定的配置信息变量
* 出口参数：
* 说    明：本函数用于配置，停止合成、暂停合成等设置 ，默认波特率9600bps。
* 调用方法：通过调用已经定义的相关数组进行配置。
**********************************************************/
static void SYN6288_Set(uint8_t *Info_data)
{
    uint8_t Com_Len;
    Com_Len = strlen((char *)Info_data);
    UART5_SendData(Info_data, Com_Len);
}



/******************************************************************************
 * 函  数： SYN6288_Say
 * 功  能： 输出合成语音
 * 参  数： 格式化参数，如printf参数般一样的用法
 * 返回值： 无
 * 示  例： SYN6288_Say("你好吗?");
*******************************************************************************/
void SYN6288_Say(char *fmt, ...)
{
    static char  str_1[200];   // 缓存区1，模块每次可转换200字节
    static char  str_2[200];   // 缓存区2，模块每次可转换200字节

    va_list ap;
    va_start(ap, fmt);
    vsprintf(str_1, fmt, ap);
    va_end(ap);

    sprintf(str_2, "[d][V12][m15][t5]%s", str_1);  // [d]恢复默认状态，[V12]朗读音量0~16，[m15]背景音量0~16，[t5]语速0~5
    SYN6288_SendFrameInfo(0, (uint8_t *)str_2); // 无背景音乐
}


/******************************************************************************
 * 函  数： SYN6288_Init
 * 功  能： 初始化所用串口, 模块默认通信波特率9600
 * 参  数： 串口-USARTx
 * 返回值： 无
 * 示  例： SYN6288_Init(USART1);
*******************************************************************************/
void SYN6288_Init(USART_TypeDef *USARTx)
{
    uint16_t baudrate = 9600;      // 默认波特率9600bps。

    delay_ms(200);                 // 上电后，稍作延时，等待模块进入稳定状态

    if (USARTx == USART1)    USART1_Init(baudrate);
    if (USARTx == USART2)    USART2_Init(baudrate);
    if (USARTx == USART3)    USART3_Init(baudrate);
#ifdef STM32F10X_HD
    if (USARTx == UART4)     UART4_Init(baudrate);
    if (USARTx == UART5)     UART5_Init(baudrate);
#endif
    xSYN6288.FlagOkay = 0;         // 初始化状态
    xSYN6288.USARTx   = USARTx;    // 记录所用串口端口
}
