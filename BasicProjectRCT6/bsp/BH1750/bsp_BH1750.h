#ifndef __BSP_BH1750_H__
#define __BSP_BH1750_H__
#include <stm32f10x.h>
#include <stdio.h>



//移植参数区 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// SDA
#define    BH1750_SDA_GPIOx   GPIOB         // 模拟IIC
#define    BH1750_SDA_PINx    GPIO_Pin_0
// SCL
#define    BH1750_SCL_GPIOx   GPIOB         // 模拟IIC
#define    BH1750_SCL_PINx    GPIO_Pin_1
// IIC器件地址
#define    BH1750_ADDR   0x46               // 定义器件在IIC总线中的从地址, 根据ADDR引脚不同修改：当ADDR引脚接GND或空置时地址为0x46，接3.3V时地址为0xB8
//END 移植 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



/*****************************************************************************
 ** 声明  全局函数
****************************************************************************/
void     BH1750_Init(void);           // 初始化BH1750
float    BH1750_GetData(void);                                      // 读取BH1750数据




#endif

