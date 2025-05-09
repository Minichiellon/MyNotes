#include "bsp_RTC.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_rcc.h"


#define WEEK_0  "日"
#define WEEK_1  "一"
#define WEEK_2  "二"
#define WEEK_3  "三"
#define WEEK_4  "四"
#define WEEK_5  "五"
#define WEEK_6  "六"



xRTC_TypeDef  xRTC;                                   // 声明为全局变量,方便记录信息、状态

uint8_t const table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};            // 月修正数据表
const uint8_t mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // 平年的月份日期表



static void delay_ms(uint32_t ms)                     // 定义一个ms延时函数，减少移植时对外部文件依赖;
{
    ms = ms * 10286;                                  // 注意：打勾 "Options --> C/C++ ---> One ELF Section per Function选项"
    for (uint32_t i = 0; i < ms; i++);                // 72MHz系统时钟下，大约多少个空循环耗时1ms
}



// 返回值:该年份是不是闰年.1,是.0,不是
uint8_t isLeapYear(uint16_t year)
{
    if (year % 4 == 0)                     // 必须能被4整除
    {
        if (year % 100 == 0)
        {
            if (year % 400 == 0) return 1; // 如果以00结尾,还要能被400整除
            else            return 0;
        }
        else
            return 1;
    }
    else
        return 0;
}

// 获得现在是星期几
// year,month,day：公历年月日
// 返回值：星期几
uint8_t getWeek(uint16_t year, uint8_t month, uint8_t day)
{
    uint16_t temp2;
    uint8_t yearH, yearL;

    yearH = year / 100;
    yearL = year % 100;
    // 如果为21世纪,年份数加100
    if (yearH > 19)yearL += 100;
    // 所过闰年数只算1900年之后的
    temp2 = yearL + yearL / 4;
    temp2 = temp2 % 7;
    temp2 = temp2 + day + table_week[month - 1];
    if (yearL % 4 == 0 && month < 3)temp2--;
    return (temp2 % 7);
}



/******************************************************************************
 * 函  数： RTC_Init
 * 功  能： 检查并配置RTC
 *
 * 参  数：
 * 返回值：　0:成功   1:失败
 * 备  注：
 ******************************************************************************/
uint8_t RTC_Init(void)
{
    uint8_t temp = 0;
    if (BKP->DR1 != 0X1314)                        // 读取后备域数据，以判断RTC是否正常运作中(只要VBAT保持工作电压,RTC和后备域不受系统掉电影响)
    {
        RCC->APB1ENR |= 1 << 28;                   // 使能 电源时钟
        RCC->APB1ENR |= 1 << 27;                   // 使能 备份时钟
        PWR->CR      |= 1 << 8;                    // 取消备份区写保护
        RCC->BDCR    |= 1 << 16;                   // 备份区域软复位
        RCC->BDCR    &= ~(1 << 16);                // 备份区域软复位结束
        RCC->BDCR    |= 1 << 0;                    // 开启外部低速振荡器
        while ((!(RCC->BDCR & 0X02)) && (++temp) < 250) // 等待外部时钟就绪
            delay_ms(10);
        if (temp >= 250)   return 1;               // 初始化时钟失败,晶振有问题
        RCC->BDCR |= 1 << 8;                       // RTC时钟源选择   00:无　　01:LSE   10:LSI   11:HSE/128
        RCC->BDCR |= 1 << 15;                      // RTC时钟使能

        while ((RTC->CRL & (1 << 5)) == 0);        // 等待上一次对RTC寄存器的写操作完成   0:正在进行   1:已完成
        while ((RTC->CRL & (1 << 3)) == 0);        // 等待RTC寄存器同步    0:未同步   2:已同步
        RTC->CRL |= 1 << 4;                        // 配置标志, 0:退出配置,RTC开始更新，　1:进入配置
        RTC->PRLH = 0X0000;
        RTC->PRLL = 32767;                         // 时钟周期设置(有待观察,看是否跑慢了?)理论值：32767

        RTC_SetDate(1900, 1, 1, 1, 1, 1);          // 设置日期、时间
        RTC->CRL &= ~(1 << 4);                     // 配置标志, 0:退出配置,RTC开始更新，　1:进入配置
        while ((RTC->CRL & (1 << 5)) == 0);        // 等待上一次对RTC寄存器的写操作完成   0:正在进行   1:已完成
        BKP->DR1 = 0X1314;                         // 在备份数据域第一个寄存器写入配置标志值(共42个)，方便复位后判断是否需要初始化
    }
    else                                           // 成功读取到旧数据标志，表示RTC正在正常工作
    {
        while ((RTC->CRL & (1 << 3)) == 0);        // 等待RTC寄存器同步    0:未同步   2:已同步
        while ((RTC->CRL & (1 << 5)) == 0);        // 等待上一次对RTC寄存器的写操作完成   0:正在进行   1:已完成
    }
    return 0;
}



/******************************************************************************
 * 函  数： RTC_SetDate
 * 功  能： 设置日期、时间
 *
 * 参  数：
 * 返回值：　0:成功   1:失败
 * 备  注：
 ******************************************************************************/
uint8_t RTC_SetDate(uint16_t year, uint8_t mon, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint32_t seccount = 0;

    if (year < 1970 || year > 2099)   return 1;      // 检查年参数正确性
    for (uint16_t t = 1970; t < year; t++)                // 累加年份的秒钟数
    {
        if (isLeapYear(t))  seccount += 31622400;    // 闰年的秒钟数
        else                 seccount += 31536000;   // 平年的秒钟数
    }

    mon -= 1;
    for (uint16_t t = 0; t < mon; t++)                    // 累加月份的秒钟数
    {
        seccount += (uint32_t)mon_table[t] * 86400;       // 月份秒钟数相加
        if (isLeapYear(year) && t == 1)seccount += 86400; // 闰年2月份增加一天的秒钟数
    }

    seccount += (uint32_t)(day - 1) * 86400;              // 累加日期的秒钟数
    seccount += (uint32_t)hour * 3600;                    // 累加小时秒钟数
    seccount += (uint32_t)min * 60;                       // 累加分钟秒钟数
    seccount += sec;                                 // 累加秒钟数

    RCC->APB1ENR |= 1 << 28;                         // 使能电源时钟
    RCC->APB1ENR |= 1 << 27;                         // 使能备份时钟
    PWR->CR      |= 1 << 8;                          // 取消备份区写保护

    RTC->CRL     |= 1 << 4;                          // 配置标志, 0:退出配置,RTC开始更新，　1:进入配置
    RTC->CNTL     = seccount & 0xffff;
    RTC->CNTH     = seccount >> 16;
    RTC->CRL     &= ~(1 << 4);                       // 配置标志, 0:退出配置,RTC开始更新，　1:进入配置
    while ((RTC->CRL & (1 << 5)) == 0);              // 等待上一次对RTC寄存器的写操作完成     0:正在进行   1:已完成
    BKP->DR1 = 0X1314;                         // 在备份数据域第一个寄存器写入配置标志值(共42个)，方便复位后判断是否需要初始化
    RTC_GetDate();                                   // 获取日期、时间数据，存放到结构体xRTC中
    return 0;
}



/******************************************************************************
 * 函  数： RTC_GetDate
 * 功  能： 获取日期、时间数据，存放到结构体xRTC中
 *
 * 参  数：
 * 返回值：　0:成功   1:失败
 * 备  注：
 ******************************************************************************/
uint8_t RTC_GetDate(void)
{
    static uint16_t daycnt = 0;
    uint32_t timecount = 0;
    uint32_t temp = 0;
    uint16_t temp1 = 0;

    timecount = RTC->CNTH;                            // 得到计数器中的值, 注意:这个值包括了年月日时分秒，要分拆计算
    timecount <<= 16;
    timecount += RTC->CNTL;

    temp = timecount / 86400;                         // 得到天数(秒钟数对应的)
    if (daycnt != temp)                               // 超过一天了
    {
        daycnt = temp;
        temp1 = 1970;                                 // 从1970年开始
        while (temp >= 365)
        {
            if (isLeapYear(temp1))                    // 是闰年
            {
                if (temp >= 366)temp -= 366;          // 闰年的秒钟数
                else break;
            }
            else temp -= 365;                         // 平年
            temp1++;
        }
        xRTC.Year = temp1;                            // 得到年份
        temp1 = 0;
        while (temp >= 28)                            // 超过了一个月
        {
            if (isLeapYear(xRTC.Year) && temp1 == 1)  // 当年是不是闰年/2月份
            {
                if (temp >= 29)temp -= 29;            // 闰年的秒钟数
                else break;
            }
            else
            {
                if (temp >= mon_table[temp1])temp -= mon_table[temp1]; //平年
                else break;
            }
            temp1++;
        }
        xRTC.Month = temp1 + 1;                       // 得到月份
        xRTC.Day = temp + 1;                          // 得到日期
    }
    temp = timecount % 86400;                         // 得到秒钟数
    xRTC.Hour = temp / 3600;                          // 小时
    xRTC.Minute = (temp % 3600) / 60;                 // 分钟
    xRTC.Second = (temp % 3600) % 60;                 // 秒钟
    xRTC.Week = getWeek(xRTC.Year, xRTC.Month, xRTC.Day);     // 获取星期

    switch (xRTC.Week)
    {
    case 0:
        xRTC.WeekString = WEEK_0;
        break;
    case 1:
        xRTC.WeekString = WEEK_1;
        break;
    case 2:
        xRTC.WeekString = WEEK_2;
        break;
    case 3:
        xRTC.WeekString = WEEK_3;
        break;
    case 4:
        xRTC.WeekString = WEEK_4;
        break;
    case 5:
        xRTC.WeekString = WEEK_5;
        break;
    case 6:
        xRTC.WeekString = WEEK_6;
        break;
    default :
        xRTC.WeekString = WEEK_0;
        break;



    }
    return 0;
}





