#ifndef __OLED_H
#define __OLED_H
/***********************************************************************************************************************************
 ** 【代码编写】  魔女开发板团队
 ** 【代码更新】
 ** 【淘    宝】  魔女开发板      https://demoboard.taobao.com
 ***********************************************************************************************************************************
 ** 【文件名称】  bsp_OLED.h
 **
 ** 【适用平台】  STM32F407 + KEIL5.31 + 0.96寸7针OLED
 **
 ** 【移植说明】  1- 复制本工程的OLED文件夹，到目标工程文件夹中;
 **               2- 把c文件添加到工程管理器;
 **               3- 添加h文件存放路径;
 **               4- 在需要串口功能的文件中，#include "bsp_OLED.h＂；
 **
 ** 【硬件重点】  供电使用3.3V;  尽量不要接5V, 可能产生莫名错误
 **
 ** 【软件重点】  OLED的显存, 存放格式如下.
 **               [0]0 1 2 3 ... 127
 **               [1]0 1 2 3 ... 127
 **               [2]0 1 2 3 ... 127
 **               [3]0 1 2 3 ... 127
 **               [4]0 1 2 3 ... 127
 **               [5]0 1 2 3 ... 127
 **               [6]0 1 2 3 ... 127
 **               [7]0 1 2 3 ... 127
 **
 ** 【更新记录】  
 **               2023-01-10   更新至F407
 **
 ** 【备注说明】  版权归魔女科技所有，限于学习测试，切勿商用，谢谢！
 **               https://demoboard.taobao.com
************************************************************************************************************************************/
#include "stdlib.h"
#include <stm32f10x.h>
#include "stdio.h"
#include "bsp_W25Q128.h"    // Flash字模





/*****************************************************************************
 ** 移植
 ** 
****************************************************************************/
#define OLED_D0_GPIO    GPIOA             // SCL
#define OLED_D0_PIN     GPIO_Pin_11

#define OLED_D1_GPIO    GPIOA             // SDA
#define OLED_D1_PIN     GPIO_Pin_9

#define OLED_RES_GPIO   GPIOA             // RST、RES
#define OLED_RES_PIN    GPIO_Pin_8

#define OLED_DC_GPIO    GPIOC             // DC 
#define OLED_DC_PIN     GPIO_Pin_9

#define OLED_CS_GPIO    GPIOC             // 6针屏，不用接此引脚
#define OLED_CS_PIN     GPIO_Pin_7



/*****************************************************************************
 ** 全局变量
****************************************************************************/
typedef struct                  // OLED 重要参数集
{										    
	uint16_t width;			    // OLED 宽度
	uint16_t height;		    // OLED 高度
    uint8_t  FlagInit;          // 初始化完成标志
}xOLED_TypeDef; 	  
extern xOLED_TypeDef xLCD;	    // 管理OLED重要参数



/*****************************************************************************
 ** 声明全局函数
****************************************************************************/
void OLED_Init(void);                                                   // 初始化
void OLED_Refresh(void);                                                // 更新显存到OLED
void OLED_DisplayTurn(uint8_t i);                                       // 显示方向水平反转
void OLED_Display(uint8_t sw);                                          // 显示电源开关
void OLED_Clear(void);                                                  // 清屏
void OLED_Line(uint16_t  x1, uint16_t  y1, uint16_t  x2, uint16_t  y2); // 画线
void OLED_Circle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);        // 画圆
void OLED_String(uint16_t x, uint16_t y, char *pFont, uint8_t size);    // 显示中英文字符号
void OLED_ShowChinese(uint8_t x,uint8_t y,uint8_t num,uint8_t size1);   // 显示汉字(字模数据在font.h)

#endif


