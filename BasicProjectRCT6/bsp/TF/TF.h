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
extern u8 *fatbuf;//SD卡数据缓存区


// 默认使用SPI1，如果使用SPI2\3,需修改TF_FatFs.c中的TF_Init函数部分
#define TF_SPI_PORT      SPI1

#define TF_CS_PORTEN     RCC_APB2Periph_GPIOA
#define TF_CS_GPIO       GPIOA
#define TF_CS_PIN        GPIO_Pin_8

#define TF_CS_HIGH       (TF_CS_GPIO->BSRR = TF_CS_PIN)
#define TF_CS_LOW        (TF_CS_GPIO->BRR  = TF_CS_PIN)
                                  

                                                              
                  




u8  TF_Init(void);                                 // GPIO、SPI、卡型号识别及配置
u8  TF_WaitReady(void);                            // 等待SD卡准备
u8  TF_ReadDisk(u8*buf,u32 sector,u8 cnt);         // 读块
u8  TF_WriteDisk(u8*buf,u32 sector,u8 cnt);        // 写块
u32 TF_GetSectorCount(void);                       // 读扇区数
u8  TF_GetCID(u8 *cid_data);                       // 读SD卡CID
u8  TF_GetCSD(u8 *csd_data);                       // 读SD卡CSD
u8  TF_GetType(void);                              // 获取TF卡型号值
                                                   
u8  f_typetell(u8 *fname);                         //识别文件类型
u8  exf_getfree(u8 *drv,u32 *total,u32 *free);     //得到磁盘总容量和剩余容量
u32 exf_fdsize(u8 *fdname);                        //得到文件夹大小    
                                                   
#endif


