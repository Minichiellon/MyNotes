/***********************************************************************************************************************************
 **【购买链接】  魔女科技    https://demoboard.taobao.com
 **【更新分享】  Q群文件夹   887199504
 ***********************************************************************************************************************************
 **【文件名称】  ds18b20.c
 **【文件功能】
 **【适用平台】  STM32F103 + 标准库v3.5 + keil5
 **
 **【推荐视频】  https://www.bilibili.com/video/BV1CV411o7Di?from=search&seid=1639361392451575575
 **
 **【工作指令】  DS18B20的工作指令
 **              温度转换指令    0x44，启动Ds18b20启动转换温度
 **              读暂存器指令    0xBE，读取暂存器中的九字节数据
 **              写暂存器置零    0x4E，把数据写入暂存器的TH、TL
 **              赋值暂存器      0x48，把暂存器中的TH、TL写入EEPROM中
 **              读电源供电方式  0xB4，启动Ds18b20,发送电源供电方式
 **              重调EEPROM      0xB8，把EEPROM中的TH、TL读至暂存器
 **
 **
 **【更新记录】  2021-05-12  创建
 **
 **
************************************************************************************************************************************/
#include "bsp_DS18B20.h"



static GPIO_TypeDef *DS18B20_GPIO = 0;
static uint32_t      DS18B20_Pin  = 0;

#define DS18B20_BUS_HIGH    (DS18B20_GPIO->BSRR = DS18B20_Pin)
#define DS18B20_BUS_LOW     (DS18B20_GPIO->BSRR = DS18B20_Pin<<16)
#define DS18B20_BUS_READ   ((DS18B20_GPIO->IDR & DS18B20_Pin)?1:0)



xDS18B20_TypeDef  xDS18B20;              // 声明全局结构体, 用于记录信息



static void    delayUS(uint32_t times);  // 本地US粗略延时函数，减少移植时对外部文件依赖；
static void    writeByte(uint8_t byte);  // 写入一个字节;
static uint8_t readByte(void);           // 读出一个字节;
static uint8_t resetAndCheck(void);      // 产生复位信号，并检查回应



// 本地US粗略延时函数，减少移植时对外部文件依赖；
static void delayUS(u32 times)
{
    times = times * 8;
    while (--times)
        __nop();
}

// 写一个字节到DS18B20
// byte：要写入的字节
// 注意：低位先行LSB
static void writeByte(u8 byte)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        DS18B20_BUS_LOW;        // 主机把总线拉低
        delayUS(2);             // 保持1US以上

        if ((byte >> i) & 0x01)    DS18B20_BUS_HIGH; // 写1: 拉低总线1US后，释放总线为高电平，持续至少60US以上
        // 写0: 拉低总线1US后，总线保持在低电平，持续至少60US以上
        delayUS(60);

        DS18B20_BUS_HIGH;       // 释放总线, 使时序完整
        delayUS(1) ;
    }
}

// 从DS18B20读取一个字节
// 先接收到的是低位
static uint8_t readByte(void)
{
    u8 data = 0;

    for (uint8_t i = 0; i < 8; i++)
    {
        DS18B20_BUS_LOW;         // 主机把总线拉低
        delayUS(4);              // 保持1US以上
        DS18B20_BUS_HIGH;        // 主机释放总线

        delayUS(20);             // DS18B20会保持一个至少15US的信号
        if (DS18B20_BUS_READ)   data = (data >> 1) | 0x80;
        else                    data = data >> 1;
        delayUS(100);            // 适当延时，读周期至少60US

        DS18B20_BUS_HIGH;        // 释放总线, 使时序完整
        delayUS(1) ;
    }
    return data;
}

// 产生复位信号，并检查回应
// 1-主机将单总线拉低，最少480US后，释放总线
// 2-延时15~60US后
// 3-在60~240US内，检测DQ是否为低电平，
// 4-释放总线，延时240US，保持起始时序的完整
static uint8_t resetAndCheck(void)
{
    u32 timeOut = 0;

    // 这两行可不用的，只是为了使总线电平稳定
    //DS18B20_BUS_HIGH;   // 释放总线，为开始时序作准备
    //delayUS(500);

    // 复位时序
    DS18B20_BUS_LOW;    // 拉低总线
    delayUS(600);       // 保持至少480US
    DS18B20_BUS_HIGH;   // 释放总线
    delayUS(40);        // 等待15~60US

    // 检测总线回应: 在60~240US内，检测DQ是否为低电平
    while (DS18B20_BUS_READ && (timeOut < 200))
    {
        timeOut++;
        delayUS(1);
    }
    if (timeOut >= 200)
        return 0;        // 超时返回；0-正常，有回应；　1-失败，无回应；
    else
        timeOut = 0;     // 有回应，超时函数清０

    // 检查回应后的总线是否释放：延时240US，保持起始时序的完整
    while ((!DS18B20_BUS_READ) && (timeOut < 240))
    {
        timeOut++;
        delayUS(1);
    }
    if (timeOut >= 240)
        return 0;        // 超时返回；0-正常，有回应；　1-失败，无回应；
    else
        return 1;        // 在正常时间里发现总线释放
}



//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回0:不存在
//返回1:存在
uint8_t DS18B20_Init(GPIO_TypeDef *GPIOx, uint32_t PINx)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    xDS18B20.initState = 0;    // 初始化状态，配置为0，即未初始化；
    DS18B20_GPIO = GPIOx;
    DS18B20_Pin = PINx;

    // 时钟使能：引脚端口;用判断端口的方式使能时钟线, 减少移植时的工作
    if (GPIOx == GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    if (GPIOx == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    if (GPIOx == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    if (GPIOx == GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    if (GPIOx == GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    if (GPIOx == GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
    if (GPIOx == GPIOG)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);

    // 配置：引脚工作模式
    GPIO_InitStructure.GPIO_Pin   = PINx;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;          // 开漏模式！总线！
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStructure);

    DS18B20_BUS_HIGH ;                                         // 释放总线：拉高！

    /*********** 下午一段，为了配合单个DS18B20而设置，可不要
    xDS18B20.initState = resetAndCheck();                      // 初始化状态：0-正常; 1-失败
    if (xDS18B20.initState == 0)                               // 初始化状态：0-正常; 1-失败
        printf("DS18B20 检测...       发现设备\r\n");
    else
        printf("DS18B20 检测...       没有找到设备！！\r\n");

    return xDS18B20.initState;
    *************/
    return 1;
}

//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-55~125）
float   DS18B20_GetDtat(GPIO_TypeDef *GPIOx, uint32_t PINx)
{
    uint8_t TL, TH;
    short temp;

    //if (xDS18B20 .initState != 1)        // 如果没有初始化，先初始化引脚
    DS18B20_Init(GPIOx, PINx);

    // 启动转换
    resetAndCheck();    // 复位
    writeByte(0xcc);    // skip rom
    writeByte(0x44);    // 启动Ds18b20启动转换温度

    // 读取数据
    resetAndCheck();
    writeByte(0xcc);    // skip rom
    writeByte(0xbe);    // 读取暂存器中的九字节数据
    TL = readByte();    // LSB
    TH = readByte();    // MSB

    temp = TH << 8 | TL;
    if (temp >= 127)    temp = (0 - (~temp));

    return (((float)temp * 0.625) / 10);
}





