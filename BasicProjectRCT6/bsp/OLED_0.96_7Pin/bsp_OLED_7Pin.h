#ifndef __OLED_H
#define __OLED_H
/***********************************************************************************************************************************
 ** �������д��  ħŮ�������Ŷ�
 ** ��������¡�
 ** ����    ����  ħŮ������      https://demoboard.taobao.com
 ***********************************************************************************************************************************
 ** ���ļ����ơ�  bsp_OLED.h
 **
 ** ������ƽ̨��  STM32F407 + KEIL5.31 + 0.96��7��OLED
 **
 ** ����ֲ˵����  1- ���Ʊ����̵�OLED�ļ��У���Ŀ�깤���ļ�����;
 **               2- ��c�ļ���ӵ����̹�����;
 **               3- ���h�ļ����·��;
 **               4- ����Ҫ���ڹ��ܵ��ļ��У�#include "bsp_OLED.h����
 **
 ** ��Ӳ���ص㡿  ����ʹ��3.3V;  ������Ҫ��5V, ���ܲ���Ī������
 **
 ** ������ص㡿  OLED���Դ�, ��Ÿ�ʽ����.
 **               [0]0 1 2 3 ... 127
 **               [1]0 1 2 3 ... 127
 **               [2]0 1 2 3 ... 127
 **               [3]0 1 2 3 ... 127
 **               [4]0 1 2 3 ... 127
 **               [5]0 1 2 3 ... 127
 **               [6]0 1 2 3 ... 127
 **               [7]0 1 2 3 ... 127
 **
 ** �����¼�¼��  
 **               2023-01-10   ������F407
 **
 ** ����ע˵����  ��Ȩ��ħŮ�Ƽ����У�����ѧϰ���ԣ��������ã�лл��
 **               https://demoboard.taobao.com
************************************************************************************************************************************/
#include "stdlib.h"
#include <stm32f10x.h>
#include "stdio.h"
#include "bsp_W25Q128.h"    // Flash��ģ





/*****************************************************************************
 ** ��ֲ
 ** 
****************************************************************************/
#define OLED_D0_GPIO    GPIOA             // SCL
#define OLED_D0_PIN     GPIO_Pin_11

#define OLED_D1_GPIO    GPIOA             // SDA
#define OLED_D1_PIN     GPIO_Pin_9

#define OLED_RES_GPIO   GPIOA             // RST��RES
#define OLED_RES_PIN    GPIO_Pin_8

#define OLED_DC_GPIO    GPIOC             // DC 
#define OLED_DC_PIN     GPIO_Pin_9

#define OLED_CS_GPIO    GPIOC             // 6���������ýӴ�����
#define OLED_CS_PIN     GPIO_Pin_7



/*****************************************************************************
 ** ȫ�ֱ���
****************************************************************************/
typedef struct                  // OLED ��Ҫ������
{										    
	uint16_t width;			    // OLED ���
	uint16_t height;		    // OLED �߶�
    uint8_t  FlagInit;          // ��ʼ����ɱ�־
}xOLED_TypeDef; 	  
extern xOLED_TypeDef xLCD;	    // ����OLED��Ҫ����



/*****************************************************************************
 ** ����ȫ�ֺ���
****************************************************************************/
void OLED_Init(void);                                                   // ��ʼ��
void OLED_Refresh(void);                                                // �����Դ浽OLED
void OLED_DisplayTurn(uint8_t i);                                       // ��ʾ����ˮƽ��ת
void OLED_Display(uint8_t sw);                                          // ��ʾ��Դ����
void OLED_Clear(void);                                                  // ����
void OLED_Line(uint16_t  x1, uint16_t  y1, uint16_t  x2, uint16_t  y2); // ����
void OLED_Circle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);        // ��Բ
void OLED_String(uint16_t x, uint16_t y, char *pFont, uint8_t size);    // ��ʾ��Ӣ���ַ���
void OLED_ShowChinese(uint8_t x,uint8_t y,uint8_t num,uint8_t size1);   // ��ʾ����(��ģ������font.h)

#endif


