#include "bsp_relay.h"



void Relay_1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;   // 定义IO硬件初始化结构体变量 

    // 使能所用引脚端口时钟；使用端口判断的方法使能时钟, 以使代码移植更方便
    if (RELAY_1_GPIO == GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    if (RELAY_1_GPIO == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    if (RELAY_1_GPIO == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    if (RELAY_1_GPIO == GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    if (RELAY_1_GPIO == GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    if (RELAY_1_GPIO == GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
    if (RELAY_1_GPIO == GPIOG)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);

    GPIO_InitStructure.GPIO_Pin = RELAY_1_PIN;          // 设定继电器对应引脚IO编号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // 设定继电器对应引脚IO最大操作速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    // 设定继电器对应引脚IO为输出模式
    GPIO_Init(RELAY_1_GPIO, &GPIO_InitStructure);       // 初始化继电器对应引脚IO
    Relay_1_OFF();                                      // 关闭继电器
}


#if RELAY_1_TRIGGER      // 重要：第1路继电器有效工作电平，1-高电平，0-低电平
void Relay_1_ON(void)
{
    RELAY_1_GPIO->BSRR = RELAY_1_PIN;
}

void Relay_1_OFF(void)
{
    RELAY_1_GPIO->BRR = RELAY_1_PIN;
}
#else
void Relay_1_ON(void)
{
    RELAY_1_GPIO->BRR = RELAY_1_PIN;
}

void Relay_1_OFF(void)
{
    RELAY_1_GPIO->BSRR = RELAY_1_PIN;
}
}
#endif


// 返回：实际开启状态
uint8_t Relay_1_GetState(void)
{
    uint8_t state = 0;

    (RELAY_1_GPIO->IDR & RELAY_1_PIN) ? (state = 1) : (state = 0);

    if (state == RELAY_1_TRIGGER)
        return 1;
    else
        return 0;
}

