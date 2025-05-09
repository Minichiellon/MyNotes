#ifndef __BSP_TIM_H
#define __BSP_TIM_H

#include <stm32f10x.h>
#include "stm32f10x_tim.h"



/*****************************************************************************
 ** 全局变量 (无要修改)
****************************************************************************/
typedef struct 
{
    uint16_t   TIM6Counter;                     // 进入中断的次数，可理解为已产生了多少次周期
    uint16_t   TIM7Counter;                     // 进入中断的次数
    
}xTIM_TypeDef;

extern xTIM_TypeDef  xTIM;                      // 声明为全局变量,方便记录信息、状态
    


// 基本定时器 TIM6
void TIM6_Init(uint16_t PSC, uint16_t ARR);     // 初始化TIM6; PSC-时钟分频值，ARR-自动重装载值(多少个分频后的脉冲，组成一频率周期)
void TIM6_SetARR(uint16_t ARR);                 // 重设TIM6的自动重装载值，即频率周期值(多少个分频后的脉冲，组成一周期)
void TIM6_Start(void);                          // 启动TIM6, 使用TIM6_Init函数已设置参数
void TIM6_Stop(void);                           // 停止TIM6, 原设置不被清0;
// 基本定时器 TIM7
void TIM7_Init(uint16_t PSC, uint16_t ARR);     // 初始化TIM7; PSC-时钟分频值，ARR-自动重装载值(多少个分频后的脉冲，组成一频率周期)
void TIM7_SetARR(uint16_t ARR);                 // 重设TIM7的自动重装载值，即频率周期值(多少个分频后的脉冲，组成一周期)
void TIM7_Start(void);                          // 启动TIM7, 使用TIM6_Init函数已设置参数
void TIM7_Stop(void);                           // 停止TIM7, 原设置不被清0;


#endif



