/***********************************************************************************************************************************
 **【购买链接】  魔女科技    https://demoboard.taobao.com
 **【更新分享】  Q群文件夹
 ***********************************************************************************************************************************
 **【文件名称】  bsp_DHT11.c
 **【文件功能】
 **【适用平台】  STM32F103 + 标准库v3.5 + keil5
 **
 **
 **【更新记录】  2021-12-23  修改初始化函数，使文件移植更方便: DHT11_Init(GPIOx, GPIO_Pin_x);
 **              2021-10-26  修改c和h文件文件格式
 **              2021-10-26  修改DHT11_GetData()函数
 **              2021-05-20  创建文件
 **
************************************************************************************************************************************/
#include "bsp_DHT11.h"





xDHT11_TypeDef       xDHT11;             // 声明全局结构体, 用于记录信息



static GPIO_TypeDef *DHT11_GPIOx = 0;    // 引脚端口
static uint32_t      DHT11_PINx  = 0;    // 引脚编号



#define  DHT11_BUS_HIGH     (DHT11_GPIOx->BSRR = (uint32_t)DHT11_PINx)           // DAT引脚 置高电平
#define  DHT11_BUS_LOW      (DHT11_GPIOx->BSRR = ((uint32_t)DHT11_PINx) << 16)   // DAT引脚 置低电平
#define  DHT11_BUS_READ     ((DHT11_GPIOx->IDR & DHT11_PINx) ? 1: 0)             // 读取引脚的电平



#if DELAY_MODE_TIM2
#include "stm32f10x_tim.h"
#include "misc.h"
// 定时器初始化配置
// 不使用中断
void Tim2_Init(uint16_t psc, uint16_t arr, FunctionalState NewState)
{
    TIM_TimeBaseInitTypeDef tim;                         // 结构体

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // 开启TIM时钟
    TIM_DeInit(TIM2);
    tim.TIM_ClockDivision = TIM_CKD_DIV1;                // 采样分频
    tim.TIM_CounterMode   = TIM_CounterMode_Up;          // 向上计数
    tim.TIM_Period        = arr;                         // 自动重装载寄存器的值
    tim.TIM_Prescaler     = psc - 1;                     // 时钟预分频
    TIM_TimeBaseInit(TIM2, &tim);                        // 初始化结构体
    TIM2->SR = (uint16_t)~((uint16_t)0x01);              // 清除更新标志 
    TIM2->CNT = 0;                                       // 清0计数器
    TIM_Cmd(TIM2, NewState);                             // 是否开始工作
}



static void delay_us(uint32_t us)
{
    Tim2_Init(72, us, ENABLE);
    while ((TIM2->SR & 1) == 0);
    TIM_Cmd(TIM2, DISABLE);
}



static void delay_ms(uint32_t ms)
{
    for (uint16_t i = 0; i < ms; i++)
    {
        Tim2_Init(72, 990, ENABLE);
        while ((TIM2->SR & 1) == 0);
        TIM_Cmd(TIM2, DISABLE);
    }
}
#else

// 本地US粗略延时函数，减少移植时对外部文件依赖；
static void delay_us(uint32_t us)
{
    uint16_t i = 0;
    while (us--)
    {
        i = 7;
        while (i--);
    }
}

// 本地MS粗略延时函数，减少移植时对外部文件依赖；
static void delay_ms(uint32_t ms)
{
    uint32_t i = 0;
    while (ms--)
    {
        i = 12000;
        while (i--);
    }
}
#endif // 结束延时



static void DHT11_Mode_IPU(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = DHT11_PINx ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_GPIOx, &GPIO_InitStructure);
}

static void DHT11_Mode_Out_PP(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DHT11_PINx ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_GPIOx, &GPIO_InitStructure);
}

static void DHT11_Init(GPIO_TypeDef *GPIOx, uint32_t PINx)
{
    DHT11_GPIOx = GPIOx;
    DHT11_PINx = PINx;

    // 时钟使能：引脚端口;用判断端口的方式使能时钟线, 减少移植时的工作
    if (GPIOx == GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    if (GPIOx == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    if (GPIOx == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    if (GPIOx == GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    if (GPIOx == GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    if (GPIOx == GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
    if (GPIOx == GPIOG)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);

    DHT11_Mode_Out_PP();
    DHT11_BUS_HIGH;
}


// 从DHT11读取一个字节，MSB先行
// 1：每bit以50us低电平标置开始，
// 2：以26~28us的高电平表示“0”
// 3：以70us高电平表示“1”
// 4: 通过检测从高电平开后 x us后的电平即可区别这两个状态
static uint8_t readByte(void)
{
    uint8_t  temp = 0;

    for (uint8_t i = 0; i < 8; i++)
    {
        while (DHT11_BUS_READ == 0);             // 每bit以50us低电平标置开始，轮询直到从机发出 的50us 低电平 结束
        delay_us(40);                             // 延时x us 这个延时需要大于数据0持续的时间即可
        temp <<= 1;
        if (DHT11_BUS_READ == 1)                 // x us后仍为高电平表示数据“1”
        {
            while (DHT11_BUS_READ == Bit_SET);  // 等待数据1的高电平结束
            temp |= 1;                           // 位置“1“
        }
    }
    return temp;
}


/******************************************************************************
 * 函  数： DHT11_GetData
 * 功  能： 从DHT11中读取数据
 * 参  数： 1:
 * 说  明： 调用后，获取到的数据，保存到结构体xDHT11中
 *          温度值：xDHT11.Temperature (有效范围:0~50℃)
 *          湿度值: xDHT11.Humidity    (有效范围:20%~90%)
 * 返回值： 0-失败; 1-正常
 ******************************************************************************/
uint8_t DHT11_GetData(GPIO_TypeDef *GPIOx, uint32_t PINx)
{
    static uint8_t humiInt = 0;        // 湿度的整数部分
    static uint8_t humiDec = 0;        // 湿度的小数部分
    static uint8_t TempInt = 0;        // 温度的整数部分
    static uint8_t TempDec = 0;        // 温度的小数部分
    static uint8_t sum = 0;            // 校验和

    DHT11_Init(GPIOx, PINx);

    // 1:主机产生开始信号
    DHT11_Mode_Out_PP();               // 输出模式
    DHT11_BUS_LOW;                     // 主机拉低电平
    delay_ms(25);                      // 延时18~30ms
    // 2：主机拉高等待
    DHT11_BUS_HIGH;                    // 总线拉高
    delay_us(50);                      // 延时20~40us，这里设置50，是因为要直接进入下一个时序（电平状态），以方便检测

    // 3: 从机产生响应和准备信号
    DHT11_Mode_IPU();                  // 主机设为输入 判断从机响应信号

    if (DHT11_BUS_READ == 0)           // 判断从机是否产生响应信号_低电平, 如不响应则跳出
    {
        while (DHT11_BUS_READ == 0);   // 等待响应信号结束：低电平持续约80us
        while (DHT11_BUS_READ == 1);   // 等待标置信号结束：高电平持续约80us
        // 4: 从机连续输出5字节数据
        humiInt = readByte();          // 湿度的整数部分// 开始接收数据
        humiDec = readByte();          // 湿度的小数部分
        TempInt = readByte();          // 温度的整数部分
        TempDec = readByte();          // 温度的小数部分
        sum     = readByte();          // 校验和

        DHT11_Mode_Out_PP();           // 读取结束，引脚改为输出模式
        DHT11_BUS_HIGH;                // 主机拉高

        // 5: 检查读取的数据是否正确
        if (sum == (humiInt + humiDec + TempInt + TempDec))
        {
            xDHT11.Humidity = humiInt + humiDec;
            xDHT11.Temperature = (float)TempInt  + (float)TempDec / 10;
            return SUCCESS;            // 校检正确, 返回：SUCCESS=1
        }
        return ERROR;                  // 校检错误，返回：ERROR=0
    }
    return ERROR;                      // 通信错误，返回：ERROR=0
}




/******************************************************************************
 * 函  数： DHT11_GetTemperature
 * 功  能： 从DHT11中读取温度值
 * 参  数： GPIO_TypeDef *GPIOx: GPIO端口号，取值范围：GPIOA ~ GPIOG
 *          uint32_t     PINx  : 引脚编号，  取值范围：GPIO_Pin_0 ~ GPIO_Pin_15
 * 说  明： 温度值有效范围:0~50℃; 精度±2°C; 小数部份无效
 * 返回值： 0-失败，非0值-湿度值
 ******************************************************************************/
float DHT11_GetTemperature(GPIO_TypeDef *GPIOx, uint32_t PINx)
{ 
    DHT11_GetData(GPIOx, PINx);
    return xDHT11.Temperature;
}


/******************************************************************************
 * 函  数： DHT11_GetHumidity
 * 功  能： 从DHT11中读取湿度值
 * 参  数： GPIO_TypeDef *GPIOx: GPIO端口号，取值范围：GPIOA ~ GPIOG
 *          uint32_t     PINx  : 引脚编号，  取值范围：GPIO_Pin_0 ~ GPIO_Pin_15
 * 说  明： 湿度值有效范围:20%~90%; 精度±5%; 小数部分无效
 * 返回值： 0-失败，非0值-湿度值
 ******************************************************************************/
float DHT11_GetHumidity(GPIO_TypeDef *GPIOx, uint32_t PINx)
{  
    DHT11_GetData(GPIOx, PINx);
    return xDHT11.Humidity;
}
