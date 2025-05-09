
#include "test.h"

void Key_Led_test(void)
{
    if(Key_GetNum() == KEY_1_NUM)
	{
		LED_RED_TOGGLE();
	}
	if(Key_GetNum() == KEY_2_NUM)
	{
		LED_BLUE_TOGGLE();
	}
	if(Key_GetNum() == KEY_3_NUM)
	{
		LED_RED_TOGGLE();
		LED_BLUE_TOGGLE();
	}
}

void Usart_test(void)
{
    if(Key_GetNum() == KEY_1_NUM)
	{
        // 测试1：发送字符串
        USART1_SendString("测试1：发送字符串，Test.\r\n");
    }
    if(Key_GetNum() == KEY_2_NUM)
	{
        // 测试2：发送各类数值数据, 注意，串口软件可用16进制显示
        uint8_t data[30];             // 将要发送的数据缓存
        char *strTemp = "测试2：";    // 字符串
        data[0] = *strTemp++;         // 汉字的GBK编码，使用2个字节编码来表示一个汉字
        data[1] = *strTemp++;
        data[2] = *strTemp++;
        data[3] = *strTemp++;
        data[4] = *strTemp++;
        data[5] = *strTemp++;
        data[6] = *strTemp++;
        data[7] = 123;                // 整型，注意：如果用ASCII方式显示到串口上，将为‘{’
        data[8] = '5';                // 字符
        data[9] = '2';                // 字符
        data[10] = '0';               // 字符
        data[11] =  5 ;               // 整型，注意：数据在ASCII编码非可显示的范围内，在串口软上，用ASCII方式将无法显示. 要用16进制显示才能看到数据
        data[12] =  2 ;               // 整型
        data[13] =  0 ;               // 整型
        data[14] = 0x05;              // 整型，注意：很多人没能理解16进制和十进制的区别，在数据传输上，是没有区别的，下方三个数值，等同于上方5,2,0三个数值
        data[15] = 0x02;              // 整型
        data[16] = 0x00;              // 整型

        USART1_SendData(data, 20);    // USART1_SendData函数，可以很灵活地发送各种类型数据
    }
    
    /******  处理UART数据 ******/
    if (xUSART.USART1ReceivedNum)                                    // 判断是否接收到新数据：接收字节长度>0
    {
        printf("\r<<<<<< UART 接收到新数据 <<<<<<");                 // 准备输出
        printf("\r 长度(字节数)：%d", xUSART.USART1ReceivedNum);     // 长度; 单位:字节
        printf("\r 数据(16进制)：");                                 // 16进制方式显示数据，方便观察真实数据
        for (uint8_t i = 0; i < xUSART.USART1ReceivedNum; i++)       // 输出每一字节数值
            printf("0x%X ", xUSART.USART1ReceivedData[i]);
        printf("\r 数据(ASCII) ：%s\r", xUSART.USART1ReceivedData);  // ASCII方式显示数据，方便观察字符串数据;

        xUSART.USART1ReceivedNum = 0;                                // 重要：处理完数据了，把接收数量清０
    }
}

void Can_test(void)
{
    uint32_t ID = 0x480;
    uint8_t data[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    if(Key_GetNum() == KEY_3_NUM)
	{
        CAN1_SendData(ID, data);
    }
}
