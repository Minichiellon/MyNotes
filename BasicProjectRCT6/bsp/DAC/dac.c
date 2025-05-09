/***********************************************************************************************************************************
  *【文件名称】  dac.c
  *【编写人员】  魔女科技-老周
  *【更新分享】  如有更新，分享在Q群文件夹  262901124
  *【淘    宝】  魔女开发板  https://demoboard.taobao.com
  **********************************************************************************************************************************
  *【代码功能】  输出固定电压值
  *
  *【代码适用】  已于STM32F103xx中验证运行
  *              
  *              
  *【更新记录】  2021-04-21  创建; 参考原子哥、野火大神的代码作编写
  *
***********************************************************************************************************************************/
#include "dac.h"



// 初始化_DAC通道1:PA4, 并设置输出模拟电压：0mA
void DAC_InitChannel1(void)
{  
    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitType;
    // 时钟使能
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );               // 使能DAC通道时钟 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );               // 使能PORTA通道时钟
    // 引脚配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                           // 引脚编号
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;                        // 引脚工作模式：模拟输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);                             // 初始化
    GPIO_SetBits(GPIOA,GPIO_Pin_4)    ;                                  // 打开上拉电阻
    // DAC配置
    DAC_InitType.DAC_Trigger=DAC_Trigger_None;                           // 不使用触发功能 TEN1=0
    DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;           // 不使用波形发生
    DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0; // 屏蔽、幅值设置
    DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;           // DAC1输出缓存关闭 BOFF1=1  
    DAC_Init(DAC_Channel_1,&DAC_InitType);                               // 初始化DAC通道1        
    DAC_Cmd(DAC_Channel_1, ENABLE);                                    // 使能DAC通道1  
    DAC_SetChannel1Voltage(0);                                         // 设置DAC通道1输出电压,mA   
}


// 初始化_DAC通道1:PA5, 并设置输出模拟电压：0mA
void DAC_InitChannel2(void)
{  
    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitType;
    // 时钟使能
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );               // 使能DAC通道时钟 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );               // 使能PORTA通道时钟
    // 引脚配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;                           // 引脚编号
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;                        // 引脚工作模式：模拟输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);                             // 初始化
    GPIO_SetBits(GPIOA,GPIO_Pin_5)    ;                                  // 打开上拉电阻
    // DAC配置
    DAC_InitType.DAC_Trigger=DAC_Trigger_None;                           // 不使用触发功能 TEN1=0
    DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;           // 不使用波形发生
    DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0; // 屏蔽、幅值设置
    DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;           // DAC1输出缓存关闭 BOFF1=1  
    DAC_Init(DAC_Channel_2,&DAC_InitType);                               // 初始化DAC通道1        
    DAC_Cmd(DAC_Channel_2, ENABLE);                                    // 使能DAC通道1  
    DAC_SetChannel2Voltage(0);                                         // 设置DAC通道1输出电压,mA    
}



// 设置通道1电压值，单位：mA; 值域: 0~3300; 
void DAC_SetChannel1Voltage(u16 mA)
{
    u16 dacValue=(float)mA/1000 /3.3 *4096;
       DAC_SetChannel1Data(DAC_Align_12b_R, dacValue);  // 12位右对齐数据格式、DAC值
    //DAC_SoftwareTriggerCmd(DAC_Channel_1,ENABLE);    // 立即更新；可以不用这条语句
}



// 设置通道2电压值，单位：mA; 值域: 0~3300; 
void DAC_SetChannel2Voltage(u16 mA)
{
    u16 dacValue=(float)mA/1000 /3.3 *4096;
       DAC_SetChannel2Data(DAC_Align_12b_R, dacValue);  // 12位右对齐数据格式、DAC值
    //DAC_SoftwareTriggerCmd(DAC_Channel_2,ENABLE);    // 立即更新；可以不用这条语句
}

