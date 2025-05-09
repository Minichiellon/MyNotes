#include "bsp_can.h"


xCAN_InfoDef  xCAN;         // 声明为全局变量,方便记录信息、状态




void CAN1_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 
    CAN_InitTypeDef  CAN_InitStructure;                  
    
    // 时钟使能            
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1 , ENABLE);                
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);           
      
    // 初始化GPIO            
    GPIO_InitStructure.GPIO_Pin   = CAN1_TX_PIN ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;     
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CAN1_TX_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = CAN1_RX_PIN ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;     // CAN_RX引脚模式：浮空输入或上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CAN1_RX_GPIO, &GPIO_InitStructure);
           
    // 初始化CAN外设、波特率、位的组成
                
    CAN_InitStructure.CAN_ABOM = ENABLE;               // 自动离线管理
    CAN_InitStructure.CAN_AWUM = ENABLE;               // 自动唤醒
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal ;     // 工作模式，　Normal=正常, LoopBack=回环, Silent=静默, Silent_LoopBack=静默回环
    CAN_InitStructure.CAN_NART = ENABLE;               // 错误重传
    CAN_InitStructure.CAN_RFLM = ENABLE;               // FIFO锁定，新的不能覆盖旧的
    CAN_InitStructure.CAN_TTCM = DISABLE;              // 时间触发
    CAN_InitStructure.CAN_TXFP = DISABLE;              // 按报文存入邮箱的先后顺序发送，否则按默认ID优先级发送
    // CAN波特率=1Mbps=1tq*(1+BS1+BS2)=(BRR*Tpclk)*(1+BS1+BS2)
    CAN_InitStructure.CAN_Prescaler = 48;               // 分频系数
    CAN_InitStructure.CAN_SJW = CAN_SJW_2tq;           // 不影响位时序，影响同步效率
    CAN_InitStructure.CAN_BS1 = CAN_BS1_2tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;    
    CAN_Init(CAN1, &CAN_InitStructure);                // 初始化CAN工作模式
    
    // 配置筛选器，这里示范只配置一个过滤器，接收到的数据存放到FIFO0     
    CAN_FilterInitTypeDef CAN_FilterInitTypeStruct;    
    CAN_FilterInitTypeStruct.CAN_FilterActivation =ENABLE;                 // 使能过滤器
    CAN_FilterInitTypeStruct.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;  // FIFO关联；接收到的数据，存放到FIFO0；仅是这个过滤器 
    CAN_FilterInitTypeStruct.CAN_FilterNumber =0;                          // 使用哪个过滤器
    CAN_FilterInitTypeStruct.CAN_FilterScale  = CAN_FilterScale_32bit ;    // 位宽
    CAN_FilterInitTypeStruct.CAN_FilterMode   = CAN_FilterMode_IdMask ;    // 过滤模式；IDMask=0=屏蔽位模式；IdList=1=列表模式
    CAN_FilterInitTypeStruct.CAN_FilterIdHigh = ((RECIVE_ID<<3 | CAN_Id_Extended | CAN_RTR_Data) & 0xFFFF0000)>>16; 
    CAN_FilterInitTypeStruct.CAN_FilterIdLow  = ((RECIVE_ID<<3 | CAN_Id_Extended | CAN_RTR_Data) & 0x0000FFFF);  
    CAN_FilterInitTypeStruct.CAN_FilterMaskIdHigh = 0x0000;    
    CAN_FilterInitTypeStruct.CAN_FilterMaskIdLow  = 0x0000;  
    CAN_FilterInit(&CAN_FilterInitTypeStruct);
    
    // 使能中断
    CAN_ITConfig (CAN1, CAN_IT_FMP0 , ENABLE);
    
    // 中断
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn ;  // 使能邮箱0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);
    
    printf("CAN 通信配置          配置完成\r\n");
}



void USB_LP_CAN1_RX0_IRQHandler(void)
{                                                          // 无需清中断标志，使用CAN_Receive()会自动清    
    CAN_Receive(CAN1, CAN_FIFO0, &xCAN.RxData);            // 把FIFO_0接收到数据，存入CanRxMsg结构体中；
    xCAN.RxFlag =1;                                        // 自定义的一个标志：数据接收完成标志，方便在外部代码中不断轮询判断
                
}


// 发送数据
// 返回：发送报文的邮箱号
uint8_t CAN1_SendData(uint32_t targetID, uint8_t* data)    // 发送数据; 为简化操作，固定为每次发送8个字节
{       
                    
    xCAN.TxData.RTR    = CAN_RTR_Data;       // 数据帧
    xCAN.TxData.IDE    = CAN_Id_Standard;    // 标准帧        
    xCAN.TxData.StdId  = targetID;                  // 标准帧ID  
    xCAN.TxData.ExtId  = targetID;           // 扩展帧ID 
    xCAN.TxData.DLC    = 8 ;                 // 数据长度(字节数，最大8个字节)，为简化函数操作，设定为8个字节
    
    xCAN.TxData.Data[0]= data[0];            // 需要发送的数据
    xCAN.TxData.Data[1]= data[1];
    xCAN.TxData.Data[2]= data[2];
    xCAN.TxData.Data[3]= data[3];
    xCAN.TxData.Data[4]= data[4];
    xCAN.TxData.Data[5]= data[5];
    xCAN.TxData.Data[6]= data[6];
    xCAN.TxData.Data[7]= data[7];                   
                
    return  CAN_Transmit(CAN1, &xCAN.TxData);   // 发送, 返回发送所用的邮箱号        
               
}



// CAN报文接收 
void CAN1_ReceiveData(uint8_t* data)    
{           
    if(xCAN.RxFlag ==1)                        // 通过接收标志xCan.RxFlag，判断是否收到新报文
    {
        printf("\r\n<<CAN接收到数据:>>  ");   
        for(uint8_t i=0; i<8; i++)  
        printf("%c",xCAN.RxData.Data[i]);  
        printf("\r\n");   
                    
        xCAN.RxFlag = 0;                       // 处理完数据了，把接收标志清0                        
    }                
}






