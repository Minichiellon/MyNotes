#ifndef _BSP_KEY_H
#define _BSP_KEY_H
#include <stm32f10x.h>  

typedef enum
{
    IDLE,
    DEBOUNCE_PRESS,
    PRESSED,
    DEBOUNCE_RELEASE,
    WAIT_DOUBLE
} ButtonState;

typedef enum
{
	KEY_1,
	KEY_2,
	KEY_3
}KeyIndex;

/*****************************************************************************
 ** ��ֲʱ�޸�
****************************************************************************/
// KEY_1_WKUP, ��ʱ����������ʱ���øߵ�ƽ
#define KEY_1_NUM				  1						 //����1����
#define KEY_1_PRESS				  1						 //����1���±�־
#define KEY_1_GPIO                GPIOA                  // ���ö˿�
#define KEY_1_PIN                 GPIO_Pin_0             // ���ű��
#define KEY_1_MODE                GPIO_Mode_IPD          // ���Ź���ģʽ
#define KEY_1_TRIM                EXTI_Trigger_Rising    // �½��ش���:EXTI_Trigger_Falling;   �����ش���:EXTI_Trigger_Rising
#define KEY_1_IRQN                EXTI0_IRQn             // �ж��������
#define KEY_1_IRQHANDLER          EXTI0_IRQHandler       // �жϷ���������
#define KEY_1_EXTI_LINE           EXTI_Line0             // �ж��߱��
#define KEY_1_GPIOSOURCE          GPIO_PortSourceGPIOA   // �ж��߶˿�
#define KEY_1_PINSOURCE           GPIO_PinSource0    
// KEY_2, ��ʱ����������ʱ���õ͵�ƽ
#define KEY_2_NUM				  2						 //����2����
#define KEY_2_PRESS				  2						 //����2���±�־
#define KEY_2_GPIO                GPIOA                  // ���ö˿�
#define KEY_2_PIN                 GPIO_Pin_1             // ���ű��
#define KEY_2_MODE                GPIO_Mode_IPU          // ���Ź���ģʽ
#define KEY_2_TRIM                EXTI_Trigger_Falling   // �½��ش���:EXTI_Trigger_Falling;   �����ش���:EXTI_Trigger_Rising
#define KEY_2_IRQN                EXTI1_IRQn             // �ж��������
#define KEY_2_IRQHANDLER          EXTI1_IRQHandler       // �жϷ�����
#define KEY_2_GPIOSOURCE          GPIO_PortSourceGPIOA   // �ж��߶˿�
#define KEY_2_EXTI_LINE           EXTI_Line1             // �ж��߱��
#define KEY_2_PINSOURCE           GPIO_PinSource1   
// KEY_2, ��ʱ����������ʱ���õ͵�ƽ
#define KEY_3_NUM				  3						 //����3����
#define KEY_3_PRESS				  3						 //����3���±�־
#define KEY_3_GPIO                GPIOA                  // ���ö˿�
#define KEY_3_PIN                 GPIO_Pin_4             // ���ű��
#define KEY_3_MODE                GPIO_Mode_IPU          // ���Ź���ģʽ
#define KEY_3_TRIM                EXTI_Trigger_Falling   // �½��ش���:EXTI_Trigger_Falling;   �����ش���:EXTI_Trigger_Rising
#define KEY_3_IRQN                EXTI4_IRQn             // �ж��������
#define KEY_3_IRQHANDLER          EXTI4_IRQHandler       // �жϷ���������
#define KEY_3_GPIOSOURCE          GPIO_PortSourceGPIOA   // �ж��߶˿�
#define KEY_3_EXTI_LINE           EXTI_Line4             // �ж��߱��
#define KEY_3_PINSOURCE           GPIO_PinSource4    



/*****************************************************************************
 ** ����ȫ�ֺ���
****************************************************************************/
extern void Key_Init(void);
extern uint8_t Key_GetNum(void);
extern void handleClick(void);
extern void handleDoubleClick(void);
extern void handleLongPress(void);
extern void CheckKeyEvent(KeyIndex key);
#endif

