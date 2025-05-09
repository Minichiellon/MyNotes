#include "bsp_tim.h"


xTIM_TypeDef  xTIM;         // 声明为全局变量,方便记录信息、状态


/*****************************************************************************
 * 函  数： TIM6_Init
 * 功  能： 配置基本定时器TIM6
 * 参  数： uint16_t PSC :  预分频器数值,16位; 用于设置计数器1脉冲的时长(时钟频率);
 *          uint16_t ARR :  自动重装载数值,16位; 即多少个脉冲产生一个更新或中断(1周期);
 * 返回值： 无
 * 重  要： 1: F103系列，TIM的时钟源频率为72000000
 *          2: TIM_TimeBaseInitTypeDef结构体里面有5个成员，
 *             TIM6和TIM7基本定时的配置只需用到2个，另外3个是配置通用、高级定时器所用。
 *             typedef struct
 *             {
 *                 TIM_Prescaler ;          // 所有定时器 都有
 *                 TIM_CounterMode ;        // 基本定时器TIMx,x[6,7]没有，其他都有
 *                 TIM_Period ;             // 所有定时器 都有
 *                 TIM_ClockDivision ;      // 基本定时器TIMx,x[6,7]没有，其他都有
 *                 TIM_RepetitionCounter ;  // TIMx,x[1,8,15,16,17]才有
 *             }TIM_TimeBaseInitTypeDef;
*****************************************************************************/
void TIM6_Init(uint16_t PSC, uint16_t ARR)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);        // 设置中断组为2, 这个设置，放在了main()函数的第1行，以方便统一设置所有中断的分组
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;           // 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 设置主优先级为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // 设置抢占优先级为1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 允许中断
    NVIC_Init(&NVIC_InitStructure);

#if 0                                                         // 标准库写法
#include "stm32f10x_tim.h"
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);      // 开启TIM6时钟,即内部时钟CK_INT=72M
    TIM_Cmd(TIM6, DISABLE);                                   // 在配置前，先关闭计数器
    TIM_TimeBaseStructure.TIM_Prescaler = PSC - 1;            // 预分频器数值,16位; 计数器的时钟频率CK_CNT等于f CK_PSC /(PSC[15:0]+1)。在每一次更新事件时，PSC的数值被传送到实际的预分频寄存器中。
    TIM_TimeBaseStructure.TIM_Period    = ARR - 1;            // 自动重装载数值,16位; 即多少个脉冲产生一个更新或中断(1周期)。如果自动重装载数值为0，则计数器停止。
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);           // 初始化定时器

    TIM_ClearFlag(TIM6, TIM_FLAG_Update);                     // 清除计数器中断标志位
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);                // 开启更新中断(只是开启功能，未工作）
#else                                                         // 寄存器写法, 优点：方便移植、调试
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN ;                      // 开启TIM6时钟,
    TIM6->CR1  = 0;                                           // 在配置前，先关闭计数器, 并清0 CR1的配置
    TIM6->CR2  = 0;                                           // 清0 CR2的配置
    TIM6->SR   = 0;                                           // 清0 SR寄存器, 本寄存器只有一个功能: 中断标志位。
    TIM6->CNT  = 0;                                           // 清0 计数器的值
    TIM6->DIER = 0x01 ;                                       // 开启更新中断(只是开启功能，未工作）, 关闭DMA清求
    TIM6->CR1 |= 1 << 7;                                      // 开启预装载
    TIM6->PSC  = PSC - 1;                                     // 预分频器数值,16位; 计数器的时钟频率CK_CNT等于f CK_PSC /(PSC[15:0]+1)。在每一次更新事件时，PSC的数值被传送到实际的预分频寄存器中。
    TIM6->ARR  = ARR - 1;                                     // 自动重装载数值,16位; 即多少个脉冲产生一个更新或中断(1周期)。如果自动重装载数值为0，则计数器停止。
#endif
}

/*****************************************************************************
 * 函  数： TIM6_SetARR
 * 功  能： 重设TIM7的自动重装载值
 * 参  数： uint16_t ARR: 自动重装载值，即频率周期值(多少个分频后的脉冲，组成一周期)
 * 返回值： 无
*****************************************************************************/
void TIM6_SetARR(uint16_t ARR)
{
    TIM6->ARR= ARR-1;    
}

/*****************************************************************************
 * 函  数： TIM6_Start
 * 功  能： 启动TIM6: 使用TIM6_Init函数已设置参数
 * 参  数： 无
 * 返回值： 无
*****************************************************************************/
void TIM6_Start(void)
{
    xTIM.TIM6Counter = 0;       // 中断计数清0
    TIM6->CNT=0;                // 计数寄存器清0
    TIM6->CR1 |= 0x01;          // 使能TIM6, 立即开始工作
}

/*****************************************************************************
 * 函  数： TIM6_Stop
 * 功  能： 停止TIM6, 原设置不被清0;
 * 参  数： 无
 * 返回值： 无
*****************************************************************************/
void TIM6_Stop(void)
{
    TIM6->CR1 &= ~0x01;         // 停止TIM6
}

/*****************************************************************************
 * 函  数： TIM6_IRQHandler
 * 功  能： 中断服务函数
 * 参  数： 无
 * 返回值： 无
*****************************************************************************/
void  TIM6_IRQHandler(void)
{
    TIM6->SR &= (uint16_t)~0x01;    // 清0更新中断标志位
    xTIM.TIM6Counter ++ ;           // 计数+1
}
