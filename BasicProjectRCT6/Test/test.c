
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
        // ����1�������ַ���
        USART1_SendString("����1�������ַ�����Test.\r\n");
    }
    if(Key_GetNum() == KEY_2_NUM)
	{
        // ����2�����͸�����ֵ����, ע�⣬�����������16������ʾ
        uint8_t data[30];             // ��Ҫ���͵����ݻ���
        char *strTemp = "����2��";    // �ַ���
        data[0] = *strTemp++;         // ���ֵ�GBK���룬ʹ��2���ֽڱ�������ʾһ������
        data[1] = *strTemp++;
        data[2] = *strTemp++;
        data[3] = *strTemp++;
        data[4] = *strTemp++;
        data[5] = *strTemp++;
        data[6] = *strTemp++;
        data[7] = 123;                // ���ͣ�ע�⣺�����ASCII��ʽ��ʾ�������ϣ���Ϊ��{��
        data[8] = '5';                // �ַ�
        data[9] = '2';                // �ַ�
        data[10] = '0';               // �ַ�
        data[11] =  5 ;               // ���ͣ�ע�⣺������ASCII����ǿ���ʾ�ķ�Χ�ڣ��ڴ������ϣ���ASCII��ʽ���޷���ʾ. Ҫ��16������ʾ���ܿ�������
        data[12] =  2 ;               // ����
        data[13] =  0 ;               // ����
        data[14] = 0x05;              // ���ͣ�ע�⣺�ܶ���û�����16���ƺ�ʮ���Ƶ����������ݴ����ϣ���û������ģ��·�������ֵ����ͬ���Ϸ�5,2,0������ֵ
        data[15] = 0x02;              // ����
        data[16] = 0x00;              // ����

        USART1_SendData(data, 20);    // USART1_SendData���������Ժ����ط��͸�����������
    }
    
    /******  ����UART���� ******/
    if (xUSART.USART1ReceivedNum)                                    // �ж��Ƿ���յ������ݣ������ֽڳ���>0
    {
        printf("\r<<<<<< UART ���յ������� <<<<<<");                 // ׼�����
        printf("\r ����(�ֽ���)��%d", xUSART.USART1ReceivedNum);     // ����; ��λ:�ֽ�
        printf("\r ����(16����)��");                                 // 16���Ʒ�ʽ��ʾ���ݣ�����۲���ʵ����
        for (uint8_t i = 0; i < xUSART.USART1ReceivedNum; i++)       // ���ÿһ�ֽ���ֵ
            printf("0x%X ", xUSART.USART1ReceivedData[i]);
        printf("\r ����(ASCII) ��%s\r", xUSART.USART1ReceivedData);  // ASCII��ʽ��ʾ���ݣ�����۲��ַ�������;

        xUSART.USART1ReceivedNum = 0;                                // ��Ҫ�������������ˣ��ѽ��������声
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
