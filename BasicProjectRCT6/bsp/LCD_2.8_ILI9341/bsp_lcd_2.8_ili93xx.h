#ifndef __BSP__LCD_ILI93XX_H
#define __BSP__LCD_ILI93XX_H    
/***********************************************************************************************************************************
 *【文件名称】  lcd_ili9341.h
 *【淘宝链接】  https://demoboard.taobao.com
 ********************************************************************************************************************************
 *【实验平台】  魔女开发板_STM32F103RC + KEIL5.27 + 2.8寸显示屏_ILI9341
 *
 *【移植说明】  1：本代码使用在F103RC上，使用IO模块FSMC通信，注意引脚的修改
 *              2：汉字的显示，使用开发板上的外部FLASH中字库
 *     
 *【更新记录】
 *              2020-09-xx  创建
 *              2020-12-12  完善修改中英文显示函数 
 *              2020-12-xx  下次完善：更方便的引脚移植修改

    
************************************************************************************************************************************/
#include <stm32f10x.h>  
#include <stdio.h>  
#include "system_f103.h"
  


////////////////////////////////////////////////////////////////////
//-----------------LCD端口定义---------------- 
#define    LCD_LED         PAout(15)                   // LCD背光           PC10 


#define    LCD_CS_HIGH     GPIOC->BSRR=0x200   // 片选端口          PC9   1<<9
#define    LCD_RS_HIGH     GPIOC->BSRR=0x100   // 数据/命令         PC8   1<<8 
#define    LCD_WR_HIGH     GPIOC->BSRR=0x80    // 写数据            PC7   1<<7
#define    LCD_RD_HIGH     GPIOC->BSRR=0x40    // 读数据            PC6   1<<6
                                    
#define    LCD_CS_LOW      GPIOC->BRR=0x200    // 片选端口          PC9   1<<9 
#define    LCD_RS_LOW      GPIOC->BRR=0x100    // 数据/命令         PC8   1<<8  
#define    LCD_WR_LOW      GPIOC->BRR=0x80     // 写数据            PC7   1<<7
#define    LCD_RD_LOW      GPIOC->BRR=0x40     // 读数据            PC6   1<<6

// 屏幕参数
#define    LCD_WIDTH       240                 // 屏幕宽度像素，注意：0~239
#define    LCD_HIGH        320                 // 屏幕高度像素，注意：0~319
#define    LCD_DIR         0                   // 四种显示方向，0-正竖屏，3-倒竖屏，5-正横屏, 6-倒横屏
//  结束 移植修改 ************************************************************



typedef struct          // 结构体：用于保存信息、状态
{     
    u8  FlagInit;       // 初始化完成标志
    u16 width;          // LCD 宽度
    u16 height;         // LCD 高度
    u8  dir;            // 横屏还是竖屏控制：0，竖屏；1，横屏
    u16 id;
}_LCD;
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
 ** 魔女开发板     资料Q群文件：262901124
****************************************************************************/
void LCD_Init (void);                                                        // 初始化
void LCD_DisplayOn(void);                                                    // 开显示
void LCD_DisplayOff(void);                                                   // 关显示
void LCD_DisplayDir(u8 dir);                                                 // 显示方向
void LCD_DrawPoint(u16 x, u16 y, u16 color);                                 // 画点
void LCD_Circle(u16 x0,u16 y0,u8 r, u16 color);                              // 画圆
void LCD_Line(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);                    // 画线
void LCD_Cross( uint16_t x, uint16_t y, uint16_t len, uint32_t fColor);      // 画十字线
void LCD_Rectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);               // 画矩形
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);                        // 填充单色
void LCD_String(u16 x, u16 y, char* pFont, u8 size, u32 fColor, u32 bColor); // 显示中英文字符串
void LCD_Image(u16 x, u16 y, u16 width, u16 height, const u8 *image) ;       // 显示图像
void LCD_GUI(void);                                                          // 绘制简单界面
         
#endif

