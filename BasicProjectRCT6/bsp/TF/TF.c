//#include "manage_f103.h"
#include "stm32f10x.h"
#include "TF.h"
#include "string.h"
#include "mallco.h"
#include "bsp_UART.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////
// TF�����Ͷ���
#define TF_TYPE_ERR     0X00
#define TF_TYPE_MMC     0X01
#define TF_TYPE_V1      0X02
#define TF_TYPE_V2      0X04
#define TF_TYPE_V2HC    0X06
// TF��ָ���
#define CMD0    0       //����λ
#define CMD1    1
#define CMD8    8       //����8 ��SEND_IF_COND
#define CMD9    9       //����9 ����CSD����
#define CMD10   10      //����10����CID����
#define CMD12   12      //����12��ֹͣ���ݴ���
#define CMD16   16      //����16������SectorSize Ӧ����0x00
#define CMD17   17      //����17����sector
#define CMD18   18      //����18����Multi sector
#define CMD23   23      //����23�����ö�sectorд��ǰԤ�Ȳ���N��block
#define CMD24   24      //����24��дsector
#define CMD25   25      //����25��дMulti sector
#define CMD41   41      //����41��Ӧ����0x00
#define CMD55   55      //����55��Ӧ����0x01
#define CMD58   58      //����58����OCR��Ϣ
#define CMD59   59      //����59��ʹ��/��ֹCRC��Ӧ����0x00
//����д���Ӧ������
#define MSD_DATA_OK                0x05
#define MSD_DATA_CRC_ERROR         0x0B
#define MSD_DATA_WRITE_ERROR       0x0D
#define MSD_DATA_OTHER_ERROR       0xFF
//TF����Ӧ�����
#define MSD_RESPONSE_NO_ERROR      0x00
#define MSD_IN_IDLE_STATE          0x01
#define MSD_ERASE_RESET            0x02
#define MSD_ILLEGAL_COMMAND        0x04
#define MSD_COM_CRC_ERROR          0x08
#define MSD_ERASE_SEQUENCE_ERROR   0x10
#define MSD_ADDRESS_ERROR          0x20
#define MSD_PARAMETER_ERROR        0x40
#define MSD_RESPONSE_FAILURE       0xFF



//�ļ������б�
const uint8_t *FILE_TYPE_TBL[6][13] =
{
    {"BIN"},                       //BIN�ļ�
    {"LRC"},                       //LRC�ļ�
    {"NES"},                       //NES�ļ�
    {"TXT", "C", "H"},             //�ı��ļ�
    {"MP1", "MP2", "MP3", "MP4", "M4A", "3GP", "3G2", "OGG", "ACC", "WMA", "WAV", "MID", "FLAC"}, //�����ļ�
    {"BMP", "JPG", "JPEG", "GIF"}, //ͼƬ�ļ�
};



//f_typetell���ص����Ͷ���
//���ݱ�FILE_TYPE_TBL���.��exfuns.c���涨��
#define T_BIN        0X00    // bin�ļ�
#define T_LRC        0X10    // lrc�ļ�
#define T_NES        0X20    // nes�ļ�
#define T_TEXT       0X30    // .txt�ļ�
#define T_C          0X31    // .c�ļ�
#define T_H          0X32    // .h�ļ�
#define T_FLAC       0X4C    // flac�ļ�
#define T_BMP        0X50    // bmp�ļ�
#define T_JPG        0X51    // jpg�ļ�
#define T_JPEG       0X52    // jpeg�ļ�         
#define T_GIF        0X53    // gif�ļ�  

//////////////////////////////////////////////////////////////////////////////////////////////////////
// ʹ��malloc��ʱ��
FATFS *fs[2];                // �߼����̹�����.
FIL *file;                   // �ļ�1
FIL *ftemp;                  // �ļ�2
UINT br, bw;                 // ��д����
FILINFO fileinfo;            // �ļ���Ϣ
DIR dir;                     // Ŀ¼
                             
uint8_t *fatbuf;             // TF�����ݻ�����
                            
uint8_t  g_TfType = 0;       // TF������
SPI_InitTypeDef  S= {0};     // SPIģʽͨ������

static uint8_t   spiSendByte(uint8_t TxData);
static void spiSetSpeed(uint8_t SpeedSet);
static uint8_t   tfGetResponse(uint8_t Response);     // �����Ӧ
static uint8_t   tfmalloc(void);                      // Ϊexfuns�����ڴ�



/******************************************************************************
 * ��  ���� delay_ms
 * ��  �ܣ� ms ��ʱ����
 * ��  ע�� 1��ϵͳʱ��168MHz
 *          2���򹴣�Options/ c++ / One ELF Section per Function
            3�������Ż�����Level 3(-O3)
 * ��  ���� uint32_t  ms  ����ֵ
 * ����ֵ�� ��
 ******************************************************************************/
static volatile uint32_t ulTimesMS;    // ʹ��volatile��������ֹ�������������Ż�
static void delay_ms(uint16_t ms)
{
    ulTimesMS = ms * 4785;
    while (ulTimesMS)
        ulTimesMS--;                   // �����ⲿ��������ֹ��ѭ�����������Ż���
}


//SPIӲ�����ʼ��
static void spiInit(void)
{
    GPIO_InitTypeDef G = {0};
    // GPIOʱ��ʹ��
    RCC_APB2PeriphClockCmd(TF_CS_PORTEN, ENABLE);
    // SPIʱ��ȫ�ܣ�Ĭ��ʹ��SPI1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    // CS���ų�ʼ��
    G.GPIO_Pin = TF_CS_PIN;
    G.GPIO_Mode = GPIO_Mode_Out_PP ;                            // ����
    G.GPIO_Speed = GPIO_Speed_50MHz;                            
    GPIO_Init(TF_CS_GPIO, &G);                                  
    TF_CS_HIGH;                                                 // Ƭѡ���������ߣ���ֹSPI��ʼ��������
                                                                
    // SCK��MISO��MOSI���ų�ʼ��                                
    G.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;          
    G.GPIO_Mode = GPIO_Mode_AF_PP;                              // �����������
    G.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &G);

    GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);

    S.SPI_Direction = SPI_Direction_2Lines_FullDuplex;          // ����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
    S.SPI_Mode = SPI_Mode_Master;                               // ����SPI����ģʽ:����Ϊ��SPI
    S.SPI_DataSize = SPI_DataSize_8b;                           // ����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    S.SPI_CPOL = SPI_CPOL_High;                                 // ѡ���˴���ʱ�ӵ���̬:ʱ�����ո�
    S.SPI_CPHA = SPI_CPHA_2Edge;                                // ���ݲ����ڵڶ���ʱ����
    S.SPI_NSS = SPI_NSS_Soft;                                   // NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
    S.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;        // ���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
    S.SPI_FirstBit = SPI_FirstBit_MSB;                          // ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    S.SPI_CRCPolynomial = 7;                                    // CRCֵ����Ķ���ʽ
    SPI_Init(TF_SPI_PORT, &S);                                  // ����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
                                                                
    TF_SPI_PORT->CR1 |= 1 << 6;                                 // ʹ��SPI����

    //spiSendByte(0xff);//��������
    TF_CS_HIGH;
}

//SPI �ٶ����ú���
// TF��������������ģʽ��ʶ��ģʽʱԼ400KHz, ����ģʽʱԼ25MHz;
void spiSetSpeed(uint8_t SpeedSet)
{
    // SPI_BaudRatePrescaler_2   2��Ƶ   (SPI 36M@sys 72M)
    S.SPI_BaudRatePrescaler = SpeedSet ;  // SPI_BaudRatePrescaler_8   8��Ƶ   (SPI 9M@sys 72M)
    SPI_Init(SPI1, &S);                   // SPI_BaudRatePrescaler_16  16��Ƶ  (SPI 4.5M@sys 72M)
    SPI_Cmd(SPI1, ENABLE);                // SPI_BaudRatePrescaler_256 256��Ƶ (SPI 281.25K@sys 72M)
}

// SPI��дһ�ֽ�
static uint8_t spiSendByte(uint8_t TxData)
{
    uint8_t retry = 0;
    while ((TF_SPI_PORT->SR & 0x02) == 0)  // ���ָ����SPI��־λ�������:���ͻ���ձ�־λ; SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET
    {
        retry++;
        if (retry > 250) return 0;
    }
    SPI_I2S_SendData(SPI1, TxData);        // ͨ������SPIx����һ������

    retry = 0;
    while ((TF_SPI_PORT->SR & 0x01) == 0)  // ���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
    {
        retry++;
        if (retry > 250) return 0;
    }

    return TF_SPI_PORT->DR;                // ����ͨ��SPIx������յ�����
}

///////////////////////////////////////////////////////////////////////////////////
// TFȡ��ѡ��,�ͷ�SPI����
void tfDisSelect(void)
{
    TF_CS_HIGH;
    spiSendByte(0xff);       // �ṩ�����8��ʱ��
}
//ѡ��sd��,���ҵȴ���׼��OK
//����ֵ:0,�ɹ�;1,ʧ��;
uint8_t tfSelect(void)
{
    TF_CS_LOW;
    if (TF_WaitReady() == 0)
        return 0;            // �ȴ��ɹ�
    tfDisSelect();
    return 1;                // �ȴ�ʧ��
}

//�ȴ���׼����
//����ֵ:0,׼������;����,�������
uint8_t TF_WaitReady(void)
{
    u32 t = 0;
    do
    {
        if (spiSendByte(0XFF) == 0XFF)
            return 0;        // OK
        t++;
    }
    while (t < 0XFFFFFF);    // �ȴ�
    return 1;
}

//�ȴ�TF����Ӧ
//Response:Ҫ�õ��Ļ�Ӧֵ
//����ֵ:0,�ɹ��õ��˸û�Ӧֵ
//    ����,�õ���Ӧֵʧ��
uint8_t tfGetResponse(uint8_t Response)
{
    uint16_t Count = 0xFFFF;                                 // �ȴ�����
    while ((spiSendByte(0XFF) != Response) && Count)Count--; // �ȴ��õ�׼ȷ�Ļ�Ӧ
    if (Count == 0)return MSD_RESPONSE_FAILURE;              // �õ���Ӧʧ��
    else return MSD_RESPONSE_NO_ERROR;                       // ��ȷ��Ӧ
}

//��sd����ȡһ�����ݰ�������
//buf:���ݻ�����
//len:Ҫ��ȡ�����ݳ���.
//����ֵ:0,�ɹ�;����,ʧ��;
uint8_t tfRecvData(uint8_t *buf, uint16_t len)
{
    if (tfGetResponse(0xFE))return 1;  // �ȴ�TF������������ʼ����0xFE
    while (len--)                      // ��ʼ��������
    {
        *buf = spiSendByte(0xFF);
        buf++;
    }
    //������2��αCRC��dummy CRC��
    spiSendByte(0xFF);
    spiSendByte(0xFF);
    return 0;//��ȡ�ɹ�
}

//��sd��д��һ�����ݰ������� 512�ֽ�
//buf:���ݻ�����
//cmd:ָ��
//����ֵ:0,�ɹ�;����,ʧ��;
uint8_t tfSendBlock(uint8_t *buf, uint8_t cmd)
{
    uint16_t t;
    if (TF_WaitReady())return 1;                      // �ȴ�׼��ʧЧ
    spiSendByte(cmd);
    if (cmd != 0XFD)                                  // ���ǽ���ָ��
    {
        for (t = 0; t < 512; t++)spiSendByte(buf[t]); // ����ٶ�,���ٺ�������ʱ��
        spiSendByte(0xFF);                            // ����crc
        spiSendByte(0xFF);
        t = spiSendByte(0xFF);                        // ������Ӧ
        if ((t & 0x1F) != 0x05)return 2;              // ��Ӧ����
    }
    return 0;//д��ɹ�
}

//��TF������һ������
//����: uint8_t cmd   ����
//      u32 arg  �������
//      uint8_t crc   crcУ��ֵ
//����ֵ:TF�����ص���Ӧ
uint8_t tfSendCmd(uint8_t cmd, u32 arg, uint8_t crc)
{
    uint8_t r1;
    uint8_t Retry = 0;
    tfDisSelect();                      // ȡ���ϴ�Ƭѡ
    if (tfSelect())                     
        return 0XFF;                    // ƬѡʧЧ
    //����                              
    spiSendByte(cmd | 0x40);            // �ֱ�д������
    spiSendByte(arg >> 24);
    spiSendByte(arg >> 16);
    spiSendByte(arg >> 8);
    spiSendByte(arg);
    spiSendByte(crc);
    if (cmd == CMD12)spiSendByte(0xff); // Skip a stuff byte when stop reading
    
    Retry = 0XFF;                       // �ȴ���Ӧ����ʱ�˳�
    do
    {
        r1 = spiSendByte(0xFF);
    }
    while ((r1 & 0X80) && Retry--);
    
    return r1;                          // ����״ֵ̬
}

//��ȡTF����CID��Ϣ��������������Ϣ
//����: uint8_t *cid_data(���CID���ڴ棬����16Byte��
//����ֵ:0��NO_ERR
//         1������
uint8_t TF_GetCID(uint8_t *cid_data)
{
    uint8_t r1;
    
    r1 = tfSendCmd(CMD10, 0, 0x01);     // ��CMD10�����CID
    if (r1 == 0x00)
    {
        r1 = tfRecvData(cid_data, 16);  // ����16���ֽڵ�����
    }
    tfDisSelect();                      // ȡ��Ƭѡ
    if (r1)return 1;
    else return 0;
}

//��ȡTF����CSD��Ϣ�������������ٶ���Ϣ
//����:uint8_t *cid_data(���CID���ڴ棬����16Byte��
//����ֵ:0��NO_ERR
//         1������
uint8_t TF_GetCSD(uint8_t *csd_data)
{
    uint8_t r1;
    r1 = tfSendCmd(CMD9, 0, 0x01);     // ��CMD9�����CSD
    if (r1 == 0)
    {
        r1 = tfRecvData(csd_data, 16); // ����16���ֽڵ�����
    }
    tfDisSelect();                     // ȡ��Ƭѡ
    if (r1)return 1;
    else return 0;
}

//��ȡTF����������������������
//����ֵ:0�� ȡ��������
//       ����:TF��������(������/512�ֽ�)
//ÿ�������ֽ�����Ϊ512����Ϊ�������512�����ʼ������ͨ��.
u32 TF_GetSectorCount(void)
{
    uint8_t csd[16];
    u32 Capacity;
    uint8_t n;
    uint16_t csize;
    //ȡCSD��Ϣ������ڼ��������0
    if (TF_GetCSD(csd) != 0) return 0;
    //���ΪSDHC�����������淽ʽ����
    if ((csd[0] & 0xC0) == 0x40)                  // V2.00�Ŀ�
    {
        csize = csd[9] + ((u16)csd[8] << 8) + 1;
        Capacity = (u32)csize << 10;              // �õ�������
    }
    else                                          // V1.XX�Ŀ�
    {
        n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
        csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
        Capacity = (u32)csize << (n - 9);         // �õ�������
    }
    return Capacity;
}



//��ʼ��TF��
// GPIO��SPI�����ͺ�ʶ������
uint8_t TF_Init(void)
{
    uint8_t  r1;                             // ���TF���ķ���ֵ
    uint16_t retry;                          // �������г�ʱ����
    uint8_t  buf[4];
    uint16_t i;
                 
    spiInit();                               // ��ʼ��GPIO, ����SPI���� 
    
    // ����SPI������
    spiSetSpeed(SPI_BaudRatePrescaler_256);  // TF�ϵ��Ϊʶ��ģʽ��TF������ģʽ��ʶ��ģʽ(�����ʣ�Լ400KHz)������ģʽ(������,Լ25MHz��          //
    for (i = 0; i < 100; i++)
        spiSendByte(0XFF);                   // ��������74������
    
    // ����IDLE״̬
    retry = 100;
    do
    {
        r1 = tfSendCmd(CMD0, 0, 0x95);      // ����IDLE״̬
    }
    while ((r1 != 0X01) && retry--);
    
    // �жϿ��ͺţ�������ͬ�����
    g_TfType = 0;
    if (r1 == 0X01)
    {
        if (tfSendCmd(CMD8, 0x1AA, 0x87) == 1)               // TF V2.0
        {
            for (i = 0; i < 4; i++)
                buf[i] = spiSendByte(0XFF);                  // Get trailing return value of R7 resp
            if (buf[2] == 0X01 && buf[3] == 0XAA)            // ���Ƿ�֧��2.7~3.6V
            {
                retry = 0XFFFE;
                do
                {
                    tfSendCmd(CMD55, 0, 0X01);               // ����CMD55
                    r1 = tfSendCmd(CMD41, 0x40000000, 0X01); // ����CMD41
                }
                while (r1 && retry--);
                if (retry && tfSendCmd(CMD58, 0, 0X01) == 0) // ����TF2.0���汾��ʼ
                {
                    for (i = 0; i < 4; i++)
                        buf[i] = spiSendByte(0XFF);          // �õ�OCRֵ
                    if (buf[0] & 0x40)
                        g_TfType = TF_TYPE_V2HC;             // ���ͺţ�V2.0����
                    else                                     
                        g_TfType = TF_TYPE_V2;               // ���ͺţ�V2.0��׼
                }
            }
        }
        else  // TF V1.x/ MMC    V3
        {
            tfSendCmd(CMD55, 0, 0X01);                       // ����CMD55
            r1 = tfSendCmd(CMD41, 0, 0X01);                  // ����CMD41
            if (r1 <= 1)                                     
            {                                                
                g_TfType = TF_TYPE_V1;                       // ���ͺţ�V1.0
                retry = 0XFFFE;                              
                do //�ȴ��˳�IDLEģʽ                        
                {                                            
                    tfSendCmd(CMD55, 0, 0X01);               // ����CMD55
                    r1 = tfSendCmd(CMD41, 0, 0X01);          // ����CMD41
                }                                            
                while (r1 && retry--);                       
            }                                                
            else  // MMC����֧��CMD55+CMD41ʶ��              
            {                                                
                g_TfType = TF_TYPE_MMC;                      // ���ͺţ�MMC V3
                retry = 0XFFFE;                              
                do // �ȴ��˳�IDLEģʽ                       
                {                                            
                    r1 = tfSendCmd(CMD1, 0, 0X01);           // ����CMD1
                }
                while (r1 && retry--);
            }
            if (retry == 0 || tfSendCmd(CMD16, 512, 0X01) != 0)
                g_TfType = TF_TYPE_ERR;                      // ���ͺţ�����Ŀ�
        }                                                    
    }                                                        
    // ���ʶ�𣬽���                                        
    tfDisSelect();                                           // ȡ��Ƭѡ
    spiSetSpeed(SPI_BaudRatePrescaler_2);                    // ���봫��ģʽ���������ͨ�������ˡ�TF������ģʽ��ʶ��ģʽ(�����ʣ�Լ400KHz)������ģʽ(������,Լ25MHz��
                                                             
    tfmalloc();                                              // Ϊfatfs��ر��������ڴ�
                                                             
    if (g_TfType)                                            
    {            
        return 0;
    }
    else                                                     // ʧ��
    {
        if (r1)
        {
            printf("TF����ʼ��ʧ�ܣ�����ţ�%d\r", r1);
            return r1;
        }
    }
    printf("TF����ʼ��ʧ�ܣ�������ʾ֪�Ĵ���\r");
    return 0xaa;       // ��������
}

//��TF��
//buf:���ݻ�����
//sector:����
//cnt:������
//����ֵ:0,ok; ����,ʧ��.
uint8_t TF_ReadDisk(uint8_t *buf, u32 sector, uint8_t cnt)
{
    uint8_t r1;
    if (g_TfType != TF_TYPE_V2HC)sector <<= 9; //ת��Ϊ�ֽڵ�ַ
    if (cnt == 1)
    {
        r1 = tfSendCmd(CMD17, sector, 0X01); //������
        if (r1 == 0) //ָ��ͳɹ�
        {
            r1 = tfRecvData(buf, 512); //����512���ֽ�
        }
    }
    else
    {
        r1 = tfSendCmd(CMD18, sector, 0X01); //����������
        do
        {
            r1 = tfRecvData(buf, 512); //����512���ֽ�
            buf += 512;
        }
        while (--cnt && r1 == 0);
        tfSendCmd(CMD12, 0, 0X01);  //����ֹͣ����
    }
    tfDisSelect();//ȡ��Ƭѡ
    return r1;//
}
//дTF��
//buf:���ݻ�����
//sector:��ʼ����
//cnt:������
//����ֵ:0,ok;����,ʧ��.
uint8_t TF_WriteDisk(uint8_t *buf, u32 sector, uint8_t cnt)
{
    uint8_t r1;
    if (g_TfType != TF_TYPE_V2HC)sector *= 512; //ת��Ϊ�ֽڵ�ַ
    if (cnt == 1)
    {
        r1 = tfSendCmd(CMD24, sector, 0X01); //������
        if (r1 == 0) //ָ��ͳɹ�
        {
            r1 = tfSendBlock(buf, 0xFE); //д512���ֽ�
        }
    }
    else
    {
        if (g_TfType != TF_TYPE_MMC)
        {
            tfSendCmd(CMD55, 0, 0X01);
            tfSendCmd(CMD23, cnt, 0X01); //����ָ��
        }
        r1 = tfSendCmd(CMD25, sector, 0X01); //����������
        if (r1 == 0)
        {
            do
            {
                r1 = tfSendBlock(buf, 0xFC); //����512���ֽ�
                buf += 512;
            }
            while (--cnt && r1 == 0);
            r1 = tfSendBlock(0, 0xFD); //����512���ֽ�
        }
    }
    tfDisSelect();//ȡ��Ƭѡ
    return r1;//
}

// ��ȡTF���ͺ�ֵ
uint8_t TF_GetType(void)
{
    return g_TfType;
}




//Ϊexfuns�����ڴ�
//����ֵ:0,�ɹ�
//1,ʧ��
uint8_t tfmalloc(void)
{
    fs[0] = (FATFS *)mymalloc(sizeof(FATFS)); //Ϊ����0�����������ڴ�
    fs[1] = (FATFS *)mymalloc(sizeof(FATFS)); //Ϊ����1�����������ڴ�
    file = (FIL *)mymalloc(sizeof(FIL));     //Ϊfile�����ڴ�
    ftemp = (FIL *)mymalloc(sizeof(FIL));     //Ϊftemp�����ڴ�
    fatbuf = (uint8_t *)mymalloc(512);             //Ϊfatbuf�����ڴ�

    if (fs[0] && fs[1] && file && ftemp && fatbuf)return 0; //������һ��ʧ��,��ʧ��.
    else return 1;
}

//��Сд��ĸתΪ��д��ĸ,���������,�򱣳ֲ���.
uint8_t char_upper(uint8_t c)
{
    if (c < 'A')return c; //����,���ֲ���.
    if (c >= 'a')return c - 0x20; //��Ϊ��д.
    else return c;//��д,���ֲ���
}
//�����ļ�������
//fname:�ļ���
//����ֵ:0XFF,��ʾ�޷�ʶ����ļ����ͱ��.
//         ����,����λ��ʾ��������,����λ��ʾ����С��.
uint8_t f_typetell(uint8_t *fname)
{
    uint8_t tbuf[5];
    uint8_t *attr = '\0'; //��׺��
    uint8_t i = 0, j;
    while (i < 250)
    {
        i++;
        if (*fname == '\0')break; //ƫ�Ƶ��������.
        fname++;
    }
    if (i == 250)return 0XFF; //������ַ���.
    for (i = 0; i < 5; i++) //�õ���׺��
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
    for (i = 0; i < 4; i++)tbuf[i] = char_upper(tbuf[i]); //ȫ����Ϊ��д
    for (i = 0; i < 6; i++)
    {
        for (j = 0; j < 13; j++)
        {
            if (*FILE_TYPE_TBL[i][j] == 0)break; //�����Ѿ�û�пɶԱȵĳ�Ա��.
            if (strcmp((const char *)FILE_TYPE_TBL[i][j], (const char *)tbuf) == 0) //�ҵ���
            {
                return (i << 4) | j;
            }
        }
    }
    return 0XFF;//û�ҵ�
}

//�õ�����ʣ������
//drv:���̱��("0:"/"1:")
//total:������     ����λKB��
//free:ʣ������     ����λKB��
//����ֵ:0,����.����,�������
uint8_t exf_getfree(uint8_t *drv, u32 *total, u32 *free)
{
    FATFS *fs1;
    uint8_t res;
    u32 fre_clust = 0, fre_sect = 0, tot_sect = 0;
    //�õ�������Ϣ�����д�����
    res = (u32)f_getfree((const TCHAR *)drv, (DWORD *)&fre_clust, &fs1);
    if (res == 0)
    {
        tot_sect = (fs1->n_fatent - 2) * fs1->csize; //�õ���������
        fre_sect = fre_clust * fs1->csize;        //�õ�����������
#if _MAX_SS!=512                                  //������С����512�ֽ�,��ת��Ϊ512�ֽ�
        tot_sect *= fs1->ssize / 512;
        fre_sect *= fs1->ssize / 512;
#endif
        *total = tot_sect >> 1; //��λΪKB
        *free = fre_sect >> 1; //��λΪKB
    }
    return res;
}


