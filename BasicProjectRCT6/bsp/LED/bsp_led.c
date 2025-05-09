/***********************************************************************************************************************************
 ** 【文件名称】  led.c
 ** 【编写人员】  魔女开发板团队
 ** 【淘    宝】  魔女开发板      https://demoboard.taobao.com
 ***********************************************************************************************************************************
 ** 【文件功能】  实现LED指示灯常用的初始化函数、功能函数
 **                 
 ** 【移植说明】  
 **             
 ** 【更新记录】  
 **
***********************************************************************************************************************************/  
#include "bsp_led.h"


    

void Led_Init(void)
{    
    GPIO_InitTypeDef GPIO_InitStructure;                    // 定义一个GPIO_InitTypeDef类型的结构体
    
    // 使能LED_RED所用引脚端口时钟；使用端口判断的方法使能时钟, 以使代码移植更方便
    if(LED_RED_GPIO == GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    if(LED_RED_GPIO == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    if(LED_RED_GPIO == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
    if(LED_RED_GPIO == GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD , ENABLE);
    if(LED_RED_GPIO == GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);
    if(LED_RED_GPIO == GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF , ENABLE);
    if(LED_RED_GPIO == GPIOG)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF , ENABLE);
    // 使能LED_RED所用引脚端口时钟；使用端口判断的方法使能时钟, 以使代码移植更方便
    if(LED_BLUE_GPIO == GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    if(LED_BLUE_GPIO == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    if(LED_BLUE_GPIO == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
    if(LED_BLUE_GPIO == GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD , ENABLE);
    if(LED_BLUE_GPIO == GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);
    if(LED_BLUE_GPIO == GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF , ENABLE);
    if(LED_BLUE_GPIO == GPIOG)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF , ENABLE);
    
    // 配置LED_RED引脚工作模式
    GPIO_InitStructure.GPIO_Pin   = LED_RED_PIN;            // 选择要控制的GPIO引脚       
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       // 设置引脚模式为通用推挽输出   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // 设置引脚速率为50MHz            
    GPIO_Init(LED_RED_GPIO , &GPIO_InitStructure);          // 调用库函数，初始化GPIO  
    
    // 配置LED_RED引脚工作模式
    GPIO_InitStructure.GPIO_Pin   = LED_BLUE_PIN;           // 选择要控制的GPIO引脚       
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       // 设置引脚模式为通用推挽输出   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // 设置引脚速率为50MHz            
    GPIO_Init(LED_BLUE_GPIO , &GPIO_InitStructure);         // 调用库函数，初始化GPIO    
    
    LED_RED_ON();
    LED_BLUE_ON();
}

//红灯电平反转
void LED_RED_TOGGLE(void)
{
    if (GPIO_ReadOutputDataBit(LED_RED_GPIO, LED_RED_PIN) == 0)		//获取输出寄存器的状态，如果当前引脚输出低电平
	{
		GPIO_SetBits(LED_RED_GPIO, LED_RED_PIN);					//则设置PA1引脚为高电平
	}
	else													//否则，即当前引脚输出高电平
	{
		GPIO_ResetBits(LED_RED_GPIO, LED_RED_PIN);					//则设置PA1引脚为低电平
	}
}

//蓝灯电平反转
void LED_BLUE_TOGGLE(void)
{
    if (GPIO_ReadOutputDataBit(LED_BLUE_GPIO, LED_BLUE_PIN) == 0)		//获取输出寄存器的状态，如果当前引脚输出低电平
	{
		GPIO_SetBits(LED_BLUE_GPIO, LED_BLUE_PIN);					//则设置PA1引脚为高电平
	}
	else													//否则，即当前引脚输出高电平
	{
		GPIO_ResetBits(LED_BLUE_GPIO, LED_BLUE_PIN);					//则设置PA1引脚为低电平
	}
}
