#include "stm32f10x.h"
#include "bsp_SR04.h"



// 全局变量声明
xSR04_TypeDef  xSR04;    // 声明全局结构体, 用于记录信息

// 本地变量声明, 本地宏定义
#define TRIG_1  (SR04_TRIG_GPIO->BSRR = SR04_TRIG_PIN)         // 高电平
#define TRIG_0  (SR04_TRIG_GPIO->BRR  = SR04_TRIG_PIN)         // 低电平
#define ECHO    ((SR04_ECHO_GPIO->IDR & SR04_ECHO_PIN)? 1: 0)  // 判断ECHO引脚电平

// 本地函数声明
static void TIM6_Init(uint16_t PSC, uint16_t ARR, FunctionalState NewState);
static void SR04_Stop(void);



#if 1
// 本地US粗略延时函数，减少移植时对外部文件依赖；
static void delay_us(uint32_t times)
{
    times = times * 7;    //  10us内用7;
    while (--times)
        __nop();
}
#endif



/******************************************************************************
 * 函  数： SR04_Init ()
 * 功  能： 初始化SR04
 * 参  数： 无
 * 说  明： 初始化Trig、Echo两个引脚
 *          初始化Tim6
 *          开始由TIM6自动控制偱环工作，其结果值CM将不断地更新且存储于xSRO4.CM中
 *          如果想停止SR04的工作，可调用SR04_Stop()，重新开始，则调用SR04_Start();
 * 返回值： 无
 ******************************************************************************/
void SR04_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能AFIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE) ;      // 使能AFIO时钟，Echo引脚要用中断
    // 使能SR04_TRIG所用引脚端口时钟；使用端口判断的方法使能时钟, 以使代码移植更方便
    if (SR04_TRIG_GPIO == GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    if (SR04_TRIG_GPIO == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    if (SR04_TRIG_GPIO == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    if (SR04_TRIG_GPIO == GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    if (SR04_TRIG_GPIO == GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    if (SR04_TRIG_GPIO == GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
    if (SR04_TRIG_GPIO == GPIOG)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
    // 使能SR04_ECHO所用引脚端口时钟；使用端口判断的方法使能时钟, 以使代码移植更方便
    if (SR04_ECHO_GPIO == GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    if (SR04_ECHO_GPIO == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    if (SR04_ECHO_GPIO == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    if (SR04_ECHO_GPIO == GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    if (SR04_ECHO_GPIO == GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    if (SR04_ECHO_GPIO == GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
    if (SR04_ECHO_GPIO == GPIOG)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);

    // Trig引脚 配置
    GPIO_InitStructure.GPIO_Pin   = SR04_TRIG_PIN;       // Trig引脚用于控制开启测量：闲时用单片机置低电平，开启测距时，置高电平10us以上，然后再置低电平，即可开始工作;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;    // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    // 这一行，必不可少; 有不少人把这一行取消了，排查很难发现出错的原因
    GPIO_Init(SR04_TRIG_GPIO, &GPIO_InitStructure);
    TRIG_0;                                              // 低电平

    // Echo引脚 配置
    GPIO_InitStructure.GPIO_Pin   = SR04_ECHO_PIN;       // Echo引脚用于接收结果信号：高电平延长的时间，就是超声波发出到接收时长，因此，障碍物的距离计算公式为： 距离(m) = 高电平时长(s) x 声速(346M/2) ÷ 2
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;       // 下拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    // 这一行，必不可少; 有不少人把这一行取消了，排查很难发现出错的原因
    GPIO_Init(SR04_ECHO_GPIO, &GPIO_InitStructure);

    TIM6_Init(72, 30000, DISABLE);                       // TIM6初始化，分频值:72, 重装载值：30000，即：每个脉冲0.0000001秒(1MHz: 72000000/7200), 每30000次脉冲产生一次中断(0.03秒：30000*0.000001);

    xSR04.flagInitOK = 1;
    SR04_Stop();                                         // 先调用一次停止函数，没特殊的意思，只为了清0各个值的数据，防止出错
}



/*****************************************************************************
 * 函  数： TIM6_Init
 * 功  能： 配置基本定时器TIM6
 * 参  数： uint16_t PSC :  预分频器数值,16位; 用于设置计数器1脉冲的时长(时钟频率);
 *          uint16_t ARR :  自动重装载数值,16位; 即多少个脉冲产生一个更新或中断(1周期);
 *          FunctionalState NewState ：定时器开关; 初始化后，是打开还是关闭
 * 返回值： 无
 * 重  要： 1: F103系列，TIM的时钟源频率为72000000
 *          2: TIM_TimeBaseInitTypeDef结构体里面有5个成员，
 *             TIM6和TIM7基本定时的配置只需用到2个，另外3个是配置通用、高级定时器所用。
 *             typedef struct
 *             {
 *                 TIM_Prescaler ;          // 所有定时器 都有
 *                 TIM_CounterMode ;        // 基本定时器TIMx,x[6,7]没有，其他都有
 *                 TIM_Period ;             // 所有定时器 都有
 *                 TIM_ClockDivision ;      // 基本定时器TIMx,x[6,7]没有，其他都有
 *                 TIM_RepetitionCounter ;  // TIMx,x[1,8,15,16,17]才有
 *             }TIM_TimeBaseInitTypeDef;
*****************************************************************************/
static void TIM6_Init(uint16_t PSC, uint16_t ARR, FunctionalState NewState)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);        // 设置中断组为2, 这个设置，放在了main()函数的第1行，以方便统一设置所有中断的分组
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;           // 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 设置主优先级为 0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        // 设置抢占优先级为2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 允许中断*/
    NVIC_Init(&NVIC_InitStructure);

#if 1                                                     // 标准库写法
#include "stm32f10x_tim.h"

    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);  // 开启TIM6时钟,即内部时钟CK_INT=72M
    TIM_Cmd(TIM6, DISABLE);                               // 在配置前，先关闭计数器
    TIM_TimeBaseStructure.TIM_Prescaler = PSC - 1;        // 预分频器数值,16位; 计数器的时钟频率CK_CNT等于f CK_PSC /(PSC[15:0]+1)。在每一次更新事件时，PSC的数值被传送到实际的预分频寄存器中。
    TIM_TimeBaseStructure.TIM_Period    = ARR;            // 自动重装载数值,16位; 即多少个脉冲产生一个更新或中断(1周期)。如果自动重装载数值为0，则计数器停止。
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);       // 初始化定时器

    TIM_ClearFlag(TIM6, TIM_FLAG_Update);                 // 清除计数器中断标志位
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);            // 开启更新中断(只是开启功能，未工作）
    TIM_Cmd(TIM6, NewState);                              // 是否使能计数器(使能了就会立即开始工作)

#else                                                     // 寄存器写法, 优点：方便移植、调试
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN ;                  // 开启TIM6时钟,
    TIM6->CR1  = 0;                                       // 在配置前，先关闭计数器, 并清0 CR1的配置
    TIM6->CR2  = 0;                                       // 清0 CR2的配置
    TIM6->DIER = 0x01 ;                                   // 开启更新中断(只是开启功能，未工作）, 关闭DMA清求
    TIM6->SR   = 0;                                       // 清0 SR寄存器, 本寄存器只有一个功能: 中断标志位。
    TIM6->CNT  = 0;                                       // 清0 计数器的值
    TIM6->PSC  = PSC - 1;                                 // 预分频器数值,16位; 计数器的时钟频率CK_CNT等于f CK_PSC /(PSC[15:0]+1)。在每一次更新事件时，PSC的数值被传送到实际的预分频寄存器中。
    TIM6->ARR  = ARR;                                     // 自动重装载数值,16位; 即多少个脉冲产生一个更新或中断(1周期)。如果自动重装载数值为0，则计数器停止。
    if (NewState)   TIM6->CR1 |= 0x01;                    // 是否使能计数器(使能了就会立即开始工作)
#endif

}



/*****************************************************************************
 * 函  数： TIM6_IRQHandler
 * 功  能： 超过0.03秒，则判断
 * 参  数： 无
 * 返回值： 无
 * 重  要： 1: TIM定时器，必须先清中断，后干其它事！！否则可能出现莫名错误
 *          2: 为了方便代码移植，中断服务函数的位置，放置于定时器配置函数下放
 *          3: 本函数的功能：当超过0.03ms时，Echo还没收到低电平结束信号，则停止测量
*****************************************************************************/
void  TIM6_IRQHandler(void)
{
    TIM6->SR &= (uint16_t)~0x01;   // 清0更新中断标志位
    SR04_Stop() ;                  // 超时处理，0.03s ;
}



/*****************************************************************************
 * 函  数： TIM6_IRQHandler
 * 功  能： 超过0.03秒，则判断
 * 参  数： 无
 * 返回值： 无
 * 重  要： 1: TIM定时器，必须先清中断，后干其它事！！否则可能出现莫名错误
 *          2: 为了方便代码移植，中断服务函数的位置，放置于定时器配置函数下放
 *          3: 本函数的功能：当超过0.03ms时，Echo还没收到低电平结束信号，则停止测量
*****************************************************************************/
static void SR04_Stop(void)
{
    TIM6->CR1 = 0;                 // 是否使能计数器(使能了就会立即开始工作)
    TIM6->CNT = 0;
    TRIG_0 ;
}



/*****************************************************************************
 * 函  数： TIM6_IRQHandler
 * 功  能： 超过0.03秒，则判断
 * 参  数： 无
 * 返回值： 无
 * 重  要： 1: TIM定时器，必须先清中断，后干其它事！！否则可能出现莫名错误
 *          2: 为了方便代码移植，中断服务函数的位置，放置于定时器配置函数下放
 *          3: 本函数的功能：当超过0.03ms时，Echo还没收到低电平结束信号，则停止测量
*****************************************************************************/
float SR04_GetCM(void)
{
    while (ECHO == 1); // 等待Echo引脚低电平
    /********1.启动信号:10US以上的脉冲**********/
    TRIG_1;
    delay_us(20);
    TRIG_0;
    /********2.启动TIM，记录ECHO高电平时长**********/
    TIM6->CNT = 0;        // 狗血日常：调试过程无意中删了这一行，然后剧情他妈的精彩。排查一轮后，心力交瘁躺平发呆，刚眯上眼才来的灵感：是不是CNT计数器没清0！！
    while (ECHO == 0);    // 等待高电平开始(SR04工作后，ECHO引脚将返回声波来回的等长高电平时长）
    TIM6->CR1 |= 0x01;    // TIM开始工作
    while (ECHO == 1);    // 等待高电平结束
    TIM6->CR1 = 0x00;     // TIM停止工作
    /********3.计算距离,单位:CM**********/
    xSR04.CM = TIM6->CNT * 0.0173;  // 摄氏25度时音速每秒346米，因为TIM初始化成1us脉冲，换算成：0.0346厘米/1us; Echo引脚的高电平时长，是声波从发出到接收的时间，所以要除以2

    return xSR04.CM;
}




//void SR04_Start(void)
//{
////    NVIC_InitTypeDef NVIC_InitStruct ;      // 作用：配置优先级
////    EXTI_InitTypeDef EXTI_InitStruct ;      // 作用：配置引脚中断方式
//    float temp = 0;
//    float CM = 0;
//    /********如果上一个读取动作还在进行中，就等待**********/
//    while (ECHO == 1);
//    /********启动信号:10US以上的脉冲**********/
//    TRIG_1;
//    delay_us(20);
//    TRIG_0;
////    /********设置ECHO引脚上升沿触发**********/
////    // Echo 配置中断优先级
////    NVIC_InitStruct.NVIC_IRQChannel = SR04_ECHO_IRQN ;               // 中断号
////    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1 ;          // 配置抢占优先级
////    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1 ;                 // 配置子优先级
////    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE ;                    // 使能中断通道
////    NVIC_Init(&NVIC_InitStruct) ;                                    // 初始化，调用优先级配置函数
////    // 配置中断的方式
////    GPIO_EXTILineConfig(SR04_ECHO_GPIOSOURCE, SR04_ECHO_PINSOURCE);  // 选择作为EXTI线的GPIO引脚
////    EXTI_InitStruct.EXTI_Line    = SR04_ECHO_EXTI_LINE ;             // 配置中断or事件线
////    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt ;             // 模式：中断：EXTI_Mode_Interrupt、事件：EXTI_Mode_Event
////    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising ;             // 边沿触发： 上升：EXTI_Trigger_Rising 、下降：EXTI_Trigger_Falling 、浮空：EXTI_Trigger_Rising_Falling
////    EXTI_InitStruct.EXTI_LineCmd = ENABLE ;                          // 使能EXTI线
////    EXTI_Init(&EXTI_InitStruct) ;                                    // 初始化，调用中断线配置函数
//}


//// KEY_1 中断服务函数
//void SR04_ECHO_IRQHANDLER(void)                        // 提示：这个函数名，是h文件中的宏定义，在编译过程中，会被替换成宏定义的值
//{
//    if (EXTI->PR & SR04_ECHO_PIN)                      // 板子上的按键已使用电容作简单的硬件消抖,无需再使用软件延时消抖
//    {
//        EXTI->PR |= SR04_ECHO_PIN  ;                   // 清理中断标示
//        if (ECHO == 1)
//        {
//            TIM6->CNT = 0;
//            TIM_Cmd(TIM6, ENABLE);                               // 是否使能计数器(使能了就会立即开始工作)
//            // Echo引脚配置为下降沿触发

//        }
//        if (ECHO == 0)
//        {
//            TIM_Cmd(TIM6, ENABLE);                               // 是否使能计数器(使能了就会立即开始工作)
//            xSR04.CM = (TIM6->CNT) * 0.034;
//            // Echo引脚配置为上升沿触发
//        }

////        printf("第 1 个按键被按下, 蓝灯反转\r");   // 重要提示：printf是不可重入函数，中断服务函数中使用，可能会产生不可预测的错误。这里使用printf，只用代码测试使用！！
////        LED_BLUE_TOGGLE;                           // 蓝灯转换，以方便肉眼观察效果
//    }
//}



