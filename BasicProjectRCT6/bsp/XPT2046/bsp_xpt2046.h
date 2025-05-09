#ifndef  __BSP_XPT2046_H
#define  __BSP_XPT2046_H


#include "stm32f10x.h"
#include "bsp_lcd_2.8_ili93xx.h"
#include "stdio.h"



/*****************************************************************************
 ** 引脚宠定义
 ** 备注：使用的是模拟SPI, INT引脚使用状态轮询, 不使用中断;
****************************************************************************/
#define    XPT2046_IRQ_GPIO                 GPIOC        // 触摸信号指示引脚(不使用中断) 
#define    XPT2046_IRQ_PIN                  GPIO_Pin_1

#define    XPT2046_CS_GPIO                  GPIOC        // 模拟SPI_CS
#define    XPT2046_CS_PIN                   GPIO_Pin_4  

#define    XPT2046_CLK_GPIO                 GPIOC        // 模拟SPI_CLK
#define    XPT2046_CLK_PIN                  GPIO_Pin_0   

#define    XPT2046_MOSI_GPIO                GPIOC        // 模拟SPI_MOSI
#define    XPT2046_MOSI_PIN                 GPIO_Pin_3   

#define    XPT2046_MISO_GPIO                GPIOC        // 模拟SPI_MISO
#define    XPT2046_MISO_PIN                 GPIO_Pin_2   




/*****************************************************************************
 ** 触摸屏参数定义
 *****************************************************************************/  



                                            
/*****************************************************************************
 ** 声明数据类型
 *****************************************************************************/                     
typedef    struct        // 触摸屏坐标结构体          
{          
    uint8_t EN;          // 触摸检测开关, 0_关闭, 1_打开; 关闭和打开, 是指代码上的检测与否, 不用时可关闭, 减少芯片资源消耗
       
    uint16_t lcdX;       // 当前按下的LCD坐标值(
    uint16_t lcdY;                 
                
    uint16_t adcX;       // 保存最后读取的触摸屏X方向的ADC值，已用平均值滤波
    uint16_t adcY;                     
                
    uint16_t lcdWidth;   // 用于记录LCD实际的宽度像素; 在XPT2046_Init裡賦值         
    uint16_t lcdHeight;  // 用于记录LCD实际的高度像素; 在XPT2046_Init裡賦值            
                
    float xfac;          // 触摸屏与LCD的坐标比例系数,  xfac=(float)(20-320)/(t1x-t2x);
    float yfac; 
    short xoff;          // 像素点偏移值, xoff=(320-xfac*(t1x+t2x))/2;
    short yoff;                 
                
    uint8_t  dir;        // 显示方向, 0-竖屏, 1_横屏       
    uint32_t dataAddr;   // 触摸数据在内部FLASH中的存放地址                
}xXPT2046_TypeDey;  


/*****************************************************************************
 ** 声明全局变量
 *****************************************************************************/
extern xXPT2046_TypeDey xXPT2046;





/*****************************************************************************
 ** 声明全局函数声明
 ** 函数使用说明：
 **     触摸屏出货前, 已校准过屏幕坐标;
 **     调用XPT2046_Init()初始化后, 
 **     通过XPT2046_Cmd()、XPT2046_TouchDown()、XPT2046_TouchDown()这三个函数的组合,
 **     可实现大部份的运作实现;                                     
 *****************************************************************************/
void     XPT2046_Init (uint16_t lcdWidth, uint16_t lcdHeight, uint8_t dir ); // 初始化(引脚, 校准检测);  参数: 显示屏的显示方向, 取值范围: 1,2,3,4
uint8_t  XPT2046_ReCalibration(void);                                        // 触摸坐标重新校准
void     XPT2046_Cmd(uint8_t status);                                        // 触摸开关
void     XPT2046_GetXY(uint16_t* X, uint16_t* Y);                            // 获取触摸的坐标值, 存入变量X和Y中; 这个不是必要的, 通过XPT2046_Cmd()、XPT2046_TouchDown()、XPT2046_TouchDown()                            
void     XPT2046_TouchHandler(void);                                         // 触摸中断处理函数        
void     XPT2046_TouchDown(void);                                            // 空白函数, 调用按下时的处理
void     XPT2046_TouchUp(void);                                              // 空白函数, 调用释放时的处理   




#endif 

