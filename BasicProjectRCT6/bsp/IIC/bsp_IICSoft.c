 /***********************************************************************************************************************************
 ** �������ӡ�  ħŮ������    https://demoboard.taobao.com
 ***********************************************************************************************************************************
 **���ļ����ơ�  bsp_IICSoft.c
 **������������  ģ��IICʱ��
 **              ���幦�ܺ���
 **            
 **������˵����  
 **              
 ** 
 **�����¼�¼��  2020-03-05  ����
 **              2021-05-03  �����ļ���ʽ��ע�͸�ʽ
 **
***********************************************************************************************************************************/
#include "bsp_IICSoft.h"



/************************************************
 ** ��ƽ���Ƽ�_�궨��
 ***********************************************/
#define SCL_L      (I2C_MONI_SCL_GPIO->BSRR = (I2C_MONI_SCL_PIN << 16))
#define SCL_H      (I2C_MONI_SCL_GPIO->BSRR =  I2C_MONI_SCL_PIN)
#define SDA_L      (I2C_MONI_SDA_GPIO->BSRR = (I2C_MONI_SDA_PIN << 16))
#define SDA_H      (I2C_MONI_SDA_GPIO->BSRR =  I2C_MONI_SDA_PIN)
#define SDA_READ   ((I2C_MONI_SDA_GPIO->IDR & I2C_MONI_SDA_PIN)?1:0)       // ���øĳ�����ģʽ���ܶ�



/*****************************************************************************
 ** �ڲ�����
*****************************************************************************/
static void     delayUs(uint32_t time); // ����ʱ,��׼ȷ

static void     isBusy (void);          // ��������Ƿ����
static void     start  (void);          // ��ʼ�ź�
static void     stop   (void);          // ֹͣ�ź�
static void     ackYes (void);          // ����Ӧ���ź�
static void     ackNo  (void);          // ������Ӧ���ź�
static uint8_t  waitForAck(void);       // �ȴ�Ӧ���ź�

static void     sendByte(uint8_t data);      // ����һ���ֽ�
static uint8_t  readByte(uint8_t ack);       // ��ȡһ���ֽ�


/*****************************************************************************
*��  ����IICSoft_Init
*��  �ܣ���ʼ��ģ��IIC����
*��  ����
*����ֵ�� 
*��  ע��
*****************************************************************************/
void IICSoft_Init(void)
{
    // ʹ��SCL���Ŷ˿�ʱ��
    if(I2C_MONI_SCL_GPIO == GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    if(I2C_MONI_SCL_GPIO == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    if(I2C_MONI_SCL_GPIO == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
    if(I2C_MONI_SCL_GPIO == GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD , ENABLE);
    if(I2C_MONI_SCL_GPIO == GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);
    if(I2C_MONI_SCL_GPIO == GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF , ENABLE);
    if(I2C_MONI_SCL_GPIO == GPIOG)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG , ENABLE);
    // ʹ��SDA���Ŷ˿�ʱ��
    if(I2C_MONI_SDA_GPIO == GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    if(I2C_MONI_SDA_GPIO == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    if(I2C_MONI_SDA_GPIO == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
    if(I2C_MONI_SDA_GPIO == GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD , ENABLE);
    if(I2C_MONI_SDA_GPIO == GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);
    if(I2C_MONI_SDA_GPIO == GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF , ENABLE);
    if(I2C_MONI_SDA_GPIO == GPIOG)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG , ENABLE);
    
    // �������Ź���ģʽ: ��©���
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


// ������ʱ************************
static void delayUs( u32 times)
{
    times=times*7;    
	while(--times)  	   
        __nop();  
}



// �ж������Ƿ��ڿ���״̬�������ǣ���ѭ�����ṩSCLʱ��������ֱ���ӻ��ͷ�SDA��
static void isBusy(void)
{
    uint32_t i=500;                  // ��Լ3ms  
    //SDA_IN ();
    while((SDA_READ) == 0 && (--i))  // ��ȡsda���ϵĵ�ƽ״̬���͵�ƽ˵�����߱��ӻ����ƣ��ߵ�ƽ��˵�����߿��У�����׼�����Ϳ�ʼ�ź�
    { 
        SCL_L;
        delayUs(3);        
        SCL_H ;
        delayUs(3);             
    }     
    if(i==0)    printf("\r\n IIC���߳�ʱ����!! \r\n");   // ������ʾ
}



/*****************************************************************************
 * ��  ����start
 * ��  �ܣ���ʼ�ź� ��SCL�ߵ�ƽ�ڼ䣬SDA�ɸ���� ����)
*****************************************************************************/
static void start(void)
{     
    isBusy ();               // �ж������Ƿ��ڿ���״̬ 
  
    SCL_L;   delayUs(3);   
    SDA_H;   delayUs(3);       

    SCL_H;   delayUs(3);      
    SDA_L;   delayUs(3);     // ��SCL�ߵ�ƽ��䣬 SDA�ɸ���� ����        
    SCL_L;   delayUs(3);     // ��SCL����,ǯסSCL��,׼����������          
}



/*****************************************************************************
 * ��  ����stop
 * ��  �ܣ�����ֹͣ�ź� ����SCL�ߵ�ƽ��䣬 SDA�ɵ���� ���䣩
*****************************************************************************/
static void stop(void)
{
    SCL_L;   delayUs(3);       
    SDA_L;   delayUs(3);
    
    SCL_H;   delayUs(3);  
    SDA_H ;  delayUs(1);     // ��SCL�ߵ�ƽ��䣬 SDA�ɵ���� ����        
}



/*****************************************************************************
 * ��  ����ackYes
 * ��  �ܣ���������Ӧ���ź� ���ڵ�9��ʱ������SAL�ߵ�ƽ�ڼ�, SDA���ֵ͵�ƽ��
 *         ����������һ���ֽ����ݺ���������ACK֪ͨ�ӻ�һ���ֽ���������ȷ����
*****************************************************************************/
static void ackYes(void)
{
    SCL_L;    delayUs( 3); 
    SDA_L;    delayUs( 3);   
    
    SCL_H;    delayUs(10);   // ʹSDA����һ��ʱ�ӵĵ͵�ƽ    
    SCL_L;    delayUs( 3);   
    
    SDA_H ;   delayUs( 3);   // ����Ҫ: �������ͷ�SDA��,�ӻ����ܻ�ȡ����Ȩ 
}



/*****************************************************************************
 * ��  ����ackNo
 * ��  �ܣ�����������Ӧ���ź� ���ڵ�9��ʱ������SAL�ߵ�ƽ�ڼ�, SDA���ָߵ�ƽ��
 *         ����������һ���ֽ����ݺ���������NACK֪ͨ�ӻ������������Ա���������ֹͣ�ź�
*****************************************************************************/
static void ackNo(void)
{
    SCL_L;    delayUs( 3); 
    SDA_H;    delayUs( 3); 
    
    SCL_H;    delayUs(10);   // ʹSDA����һ��ʱ�ӵĸߵ�ƽ   
    SCL_L;    delayUs( 3);  
    
    SDA_H;    delayUs( 3);   // ����Ҫ: �������ͷ�SDA��,�ӻ����ܻ�ȡ����Ȩ  
}


/*****************************************************************************
 * ��  ����waitForAck
 * ��  �ܣ���ȡӦ���źŻ��߷�Ӧ���ź�
 *         ��ЧӦ���źţ��ӻ���9��SCL=0ʱ��SDA���ӻ����ͣ�����SCL=1ʱ��SDA��ȻΪ��
 * 
 * ����ֵ�� 0=�յ�Ӧ���ź�,   1=�յ���Ӧ���ź�
 * ��  ע��
*****************************************************************************/
static uint8_t waitForAck(void)
{    
    uint8_t  d=0;
    
    SCL_L;    delayUs( 3);
    SDA_H;    delayUs( 3);        // �����ͷ�SDA�ߣ�ʹ���߿��У��Ա�ӻ��ܹ���Ӧ��Ϣ
    
    SCL_H;    delayUs(10);        // �ȴ�10us; ����׼ģʽ100kbit/s 
    d=SDA_READ;
    
    SCL_L;    delayUs( 3);         
    SDA_H;    delayUs( 3);        // �����ͷ�SDA�ߣ�ʹ���߿��У��Ա�ӻ��ܹ���Ӧ��Ϣ
        
    if(d)
        return 1;                 // ���ط�Ӧ���ź�     
    else
        return 0;                 // ����Ӧ���ź�   
}



/*****************************************************************************
 * ��  ���� sendByte
 * ��  �ܣ� ���һ���ֽ�
 * ��  ����   
 * ����ֵ�� 0=�յ�Ӧ���ź�,   1=�յ���Ӧ���ź�
 * ��  ע��
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
    SDA_H;    delayUs( 3);      // �����ͷ�SDA�ߣ�ʹ�����߿��У��Ա�ӻ��ܹ�������Ӧ��Ϣ����ǯ��SCL��        
}

/*****************************************************************************
 * ��  ���� readByte
 * ��  �ܣ� ��ȡһ���ֽ�
 * ��  ���� ack=0��������û�����꣬ ack=1:����������ȫ��������� 
 * ����ֵ�� ��ȡ����Byteֵ
 * ��  ע�� ��ȡ�ӻ����͵����ݺ󣬲���������Ӧ���źŻ��Ƿ�Ӧ���ź�
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
    
    if(ack) ackNo ();               // 1, ��Ӧ��
    else    ackYes();               // 0, Ӧ��
    
    return  data;       
}



/*****************************************************************************
 * ��  ���� IICSoft_ReadByte
 * ��  �ܣ� ��ָ���ӻ��豸����ȡָ����ַ��һ��ֵ�� ��λ���ֽ�
 * ��  ���� slave: �ӻ���ַ
 *          addr�� ���ݵ�ַ
 *          *buf�� ����Ҫ�洢�ĵ�ַ
 * ����ֵ�� 0=�ɹ��� 1=ʧ��
 * ��  ע�� ע�⣬ʱ���е��ر�
*****************************************************************************/
uint8_t IICSoft_ReadByte(uint8_t slave, uint8_t addr, uint8_t *buf)
{        
    start ();                     // ��ʼ�ź�    
    sendByte (slave<<1 | 0);      // �ӻ���ַ,��д���� , 0д1��    
    if(waitForAck())
    {
        stop();
        return 1;
    }
    
    sendByte(addr);               // ���ݵ�ַ
    if(waitForAck())
    {
        stop();
        return 1;
    }
    
    // д���ݺͶ���������ʱ������ д�Ƚϼ򵥣������ݣ��ٷ�һ����ʼ�źš��ж�����Ĵӻ���ַ
    start();                      // ��ʼ�ź�
    sendByte(slave<<1 | 1);       // �ӻ���ַ,�������� ��  0д1��
    if(waitForAck())
    {
        stop();
        return 1;
    }
    
    *buf=readByte(1);             // ��ֵ, ������ackNo;
    stop();
    
    return 0;
}      



/*****************************************************************************
 * ��  ���� IICSoft_ReadBueffer
 * ��  �ܣ� ��ָ���ӻ��豸����ȡָ����ַ�Ķ��ֵ�� ��λ���ֽ�
 * ��  ���� uint8_t slave   �ӻ���ַ
 *          uint8_t addr    ���ݵ�ַ
 *          uint8_t data    Ҫд�������(1�ֽ�)
 * ����ֵ�� 0=�ɹ��� 1=ʧ��
 * ��  ע�� ע�⣬ʱ���е��ر�
*****************************************************************************/
uint8_t IICSoft_ReadBueffer(uint8_t slave, uint8_t addr, uint8_t *buf, uint8_t len)
{    
    start ();                     // ��ʼ�ź�    
    sendByte (slave<<1 | 0);      // �ӻ���ַ,��д���� , 0д1��    
    if(waitForAck())              // �ȴ�ACK
    {
        stop();
        return 1;
    }
    
    sendByte(addr);               // ���ݵ�ַ
    if(waitForAck())              // �ȴ�ACK
    {
        stop();
        return 1;
    }
    
    // д����ʱ������ �����ݣ��ٷ�һ����ʼ�źš��ж�����Ĵӻ���ַ
    start();                      // ��ʼ�ź�
    sendByte(slave<<1 | 1);       // �ӻ���ַ,�������� ��  0д1��
    if(waitForAck())              // �ȴ�ACK
    {
        stop();
        return 1;
    }
    
  
    while(len)
    {      
        if(len==1)  *buf=readByte(1);   // ���һ���ֽڣ������ݺ����NACK
        else        *buf=readByte(0);   // �����ݺ����ACK
    
        len--;
        buf++;
    }  
    
    stop ();                      // ����һ��ֹͣ�ź�
    return 0;   
}      


/*****************************************************************************
 * ��  ���� IICSoft_WriteByte
 * ��  �ܣ� ���ӻ�ĳ��ַд��һ���ֽ�
 * ��  ���� uint8_t slave   �ӻ���ַ
 *          uint8_t addr    ���ݵ�ַ
 *          uint8_t data    Ҫд�������(1�ֽ�)
 * ����ֵ�� 0=�ɹ��� 1=ʧ��
 * ��  ע�� ʱ������ǲ�ͬ��
*****************************************************************************/
uint8_t IICSoft_WriteByte(uint8_t slave, uint8_t addr, uint8_t data)
{
    start ();                     // ��ʼ�ź�    
    sendByte (slave<<1 | 0);      // �ӻ���ַ,��д���� , 0д1��    
    if(waitForAck())
    {
        stop();
        return 1;                 // ���ӻ���ַ����ʧ�ܣ��ͷ���
    }
    
    sendByte(addr);               // ���ݵ�ַ
    if(waitForAck())
    {
        stop();
        return 1;                 // ������ʧ�ܣ��ͷ���
    }    
    
    sendByte(data);               // �������ݣ� д���ݺͶ���������ʱ����������һ��ǰ
    if(waitForAck())
    {
        stop ();
        return 1;                 // ����д��ʧ��
    }
    
    stop();                       // д����ɣ�����ֹͣ�ź�
    return 0;    
}

// ���ӻ�д����ֽ�����
uint8_t IICSoft_WriteBuffer(uint8_t slave, uint8_t addr, uint8_t *buf, uint8_t len)
{ 
    start ();                     // ��ʼ�ź�    
    sendByte (slave<<1 | 0);      // �ӻ���ַ,��д���� , 0д1��    
    if(waitForAck())
    {
        stop();
        return 1;                 // ������ʧ�ܣ��ͷ���
    }  
    
    sendByte(addr);               // ���ݵ�ַ
    if(waitForAck())
    {
        stop();
        return 1;                 // ������ʧ�ܣ��ͷ���
    }  
    
    for(uint8_t i=0; i<len; i++)
    {
        sendByte(buf[i]);         // ����
        if(waitForAck())          // ÿһ���ֽڶ�Ҫ�ȴӻ�Ӧ��
        {
            stop ();
            return 1;             // ����д��ʧ��
        }
    }
    
    stop();                       // д����ɣ�����ֹͣ�ź�
    return 0;    
}             
        



