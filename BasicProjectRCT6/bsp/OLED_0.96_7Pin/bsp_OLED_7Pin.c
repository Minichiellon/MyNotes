#include "bsp_OLED_7Pin.h"
#include "stdlib.h"
#include "oledfont.h"
#include "stdio.h"
#include "string.h"





xOLED_TypeDef xLCD;	               // 管理LCD重要参数

static uint8_t OLED_GRAM[128][8];  // OLED的显存
//  [0]0 1 2 3 ... 127
//  [1]0 1 2 3 ... 127
//  [2]0 1 2 3 ... 127
//  [3]0 1 2 3 ... 127
//  [4]0 1 2 3 ... 127
//  [5]0 1 2 3 ... 127
//  [6]0 1 2 3 ... 127
//  [7]0 1 2 3 ... 127

#define SIZE 16
#define MAX_COLUMN	128
#define MAX_ROW		64



/*****************************************************************************
 ** OLED端口宏定义
*****************************************************************************/
#define  D0_0       OLED_D0_GPIO->BRR   = OLED_D0_PIN
#define  D0_1       OLED_D0_GPIO->BSRR  = OLED_D0_PIN

#define  D1_0       OLED_D1_GPIO->BRR   = OLED_D1_PIN
#define  D1_1       OLED_D1_GPIO->BSRR  = OLED_D1_PIN

#define  RES_0      OLED_RES_GPIO->BRR  = OLED_RES_PIN
#define  RES_1      OLED_RES_GPIO->BSRR = OLED_RES_PIN

#define  DC_0       OLED_DC_GPIO ->BRR  = OLED_DC_PIN
#define  DC_1       OLED_DC_GPIO ->BSRR = OLED_DC_PIN

#define  CS_0       OLED_CS_GPIO ->BRR  = OLED_CS_PIN
#define  CS_1       OLED_CS_GPIO ->BSRR = OLED_CS_PIN





static void delay_ms(uint32_t ms)
{
    ms = ms * 6500;
    for (uint32_t i = 0; i < ms; i++); // 72MHz系统时钟下，多少个空循环约耗时1ms
}



// 写命令
static void writeCmd(uint8_t data)
{
    DC_0;
    CS_0;    
    for (uint8_t i = 0; i < 8; i++)
    {
        D0_0;
        if (data & 0x80)
            D1_1;
        else
            D1_0;
        D0_1;
        data <<= 1;
    }    
    CS_1;
    DC_1;
}



// 写数据
static void writeData(uint8_t data)
{
    DC_1;    
    CS_0;    
    for (uint8_t i = 0; i < 8; i++)
    {
        D0_0;
        if (data & 0x80)
            D1_1;
        else
            D1_0;
        D0_1;
        data <<= 1;
    }
    CS_1;
    DC_1;
}



/*****************************************************************************
* 函  数： OLED_On
* 功  能： 从休眠中唤醒
* 参  数: 
* 返回值： 无
*****************************************************************************/
void OLED_On(void)
{
    writeCmd(0X8D); // 设置电荷泵
    writeCmd(0X14); // 开启电荷泵
    writeCmd(0XAF); // OLED唤醒
}



/*****************************************************************************
* 函  数： OLED_Off
* 功  能： 休眠模式下,OLED功耗不到10uA
* 参  数: 
* 返回值： 无
*****************************************************************************/
void OLED_Off(void)
{
    writeCmd(0X8D); // 设置电荷泵
    writeCmd(0X10); // 关闭电荷泵
    writeCmd(0XAE); // OLED休眠
}



/*****************************************************************************
* 函  数： OLED_Clear
* 功  能： 清屏函数, 即填充整个屏幕黑色，和没点亮一样
* 参  数: 
* 返回值： 无
* 备  注： 
*****************************************************************************/
void OLED_Clear(void)
{  
    memset(OLED_GRAM, 0, 8 * 128); // 清除缓存所有数据
    OLED_Refresh();                // 更新显示
}



/******************************************************************
 * 函数名： OLED_DrawPoint
 * 功  能： 画点函数
 * 参  数： x:0~127
 *          y:0~63，
 *          t:1 填充 0,清空
 * 返  回： 无
 *****************************************************************/
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t)
{
    uint8_t i, m, n;
    i = y / 8;
    m = y % 8;
    n = 1 << m;
    if (t)
    {
        OLED_GRAM[x][i] |= n;
    }
    else
    {
        OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
        OLED_GRAM[x][i] |= n;
        OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
    }
}



/******************************************************************
 * 函数名： OLED_Refresh
 * 功  能： 更新显存到OLED
 * 参  数： 无
 * 返  回： 无
 *****************************************************************/
void OLED_Refresh(void)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        writeCmd(0xb0 + i); // 设置行起始地址
        writeCmd(0x00);     // 设置低列起始地址
        writeCmd(0x10);     // 设置高列起始地址
        writeData(0x78);
        writeData(0x40);
        for (uint8_t n = 0; n < 128; n++)
             writeData(OLED_GRAM[n][i]); 
    }
}



/******************************************************************
 * 函数名： OLED_Init
 * 功  能： 初始化
 * 参  数： 无
 * 返  回： 无
 *****************************************************************/
void OLED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    // 为了方便移植，本示例使能所有引脚
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);	 //使能端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG , ENABLE);	                     //使能端口时钟
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_InitStructure.GPIO_Pin = OLED_D0_PIN;	         // D0
    GPIO_Init(OLED_D0_GPIO, &GPIO_InitStructure);	
    
    GPIO_InitStructure.GPIO_Pin = OLED_D1_PIN;           // D1
    GPIO_Init(OLED_D1_GPIO, &GPIO_InitStructure);	
    
    GPIO_InitStructure.GPIO_Pin = OLED_RES_PIN;          // RES
    GPIO_Init(OLED_RES_GPIO, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = OLED_DC_PIN;           // DC
    GPIO_Init(OLED_DC_GPIO, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = OLED_CS_PIN;           // CS
    GPIO_Init(OLED_CS_GPIO, &GPIO_InitStructure);
    CS_1;

    RES_1;
    delay_ms(100);
    RES_0;
    delay_ms(100);
    RES_1;

    writeCmd(0xAE);   // OLED休眠
    writeCmd(0x02);   // 设置低列地址
    writeCmd(0x10);   // 设置高列地址
    writeCmd(0x40);   // 设置起始地址线
    //writeCmd(0xB0); // set page address
    writeCmd(0x81);   // 设置对比度
    writeCmd(0xCF);   // Set SEG Output Current Brightness
    writeCmd(0xA1);   // 0xa0左右反置 0xa1正常
    writeCmd(0xC8);   // 0xc0上下反置 0xc8正常
    writeCmd(0xA6);   // set normal display
    writeCmd(0xA8);   // 设置多路复用(1 to 64)
    writeCmd(0x3f);   // 1/64 duty
    writeCmd(0x81);   // 对比度设置
    writeCmd(0xFF);   // 1~255;默认0X7F (亮度设置,越大越亮)
    writeCmd(0xD3);   // 设置显示的偏移映射内存计数器 (0x00~0x3F)
    writeCmd(0x00);   // 不偏移
    writeCmd(0xd5);   // 设置显示时钟分频比/振荡器频率
    writeCmd(0x80);   // 设置分频比例，时钟设置为100帧/秒
    writeCmd(0xD9);   // 预充电时间
    writeCmd(0xF1);   // 预充电为15个脉冲，释放为1个脉冲
    writeCmd(0xDA);   // 引脚设置硬件配置
    writeCmd(0x12);
    writeCmd(0xDB);   // 设置VCOM电平
    writeCmd(0x40);   // Set VCOM Deselect Level
    writeCmd(0x20);   // -Set Page Addressing Mode (0x00/0x01/0x02)
    writeCmd(0x02);   //
   
    OLED_On();
    OLED_Clear();
    
    xLCD.FlagInit = 1;
    xLCD.width = 128;
    xLCD.height = 64;
}



/******************************************************************
 * 函数名： drawAscii
 * 功  能： 在指定位置显示一个字符
 * 参  数： uint16_t x,y     起始坐标
 *          uint8_t  num     要显示的字符:" "--->"~"
 *          uint8_t  size    字体大小 12/16/24/32
 *****************************************************************/
static void drawAscii(uint16_t x, uint16_t y, uint8_t num, uint8_t size)
{
    static uint8_t temp;
    static uint8_t csize;
    static uint16_t y0;

    y0 = y;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);   // 得到字体一个字符对应点阵集所占的字节数
    num = num - ' ';                                          // 得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
    for (uint8_t t = 0; t < csize; t++)
    {
        if (size == 12)         temp = aFontASCII12[num][t];  // 调用1206字体
        else if (size == 16)    temp = aFontASCII16[num][t];  // 调用1608字体
        else if (size == 24)    temp = aFontASCII24[num][t];  // 调用2412字体
        else if (size == 32)    temp = aFontASCII32[num][t];  // 调用3216字体
        else return;                                          // 没有的字库

        for (uint8_t t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80) OLED_DrawPoint(x, y,  1);        // 字体 画点
            else             OLED_DrawPoint(x, y,  0);        // 背景 画点

            temp <<= 1;
            y++;
            if (y >= xLCD.height)    return;                  // 超出屏幕高度(底)
            if ((y - y0) == size)
            {
                y = y0;
                x++;
                if (x >= xLCD.width) return;                  // 超出屏幕宽度(宽)
                break;
            }
        }
    }
}



/******************************************************************
 * 函数名： drawGBK
 * 功  能： 在指定位置显示一个字符
 * 参  数： uint16_t x,y     起始坐标
 *          uint8_t  num     要显示的字符:" "--->"~"
 *          uint8_t  size    字体大小 12/16/24/32
 *****************************************************************/
static void drawGBK(uint16_t x, uint16_t y, uint8_t *font, uint8_t size)
{
    static uint8_t temp;
    static uint16_t y0;
    static uint8_t GBK[128];
    static uint8_t csize;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size);   // 得到字体一个字符对应点阵集所占的字节数
    W25Q128_ReadGBK(font, size, GBK);                     // 得到相应大小的点阵数据

    y0 = y;
    for (uint8_t t = 0; t < csize; t++)
    {
        temp = GBK[t];                                    // 得到GBK点阵数据
        for (uint8_t t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)   OLED_DrawPoint(x, y, 1);
            else               OLED_DrawPoint(x, y, 0);
            temp <<= 1;
            y++;
            if ((y - y0) == size)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}



/******************************************************************************
 * 函  数： OLED_String
 * 功  能： 在LCD上显示字符串(支持英文、汉字)
 * 描  述： 英文：字模数据保存在font.h，编译后和代码一起保存在芯片内部Flash
 *          汉字：字模保存在外部Flash中，本函数字库在W25Q128中
 *                魔女开发板中W25Q128已烧录宋体4种字号大小字模数据
 * 参  数： uint16_t   x      字体左上角X坐标
 *          uint16_t   y      字体左上角y坐标
 *          char*      pFont  要显示的字符串数据
 *          uint8_t    size   字号大小：12 16 24 32
 * 返回值:  无
 ******************************************************************************/
void OLED_String(uint16_t x, uint16_t y, char *pFont, uint8_t size)
{
    if (xLCD .FlagInit == 0) return;

    uint16_t xStart = x;

    if (size != 12 && size != 16 && size != 24 && size != 32) // 字体大小控制
        size = 24;

    while (*pFont != 0)                            // 连续读取字符串数据，直到'\0'时停止
    {
        if (x > (xLCD.width - size))               // 行位置判断，如果到了行末，就把光标换行
        {
            x = xStart;
            y = y + size;
        }
        if (y > (xLCD.height - size))              // 列位置判断，如果到了列末，就返回，不再输出
            return;

        if (*pFont < 127)                          // ASCII字符
        {
            drawAscii(x, y, *pFont, size);
            pFont++;
            x += size / 2;
        }
        else                                       // 汉字显示
        {
            // 重要: 如果用的不是魔女开发板的字库, 就要修改或注释下面这一行, 这样就不影响ASCII英文字符的输出
            drawGBK(x, y, (uint8_t *)pFont, size);
            pFont = pFont + 2;                     // 下一个要显示的数据在内存中的位置
            x = x + size;                          // 下一个要显示的数据在屏幕上的X位置
        }
    }
    OLED_Refresh();                                // 更新显存到OLED
}



/******************************************************************
 * 函数名： OLED_ShowChinese
 * 功  能： 显示自行取模的汉字,
 *          字库数据在font文件中，只适合少量汉字固定输出
 *          PCtoLCD2018取模：阴码+列行式+逆向+C51格式
 * 参  数： uint16_t  x         坐标x
 *          uint16_t  y         坐标y
 *          uint8_t   index     字模数据在数组中的序号
 * 返  回:  无
 *****************************************************************/
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t num, uint8_t size1)
{
    uint8_t m, temp;
    uint8_t x0 = x, y0 = y;
    uint16_t size3 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * size1; // 得到字体一个字符对应点阵集所占的字节数

    for (uint16_t i = 0; i < size3; i++)
    {
        if (size1 == 12)
        {
            temp = aFontChinese12[num][i];   // 调用12*12字体
        }
        else if (size1 == 16)
        {
            temp = aFontChinese16[num][i];   // 调用16*16字体
        }
        else if (size1 == 24)
        {
            temp = aFontChinese24[num][i];   // 调用24*24字体
        }
        else if (size1 == 32)
        {
            temp = aFontChinese32[num][i];   // 调用32*32字体
        }
        else
        {
            temp = aFontChinese12[num][i];   // 如果是非法字形，则调用12*12字体
        }
        for (m = 0; m < 8; m++)
        {
            if (temp & 0x01)
                OLED_DrawPoint(x, y, 1);
            else
                OLED_DrawPoint(x, y, 0);
            temp >>= 1;
            y++;
        }
        x++;
        if ((x - x0) == size1)
        {
            x = x0;
            y0 = y0 + 8;
        }
        y = y0;
    }
    OLED_Refresh();                          // 更新显存到OLED
}


/******************************************************************
 * 函数名： OLED_Line
 * 功  能： 画直线
 * 参  数： uint16_t x1     起点X坐标
 *          uint16_t y1     起点Y坐标
 *          uint16_t x2     终点X坐标
 *          uint16_t y2     终点Y坐标
 *          uint16_t color  颜色值
 * 备  注：
 *****************************************************************/
void OLED_Line(uint16_t  x1, uint16_t  y1, uint16_t  x2, uint16_t  y2)
{
    uint16_t  t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1;                        // 计算坐标增量
    delta_y = y2 - y1;                        
    uRow = x1;                                
    uCol = y1;                                
    if (delta_x > 0)incx = 1;                 // 设置单步方向
    else if (delta_x == 0)incx = 0;           // 垂直线
    else                                      
    {                                         
        incx = -1;                            
        delta_x = -delta_x;                   
    }                                         
    if (delta_y > 0)incy = 1;                 
    else if (delta_y == 0)incy = 0;           // 水平线
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    if (delta_x > delta_y)distance = delta_x; // 选取基本增量坐标轴
    else distance = delta_y;
    for (t = 0; t <= distance + 1; t++)       // 画线输出
    {
        OLED_DrawPoint(uRow, uCol, 1);             // 画点
        xerr += delta_x ;
        yerr += delta_y ;
        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
    OLED_Refresh();  // 更新显存到OLED
}



/******************************************************************
 * 函数名： OLED_Circle
 * 功  能： 在指定位置画圆
 * 参  数： uint16_t Xpos     X坐标
 *          uint16_t Ypos     起点Y坐标
 *          uint16_t Radius   半径
 * 备  注：
 *****************************************************************/
void OLED_Circle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
    int16_t mx = Xpos, my = Ypos, x = 0, y = Radius;
    int16_t d = 1 - Radius;
    while (y > x)
    {
        OLED_DrawPoint(x + mx, y + my, 1);
        OLED_DrawPoint(-x + mx, y + my, 1);
        OLED_DrawPoint(-x + mx, -y + my, 1);
        OLED_DrawPoint(x + mx, -y + my, 1);
        OLED_DrawPoint(y + mx, x + my, 1);
        OLED_DrawPoint(-y + mx, x + my, 1);
        OLED_DrawPoint(y + mx, -x + my, 1);
        OLED_DrawPoint(-y + mx, -x + my, 1);
        if (d < 0)
        {
            d += 2 * x + 3;
        }
        else
        {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
    OLED_Refresh();  // 更新显存到OLED
}

