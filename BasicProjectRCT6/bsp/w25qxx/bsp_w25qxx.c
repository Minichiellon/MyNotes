/***********************************************************************************************************************************
 ** 【文件名称】  bsp_w25qxx.c
 ** 【编写人员】  魔女开发板团队
 ** 【更新分享】  Q群文件夹       1126717453
 ** 【淘    宝】  魔女开发板      https://demoboard.taobao.com
 ***********************************************************************************************************************************
 ** 【文件功能】  初始化GPIO、SPI, 各功能函数
 ** 【适用平台】  STM32F103 + 标准库v3.5 + keil5
 **
 ** 【本地函数】
 **
 ** 【更新记录】  2019-05-11  创建
 **               2019-12-03  大幅修改write功能, 使其更简单清晰；理解重点：明确擦扇和写页的区别, 写页指令最大缓存字节数. 
 **               2020-08-15  完善代码注释，文件格式 
 **               2021-02-24  多个函数增加判断，W25Qxx初始化失败时，跳过函数，防止卡死
 ** 
************************************************************************************************************************************/
#include "bsp_w25qxx.h" 


  
  
  
/*****************************************************************************
 ** 变量声明
 *****************************************************************************/
 // 设备状态
xW25QXX_TypeDef   xW25Qxx;
//W25Q系列芯片型号返回值       
#define    W25Q80            0XEF13     
#define    W25Q16            0XEF14
#define    W25Q32            0XEF15
#define    W25Q64            0XEF16
#define    W25Q128           0XEF17
#define    W25Q256           0XEF18
//#define  W25Qxx    65519   // 很多时候重新下载后读出的都是65519
#define    W25QX_NSS_HIGH    (W25QXX_NSS_GPIO -> BSRR =  W25QXX_NSS_PIN)
#define    W25QX_NSS_LOW     (W25QXX_NSS_GPIO -> BSRR =  W25QXX_NSS_PIN<<16)
  
/*****************************************************************************
 ** 内部函数声明
****************************************************************************/
// 5个基本功能
static u8    sendByte(u8 d);                          // 5_1    字节读写
static void  writeEnable(void) ;                      // 5_2    写使能
static void  WaitReady(void) ;                        // 5_3    等待空闲
static void  eraseSector(u32 addr);                   // 5_4    擦扇区
static void  writeSector(u32 addr, u8* p, u16 num);   // 5_5    写扇区
// 测试
static void  readID(void);
static void  spiInit(void);
static void  checkFlagGBKStorage(void);                   // 检查字库数据正确性
      
      
      
// 本地US粗略延时函数，减少移植时对外部文件依赖；
#if 1
static void delayUS( __IO u32 times)
{
    times=times*7;      //  10us内用7;
    while(--times)         
        __nop();  
}
#endif 

// ms延时函数，减少移植时对外部文件依赖；
#if 0
static void delayMS(u32 ms)
{
    ms=ms*6500;                  
    for(u32 i=0; i<ms; i++);      // 72MHz系统时钟下，多少个空循环约耗时1ms
}
#endif



      
/***************************************************************************** 
* @Fun    W25Qxx_Init
* @brief  字模存储设备  
*/  
void W25qx_Init()
{
    xW25Qxx.FlagInit =0;            // 复位初始化成功标志
    xW25Qxx.FlagGBKStorage = 1;       // 假设字库存在, 先开启字库地址段写保护标志, 本函数最后检测字库赋真实值
    
    // 时钟使能;用判断端口的方式使能时钟线, 减少移植时的工作
    // 使能SPI时钟
    if(W25QXX_SPI == SPI1)        RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    if(W25QXX_SPI == SPI2)        RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    if(W25QXX_SPI == SPI3)        RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
    // 使能NSS引脚端口时钟
    if(W25QXX_NSS_GPIO == GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    if(W25QXX_NSS_GPIO == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    if(W25QXX_NSS_GPIO == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
    if(W25QXX_NSS_GPIO == GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD , ENABLE);
    if(W25QXX_NSS_GPIO == GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);
    if(W25QXX_NSS_GPIO == GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF , ENABLE);
    if(W25QXX_NSS_GPIO == GPIOG)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF , ENABLE);    
    // 使能SPIx引脚端口时钟
    if(W25QXX_CLK_GPIO == GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    if(W25QXX_CLK_GPIO == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    if(W25QXX_CLK_GPIO == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
    if(W25QXX_CLK_GPIO == GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD , ENABLE);
    if(W25QXX_CLK_GPIO == GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);
    if(W25QXX_CLK_GPIO == GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF , ENABLE);
    if(W25QXX_CLK_GPIO == GPIOG)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF , ENABLE);
    
    // 配置引脚工作模式
    GPIO_InitTypeDef  G;         
    G.GPIO_Pin   = W25QXX_NSS_PIN;       // 片选引脚
    G.GPIO_Mode  = GPIO_Mode_Out_PP ;
    G.GPIO_Speed = GPIO_Speed_50MHz ;  
    GPIO_Init ( W25QXX_NSS_GPIO, &G);     
    W25QX_NSS_HIGH ;                     // 片选线拉高  
    
    G.GPIO_Pin   = W25QXX_CLK_PIN | W25QXX_MISO_PIN | W25QXX_MOSI_PIN;   
    G.GPIO_Mode  = GPIO_Mode_AF_PP;      // SPI通信引脚
    GPIO_Init ( W25QXX_CLK_GPIO, &G); 
    
    // SPI_通信参数配置
    spiInit ();                          // spi初始化，独立封装，便于不同设备使用同一spi
    
    // 功能性检测
    readID();                            // 读取芯片型号,以判断通讯是否正常   
    checkFlagGBKStorage();                   // 检查字库 
}

                        
/***************************************************************************** 
  * spi初始化
  * 只要是w25qxx系列，都适用
  * 注意，设备的SPI初始化，必须做成一个单独的函数。因为STM32在设备间切换时，SPI的参数也得重新调换
  */   
static void  spiInit(void)
{   
    W25QXX_SPI -> CR1  = 0x1<<0;         // CPHA:时钟相位,0x1=在第2个时钟边沿进行数据采样
    W25QXX_SPI -> CR1 |= 0x1<<1;         // CPOL:时钟极性,0x1=空闲状态时，SCK保持高电平
    W25QXX_SPI -> CR1 |= 0x1<<2;         // 主从模式:         1 = 主配置
    W25QXX_SPI -> CR1 |= 0x0<<3;         // 波特率控制[5:3]:  0 = fPCLK /2
    W25QXX_SPI -> CR1 |= 0x0<<7;         // 帧格式:           0 = 先发送MSB
    W25QXX_SPI -> CR1 |= 0x1<<9;         // 软件从器件管理 :  1 = 使能软件从器件管理(软件NSS)
    W25QXX_SPI -> CR1 |= 0x1<<8;         // 内部从器件选择,根据9位设置(失能内部NSS)
    W25QXX_SPI -> CR1 |= 0x0<<11;        // 数据帧格式,       0 = 8位
    W25QXX_SPI -> CR1 |= 0x1<<6;         // SPI使能           1 = 使能外设
         
    delayUS(10);                         // 稍作延时
}



// 5_1 发送1字节,返回1字节
// SPI通信,只一个动作:向DR写入从设命令值,同步读出数据!写读组合,按从设时序图来. 作为主设,因为收发同步,连接收发送中断也不用开,未验证其它中断对其工作的影响. 
u8  sendByte(u8 d)
{
    u8 retry=0;
    
    while((W25QXX_SPI ->SR & 2) == 0)       // 等待发送区为空    
    {  
    retry++;
    if(retry>250)    return 0;
    }
    W25QXX_SPI ->DR =d;
    
    retry =0;    
    while((W25QXX_SPI->SR & 1) == 0)        // 等待接收完数据      
    {          
        retry++;
        if(retry>250)    return 0;
    }
    return W25QXX_SPI->DR ;     
} 



// 5_2 写使能
void writeEnable()
{
    W25QX_NSS_LOW ;
    sendByte (0x6);                          // 命令: Write Enable : 06h
    W25QX_NSS_HIGH ;              
}



// 5_3 等待空闲
void WaitReady()
{    
    W25QX_NSS_LOW ;
    
    sendByte (0x05);                         // 命令: Read Status Register : 05h
    while(sendByte(0xFF) & 1) {}             // 只要发送读状态寄存器指令，芯片就会持续向主机发送最新的状态寄存器内容 ，直到收到通信的停止信号。
           
    W25QX_NSS_HIGH ;    
} 
         


// 5_4 擦除一个扇区, 每扇区>150ms
void eraseSector(u32 addr)
{
   if(xW25Qxx .FlagInit ==0) return;         // 如果W25Qxx初始化失败，则跳过检测，防止卡死

    addr=addr*4096;                          // 从第几扇区开始
    
    writeEnable();
    WaitReady();
    // 命令
    W25QX_NSS_LOW ;
    sendByte (0x20);                         // 命令: Sector Erase(4K) : 20h
    sendByte ((u8)(addr>>16));
    sendByte ((u8)(addr>>8));
    sendByte ((u8)addr);
    W25QX_NSS_HIGH ;    
    
    WaitReady();       
} 



// 5_5 写扇区. 要分页写入
void writeSector(u32 addr, u8 *p, u16 num)
{
    if(xW25Qxx .FlagInit ==0) return;    // 如果W25Qxx初始化失败，则跳过检测，防止卡死

    u16 pageRemain = 256;                // 重要，重要，重要：W25Qxx每个页命令最大写入字节数:256字节;    
  
    // 扇区:4096bytes, 缓存页:256bytes, 写扇区要分16次页命令写入     
    for(char i=0; i<16; i++)
    {              
        writeEnable ();                  // 写使能
        WaitReady ();                    // 等待空闲
        
        W25QX_NSS_LOW ;                  // 低电平,开始
        sendByte(0x02);                  // 命令: page program : 02h , 每个写页命令最大缓存256字节
        sendByte((u8)(addr>>16));        // 地址
        sendByte((u8)(addr>> 8));
        sendByte ((u8)addr); 
        for(u16 i=0;i<pageRemain; i++)   // 发送写入的数据 
        sendByte( p[i] );                // 高电平, 结束
        W25QX_NSS_HIGH ;     
        
        WaitReady ();                    // 等待空闲    
      
        p = p + pageRemain;              // 缓存指针增加一页字节数 
        addr = addr + pageRemain ;       // 写地址增加一页字节数
    }      
}
/***************************************************************************** 
  * @Fun    W25Qxx_readID
  * @brief  读取芯片型号,用于判断通讯状况         
  */        
static void readID()
{   
    u16 W25QxxType = 0 ;     
    // 1: 读取芯片型号, 判断联接状况    
    W25QX_NSS_LOW; 
    sendByte(0x90);  // 发送读取ID命令,命令分两分,第一字节是命令,第四字节是0
    sendByte(0x00);
    sendByte(0x00);
    sendByte(0x00);  // 第四字节必节须是 0h      
    W25QxxType  = (sendByte(0xFF))<<8;   // u16 W25QxxType  在本文件定义,全局
    W25QxxType |= sendByte(0xFF);    
    W25QX_NSS_HIGH;  
    
    xW25Qxx.FlagInit  =1;    
    switch (W25QxxType)    {                        
        case W25Q16:            
            sprintf((char*)xW25Qxx.type, "%s", "W25Q16");               
            break;        
        case W25Q32:
            sprintf((char*)xW25Qxx.type, "%s", "W25Q32");              
            break;
        case W25Q64:
            sprintf((char*)xW25Qxx.type, "%s", "W25Q64");              
            break;
        case W25Q128:
            sprintf((char*)xW25Qxx.type, "%s","W25Q128");              
            break;
        case W25Q256:
            sprintf((char*)xW25Qxx.type, "%s", "W25Q256");           // 注意:W25Q256的地址是4字节               
            break;        
        default:             
            sprintf((char*)xW25Qxx.type, "%s", "Flash设备失败 !!!");              
            xW25Qxx.FlagInit =0;
            printf("读取到的错误型号数据：%d\r\n",W25QxxType);
            break;
    }        
   
    // 2:读取存储数据, 增加启动次数记录      
    if(xW25Qxx.FlagInit  == 1 ) 
    {   
        u32 Addr = 0x00;                  // 数据地址,  W25Q128最大地址:0X0100 0000
        u8 d[4]={0};                      // 数据缓存， 0x0000:标志0xEE, 0x0001:标志0X00,     0x0002:数据高位, 0x0003:数据低位
        u16 startFlag  = 0;               // 标志
        u16 startNum   = 0;               // 启动次数
        
        W25qxx_ReadBuffer(  Addr, d, 4);  // 读取4个字节数据
        startFlag = (d[0]<<8) | d[1];     // 标志
        startNum  = (d[2]<<8) | d[3];     // 启动次数        
        
        if(startFlag!=0xEE00)             // 没有旧记录    
        {         
            startNum=1;            
            d[2]=0;
            d[3]=1;                
        }
        else
        {
            startNum++;                   // 成功读取数据， 次数增加1                    
            d[2]=(u8)(startNum>>8);
            d[3]=(u8)startNum;   
            if(STARTUPTIMES_RESET==1)
            {                
                d[2]=(u8)0;              // 回复启动次数 = 0, 取消这两行即可，
                d[3]=(u8)0;              // 编译烧录后，得重新注释，编译再次烧录 
            }                
        }
        d[0]=0xEE;
        d[1]=0x00;
        
        // 下载后，f=0xEE00时，来到这里就莫名复位
        W25qxx_WriteBuffer( Addr, d, 4);  
        xW25Qxx.StartupTimes = startNum; 
        
        printf("Flash存储 检测...     型号:%s ,第%d次使用\r", xW25Qxx.type, startNum); 
    }
    else 
    {    // 检测W25Qxx失败      
        printf("数据存储检测：        型号读取错误，设备不可用!\r"); 
        printf("尝试复位芯片......\r"); 
        //System_Reset ();
    }          
}



/******************************************************************************
 * @Function        W25Qxx_Read  全局 4_3
 * @Description     读取数据
 *                                 
 * @Input           u8   *p    读出的数值存放位置    
 *                  u32  addr  读取地址
 *                  u16  num   连续读取的字节数 
**/
void W25qxx_ReadBuffer( u32 addr, u8 *p, u16 num)
{
    if(xW25Qxx .FlagInit ==0) return;   // 如果W25Qxx初始化失败，则跳过检测，防止卡死

    spiInit ();                       // 每次读写前，先重新配置SPI，避免多个设备共用一SPI时的配置不同
    
    W25QX_NSS_LOW ;
    sendByte ( 0x03);                 // 发送读取命令 03h
    sendByte ((u8)(addr>>16));
    sendByte ((u8)(addr>>8));
    sendByte ((u8)addr);
            
    for(u32 i=0;i<num;i++)
    {
        p[i]=sendByte(0xFF);
    }
                
    W25QX_NSS_HIGH ;    
}


/******************************************************************************
 * 函数名： W25Qxx_Write
 * 功  能： 从addr处起，读取num个字节，存放到缓存p
 * 参  数： u8   *p    要写入的数据存储区  
 *          u32  addr  写入地址         (W25Q128 只用3字节, W25Q256用4字节)
 *          u16  num   连续写入的字节数  
 * 返  回： 无
 * 备  注： 魔女开发板团队    资料更新Q群：1126717453    最后修改_2020年12月15日
 ******************************************************************************/  
u8 W25QXX_buffer[4096];                       // 开辟一段内存空间

void W25qxx_WriteBuffer( u32 addr, u8* p, u16 num)
{
    if(xW25Qxx.FlagInit ==0) return ;           // 如果w25qxx设备初始化失败，则跳过本函数，防止卡死

    // 字库段写保护, 防止字库被错误写入履盖
    if(((addr+num)>0x00A00000) && (xW25Qxx.FlagGBKStorage ==1 )) 
    {
        printf("要写入的数据在字库数据存储区内，已跳过本次操作!!\r");
        return;
    }
    
    u32  secPos      = addr/4096;             // 扇区地址,第几个扇区
    u16  secOff      = addr%4096;             // 开始地始偏移字节数: 数据在扇区的第几字节存放
    u16  secRemain   = 4096-secOff;           // 扇区剩余空间字节数 ,用于判断够不够存放余下的数据
    u8*  buf = W25QXX_buffer;                 // 原子哥代码,为什么不直接使用所声明的数组. (回看前面的疑问, 接触C有15年, 原来没下过工夫) 
    
    spiInit ();                               // 每次读写前，先重新配置SPI，避免多个设备共用一SPI时的配置不同
    if(num<=secRemain) secRemain=num;  
    while(1) 
    {
        W25qxx_ReadBuffer ( secPos*4096, buf, 4096);   // 读取扇区内容到缓存
        
        eraseSector(secPos );                 // 擦扇区
        for(u16 i=0;i<secRemain ;i++)         // 原始数据写入缓存
            buf[secOff +i]=p[i];
        writeSector( secPos*4096, buf, 4096); // 缓存数据写入设备
        
        if(secRemain == num)                  // 已全部写入
            break;                                         
        else
        {                                     // 未写完
            p=p+secRemain ;                   // 原始数据指针偏移
            secPos ++;                        // 新扇区
            secOff =0;                        // 新偏移位,扇区内数据起始地址            
            num=num-secRemain ;               // 剩余未写字节数            
            secRemain = (num>4096)?4096:num;  // 计算新扇区写入字节数                  
        }          
    }    
}



// 检查字库样本的正确性
void checkFlagGBKStorage(void)
{
    if(xW25Qxx .FlagInit ==0) return;                      // 如果W25Qxx初始化失败，则跳过检测，防止卡死

    printf("GBK字库 测试...       ");                   
    u8 sub = 0;
    u8 f=0 ;                                                            
                                                   
    for(u32 i=0; i< 6128640; i=i+1000000)                               
    {
        W25qxx_ReadBuffer(GBK_STORAGE_ADDR +i, &f , 1);               
        sub = sub + f;                                   // 80 , 0, 98, 79, 0, 1, 0
    }      
    xW25Qxx.FlagGBKStorage = (sub==146 ? 1 : 0);             // 判断是否有字库,打开地址写保护, 防止字库被错误写入履盖
    
    if(xW25Qxx.FlagGBKStorage==1)    printf("字库可用\r");   // 标记字库可用
    else        printf(" 错误，字库不可用!\r");        
}



/******************************************************************************
 * 函数名： W25qxx_ReadGBK
 * 功  能： 从w25qxx的字库中读取出字模数据    
 *          (参考了原子、野火大神的代码后作出的完善修改)
 * 参  数： u8* code   字符指针起始位，GBK码
 *          u8  size   字体大小 12/16/24/32
 *          u3* macr   数据存放地址 (size/8+((size%8)?1:0))*(size) bytes大小
 * 返  回： 无
 * 备  注： 魔女开发板团队    资料更新Q群：1126717453    最后修改_2020年12月15日
 ******************************************************************************/  
void W25qxx_ReadGBK(u8* typeface, u8 size, u8* dataBuf)
{            
    u8 qh,ql;                          
    u32 foffset; 
    u8 csize=(size/8+((size%8)?1:0))*(size);        // 计算汉字点阵大小，单位字节数     
    
    qh=*typeface;
    ql=*(++typeface);    
    
    if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)        // 非常用汉字，将用填充显示整个位置
    {                 
        for(u8 i=0; i<csize; i++) *dataBuf++=0x00;  // 填充满格
        return;                                     // 结束访问
    }     

    if(ql<0x7f) ql-=0x40;                           // 计算要提取的汉字在字库中的偏移位置
    else        ql-=0x41;
    qh-=0x81;   
    foffset=((unsigned long)190*qh+ql)*csize;        // 得到汉字在字库中的偏移位置          
    
    switch(size)
    {                                                                                 // 按字体的不同，在不同字库读取字体点阵
        case 12:
        W25qxx_ReadBuffer( foffset + GBK_STORAGE_ADDR,            dataBuf, csize);    // 12号字体           
        break;  
        case 16:
        W25qxx_ReadBuffer( foffset + GBK_STORAGE_ADDR+0x0008c460, dataBuf, csize);    // 16号字体
        break;
        case 24:
        W25qxx_ReadBuffer( foffset + GBK_STORAGE_ADDR+0x001474E0, dataBuf, csize);    // 24号字体
        break;
        case 32:
        W25qxx_ReadBuffer( foffset + GBK_STORAGE_ADDR+0x002EC200, dataBuf, csize);    // 32号字体
        break;            
    }         
}  


