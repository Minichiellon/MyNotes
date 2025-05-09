#ifndef __BSP__LCD_ILI93XX_H
#define __BSP__LCD_ILI93XX_H
/**==================================================================================================================
 **【文件名称】  bsp_lcd_2.8_ili93xx.h
 **【功能测试】  显示屏
 **==================================================================================================================
 **【适用平台】  STM32F103 + KEIL5.27 + 2.8寸显示屏_ILI9341
 **
 **【实验操作】
 **
 **
 **【文件移植】
 **
 **【更新记录】  2022-05-22  修改引脚配置代码，使用标准库
 **              2020-12-12  完善修改中英文显示函数
 **              2020-09     创建
 **
 **【备注说明】  代码版权归魔女科技所有，请勿商用，谢谢！
 **              https://demoboard.taobao.com
====================================================================================================================*/
#include <stm32f10x.h>
#include <stdio.h>
#include "system_f103.h"



/*****************************************************************************
 ** 移植配置
****************************************************************************/
// BL_背光  
#define    LCD_BL_GPIO     GPIOA              
#define    LCD_BL_PIN      GPIO_Pin_15
// BL_背光  
#define    LCD_RD_GPIO     GPIOC              
#define    LCD_RD_PIN      GPIO_Pin_6
// BL_背光  
#define    LCD_WE_GPIO     GPIOC              
#define    LCD_WE_PIN      GPIO_Pin_7
// BL_背光  
#define    LCD_RS_GPIO     GPIOC             
#define    LCD_RS_PIN      GPIO_Pin_8
// BL_背光  
#define    LCD_CS_GPIO     GPIOC              
#define    LCD_CS_PIN      GPIO_Pin_9

// 屏幕参数
#define    LCD_WIDTH       240             // 屏幕宽度像素，注意：0~239
#define    LCD_HIGH        320             // 屏幕高度像素，注意：0~319
#define    LCD_DIR         0               // 四种显示方向，0-正竖屏，3-倒竖屏，5-正横屏, 6-倒横屏
//  结束 移植修改 ************************************************************



typedef struct          // 结构体：用于保存信息、状态
{
    u8  FlagInit;       // 初始化完成标志
    uint16_t width;     // LCD 宽度
    uint16_t height;    // LCD 高度
    u8  dir;            // 横屏还是竖屏控制：0，竖屏；1，横屏
    uint16_t id;
} _LCD;
extern _LCD xLCD;



/******************************* 定义 ILI934 显示屏常用颜色 ********************************/
#define      WHITE               0xFFFF       // 白色
#define      BLACK               0x0000       // 黑色 
#define      GREY                0xF7DE       // 灰色 
#define      RED                 0xF800       // 红 
#define      MAGENTA             0xF81F       // 洋红色 
#define      GRED                0xFFE0       // 深红色
#define      GREEN               0x07E0       // 绿 
#define      CYAN                0x7FFF       // 青色 
#define      YELLOW              0xFFE0       // 黄色 
#define      LIGHTGREEN          0X841F       // 浅绿色 
#define      BLUE                0x001F       // 蓝 
#define      GBLUE               0x07FF       // 浅蓝 1
#define      LIGHTBLUE           0X7D7C       // 浅蓝 2
#define      BLUE2               0x051F       // 浅蓝 3
#define      GRAYBLUE            0X5458       // 灰蓝 
#define      DARKBLUE            0X01CF       // 深蓝

#define      LGRAY               0XC618       // 浅灰色,窗体背景色
#define      LGRAYBLUE           0XA651       // 浅灰蓝色(中间层颜色)
#define      LBBLUE              0X2B12       // 浅棕蓝色(选择条目的反色)



/*****************************************************************************
 ** 声明全局函数

****************************************************************************/
void LCD_Init(void);                                                                             // 初始化
void LCD_DisplayOn(void);                                                                        // 开显示
void LCD_DisplayOff(void);                                                                       // 关显示
void LCD_DisplayDir(u8 dir);                                                                     // 显示方向
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color);                                      // 画点
void LCD_Circle(uint16_t x0, uint16_t y0, u8 r, uint16_t color);                                 // 画圆
void LCD_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);               // 画线
void LCD_Cross(uint16_t x, uint16_t y, uint16_t len, uint32_t fColor);                           // 画十字线
void LCD_Rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);          // 画矩形
void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);               // 填充单色
void LCD_String(uint16_t x, uint16_t y, char *pFont, u8 size, uint32_t fColor, uint32_t bColor); // 显示中英文字符串
void LCD_Image(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const u8 *image) ;       // 显示图像
void LCD_GUI(void);                                                                              // 绘制简单界面

#endif

