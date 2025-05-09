 /***********************************************************************************************************************************
 ** 购买链接】  魔女开发板    https://demoboard.taobao.com
 ***********************************************************************************************************************************
 **【文件名称】  bsp_IICSoft.c
 **【功能描述】  模拟IIC时序
 **              定义功能函数
 **            
 **【函数说明】  
 **              
 ** 
 **【更新记录】  2020-03-05  创建
 **              2021-05-03  完善文件格式、注释格式
 **
***********************************************************************************************************************************/
#include "bsp_IICSoft.h"



/************************************************
 ** 电平控制简化_宏定义
 ***********************************************/
#define SCL_L      (I2C_MONI_SCL_GPIO->BSRR = (I2C_MONI_SCL_PIN << 16))
#define SCL_H      (I2C_MONI_SCL_GPIO->BSRR =  I2C_MONI_SCL_PIN)
#define SDA_L      (I2C_MONI_SDA_GPIO->BSRR = (I2C_MONI_SDA_PIN << 16))
#define SDA_H      (I2C_MONI_SDA_GPIO->BSRR =  I2C_MONI_SDA_PIN)
#define SDA_READ   ((I2C_MONI_SDA_GPIO->IDR & I2C_MONI_SDA_PIN)?1:0)       // 不用改成输入模式就能读



/*****************************************************************************
 ** 内部函数
*****************************************************************************/
static void     delayUs(uint32_t time); // 简单延时,不准确

static void     isBusy (void);          // 检测总线是否空闲
static void     start  (void);          // 起始信号
static void     stop   (void);          // 停止信号
static void     ackYes (void);          // 发送应答信号
static void     ackNo  (void);          // 不发送应答信号
static uint8_t  waitForAck(void);       // 等待应答信号

static void     sendByte(uint8_t data);      // 发送一个字节
static uint8_t  readByte(uint8_t ack);       // 读取一个字节


/*****************************************************************************
*函  数：IICSoft_Init
*功  能：初始化模拟IIC引脚
*参  数：
*返回值： 
*备  注：
*****************************************************************************/
void IICSoft_Init(void)
{
    // 使能SCL引脚端口时钟
    if(I2C_MONI_SCL_GPIO == GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    if(I2C_MONI_SCL_GPIO == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    if(I2C_MONI_SCL_GPIO == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
    if(I2C_MONI_SCL_GPIO == GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD , ENABLE);
    if(I2C_MONI_SCL_GPIO == GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);
    if(I2C_MONI_SCL_GPIO == GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF , ENABLE);
    if(I2C_MONI_SCL_GPIO == GPIOG)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG , ENABLE);
    // 使能SDA引脚端口时钟
    if(I2C_MONI_SDA_GPIO == GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    if(I2C_MONI_SDA_GPIO == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    if(I2C_MONI_SDA_GPIO == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
    if(I2C_MONI_SDA_GPIO == GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD , ENABLE);
    if(I2C_MONI_SDA_GPIO == GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);
    if(I2C_MONI_SDA_GPIO == GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF , ENABLE);
    if(I2C_MONI_SDA_GPIO == GPIOG)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG , ENABLE);
    
    // 配置引脚工作模式: 开漏输出
    GPIO_InitTypeDef G;
    G.GPIO_Pin   = I2C_MONI_SCL_PIN;
    G.GPIO_Mode  = GPIO_Mode_Out_OD;
    G.GPIO_Speed = GPIO_Speed_50MHz ;
    GPIO_Init(I2C_MONI_SCL_GPIO, &G);
    delayUs(2);
    
    G.GPIO_Pin   = I2C_MONI_SDA_PIN;
    GPIO_Init(I2C_MONI_SDA_GPIO, &G);
    delayUs(2);    
    	
	SCL_L ;   
    delayUs(2);
    SDA_H ;    
}


// 粗略延时************************
static void delayUs( u32 times)
{
    times=times*7;    
	while(--times)  	   
        __nop();  
}



// 判断总线是否处于空闲状态，若不是，则循环的提供SCL时钟驱动，直到从机释放SDA线
static void isBusy(void)
{
    uint32_t i=500;                  // 大约3ms  
    //SDA_IN ();
    while((SDA_READ) == 0 && (--i))  // 读取sda线上的电平状态，低电平说明总线被从机控制，高电平则说明总线空闲，可以准备发送开始信号
    { 
        SCL_L;
        delayUs(3);        
        SCL_H ;
        delayUs(3);             
    }     
    if(i==0)    printf("\r\n IIC总线超时错误!! \r\n");   // 错误提示
}



/*****************************************************************************
 * 函  数：start
 * 功  能：开始信号 （SCL高电平期间，SDA由高向低 跳变)
*****************************************************************************/
static void start(void)
{     
    isBusy ();               // 判断总线是否处于空闲状态 
  
    SCL_L;   delayUs(3);   
    SDA_H;   delayUs(3);       

    SCL_H;   delayUs(3);      
    SDA_L;   delayUs(3);     // 在SCL高电平其间， SDA由高向低 跳变        
    SCL_L;   delayUs(3);     // 将SCL拉低,钳住SCL线,准备发送数据          
}



/*****************************************************************************
 * 函  数：stop
 * 功  能：产生停止信号 （在SCL高电平其间， SDA由低向高 跳变）
*****************************************************************************/
static void stop(void)
{
    SCL_L;   delayUs(3);       
    SDA_L;   delayUs(3);
    
    SCL_H;   delayUs(3);  
    SDA_H ;  delayUs(1);     // 在SCL高电平其间， SDA由低向高 跳变        
}



/*****************************************************************************
 * 函  数：ackYes
 * 功  能：主机产生应答信号 （在第9个时钟周期SAL高电平期间, SDA保持低电平）
 *         主机接收完一个字节数据后，主机产生ACK通知从机一个字节数据已正确接收
*****************************************************************************/
static void ackYes(void)
{
    SCL_L;    delayUs( 3); 
    SDA_L;    delayUs( 3);   
    
    SCL_H;    delayUs(10);   // 使SDA保持一个时钟的低电平    
    SCL_L;    delayUs( 3);   
    
    SDA_H ;   delayUs( 3);   // 很重要: 在这里释放SDA线,从机才能获取控制权 
}



/*****************************************************************************
 * 函  数：ackNo
 * 功  能：主机产生非应答信号 （在第9个时钟周期SAL高电平期间, SDA保持高电平）
 *         主机接收完一个字节数据后，主机产生NACK通知从机发送线束，以便主机产生停止信号
*****************************************************************************/
static void ackNo(void)
{
    SCL_L;    delayUs( 3); 
    SDA_H;    delayUs( 3); 
    
    SCL_H;    delayUs(10);   // 使SDA保持一个时钟的高电平   
    SCL_L;    delayUs( 3);  
    
    SDA_H;    delayUs( 3);   // 很重要: 在这里释放SDA线,从机才能获取控制权  
}


/*****************************************************************************
 * 函  数：waitForAck
 * 功  能：获取应答信号或者非应答信号
 *         有效应答信号：从机第9个SCL=0时，SDA被从机拉低，并且SCL=1时，SDA依然为低
 * 
 * 返回值： 0=收到应答信号,   1=收到非应答信号
 * 备  注：
*****************************************************************************/
static uint8_t waitForAck(void)
{    
    uint8_t  d=0;
    
    SCL_L;    delayUs( 3);
    SDA_H;    delayUs( 3);        // 主机释放SDA线，使总线空闲，以便从机能够响应信息
    
    SCL_H;    delayUs(10);        // 等待10us; 按标准模式100kbit/s 
    d=SDA_READ;
    
    SCL_L;    delayUs( 3);         
    SDA_H;    delayUs( 3);        // 主机释放SDA线，使总线空闲，以便从机能够响应信息
        
    if(d)
        return 1;                 // 返回非应答信号     
    else
        return 0;                 // 返回应答信号   
}



/*****************************************************************************
 * 函  数： sendByte
 * 功  能： 输出一个字节
 * 参  数：   
 * 返回值： 0=收到应答信号,   1=收到非应答信号
 * 备  注：
*****************************************************************************/
static void sendByte(uint8_t data)
{   
    for(uint8_t i=0; i<8; i++)
    {             
        SCL_L;     delayUs( 3);     
        
        (data&0x80)? SDA_H : SDA_L;      
        data<<=1;  delayUs( 3);    
        
        SCL_H;     delayUs( 3);        
    } 
    
    SCL_L;    delayUs( 3); 
    SDA_H;    delayUs( 3);      // 主机释放SDA线，使用总线空闲，以便从机能够发出响应信息，并钳信SCL线        
}

/*****************************************************************************
 * 函  数： readByte
 * 功  能： 读取一个字节
 * 参  数： ack=0：主机还没接收完， ack=1:主机数据已全部接收完成 
 * 返回值： 读取到的Byte值
 * 备  注： 读取从机发送的数据后，并决定发出应答信号还是非应答信号
*****************************************************************************/
static uint8_t readByte(uint8_t ack)
{
    uint8_t data=0;
    
    for(uint8_t i=0; i<8; i++)
    {            
        SCL_H;    delayUs( 3);
        data<<=1;
        if(SDA_READ)  data|=0x01;            
        SCL_L;    delayUs( 3);  
    }
    
    if(ack) ackNo ();               // 1, 不应答
    else    ackYes();               // 0, 应答
    
    return  data;       
}



/*****************************************************************************
 * 函  数： IICSoft_ReadByte
 * 功  能： 向指定从机设备，读取指定地址的一个值， 单位：字节
 * 参  数： slave: 从机地址
 *          addr： 数据地址
 *          *buf： 数据要存储的地址
 * 返回值： 0=成功， 1=失败
 * 备  注： 注意，时序有点特别
*****************************************************************************/
uint8_t IICSoft_ReadByte(uint8_t slave, uint8_t addr, uint8_t *buf)
{        
    start ();                     // 起始信号    
    sendByte (slave<<1 | 0);      // 从机地址,　写方向 , 0写1读    
    if(waitForAck())
    {
        stop();
        return 1;
    }
    
    sendByte(addr);               // 数据地址
    if(waitForAck())
    {
        stop();
        return 1;
    }
    
    // 写数据和读数据最大的时序区别， 写比较简单，读数据：再发一次起始信号、有读方向的从机地址
    start();                      // 起始信号
    sendByte(slave<<1 | 1);       // 从机地址,　读方向 ，  0写1读
    if(waitForAck())
    {
        stop();
        return 1;
    }
    
    *buf=readByte(1);             // 读值, 并发送ackNo;
    stop();
    
    return 0;
}      



/*****************************************************************************
 * 函  数： IICSoft_ReadBueffer
 * 功  能： 向指定从机设备，读取指定地址的多个值， 单位：字节
 * 参  数： uint8_t slave   从机地址
 *          uint8_t addr    数据地址
 *          uint8_t data    要写入的数据(1字节)
 * 返回值： 0=成功， 1=失败
 * 备  注： 注意，时序有点特别
*****************************************************************************/
uint8_t IICSoft_ReadBueffer(uint8_t slave, uint8_t addr, uint8_t *buf, uint8_t len)
{    
    start ();                     // 起始信号    
    sendByte (slave<<1 | 0);      // 从机地址,　写方向 , 0写1读    
    if(waitForAck())              // 等待ACK
    {
        stop();
        return 1;
    }
    
    sendByte(addr);               // 数据地址
    if(waitForAck())              // 等待ACK
    {
        stop();
        return 1;
    }
    
    // 写、读时序区别， 读数据：再发一次起始信号、有读方向的从机地址
    start();                      // 起始信号
    sendByte(slave<<1 | 1);       // 从机地址,　读方向 ，  0写1读
    if(waitForAck())              // 等待ACK
    {
        stop();
        return 1;
    }
    
  
    while(len)
    {      
        if(len==1)  *buf=readByte(1);   // 最后一个字节，读数据后产生NACK
        else        *buf=readByte(0);   // 读数据后产生ACK
    
        len--;
        buf++;
    }  
    
    stop ();                      // 产生一个停止信号
    return 0;   
}      


/*****************************************************************************
 * 函  数： IICSoft_WriteByte
 * 功  能： 往从机某地址写入一个字节
 * 参  数： uint8_t slave   从机地址
 *          uint8_t addr    数据地址
 *          uint8_t data    要写入的数据(1字节)
 * 返回值： 0=成功， 1=失败
 * 备  注： 时序与读是不同的
*****************************************************************************/
uint8_t IICSoft_WriteByte(uint8_t slave, uint8_t addr, uint8_t data)
{
    start ();                     // 起始信号    
    sendByte (slave<<1 | 0);      // 从机地址,　写方向 , 0写1读    
    if(waitForAck())
    {
        stop();
        return 1;                 // 若从机地址发送失败，就返回
    }
    
    sendByte(addr);               // 数据地址
    if(waitForAck())
    {
        stop();
        return 1;                 // 若发送失败，就返回
    }    
    
    sendByte(data);               // 发送数据， 写数据和读数据最大的时序区别在这一步前
    if(waitForAck())
    {
        stop ();
        return 1;                 // 数据写入失败
    }
    
    stop();                       // 写入完成，产生停止信号
    return 0;    
}

// 往从机写多个字节数据
uint8_t IICSoft_WriteBuffer(uint8_t slave, uint8_t addr, uint8_t *buf, uint8_t len)
{ 
    start ();                     // 起始信号    
    sendByte (slave<<1 | 0);      // 从机地址,　写方向 , 0写1读    
    if(waitForAck())
    {
        stop();
        return 1;                 // 若发送失败，就返回
    }  
    
    sendByte(addr);               // 数据地址
    if(waitForAck())
    {
        stop();
        return 1;                 // 若发送失败，就返回
    }  
    
    for(uint8_t i=0; i<len; i++)
    {
        sendByte(buf[i]);         // 数据
        if(waitForAck())          // 每一个字节都要等从机应答
        {
            stop ();
            return 1;             // 数据写入失败
        }
    }
    
    stop();                       // 写入完成，产生停止信号
    return 0;    
}             
        



