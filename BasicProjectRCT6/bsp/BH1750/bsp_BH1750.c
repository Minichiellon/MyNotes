/**************************************************************************
 * 文件名：bh1750.c
 * 功 能 ：光强度传感模块
 *
 * 作 者 ：魔女科技团队
****************************************************************************/
#include "bsp_BH1750.h"

#define SDA_1   (BH1750_SDA_GPIOx -> BSRR = BH1750_SDA_PINx)
#define SDA_0   (BH1750_SDA_GPIOx -> BRR  = BH1750_SDA_PINx)

#define SCL_1   (BH1750_SCL_GPIOx -> BSRR = BH1750_SCL_PINx)
#define SCL_0   (BH1750_SCL_GPIOx -> BRR  = BH1750_SCL_PINx)




static uint8_t   Buffer[8];       // 接收数据缓存区




static void delay_us(uint32_t times)
{
    times = times * 7;   //  10us内用7;
    while (--times)
        __nop();
}


static void delay_ms(uint32_t ms)
{
    ms = ms * 6500;
    for (u32 i = 0; i < ms; i++); // 72MHz系统时钟下，多少个空循环约耗时1ms
}




// 产生IIC开始信号
void BH1750_Start()
{
    SDA_1 ;              // 拉高数据线
    SCL_1;               // 拉高时钟线
    delay_us(5);         // 延时
    SDA_0;               // 产生下降沿
    delay_us(5);         // 延时
    SCL_0;               // 拉低时钟线
}

// 产生IIC停止信号
void BH1750_Stop()
{
    SDA_0;               // 拉低数据线
    SCL_1;               // 拉高时钟线
    delay_us(5);         // 延时
    SDA_1;               // 产生上升沿
    delay_us(5);         // 延时
}

// 发送应答信号, 参数:ack (0:ACK 1:NAK)
static void BH1750_SendACK(uint8_t ack)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Pin   = BH1750_SDA_PINx ;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BH1750_SDA_GPIOx, &GPIO_InitStruct);

    if (ack == 1)    // 写应答信号
        SDA_1;
    else 
        SDA_0;

    SCL_1;           // 拉高时钟线
    delay_us(5);     // 延时
    SCL_0;           // 拉低时钟线
    delay_us(5);     // 延时
}

// 接收应答信号
static uint8_t BH1750_RecvACK()
{
    uint8_t value = 0;
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Pin = BH1750_SDA_PINx ;    // 配置为输入模式
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;      // 上拉输入
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BH1750_SDA_GPIOx, &GPIO_InitStruct);

    SCL_1;                                          // 拉高时钟线
    delay_us(5);                                    // 延时
    if (BH1750_SDA_GPIOx->IDR & BH1750_SDA_PINx)    // 读应答信号  (GPIO_ReadInputDataBit(GPIOA, sda) == 1)
        value = 1 ;
    else
        value = 0 ;
    SCL_0;                                          // 拉低时钟线
    delay_us(5);                                    // 延时
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;   // 配置为输出模式
    GPIO_Init(BH1750_SDA_GPIOx, &GPIO_InitStruct);
    return value;
}

// 向IIC总线发送一个字节数据
static void BH1750_SendByte(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++)     //8位计数器
    {
        if (0X80 & data)
            SDA_1;
        else
            SDA_0;
        data <<= 1;
        SCL_1;             // 拉高时钟线
        delay_us(5);       // 延时
        SCL_0;             // 拉低时钟线
        delay_us(5);       // 延时
    }
    BH1750_RecvACK();      // 接收应答信号
}

// 向IIC总线接收一个字节
static uint8_t BH1750_RecvByte()
{
    uint8_t i;
    uint8_t dat = 0;
    uint8_t bit;

    GPIO_InitTypeDef GPIO_InitStruct;              
    GPIO_InitStruct.GPIO_Pin = BH1750_SDA_PINx ;     // 配置为输入模式
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;       // 上拉输入 
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BH1750_SDA_GPIOx, &GPIO_InitStruct);

    SDA_1;                                           // 上拉,准备读取数据,
    for (i = 0; i < 8; i++)                          // 8位计数器
    {
        dat <<= 1;
        SCL_1;                                       // 拉高时钟线
        delay_us(5);                                 // 延时

        if (BH1750_SDA_GPIOx->IDR & BH1750_SDA_PINx) 
            bit = 0X01;
        else
            bit = 0x00;
        dat |= bit;                                  // 读数据
        SCL_0;                                       // 拉低时钟线
        delay_us(5);                                 // 延时
    }
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;    // 配置为输出模式
    GPIO_Init(BH1750_SDA_GPIOx, &GPIO_InitStruct);
    return dat;
}

static void BH1750_WriteOrder(uint8_t REG_Address)
{
    BH1750_Start();                  //起始信号
    BH1750_SendByte(BH1750_ADDR);    //发送设备地址+写信号
    BH1750_SendByte(REG_Address);    //内部寄存器地址，请参考中文pdf22页
//  BH1750_SendByte(REG_data);       //内部寄存器数据，请参考中文pdf22页
    BH1750_Stop();                   //发送停止信号
}

//初始化BH1750，根据需要请参考pdf进行修改
void BH1750_Init()
{
    /*** 特别地说明：引脚需要在h文件中修改 ***/
        
    GPIO_InitTypeDef GPIO_InitStruct;
    // 使能SDA引脚端口时钟
    if (BH1750_SDA_GPIOx == GPIOA)  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    if (BH1750_SDA_GPIOx == GPIOB)  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    if (BH1750_SDA_GPIOx == GPIOC)  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    if (BH1750_SDA_GPIOx == GPIOD)  RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
    if (BH1750_SDA_GPIOx == GPIOE)  RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;
    if (BH1750_SDA_GPIOx == GPIOF)  RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;
    if (BH1750_SDA_GPIOx == GPIOG)  RCC->APB2ENR |= RCC_APB2ENR_IOPGEN;
    // 使能SCL引脚端口时钟
    if (BH1750_SCL_GPIOx == GPIOA)  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    if (BH1750_SCL_GPIOx == GPIOB)  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    if (BH1750_SCL_GPIOx == GPIOC)  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    if (BH1750_SCL_GPIOx == GPIOD)  RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
    if (BH1750_SCL_GPIOx == GPIOE)  RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;
    if (BH1750_SCL_GPIOx == GPIOF)  RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;
    if (BH1750_SCL_GPIOx == GPIOG)  RCC->APB2ENR |= RCC_APB2ENR_IOPGEN;

    // 配置SDA引脚工作模式
    GPIO_InitStruct.GPIO_Pin   = BH1750_SDA_PINx;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BH1750_SDA_GPIOx, &GPIO_InitStruct);
    // 配置SCL引脚工作模式
    GPIO_InitStruct.GPIO_Pin   = BH1750_SCL_PINx;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BH1750_SCL_GPIOx, &GPIO_InitStruct);

    BH1750_WriteOrder(0x01);
    delay_ms(180);                // 延时180ms
}

//连续读出BH1750内部数据
static void BH1750_ReadData(void)
{
    BH1750_Start();                        // 起始信号
    BH1750_SendByte(BH1750_ADDR + 1);      // 发送设备地址+读信号

    for (uint8_t i = 0; i < 3; i++)        // 连续读取6个地址数据，存储buffer中
    {
        Buffer[i] = BH1750_RecvByte();     // buffer[0]存储0x32地址中的数据
        if (i == 3)
        {
            BH1750_SendACK(1);             // 最后一个数据需要回NOACK
        }
        else
        {
            BH1750_SendACK(0);             // 回应ACK
        }
    }
    BH1750_Stop();                         // 停止信号
    delay_ms(5);
}



float BH1750_GetData(void)
{
    uint32_t dataTem;                      // 结果值
    static uint8_t flagInit = 0;
    if(flagInit==0)
    {
        BH1750_WriteOrder(0x01);           // power on
        BH1750_WriteOrder(0x10);           // H- resolution mode
        delay_ms(180);                     // 延时180ms
        flagInit=1;
    }        
    BH1750_ReadData();                     // 读取原始数据，存储在Buffer中
    dataTem = Buffer[0];
    dataTem = (dataTem << 8) + Buffer[1];  // 转换成结果光强度值
    return dataTem / 1.2;
}
