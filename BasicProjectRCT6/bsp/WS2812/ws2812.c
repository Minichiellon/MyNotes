/***********************************************************************************************************************************
  *【文件名称】  w25qxx.h
  *【编写人员】  魔女开发板团队
  *【更新分享】  如有更新，分享在Q群文件夹  262901124
  *【淘    宝】  魔女开发板  https://demoboard.taobao.com
  **********************************************************************************************************************************
  *【代码适用】  已于STM32F103xx中验证运行
  *
  *                 
  *【移植说明】  本示例中，为简化代码，使用了manage_f103.c和h文件中的一些函数，移植时特别注意
  *              
  *              
  *【更新记录】  2020-04-15  创建、完成调试
  *              2021-01-13  完善文件及函数结构、完善注释
  *               
  *
  *
***********************************************************************************************************************************/
#include "ws2812.h"



/*****************************************************************************
 ** 本地变量
*****************************************************************************/
#define  LEDRGB_1  WS2812_GPIO->BSRR |= WS2812_PIN ;
#define  LEDRGB_0  WS2812_GPIO->BSRR |= WS2812_PIN << 16;



/*****************************************************************************
 ** 本地函数定义
 **
 ** 说明1：很多人不明白ws2812的通信协议、0码、1码，可参考下面这个博客
    https://blog.csdn.net/qq_38190041/article/details/91437493
 **
 ** 说明2：也有很多人不明白如何控制、监测__nop()的时长，请认真参考下面方法
    https://blog.csdn.net/ybhuangfugui/article/details/105190292?utm_medium=distribute.pc_relevant.none-task-blog-baidujs_utm_term-7&spm=1001.2101.3001.4242
    https://blog.csdn.net/qq_25144391/article/details/103552526?utm_medium=distribute.pc_relevant_t0.none-task-blog-BlogCommendFromMachineLearnPai2-1.control&depth_1-utm_source=distribute.pc_relevant_t0.none-task-blog-BlogCommendFromMachineLearnPai2-1.control
*****************************************************************************/
// 0码； 注意：ws2812的0码，非单指低电平！是高低电平的组合时间！
void write0(void)
{    
    u8 cnt0=3;
    
    LEDRGB_1;        // 0码时，前段高电平时长: 220~380ns
    __nop();
    
    LEDRGB_0;        // 0码时，后段低电平时长: 580~1000ns   
    while(cnt0--)    
        __nop();  
}

// 1码;  注意：ws2812的1码，非单指高电平！是高低电平的组合时间！
void write1(void)
{    
    u8  cnt0=3;
    LEDRGB_1;        // 1码时，前段高电平时长: 580~1000us  
    while(cnt0--)     
        __nop();    
    
    LEDRGB_0;        // 1码时，前段高电平时长: 580~1000us   
    while(cnt0--)   
        __nop();     
}

// 写1字节值
void writeByte(u8 data)
{
    for(u8 i=0; i<8; i++)
    {
        if((data<<i) & 0x80)   write1();     // 控制引脚，输出1码的电平时长组合
        else                   write0();     // 控制引脚，输出0码的电平时长组合
    }
}



/*****************************************************************************
 ** 函  数：W2812_Init
 ** 功  能：初始化ws2812所用的引脚
 ** 说  明：普通推挽输出
 ** 参  数：无
 ** 返回值：无 
 ** 备  注：
*****************************************************************************/
void Ws2812_Init(void)
{   
    GPIO_InitTypeDef G;                              /* 定义一个GPIO_InitTypeDef类型的结构体 */   

    // 使用端口判断的方法使能时钟, 以使代码移植更方便
    // 使能引脚端口时钟
    if(WS2812_GPIO == GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    if(WS2812_GPIO == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    if(WS2812_GPIO == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
    if(WS2812_GPIO == GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD , ENABLE);
    if(WS2812_GPIO == GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);
    if(WS2812_GPIO == GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF , ENABLE);
    if(WS2812_GPIO == GPIOG)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF , ENABLE);
    
    G.GPIO_Pin   = WS2812_PIN;                         /* 选择要控制的GPIO引脚       */    
    G.GPIO_Mode  = GPIO_Mode_Out_PP;                 /* 设置引脚模式为通用推挽输出 */     
    G.GPIO_Speed = GPIO_Speed_50MHz;                 /* 设置引脚速率为50MHz        */      
    GPIO_Init(WS2812_GPIO, &G);                    /* 调用库函数，初始化GPIO     */
      
    GPIO_ResetBits(WS2812_GPIO,  WS2812_PIN);        

    printf("LED 指示灯            配置完成\r");     
    //GPIOSet (LED_RGB_GPIO , LED_RGB_PIN, GPIO_Mode_OUT ,GPIO_OType_PP , GPIO_Speed_50M , GPIO_PuPd_DOWN );   
    // 初始化后，先复位一次，避免电平扰动而造成的通信错误 
    Ws2812_Reset();      
}
   


/*****************************************************************************
 ** 函  数：Ws2812_SetColor
 ** 功  能：设置颜色值
 ** 说  明：分别设置R\G\B的值
 ** 参  数：
 ** 返回值：无 
 ** 备  注：
*****************************************************************************/
void Ws2812_SetColor(u8 red, u8 green, u8 blue)
{
    writeByte(green);
    writeByte(red);
    writeByte(blue);
}  



/*****************************************************************************
 ** 函  数：Ws2812_SetRGB
 ** 功  能：设置颜色值
 ** 说  明：与Ws2812_SetColor函数的功能基本一样的，只是直接输入RGB值
 ** 参  数：u16 rgb: 16位RGB颜色值; 声明为u32类型，只为了兼容日后可能扩展
 ** 返回值：无 
 ** 备  注：
*****************************************************************************/
void Ws2812_SetRGB(u32 rgb)
{
    u8 red   = (rgb>>16)&0xFF;
    u8 green = (rgb>>8) &0xFF;
    u8 blue  = rgb & 0xFF;
    
    Ws2812_SetColor(red, green, blue);
}



/*****************************************************************************
 ** 函  数：Ws2812_Off
 ** 功  能：关闭灯光
 ** 说  明：
 ** 参  数：元
 ** 返回值：无 
 ** 备  注：
*****************************************************************************/
void Ws2812_Off(void)
{
    Ws2812_SetColor(0, 0, 0);  
}



/*****************************************************************************
 ** 函  数：Ws2812_WriteReset
 ** 功  能：复位码
 ** 说  明：一个大于某时长的低电平
 ** 参  数：元
 ** 返回值：无 
 ** 备  注：
*****************************************************************************/
void Ws2812_Reset(void)
{
    LEDRGB_0;      // RES : >280us  
    __nop();
    __nop();    
    __nop();
    __nop();
}

