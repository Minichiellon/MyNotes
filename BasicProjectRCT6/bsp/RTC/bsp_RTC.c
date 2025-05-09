#include "bsp_RTC.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_rcc.h"


#define WEEK_0  "��"
#define WEEK_1  "һ"
#define WEEK_2  "��"
#define WEEK_3  "��"
#define WEEK_4  "��"
#define WEEK_5  "��"
#define WEEK_6  "��"



xRTC_TypeDef  xRTC;                                   // ����Ϊȫ�ֱ���,�����¼��Ϣ��״̬

uint8_t const table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};            // ���������ݱ�
const uint8_t mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // ƽ����·����ڱ�



static void delay_ms(uint32_t ms)                     // ����һ��ms��ʱ������������ֲʱ���ⲿ�ļ�����;
{
    ms = ms * 10286;                                  // ע�⣺�� "Options --> C/C++ ---> One ELF Section per Functionѡ��"
    for (uint32_t i = 0; i < ms; i++);                // 72MHzϵͳʱ���£���Լ���ٸ���ѭ����ʱ1ms
}



// ����ֵ:������ǲ�������.1,��.0,����
uint8_t isLeapYear(uint16_t year)
{
    if (year % 4 == 0)                     // �����ܱ�4����
    {
        if (year % 100 == 0)
        {
            if (year % 400 == 0) return 1; // �����00��β,��Ҫ�ܱ�400����
            else            return 0;
        }
        else
            return 1;
    }
    else
        return 0;
}

// ������������ڼ�
// year,month,day������������
// ����ֵ�����ڼ�
uint8_t getWeek(uint16_t year, uint8_t month, uint8_t day)
{
    uint16_t temp2;
    uint8_t yearH, yearL;

    yearH = year / 100;
    yearL = year % 100;
    // ���Ϊ21����,�������100
    if (yearH > 19)yearL += 100;
    // ����������ֻ��1900��֮���
    temp2 = yearL + yearL / 4;
    temp2 = temp2 % 7;
    temp2 = temp2 + day + table_week[month - 1];
    if (yearL % 4 == 0 && month < 3)temp2--;
    return (temp2 % 7);
}



/******************************************************************************
 * ��  ���� RTC_Init
 * ��  �ܣ� ��鲢����RTC
 *
 * ��  ����
 * ����ֵ����0:�ɹ�   1:ʧ��
 * ��  ע��
 ******************************************************************************/
uint8_t RTC_Init(void)
{
    uint8_t temp = 0;
    if (BKP->DR1 != 0X1314)                        // ��ȡ�������ݣ����ж�RTC�Ƿ�����������(ֻҪVBAT���ֹ�����ѹ,RTC�ͺ�����ϵͳ����Ӱ��)
    {
        RCC->APB1ENR |= 1 << 28;                   // ʹ�� ��Դʱ��
        RCC->APB1ENR |= 1 << 27;                   // ʹ�� ����ʱ��
        PWR->CR      |= 1 << 8;                    // ȡ��������д����
        RCC->BDCR    |= 1 << 16;                   // ����������λ
        RCC->BDCR    &= ~(1 << 16);                // ����������λ����
        RCC->BDCR    |= 1 << 0;                    // �����ⲿ��������
        while ((!(RCC->BDCR & 0X02)) && (++temp) < 250) // �ȴ��ⲿʱ�Ӿ���
            delay_ms(10);
        if (temp >= 250)   return 1;               // ��ʼ��ʱ��ʧ��,����������
        RCC->BDCR |= 1 << 8;                       // RTCʱ��Դѡ��   00:�ޡ���01:LSE   10:LSI   11:HSE/128
        RCC->BDCR |= 1 << 15;                      // RTCʱ��ʹ��

        while ((RTC->CRL & (1 << 5)) == 0);        // �ȴ���һ�ζ�RTC�Ĵ�����д�������   0:���ڽ���   1:�����
        while ((RTC->CRL & (1 << 3)) == 0);        // �ȴ�RTC�Ĵ���ͬ��    0:δͬ��   2:��ͬ��
        RTC->CRL |= 1 << 4;                        // ���ñ�־, 0:�˳�����,RTC��ʼ���£���1:��������
        RTC->PRLH = 0X0000;
        RTC->PRLL = 32767;                         // ʱ����������(�д��۲�,���Ƿ�������?)����ֵ��32767

        RTC_SetDate(1900, 1, 1, 1, 1, 1);          // �������ڡ�ʱ��
        RTC->CRL &= ~(1 << 4);                     // ���ñ�־, 0:�˳�����,RTC��ʼ���£���1:��������
        while ((RTC->CRL & (1 << 5)) == 0);        // �ȴ���һ�ζ�RTC�Ĵ�����д�������   0:���ڽ���   1:�����
        BKP->DR1 = 0X1314;                         // �ڱ����������һ���Ĵ���д�����ñ�־ֵ(��42��)�����㸴λ���ж��Ƿ���Ҫ��ʼ��
    }
    else                                           // �ɹ���ȡ�������ݱ�־����ʾRTC������������
    {
        while ((RTC->CRL & (1 << 3)) == 0);        // �ȴ�RTC�Ĵ���ͬ��    0:δͬ��   2:��ͬ��
        while ((RTC->CRL & (1 << 5)) == 0);        // �ȴ���һ�ζ�RTC�Ĵ�����д�������   0:���ڽ���   1:�����
    }
    return 0;
}



/******************************************************************************
 * ��  ���� RTC_SetDate
 * ��  �ܣ� �������ڡ�ʱ��
 *
 * ��  ����
 * ����ֵ����0:�ɹ�   1:ʧ��
 * ��  ע��
 ******************************************************************************/
uint8_t RTC_SetDate(uint16_t year, uint8_t mon, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint32_t seccount = 0;

    if (year < 1970 || year > 2099)   return 1;      // ����������ȷ��
    for (uint16_t t = 1970; t < year; t++)                // �ۼ���ݵ�������
    {
        if (isLeapYear(t))  seccount += 31622400;    // �����������
        else                 seccount += 31536000;   // ƽ���������
    }

    mon -= 1;
    for (uint16_t t = 0; t < mon; t++)                    // �ۼ��·ݵ�������
    {
        seccount += (uint32_t)mon_table[t] * 86400;       // �·����������
        if (isLeapYear(year) && t == 1)seccount += 86400; // ����2�·�����һ���������
    }

    seccount += (uint32_t)(day - 1) * 86400;              // �ۼ����ڵ�������
    seccount += (uint32_t)hour * 3600;                    // �ۼ�Сʱ������
    seccount += (uint32_t)min * 60;                       // �ۼӷ���������
    seccount += sec;                                 // �ۼ�������

    RCC->APB1ENR |= 1 << 28;                         // ʹ�ܵ�Դʱ��
    RCC->APB1ENR |= 1 << 27;                         // ʹ�ܱ���ʱ��
    PWR->CR      |= 1 << 8;                          // ȡ��������д����

    RTC->CRL     |= 1 << 4;                          // ���ñ�־, 0:�˳�����,RTC��ʼ���£���1:��������
    RTC->CNTL     = seccount & 0xffff;
    RTC->CNTH     = seccount >> 16;
    RTC->CRL     &= ~(1 << 4);                       // ���ñ�־, 0:�˳�����,RTC��ʼ���£���1:��������
    while ((RTC->CRL & (1 << 5)) == 0);              // �ȴ���һ�ζ�RTC�Ĵ�����д�������     0:���ڽ���   1:�����
    BKP->DR1 = 0X1314;                         // �ڱ����������һ���Ĵ���д�����ñ�־ֵ(��42��)�����㸴λ���ж��Ƿ���Ҫ��ʼ��
    RTC_GetDate();                                   // ��ȡ���ڡ�ʱ�����ݣ���ŵ��ṹ��xRTC��
    return 0;
}



/******************************************************************************
 * ��  ���� RTC_GetDate
 * ��  �ܣ� ��ȡ���ڡ�ʱ�����ݣ���ŵ��ṹ��xRTC��
 *
 * ��  ����
 * ����ֵ����0:�ɹ�   1:ʧ��
 * ��  ע��
 ******************************************************************************/
uint8_t RTC_GetDate(void)
{
    static uint16_t daycnt = 0;
    uint32_t timecount = 0;
    uint32_t temp = 0;
    uint16_t temp1 = 0;

    timecount = RTC->CNTH;                            // �õ��������е�ֵ, ע��:���ֵ������������ʱ���룬Ҫ�ֲ����
    timecount <<= 16;
    timecount += RTC->CNTL;

    temp = timecount / 86400;                         // �õ�����(��������Ӧ��)
    if (daycnt != temp)                               // ����һ����
    {
        daycnt = temp;
        temp1 = 1970;                                 // ��1970�꿪ʼ
        while (temp >= 365)
        {
            if (isLeapYear(temp1))                    // ������
            {
                if (temp >= 366)temp -= 366;          // �����������
                else break;
            }
            else temp -= 365;                         // ƽ��
            temp1++;
        }
        xRTC.Year = temp1;                            // �õ����
        temp1 = 0;
        while (temp >= 28)                            // ������һ����
        {
            if (isLeapYear(xRTC.Year) && temp1 == 1)  // �����ǲ�������/2�·�
            {
                if (temp >= 29)temp -= 29;            // �����������
                else break;
            }
            else
            {
                if (temp >= mon_table[temp1])temp -= mon_table[temp1]; //ƽ��
                else break;
            }
            temp1++;
        }
        xRTC.Month = temp1 + 1;                       // �õ��·�
        xRTC.Day = temp + 1;                          // �õ�����
    }
    temp = timecount % 86400;                         // �õ�������
    xRTC.Hour = temp / 3600;                          // Сʱ
    xRTC.Minute = (temp % 3600) / 60;                 // ����
    xRTC.Second = (temp % 3600) % 60;                 // ����
    xRTC.Week = getWeek(xRTC.Year, xRTC.Month, xRTC.Day);     // ��ȡ����

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





