//#include "manage_f103.h"
#include "stm32f10x.h"
#include "TF.h"
#include "string.h"
#include "mallco.h"
#include "bsp_UART.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////
// TF卡类型定义
#define TF_TYPE_ERR     0X00
#define TF_TYPE_MMC     0X01
#define TF_TYPE_V1      0X02
#define TF_TYPE_V2      0X04
#define TF_TYPE_V2HC    0X06
// TF卡指令表
#define CMD0    0       //卡复位
#define CMD1    1
#define CMD8    8       //命令8 ，SEND_IF_COND
#define CMD9    9       //命令9 ，读CSD数据
#define CMD10   10      //命令10，读CID数据
#define CMD12   12      //命令12，停止数据传输
#define CMD16   16      //命令16，设置SectorSize 应返回0x00
#define CMD17   17      //命令17，读sector
#define CMD18   18      //命令18，读Multi sector
#define CMD23   23      //命令23，设置多sector写入前预先擦除N个block
#define CMD24   24      //命令24，写sector
#define CMD25   25      //命令25，写Multi sector
#define CMD41   41      //命令41，应返回0x00
#define CMD55   55      //命令55，应返回0x01
#define CMD58   58      //命令58，读OCR信息
#define CMD59   59      //命令59，使能/禁止CRC，应返回0x00
//数据写入回应字意义
#define MSD_DATA_OK                0x05
#define MSD_DATA_CRC_ERROR         0x0B
#define MSD_DATA_WRITE_ERROR       0x0D
#define MSD_DATA_OTHER_ERROR       0xFF
//TF卡回应标记字
#define MSD_RESPONSE_NO_ERROR      0x00
#define MSD_IN_IDLE_STATE          0x01
#define MSD_ERASE_RESET            0x02
#define MSD_ILLEGAL_COMMAND        0x04
#define MSD_COM_CRC_ERROR          0x08
#define MSD_ERASE_SEQUENCE_ERROR   0x10
#define MSD_ADDRESS_ERROR          0x20
#define MSD_PARAMETER_ERROR        0x40
#define MSD_RESPONSE_FAILURE       0xFF



//文件类型列表
const uint8_t *FILE_TYPE_TBL[6][13] =
{
    {"BIN"},                       //BIN文件
    {"LRC"},                       //LRC文件
    {"NES"},                       //NES文件
    {"TXT", "C", "H"},             //文本文件
    {"MP1", "MP2", "MP3", "MP4", "M4A", "3GP", "3G2", "OGG", "ACC", "WMA", "WAV", "MID", "FLAC"}, //音乐文件
    {"BMP", "JPG", "JPEG", "GIF"}, //图片文件
};



//f_typetell返回的类型定义
//根据表FILE_TYPE_TBL获得.在exfuns.c里面定义
#define T_BIN        0X00    // bin文件
#define T_LRC        0X10    // lrc文件
#define T_NES        0X20    // nes文件
#define T_TEXT       0X30    // .txt文件
#define T_C          0X31    // .c文件
#define T_H          0X32    // .h文件
#define T_FLAC       0X4C    // flac文件
#define T_BMP        0X50    // bmp文件
#define T_JPG        0X51    // jpg文件
#define T_JPEG       0X52    // jpeg文件         
#define T_GIF        0X53    // gif文件  

//////////////////////////////////////////////////////////////////////////////////////////////////////
// 使用malloc的时候
FATFS *fs[2];                // 逻辑磁盘工作区.
FIL *file;                   // 文件1
FIL *ftemp;                  // 文件2
UINT br, bw;                 // 读写变量
FILINFO fileinfo;            // 文件信息
DIR dir;                     // 目录
                             
uint8_t *fatbuf;             // TF卡数据缓存区
                            
uint8_t  g_TfType = 0;       // TF卡类型
SPI_InitTypeDef  S= {0};     // SPI模式通信配置

static uint8_t   spiSendByte(uint8_t TxData);
static void spiSetSpeed(uint8_t SpeedSet);
static uint8_t   tfGetResponse(uint8_t Response);     // 获得相应
static uint8_t   tfmalloc(void);                      // 为exfuns申请内存



/******************************************************************************
 * 函  数： delay_ms
 * 功  能： ms 延时函数
 * 备  注： 1、系统时钟168MHz
 *          2、打勾：Options/ c++ / One ELF Section per Function
            3、编译优化级别：Level 3(-O3)
 * 参  数： uint32_t  ms  毫秒值
 * 返回值： 无
 ******************************************************************************/
static volatile uint32_t ulTimesMS;    // 使用volatile声明，防止变量被编译器优化
static void delay_ms(uint16_t ms)
{
    ulTimesMS = ms * 4785;
    while (ulTimesMS)
        ulTimesMS--;                   // 操作外部变量，防止空循环被编译器优化掉
}


//SPI硬件层初始化
static void spiInit(void)
{
    GPIO_InitTypeDef G = {0};
    // GPIO时钟使能
    RCC_APB2PeriphClockCmd(TF_CS_PORTEN, ENABLE);
    // SPI时钟全能，默认使用SPI1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    // CS引脚初始化
    G.GPIO_Pin = TF_CS_PIN;
    G.GPIO_Mode = GPIO_Mode_Out_PP ;                            // 推挽
    G.GPIO_Speed = GPIO_Speed_50MHz;                            
    GPIO_Init(TF_CS_GPIO, &G);                                  
    TF_CS_HIGH;                                                 // 片选引脚先拉高，防止SPI初始化后误动作
                                                                
    // SCK、MISO、MOSI引脚初始化                                
    G.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;          
    G.GPIO_Mode = GPIO_Mode_AF_PP;                              // 复用推挽输出
    G.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &G);

    GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);

    S.SPI_Direction = SPI_Direction_2Lines_FullDuplex;          // 设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
    S.SPI_Mode = SPI_Mode_Master;                               // 设置SPI工作模式:设置为主SPI
    S.SPI_DataSize = SPI_DataSize_8b;                           // 设置SPI的数据大小:SPI发送接收8位帧结构
    S.SPI_CPOL = SPI_CPOL_High;                                 // 选择了串行时钟的稳态:时钟悬空高
    S.SPI_CPHA = SPI_CPHA_2Edge;                                // 数据捕获于第二个时钟沿
    S.SPI_NSS = SPI_NSS_Soft;                                   // NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    S.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;        // 定义波特率预分频的值:波特率预分频值为256
    S.SPI_FirstBit = SPI_FirstBit_MSB;                          // 指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    S.SPI_CRCPolynomial = 7;                                    // CRC值计算的多项式
    SPI_Init(TF_SPI_PORT, &S);                                  // 根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
                                                                
    TF_SPI_PORT->CR1 |= 1 << 6;                                 // 使能SPI外设

    //spiSendByte(0xff);//启动传输
    TF_CS_HIGH;
}

//SPI 速度设置函数
// TF卡工作上有两种模式，识别模式时约400KHz, 传输模式时约25MHz;
void spiSetSpeed(uint8_t SpeedSet)
{
    // SPI_BaudRatePrescaler_2   2分频   (SPI 36M@sys 72M)
    S.SPI_BaudRatePrescaler = SpeedSet ;  // SPI_BaudRatePrescaler_8   8分频   (SPI 9M@sys 72M)
    SPI_Init(SPI1, &S);                   // SPI_BaudRatePrescaler_16  16分频  (SPI 4.5M@sys 72M)
    SPI_Cmd(SPI1, ENABLE);                // SPI_BaudRatePrescaler_256 256分频 (SPI 281.25K@sys 72M)
}

// SPI读写一字节
static uint8_t spiSendByte(uint8_t TxData)
{
    uint8_t retry = 0;
    while ((TF_SPI_PORT->SR & 0x02) == 0)  // 检查指定的SPI标志位设置与否:发送缓存空标志位; SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET
    {
        retry++;
        if (retry > 250) return 0;
    }
    SPI_I2S_SendData(SPI1, TxData);        // 通过外设SPIx发送一个数据

    retry = 0;
    while ((TF_SPI_PORT->SR & 0x01) == 0)  // 检查指定的SPI标志位设置与否:接受缓存非空标志位
    {
        retry++;
        if (retry > 250) return 0;
    }

    return TF_SPI_PORT->DR;                // 返回通过SPIx最近接收的数据
}

///////////////////////////////////////////////////////////////////////////////////
// TF取消选择,释放SPI总线
void tfDisSelect(void)
{
    TF_CS_HIGH;
    spiSendByte(0xff);       // 提供额外的8个时钟
}
//选择sd卡,并且等待卡准备OK
//返回值:0,成功;1,失败;
uint8_t tfSelect(void)
{
    TF_CS_LOW;
    if (TF_WaitReady() == 0)
        return 0;            // 等待成功
    tfDisSelect();
    return 1;                // 等待失败
}

//等待卡准备好
//返回值:0,准备好了;其他,错误代码
uint8_t TF_WaitReady(void)
{
    u32 t = 0;
    do
    {
        if (spiSendByte(0XFF) == 0XFF)
            return 0;        // OK
        t++;
    }
    while (t < 0XFFFFFF);    // 等待
    return 1;
}

//等待TF卡回应
//Response:要得到的回应值
//返回值:0,成功得到了该回应值
//    其他,得到回应值失败
uint8_t tfGetResponse(uint8_t Response)
{
    uint16_t Count = 0xFFFF;                                 // 等待次数
    while ((spiSendByte(0XFF) != Response) && Count)Count--; // 等待得到准确的回应
    if (Count == 0)return MSD_RESPONSE_FAILURE;              // 得到回应失败
    else return MSD_RESPONSE_NO_ERROR;                       // 正确回应
}

//从sd卡读取一个数据包的内容
//buf:数据缓存区
//len:要读取的数据长度.
//返回值:0,成功;其他,失败;
uint8_t tfRecvData(uint8_t *buf, uint16_t len)
{
    if (tfGetResponse(0xFE))return 1;  // 等待TF卡发回数据起始令牌0xFE
    while (len--)                      // 开始接收数据
    {
        *buf = spiSendByte(0xFF);
        buf++;
    }
    //下面是2个伪CRC（dummy CRC）
    spiSendByte(0xFF);
    spiSendByte(0xFF);
    return 0;//读取成功
}

//向sd卡写入一个数据包的内容 512字节
//buf:数据缓存区
//cmd:指令
//返回值:0,成功;其他,失败;
uint8_t tfSendBlock(uint8_t *buf, uint8_t cmd)
{
    uint16_t t;
    if (TF_WaitReady())return 1;                      // 等待准备失效
    spiSendByte(cmd);
    if (cmd != 0XFD)                                  // 不是结束指令
    {
        for (t = 0; t < 512; t++)spiSendByte(buf[t]); // 提高速度,减少函数传参时间
        spiSendByte(0xFF);                            // 忽略crc
        spiSendByte(0xFF);
        t = spiSendByte(0xFF);                        // 接收响应
        if ((t & 0x1F) != 0x05)return 2;              // 响应错误
    }
    return 0;//写入成功
}

//向TF卡发送一个命令
//输入: uint8_t cmd   命令
//      u32 arg  命令参数
//      uint8_t crc   crc校验值
//返回值:TF卡返回的响应
uint8_t tfSendCmd(uint8_t cmd, u32 arg, uint8_t crc)
{
    uint8_t r1;
    uint8_t Retry = 0;
    tfDisSelect();                      // 取消上次片选
    if (tfSelect())                     
        return 0XFF;                    // 片选失效
    //发送                              
    spiSendByte(cmd | 0x40);            // 分别写入命令
    spiSendByte(arg >> 24);
    spiSendByte(arg >> 16);
    spiSendByte(arg >> 8);
    spiSendByte(arg);
    spiSendByte(crc);
    if (cmd == CMD12)spiSendByte(0xff); // Skip a stuff byte when stop reading
    
    Retry = 0XFF;                       // 等待响应，或超时退出
    do
    {
        r1 = spiSendByte(0xFF);
    }
    while ((r1 & 0X80) && Retry--);
    
    return r1;                          // 返回状态值
}

//获取TF卡的CID信息，包括制造商信息
//输入: uint8_t *cid_data(存放CID的内存，至少16Byte）
//返回值:0：NO_ERR
//         1：错误
uint8_t TF_GetCID(uint8_t *cid_data)
{
    uint8_t r1;
    
    r1 = tfSendCmd(CMD10, 0, 0x01);     // 发CMD10命令，读CID
    if (r1 == 0x00)
    {
        r1 = tfRecvData(cid_data, 16);  // 接收16个字节的数据
    }
    tfDisSelect();                      // 取消片选
    if (r1)return 1;
    else return 0;
}

//获取TF卡的CSD信息，包括容量和速度信息
//输入:uint8_t *cid_data(存放CID的内存，至少16Byte）
//返回值:0：NO_ERR
//         1：错误
uint8_t TF_GetCSD(uint8_t *csd_data)
{
    uint8_t r1;
    r1 = tfSendCmd(CMD9, 0, 0x01);     // 发CMD9命令，读CSD
    if (r1 == 0)
    {
        r1 = tfRecvData(csd_data, 16); // 接收16个字节的数据
    }
    tfDisSelect();                     // 取消片选
    if (r1)return 1;
    else return 0;
}

//获取TF卡的总扇区数（扇区数）
//返回值:0： 取容量出错
//       其他:TF卡的容量(扇区数/512字节)
//每扇区的字节数必为512，因为如果不是512，则初始化不能通过.
u32 TF_GetSectorCount(void)
{
    uint8_t csd[16];
    u32 Capacity;
    uint8_t n;
    uint16_t csize;
    //取CSD信息，如果期间出错，返回0
    if (TF_GetCSD(csd) != 0) return 0;
    //如果为SDHC卡，按照下面方式计算
    if ((csd[0] & 0xC0) == 0x40)                  // V2.00的卡
    {
        csize = csd[9] + ((u16)csd[8] << 8) + 1;
        Capacity = (u32)csize << 10;              // 得到扇区数
    }
    else                                          // V1.XX的卡
    {
        n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
        csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
        Capacity = (u32)csize << (n - 9);         // 得到扇区数
    }
    return Capacity;
}



//初始化TF卡
// GPIO、SPI、卡型号识别及配置
uint8_t TF_Init(void)
{
    uint8_t  r1;                             // 存放TF卡的返回值
    uint16_t retry;                          // 用来进行超时计数
    uint8_t  buf[4];
    uint16_t i;
                 
    spiInit();                               // 初始化GPIO, 配置SPI参数 
    
    // 设置SPI低速率
    spiSetSpeed(SPI_BaudRatePrescaler_256);  // TF上电后为识别模式。TF有两个模式，识别模式(低速率，约400KHz)，传输模式(高速率,约25MHz）          //
    for (i = 0; i < 100; i++)
        spiSendByte(0XFF);                   // 发送最少74个脉冲
    
    // 进入IDLE状态
    retry = 100;
    do
    {
        r1 = tfSendCmd(CMD0, 0, 0x95);      // 进入IDLE状态
    }
    while ((r1 != 0X01) && retry--);
    
    // 判断卡型号，以作不同命令处理
    g_TfType = 0;
    if (r1 == 0X01)
    {
        if (tfSendCmd(CMD8, 0x1AA, 0x87) == 1)               // TF V2.0
        {
            for (i = 0; i < 4; i++)
                buf[i] = spiSendByte(0XFF);                  // Get trailing return value of R7 resp
            if (buf[2] == 0X01 && buf[3] == 0XAA)            // 卡是否支持2.7~3.6V
            {
                retry = 0XFFFE;
                do
                {
                    tfSendCmd(CMD55, 0, 0X01);               // 发送CMD55
                    r1 = tfSendCmd(CMD41, 0x40000000, 0X01); // 发送CMD41
                }
                while (r1 && retry--);
                if (retry && tfSendCmd(CMD58, 0, 0X01) == 0) // 鉴别TF2.0卡版本开始
                {
                    for (i = 0; i < 4; i++)
                        buf[i] = spiSendByte(0XFF);          // 得到OCR值
                    if (buf[0] & 0x40)
                        g_TfType = TF_TYPE_V2HC;             // 卡型号：V2.0高速
                    else                                     
                        g_TfType = TF_TYPE_V2;               // 卡型号：V2.0标准
                }
            }
        }
        else  // TF V1.x/ MMC    V3
        {
            tfSendCmd(CMD55, 0, 0X01);                       // 发送CMD55
            r1 = tfSendCmd(CMD41, 0, 0X01);                  // 发送CMD41
            if (r1 <= 1)                                     
            {                                                
                g_TfType = TF_TYPE_V1;                       // 卡型号：V1.0
                retry = 0XFFFE;                              
                do //等待退出IDLE模式                        
                {                                            
                    tfSendCmd(CMD55, 0, 0X01);               // 发送CMD55
                    r1 = tfSendCmd(CMD41, 0, 0X01);          // 发送CMD41
                }                                            
                while (r1 && retry--);                       
            }                                                
            else  // MMC卡不支持CMD55+CMD41识别              
            {                                                
                g_TfType = TF_TYPE_MMC;                      // 卡型号：MMC V3
                retry = 0XFFFE;                              
                do // 等待退出IDLE模式                       
                {                                            
                    r1 = tfSendCmd(CMD1, 0, 0X01);           // 发送CMD1
                }
                while (r1 && retry--);
            }
            if (retry == 0 || tfSendCmd(CMD16, 512, 0X01) != 0)
                g_TfType = TF_TYPE_ERR;                      // 卡型号：错误的卡
        }                                                    
    }                                                        
    // 完成识别，进入                                        
    tfDisSelect();                                           // 取消片选
    spiSetSpeed(SPI_BaudRatePrescaler_2);                    // 进入传输模式，可以提高通信速率了。TF有两个模式，识别模式(低速率，约400KHz)，传输模式(高速率,约25MHz）
                                                             
    tfmalloc();                                              // 为fatfs相关变量申请内存
                                                             
    if (g_TfType)                                            
    {            
        return 0;
    }
    else                                                     // 失败
    {
        if (r1)
        {
            printf("TF卡初始化失败，错误号：%d\r", r1);
            return r1;
        }
    }
    printf("TF卡初始化失败，发生了示知的错误\r");
    return 0xaa;       // 其他错误
}

//读TF卡
//buf:数据缓存区
//sector:扇区
//cnt:扇区数
//返回值:0,ok; 其他,失败.
uint8_t TF_ReadDisk(uint8_t *buf, u32 sector, uint8_t cnt)
{
    uint8_t r1;
    if (g_TfType != TF_TYPE_V2HC)sector <<= 9; //转换为字节地址
    if (cnt == 1)
    {
        r1 = tfSendCmd(CMD17, sector, 0X01); //读命令
        if (r1 == 0) //指令发送成功
        {
            r1 = tfRecvData(buf, 512); //接收512个字节
        }
    }
    else
    {
        r1 = tfSendCmd(CMD18, sector, 0X01); //连续读命令
        do
        {
            r1 = tfRecvData(buf, 512); //接收512个字节
            buf += 512;
        }
        while (--cnt && r1 == 0);
        tfSendCmd(CMD12, 0, 0X01);  //发送停止命令
    }
    tfDisSelect();//取消片选
    return r1;//
}
//写TF卡
//buf:数据缓存区
//sector:起始扇区
//cnt:扇区数
//返回值:0,ok;其他,失败.
uint8_t TF_WriteDisk(uint8_t *buf, u32 sector, uint8_t cnt)
{
    uint8_t r1;
    if (g_TfType != TF_TYPE_V2HC)sector *= 512; //转换为字节地址
    if (cnt == 1)
    {
        r1 = tfSendCmd(CMD24, sector, 0X01); //读命令
        if (r1 == 0) //指令发送成功
        {
            r1 = tfSendBlock(buf, 0xFE); //写512个字节
        }
    }
    else
    {
        if (g_TfType != TF_TYPE_MMC)
        {
            tfSendCmd(CMD55, 0, 0X01);
            tfSendCmd(CMD23, cnt, 0X01); //发送指令
        }
        r1 = tfSendCmd(CMD25, sector, 0X01); //连续读命令
        if (r1 == 0)
        {
            do
            {
                r1 = tfSendBlock(buf, 0xFC); //接收512个字节
                buf += 512;
            }
            while (--cnt && r1 == 0);
            r1 = tfSendBlock(0, 0xFD); //接收512个字节
        }
    }
    tfDisSelect();//取消片选
    return r1;//
}

// 获取TF卡型号值
uint8_t TF_GetType(void)
{
    return g_TfType;
}




//为exfuns申请内存
//返回值:0,成功
//1,失败
uint8_t tfmalloc(void)
{
    fs[0] = (FATFS *)mymalloc(sizeof(FATFS)); //为磁盘0工作区申请内存
    fs[1] = (FATFS *)mymalloc(sizeof(FATFS)); //为磁盘1工作区申请内存
    file = (FIL *)mymalloc(sizeof(FIL));     //为file申请内存
    ftemp = (FIL *)mymalloc(sizeof(FIL));     //为ftemp申请内存
    fatbuf = (uint8_t *)mymalloc(512);             //为fatbuf申请内存

    if (fs[0] && fs[1] && file && ftemp && fatbuf)return 0; //申请有一个失败,即失败.
    else return 1;
}

//将小写字母转为大写字母,如果是数字,则保持不变.
uint8_t char_upper(uint8_t c)
{
    if (c < 'A')return c; //数字,保持不变.
    if (c >= 'a')return c - 0x20; //变为大写.
    else return c;//大写,保持不变
}
//报告文件的类型
//fname:文件名
//返回值:0XFF,表示无法识别的文件类型编号.
//         其他,高四位表示所属大类,低四位表示所属小类.
uint8_t f_typetell(uint8_t *fname)
{
    uint8_t tbuf[5];
    uint8_t *attr = '\0'; //后缀名
    uint8_t i = 0, j;
    while (i < 250)
    {
        i++;
        if (*fname == '\0')break; //偏移到了最后了.
        fname++;
    }
    if (i == 250)return 0XFF; //错误的字符串.
    for (i = 0; i < 5; i++) //得到后缀名
    {
        fname--;
        if (*fname == '.')
        {
            fname++;
            attr = fname;
            break;
        }
    }
    strcpy((char *)tbuf, (const char *)attr); //copy
    for (i = 0; i < 4; i++)tbuf[i] = char_upper(tbuf[i]); //全部变为大写
    for (i = 0; i < 6; i++)
    {
        for (j = 0; j < 13; j++)
        {
            if (*FILE_TYPE_TBL[i][j] == 0)break; //此组已经没有可对比的成员了.
            if (strcmp((const char *)FILE_TYPE_TBL[i][j], (const char *)tbuf) == 0) //找到了
            {
                return (i << 4) | j;
            }
        }
    }
    return 0XFF;//没找到
}

//得到磁盘剩余容量
//drv:磁盘编号("0:"/"1:")
//total:总容量     （单位KB）
//free:剩余容量     （单位KB）
//返回值:0,正常.其他,错误代码
uint8_t exf_getfree(uint8_t *drv, u32 *total, u32 *free)
{
    FATFS *fs1;
    uint8_t res;
    u32 fre_clust = 0, fre_sect = 0, tot_sect = 0;
    //得到磁盘信息及空闲簇数量
    res = (u32)f_getfree((const TCHAR *)drv, (DWORD *)&fre_clust, &fs1);
    if (res == 0)
    {
        tot_sect = (fs1->n_fatent - 2) * fs1->csize; //得到总扇区数
        fre_sect = fre_clust * fs1->csize;        //得到空闲扇区数
#if _MAX_SS!=512                                  //扇区大小不是512字节,则转换为512字节
        tot_sect *= fs1->ssize / 512;
        fre_sect *= fs1->ssize / 512;
#endif
        *total = tot_sect >> 1; //单位为KB
        *free = fre_sect >> 1; //单位为KB
    }
    return res;
}


