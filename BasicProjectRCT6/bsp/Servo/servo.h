#ifndef __SERVO_H
#define __SERVO_H
/***********************************************************************************************************************************
 ** 【文件名称】   servo.h
 ** 【编写人员】   魔女开发板团队-老周
 ** 【更新分享】   Q群文件夹   887199504
 ** 【淘    宝】   魔女开发板  https://demoboard.taobao.com
 ***********************************************************************************************************************************
 **	【实验名称】   TIM_舵机控制
 **               
 **	【实验平台】   STM32F103RCT6 + 舵机MG996R(180度) + 1.8寸显示屏
 **               
 ** 【函数简介】   为统一代码，已封装成4个全局函数, 只要修改h文件中端口参数,即可调用;
 **                void AnalogServo_1_Angle(float angle);                           // 舵机1#配置角度;【可选参数】 [angle]0.0°~180.0°;
 **                void AnalogServo_ccrSet(TIM_TypeDef* TIMx, u8 CHx, u16 CCR);     // 控制TIMx的CCRx的值;【可选参数】 [TIMx]TIM1~8, [CHx]1~4, [CCR]0~65535;
 **               

 **
 **  【更新记录】  2021-04-23  创建文件;
 **                2021-04-25  完善角度转换函数;
 **                2021-04-25  TIM_PwmInit(),TIM_SetCCR()两个函数, 方便代码利用;
 ** 
************************************************************************************************************************************/
#include <stm32f10x.h>  
#include <stdio.h>
#include "stm32f10x_tim.h"




/*****************************************************************************
 ** 移植配置
****************************************************************************/
// 舵机_1
#define SERVO_1_GPIO                GPIOB                     // GPIO
#define SERVO_1_PIN                 GPIO_Pin_8                // PIN
#define SERVO_1_TIM_PORT            TIM4                      // 定时器: TIMx
#define SERVO_1_TIM_CH              3                         // 通道: CHx
#define SERVO_1_ANGLE_RESET         90                        // 上电后复位角度
// 舵机_2
#define SERVO_2_GPIO                GPIOB                     // GPIO
#define SERVO_2_PIN                 GPIO_Pin_9                // PIN
#define SERVO_2_TIM_PORT            TIM4                      // 定时器: TIMx
#define SERVO_2_TIM_CH              4                         // 通道: CHx
#define SERVO_2_ANGLE_RESET         90                        // 上电后复位角度
// 时基配置, 适用20ms周期的舵机
#define SERVO_TIM_PSC               72                        // PSC分频值，即计数器脉冲=72000000/72=1000000次/秒=1us/次，(这个尽量不修改)              
#define SERVO_TIM_ARR               20000                     // 周期=20000*1us=20ms




/*****************************************************************************
 ** 全局变量
****************************************************************************/
typedef struct 
{
    u8     initOK;                  // 初始化标记; 0=未初始化, 1=已初始化
    float  angleNow_1;              // 舵机1目前角度
    float  angleNow_2;              // 舵机2目前角度(未用到)
    float  angleNow_3;              // 舵机3目前角度(未用到)
    float  angleNow_4;              // 舵机4目前角度(未用到)
}xServo_TypeDef;
extern xServo_TypeDef  xServo;    // 声明为全局变量,方便记录角度
    






/*****************************************************************************
 ** 声明全局函数
****************************************************************************/
// 舵机1#函数
void AnalogServo_1_Init(void);                                   // 舵机1#初始化: GPIO、TIM、上电角度;
void AnalogServo_1_Angle(float angle);                           // 舵机1#配置角度;【可选参数】 [angle]0.0°~180.0°;
// 舵机2#函数,未用到
void AnalogServo_2_Init(void);                                   // 舵机2#初始化: GPIO、TIM、上电角度;
void AnalogServo_2_Angle(float angle);                           // 舵机2#配置角度;【可选参数】 [angle]0.0°~180.0°;
// 完整通用的TIM_PWM初始化函数, 上面的器件函数通过调用下方函数实现)
void TIM_PwmInit(GPIO_TypeDef* GPIOx, u16 PINx, TIM_TypeDef* TIMx, u8 CHx, u16 PSC, u16 ARR, u16 CCR);
void TIM_SetCCR(TIM_TypeDef* TIMx, u8 CHx, u16 CCR);             // 控制TIMx的CCRx的值;【可选参数】 [TIMx]TIM1~8, [CHx]1~4, [CCR]0~65535;


#endif

