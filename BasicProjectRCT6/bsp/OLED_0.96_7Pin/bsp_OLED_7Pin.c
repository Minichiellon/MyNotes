#include "bsp_OLED_7Pin.h"
#include "stdlib.h"
#include "oledfont.h"
#include "stdio.h"
#include "string.h"





xOLED_TypeDef xLCD;	               // ����LCD��Ҫ����

static uint8_t OLED_GRAM[128][8];  // OLED���Դ�
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
 ** OLED�˿ں궨��
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
    for (uint32_t i = 0; i < ms; i++); // 72MHzϵͳʱ���£����ٸ���ѭ��Լ��ʱ1ms
}



// д����
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



// д����
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
* ��  ���� OLED_On
* ��  �ܣ� �������л���
* ��  ��: 
* ����ֵ�� ��
*****************************************************************************/
void OLED_On(void)
{
    writeCmd(0X8D); // ���õ�ɱ�
    writeCmd(0X14); // ������ɱ�
    writeCmd(0XAF); // OLED����
}



/*****************************************************************************
* ��  ���� OLED_Off
* ��  �ܣ� ����ģʽ��,OLED���Ĳ���10uA
* ��  ��: 
* ����ֵ�� ��
*****************************************************************************/
void OLED_Off(void)
{
    writeCmd(0X8D); // ���õ�ɱ�
    writeCmd(0X10); // �رյ�ɱ�
    writeCmd(0XAE); // OLED����
}



/*****************************************************************************
* ��  ���� OLED_Clear
* ��  �ܣ� ��������, �����������Ļ��ɫ����û����һ��
* ��  ��: 
* ����ֵ�� ��
* ��  ע�� 
*****************************************************************************/
void OLED_Clear(void)
{  
    memset(OLED_GRAM, 0, 8 * 128); // ���������������
    OLED_Refresh();                // ������ʾ
}



/******************************************************************
 * �������� OLED_DrawPoint
 * ��  �ܣ� ���㺯��
 * ��  ���� x:0~127
 *          y:0~63��
 *          t:1 ��� 0,���
 * ��  �أ� ��
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
 * �������� OLED_Refresh
 * ��  �ܣ� �����Դ浽OLED
 * ��  ���� ��
 * ��  �أ� ��
 *****************************************************************/
void OLED_Refresh(void)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        writeCmd(0xb0 + i); // ��������ʼ��ַ
        writeCmd(0x00);     // ���õ�����ʼ��ַ
        writeCmd(0x10);     // ���ø�����ʼ��ַ
        writeData(0x78);
        writeData(0x40);
        for (uint8_t n = 0; n < 128; n++)
             writeData(OLED_GRAM[n][i]); 
    }
}



/******************************************************************
 * �������� OLED_Init
 * ��  �ܣ� ��ʼ��
 * ��  ���� ��
 * ��  �أ� ��
 *****************************************************************/
void OLED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    // Ϊ�˷�����ֲ����ʾ��ʹ����������
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);	 //ʹ�ܶ˿�ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG , ENABLE);	                     //ʹ�ܶ˿�ʱ��
    
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

    writeCmd(0xAE);   // OLED����
    writeCmd(0x02);   // ���õ��е�ַ
    writeCmd(0x10);   // ���ø��е�ַ
    writeCmd(0x40);   // ������ʼ��ַ��
    //writeCmd(0xB0); // set page address
    writeCmd(0x81);   // ���öԱȶ�
    writeCmd(0xCF);   // Set SEG Output Current Brightness
    writeCmd(0xA1);   // 0xa0���ҷ��� 0xa1����
    writeCmd(0xC8);   // 0xc0���·��� 0xc8����
    writeCmd(0xA6);   // set normal display
    writeCmd(0xA8);   // ���ö�·����(1 to 64)
    writeCmd(0x3f);   // 1/64 duty
    writeCmd(0x81);   // �Աȶ�����
    writeCmd(0xFF);   // 1~255;Ĭ��0X7F (��������,Խ��Խ��)
    writeCmd(0xD3);   // ������ʾ��ƫ��ӳ���ڴ������ (0x00~0x3F)
    writeCmd(0x00);   // ��ƫ��
    writeCmd(0xd5);   // ������ʾʱ�ӷ�Ƶ��/����Ƶ��
    writeCmd(0x80);   // ���÷�Ƶ������ʱ������Ϊ100֡/��
    writeCmd(0xD9);   // Ԥ���ʱ��
    writeCmd(0xF1);   // Ԥ���Ϊ15�����壬�ͷ�Ϊ1������
    writeCmd(0xDA);   // ��������Ӳ������
    writeCmd(0x12);
    writeCmd(0xDB);   // ����VCOM��ƽ
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
 * �������� drawAscii
 * ��  �ܣ� ��ָ��λ����ʾһ���ַ�
 * ��  ���� uint16_t x,y     ��ʼ����
 *          uint8_t  num     Ҫ��ʾ���ַ�:" "--->"~"
 *          uint8_t  size    �����С 12/16/24/32
 *****************************************************************/
static void drawAscii(uint16_t x, uint16_t y, uint8_t num, uint8_t size)
{
    static uint8_t temp;
    static uint8_t csize;
    static uint16_t y0;

    y0 = y;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);   // �õ�����һ���ַ���Ӧ������ռ���ֽ���
    num = num - ' ';                                          // �õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩
    for (uint8_t t = 0; t < csize; t++)
    {
        if (size == 12)         temp = aFontASCII12[num][t];  // ����1206����
        else if (size == 16)    temp = aFontASCII16[num][t];  // ����1608����
        else if (size == 24)    temp = aFontASCII24[num][t];  // ����2412����
        else if (size == 32)    temp = aFontASCII32[num][t];  // ����3216����
        else return;                                          // û�е��ֿ�

        for (uint8_t t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80) OLED_DrawPoint(x, y,  1);        // ���� ����
            else             OLED_DrawPoint(x, y,  0);        // ���� ����

            temp <<= 1;
            y++;
            if (y >= xLCD.height)    return;                  // ������Ļ�߶�(��)
            if ((y - y0) == size)
            {
                y = y0;
                x++;
                if (x >= xLCD.width) return;                  // ������Ļ���(��)
                break;
            }
        }
    }
}



/******************************************************************
 * �������� drawGBK
 * ��  �ܣ� ��ָ��λ����ʾһ���ַ�
 * ��  ���� uint16_t x,y     ��ʼ����
 *          uint8_t  num     Ҫ��ʾ���ַ�:" "--->"~"
 *          uint8_t  size    �����С 12/16/24/32
 *****************************************************************/
static void drawGBK(uint16_t x, uint16_t y, uint8_t *font, uint8_t size)
{
    static uint8_t temp;
    static uint16_t y0;
    static uint8_t GBK[128];
    static uint8_t csize;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size);   // �õ�����һ���ַ���Ӧ������ռ���ֽ���
    W25Q128_ReadGBK(font, size, GBK);                     // �õ���Ӧ��С�ĵ�������

    y0 = y;
    for (uint8_t t = 0; t < csize; t++)
    {
        temp = GBK[t];                                    // �õ�GBK��������
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
 * ��  ���� OLED_String
 * ��  �ܣ� ��LCD����ʾ�ַ���(֧��Ӣ�ġ�����)
 * ��  ���� Ӣ�ģ���ģ���ݱ�����font.h�������ʹ���һ�𱣴���оƬ�ڲ�Flash
 *          ���֣���ģ�������ⲿFlash�У��������ֿ���W25Q128��
 *                ħŮ��������W25Q128����¼����4���ֺŴ�С��ģ����
 * ��  ���� uint16_t   x      �������Ͻ�X����
 *          uint16_t   y      �������Ͻ�y����
 *          char*      pFont  Ҫ��ʾ���ַ�������
 *          uint8_t    size   �ֺŴ�С��12 16 24 32
 * ����ֵ:  ��
 ******************************************************************************/
void OLED_String(uint16_t x, uint16_t y, char *pFont, uint8_t size)
{
    if (xLCD .FlagInit == 0) return;

    uint16_t xStart = x;

    if (size != 12 && size != 16 && size != 24 && size != 32) // �����С����
        size = 24;

    while (*pFont != 0)                            // ������ȡ�ַ������ݣ�ֱ��'\0'ʱֹͣ
    {
        if (x > (xLCD.width - size))               // ��λ���жϣ����������ĩ���Ͱѹ�껻��
        {
            x = xStart;
            y = y + size;
        }
        if (y > (xLCD.height - size))              // ��λ���жϣ����������ĩ���ͷ��أ��������
            return;

        if (*pFont < 127)                          // ASCII�ַ�
        {
            drawAscii(x, y, *pFont, size);
            pFont++;
            x += size / 2;
        }
        else                                       // ������ʾ
        {
            // ��Ҫ: ����õĲ���ħŮ��������ֿ�, ��Ҫ�޸Ļ�ע��������һ��, �����Ͳ�Ӱ��ASCIIӢ���ַ������
            drawGBK(x, y, (uint8_t *)pFont, size);
            pFont = pFont + 2;                     // ��һ��Ҫ��ʾ���������ڴ��е�λ��
            x = x + size;                          // ��һ��Ҫ��ʾ����������Ļ�ϵ�Xλ��
        }
    }
    OLED_Refresh();                                // �����Դ浽OLED
}



/******************************************************************
 * �������� OLED_ShowChinese
 * ��  �ܣ� ��ʾ����ȡģ�ĺ���,
 *          �ֿ�������font�ļ��У�ֻ�ʺ��������̶ֹ����
 *          PCtoLCD2018ȡģ������+����ʽ+����+C51��ʽ
 * ��  ���� uint16_t  x         ����x
 *          uint16_t  y         ����y
 *          uint8_t   index     ��ģ�����������е����
 * ��  ��:  ��
 *****************************************************************/
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t num, uint8_t size1)
{
    uint8_t m, temp;
    uint8_t x0 = x, y0 = y;
    uint16_t size3 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * size1; // �õ�����һ���ַ���Ӧ������ռ���ֽ���

    for (uint16_t i = 0; i < size3; i++)
    {
        if (size1 == 12)
        {
            temp = aFontChinese12[num][i];   // ����12*12����
        }
        else if (size1 == 16)
        {
            temp = aFontChinese16[num][i];   // ����16*16����
        }
        else if (size1 == 24)
        {
            temp = aFontChinese24[num][i];   // ����24*24����
        }
        else if (size1 == 32)
        {
            temp = aFontChinese32[num][i];   // ����32*32����
        }
        else
        {
            temp = aFontChinese12[num][i];   // ����ǷǷ����Σ������12*12����
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
    OLED_Refresh();                          // �����Դ浽OLED
}


/******************************************************************
 * �������� OLED_Line
 * ��  �ܣ� ��ֱ��
 * ��  ���� uint16_t x1     ���X����
 *          uint16_t y1     ���Y����
 *          uint16_t x2     �յ�X����
 *          uint16_t y2     �յ�Y����
 *          uint16_t color  ��ɫֵ
 * ��  ע��
 *****************************************************************/
void OLED_Line(uint16_t  x1, uint16_t  y1, uint16_t  x2, uint16_t  y2)
{
    uint16_t  t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1;                        // ������������
    delta_y = y2 - y1;                        
    uRow = x1;                                
    uCol = y1;                                
    if (delta_x > 0)incx = 1;                 // ���õ�������
    else if (delta_x == 0)incx = 0;           // ��ֱ��
    else                                      
    {                                         
        incx = -1;                            
        delta_x = -delta_x;                   
    }                                         
    if (delta_y > 0)incy = 1;                 
    else if (delta_y == 0)incy = 0;           // ˮƽ��
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    if (delta_x > delta_y)distance = delta_x; // ѡȡ��������������
    else distance = delta_y;
    for (t = 0; t <= distance + 1; t++)       // �������
    {
        OLED_DrawPoint(uRow, uCol, 1);             // ����
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
    OLED_Refresh();  // �����Դ浽OLED
}



/******************************************************************
 * �������� OLED_Circle
 * ��  �ܣ� ��ָ��λ�û�Բ
 * ��  ���� uint16_t Xpos     X����
 *          uint16_t Ypos     ���Y����
 *          uint16_t Radius   �뾶
 * ��  ע��
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
    OLED_Refresh();  // �����Դ浽OLED
}

