#ifndef __BSP_RELAY_H__
#define __BSP_RELAY_H__


#include <stm32f10x.h>



/*****************************************************************************
 ** 移植修改区
******************************************************************************/
// 第1路继电器
#define  RELAY_1_GPIO       GPIOC         // 引脚端口
#define  RELAY_1_PIN        GPIO_Pin_0    // 引脚编号
#define  RELAY_1_TRIGGER    1             // 继电器触发电平，1—高电平触发，0-低电平触发
// 第2路继电器，倘若不用，闲置即可
#define  RELAY_2_GPIO       GPIOC         
#define  RELAY_2_PIN        GPIO_Pin_5  
#define  RELAY_2_TRIGGER    1             
// 第3路继电器，倘若不用，闲置即可
#define  RELAY_3_GPIO       GPIOC        
#define  RELAY_3_PIN        GPIO_Pin_5  
#define  RELAY_3_TRIGGER    1            
// 第4路继电器，倘若不用，闲置即可
#define  RELAY_4_GPIO       GPIOC        
#define  RELAY_4_PIN        GPIO_Pin_5  
#define  RELAY_4_TRIGGER    1            
//END 移植 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



/*****************************************************************************
 ** 声明全局函数
******************************************************************************/
// 第1路继电器
void    Relay_1_Init(void);
void    Relay_1_ON(void);
void    Relay_1_OFF(void);
uint8_t Relay_1_GetState(void);
// 第2路继电器
void    Relay_2_Init(void);
void    Relay_2_ON(void);
void    Relay_2_OFF(void);
uint8_t Relay_2_GetState(void);
// 第3路继电器
void    Relay_3_Init(void);
void    Relay_3_ON(void);
void    Relay_3_OFF(void);
uint8_t Relay_3_GetState(void);
// 第4路继电器
void    Relay_4_Init(void);
void    Relay_4_ON(void);
void    Relay_4_OFF(void);
uint8_t Relay_4_GetState(void);



#endif  


