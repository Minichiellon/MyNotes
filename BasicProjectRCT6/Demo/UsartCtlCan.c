#include "stm32f10x.h"                  // Device header
#include "system_f103.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bsp_usart.h"
#include "bsp_can.h"
extern void delay_ms(uint32_t ms);

void ProcessSingleLine(char *line) {
    // 跳过行首空白字符
    while(*line == ' ' || *line == '\r') line++;
    
    char *ptr = strtok(line, " ");
    if(!ptr) return;

    // 解析命令位
    uint8_t command = atoi(ptr);
    if(command != 0 && command != 1) return;

    // 解析CAN ID（关键修改）
    ptr = strtok(NULL, " ");
    if(!ptr) return;
    
    // 使用strtoul直接转换完整十六进制字符串
    uint32_t can_id = strtoul(ptr, NULL, 16);
    
    // 验证ID范围（标准帧ID范围0x000-0x7FF）
    if(can_id > 0x7FF) return;

    // 解析数据段（保持原逻辑）
    uint8_t can_data[8];
    for(int i = 0; i < 8; i++) {
        ptr = strtok(NULL, " ");
        if(!ptr) return;
        can_data[i] = (uint8_t)strtoul(ptr, NULL, 16);
    }
	
	if(command == 1) {
        CAN1_SendData(can_id, can_data); // 假设CAN发送函数已实现
		delay_ms(1);
    }
}

void ProcessUartData(uint8_t *data, uint16_t len) {
    char *saveptr; // 用于strtok_r的上下文指针
    char *line = strtok_r((char *)data, "\r\n", &saveptr);
    
    while(line != NULL) {
        // 跳过空行
        if(strlen(line) < 20) { // 最小有效行长度检查
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
