/***********************************************************************************************************************************
 **【购买链接】  魔女科技   https://demoboard.taobao.com
 **【更新分享】  Q群文件夹  1126717453
 ***********************************************************************************************************************************
 **【文件名称】  pwm.c
 **【功能描述】  函数实现
 **
 **【适用平台】  STM32F103 + 标准库v3.5 + keil5
 **                 
 **【移植说明】  引脚修改：在本h文件中修改
 **              TIM参数 ：在本h文件中
 **           
 **【更新记录】  2021-05-16  创建;
 **              2021-07-06  增加TIM1、TIM8判断，开启MOE位;
 **              2021-07-06  完善函数名称，增加v前辍，表示返回void, 提高代码可读性;
 **
***********************************************************************************************************************************/
#include "bsp_pwm.h"


/*****************************************************************************
 ** 全局变量声明
 *****************************************************************************/



/******************************************************************************
 * 函数名： TIM_PwmInit
 * 功  能： PWM配置
 * 参  数：【GPIOx】GPIOA~F
 *         【PINx】 GPIO_Pin_0~15
 *         【TIMx】 TIM1~8
 *         【CHx】  1~4
 *         【psc】  72             // 为了方便，配置72后，计数器的脉冲为:1us
 *         【arr】  0~65535        // 周期时长(us)
 *         【CCR】  0~65535     　 // 高电平脉宽时长(us)
 * 返  回： 无
 ******************************************************************************/  
void TIM_PwmInit(GPIO_TypeDef* GPIOx, u16 PINx, TIM_TypeDef* TIMx, u8 CHx, u16 psc, u16 arr, u16 ccr)
{    
    // TIM时钟使能
    if(TIMx==TIM1)    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    if(TIMx==TIM8)    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);  
    if(TIMx==TIM2)    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  
    if(TIMx==TIM3)    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  
    if(TIMx==TIM4)    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  
    if(TIMx==TIM5)    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);  
    if(TIMx==TIM6)    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);  
    if(TIMx==TIM7)    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);      

    // GPIO时钟使能
    if(GPIOx==GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    if(GPIOx==GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    if(GPIOx==GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    if(GPIOx==GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    if(GPIOx==GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    if(GPIOx==GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
    
    // GPIO配置
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = PINx;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStructure);
    
    // 使用寄存器配置
    #if 1
    // TIM时基配置
    TIMx->PSC  = psc-1;           // CNT预分频值=72000000/PSC
    TIMx->ARR  = arr-1;           // 自动重装载值, 即多少个CNT脉冲为一周期
    TIMx->CR1  = 0;               // 复位清0
    TIMx->CR1 |= 0<< 4;           // DIR方向, 0-向上, 1-向下
    TIMx->CR1 |= 1<< 7;           // ARR预装载使能; 0-不缓冲,立即改变ARR值; 1-缓冲,等更新事件时再改变ARR值
 
    // 输出比较模式
    u16 regCCMR =0;
    regCCMR |= 0x00<<0;           // 输出模式
    regCCMR |= 0x01<<3;           // CCRx寄存器预装载使能
    regCCMR |= 0x06<<4;           // 输出模式; 6=PW1,向上计数时,CNT<CCR时为有效电平; 7=PWM2, 向上计数时, CNT<CCR时为无效电平
    if(CHx==1)  TIMx->CCMR1 = (TIMx->CCMR1 & (~((u16)0xFF<<0))) | (regCCMR<<0);
    if(CHx==2)  TIMx->CCMR1 = (TIMx->CCMR1 & (~((u16)0xFF<<8))) | (regCCMR<<8);
    if(CHx==3)  TIMx->CCMR2 = (TIMx->CCMR2 & (~((u16)0xFF<<0))) | (regCCMR<<0);
    if(CHx==4)  TIMx->CCMR2 = (TIMx->CCMR2 & (~((u16)0xFF<<8))) | (regCCMR<<8);

    // 通道使能配置
    u16 regCCER = 0;
    regCCER |= 0<<1;              // 通道x有效电平; 0-高电平有效; 1-低电平有效
    regCCER |= 1<<0;              // 通道x使能
    TIMx->CCER = (TIMx->CCER & (~(0xFF<<((CHx-1)*4)))) | (regCCER<<((CHx-1)*4));

    if((TIMx==TIM1) || (TIMx==TIM8))  TIMx->BDTR |= TIM_BDTR_MOE;  // 高级定时器，需要开启BDTR的MOE位
    TIMx->CR1 |= 1<<0;            // 使能计数器
    
    
    // 使用标准库配置
    #else
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    // TIM时基配置
    TIM_TimeBaseStructure.TIM_Prescaler= (psc-1);                   // 驱动CNT计数器的时钟 = Fck_int/(psc+1)
    TIM_TimeBaseStructure.TIM_Period= (arr-1);                      // 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;           // 时钟分频因子 ，用于配置死区时间，没用到，随意
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;       // 计数器计数模式，设置为向上计数    
    TIM_TimeBaseStructure.TIM_RepetitionCounter=0;                  // 重复计数器的值，没用到，可以随意设置
    TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);                 // 初始化定时器        
    // 输出比较模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;               // 配置为PWM模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   // 输出使能
    TIM_OCInitStructure.TIM_Pulse = ccr;                            // 设置占空比大小
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        // 输出通道电平极性配置
    if(CHx==1)  TIM_OC1Init(TIMx, &TIM_OCInitStructure);
    if(CHx==2)  TIM_OC2Init(TIMx, &TIM_OCInitStructure);
    if(CHx==3)  TIM_OC3Init(TIMx, &TIM_OCInitStructure);
    if(CHx==4)  TIM_OC4Init(TIMx, &TIM_OCInitStructure);
    // CCR预装载
    if(CHx==1)  TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);    
    if(CHx==2)  TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
    if(CHx==3)  TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
    if(CHx==4)  TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
    
    TIM_Cmd(TIMx, ENABLE);                                          // 使能计数器
    //TIM_CtrlPWMOutputs(TIMx, ENABLE);                             // 主输出使能，当使用的是通用定时器时，这句不需要  
    #endif
    
    // 设置初始化后的占空比值
    TIM_SetCCR(TIMx, CHx, ccr);

}



/******************************************************************************
 * 函数名： TIM_SetCCR
 * 功  能： 高电平脉宽时间(us）
 * 参  数：【TIMx】TIM1~8
 *         【CHx】 1~4
 *         【CCR】 0~65535;     // 因Init配置分频后计数器时基为:1us, 那CCR值即等价于us值
 * 返  回： 无
 ******************************************************************************/  
void TIM_SetCCR(TIM_TypeDef* TIMx, u8 CHx, u16 CCR) 
{       
    //if(CCR>TIM_PWM_ARR)    CCR=TIM_PWM_ARR;  // 如果脉宽时长大于周期时长，就取最大值
    //if(CCR<0)         CCR=0;
                
    if(CHx==1)    TIMx->CCR1 = CCR;
    if(CHx==2)    TIMx->CCR2 = CCR;
    if(CHx==3)    TIMx->CCR3 = CCR;
    if(CHx==4)    TIMx->CCR4 = CCR;        
}









