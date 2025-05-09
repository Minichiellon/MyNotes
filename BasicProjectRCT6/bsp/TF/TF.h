#ifndef __TF_H_
#define __TF_H         

#include <stm32f10x.h>
#include "ff.h"


extern FATFS *fs[2];  
extern FIL *file;     
extern FIL *ftemp;     
extern UINT br,bw;
extern FILINFO fileinfo;
extern DIR dir;
extern u8 *fatbuf;//SD�����ݻ�����


// Ĭ��ʹ��SPI1�����ʹ��SPI2\3,���޸�TF_FatFs.c�е�TF_Init��������
#define TF_SPI_PORT      SPI1

#define TF_CS_PORTEN     RCC_APB2Periph_GPIOA
#define TF_CS_GPIO       GPIOA
#define TF_CS_PIN        GPIO_Pin_8

#define TF_CS_HIGH       (TF_CS_GPIO->BSRR = TF_CS_PIN)
#define TF_CS_LOW        (TF_CS_GPIO->BRR  = TF_CS_PIN)
                                  

                                                              
                  




u8  TF_Init(void);                                 // GPIO��SPI�����ͺ�ʶ������
u8  TF_WaitReady(void);                            // �ȴ�SD��׼��
u8  TF_ReadDisk(u8*buf,u32 sector,u8 cnt);         // ����
u8  TF_WriteDisk(u8*buf,u32 sector,u8 cnt);        // д��
u32 TF_GetSectorCount(void);                       // ��������
u8  TF_GetCID(u8 *cid_data);                       // ��SD��CID
u8  TF_GetCSD(u8 *csd_data);                       // ��SD��CSD
u8  TF_GetType(void);                              // ��ȡTF���ͺ�ֵ
                                                   
u8  f_typetell(u8 *fname);                         //ʶ���ļ�����
u8  exf_getfree(u8 *drv,u32 *total,u32 *free);     //�õ�������������ʣ������
u32 exf_fdsize(u8 *fdname);                        //�õ��ļ��д�С    
                                                   
#endif


