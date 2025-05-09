#include "stm32f10x.h"                  // Device header
#include "system_f103.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bsp_usart.h"
#include "bsp_can.h"
extern void delay_ms(uint32_t ms);

void ProcessSingleLine(char *line) {
    // �������׿հ��ַ�
    while(*line == ' ' || *line == '\r') line++;
    
    char *ptr = strtok(line, " ");
    if(!ptr) return;

    // ��������λ
    uint8_t command = atoi(ptr);
    if(command != 0 && command != 1) return;

    // ����CAN ID���ؼ��޸ģ�
    ptr = strtok(NULL, " ");
    if(!ptr) return;
    
    // ʹ��strtoulֱ��ת������ʮ�������ַ���
    uint32_t can_id = strtoul(ptr, NULL, 16);
    
    // ��֤ID��Χ����׼֡ID��Χ0x000-0x7FF��
    if(can_id > 0x7FF) return;

    // �������ݶΣ�����ԭ�߼���
    uint8_t can_data[8];
    for(int i = 0; i < 8; i++) {
        ptr = strtok(NULL, " ");
        if(!ptr) return;
        can_data[i] = (uint8_t)strtoul(ptr, NULL, 16);
    }
	
	if(command == 1) {
        CAN1_SendData(can_id, can_data); // ����CAN���ͺ�����ʵ��
		delay_ms(1);
    }
}

void ProcessUartData(uint8_t *data, uint16_t len) {
    char *saveptr; // ����strtok_r��������ָ��
    char *line = strtok_r((char *)data, "\r\n", &saveptr);
    
    while(line != NULL) {
        // ��������
        if(strlen(line) < 20) { // ��С��Ч�г��ȼ��
            line = strtok_r(NULL, "\r\n", &saveptr);
            continue;
        }
        
        ProcessSingleLine(line);
        line = strtok_r(NULL, "\r\n", &saveptr);
    }
}

void UsartCtlCan(void)
{
    if(xUSART.USART1ReceivedNum > 0)
    {
        ProcessUartData(xUSART.USART1ReceivedData, xUSART.USART1ReceivedNum);
        xUSART.USART1ReceivedNum = 0;
    }
}
