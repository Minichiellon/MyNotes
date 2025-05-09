/***********************************************************************************************************************************
    *   @file        lcd_ili9341.h
    *   @date     2020-12-13        
    *   @author   魔女开发板    
    *   @brief    更新下载     更新Q群文件夹：262901124
    *   @taobao   淘宝硬件     ttps://demoboard.taobao.com/
    ********************************************************************************************************************************
    *    【实验平台】 魔女开发板_STM32F103RC + KEIL5.27 + 2.8寸显示屏_ILI9341
    *
    *   【移植说明】 
    *    1：本代码使用在F103RC上，使用IO模块FSMC通信，注意引脚的修改
    *    2：汉字的显示，使用开发板上的外部FLASH中字库
    *     
    *   【更新记录】
    *    2020-09-xx  创建
    *    2020-12-12  完善修改中英文显示函数 
    *    2020-12-xx  下次完善：更方便的引脚移植修改
    
************************************************************************************************************************************/
#include "bsp_lcd_2.8_ili93xx.h" 
#include "font.h" 
#include "hardware.h"

 

/*****************************************************************************
 ** 变量声明
 *****************************************************************************/
_LCD xLCD;



/*****************************************************************************
 ** 函数声明
 ****************************************************************************/
void sendOrder (u16 data);                                    // 向LCD发送：寄存器地址值(指令), 8位
void sendDataShort (u16 data);                                // 向LCD发送：数据，8位
u16  readData (void);                                         // 读取LCD返回的数据
void sendShort(u16 Data);                                     // 向液晶屏发送：数据，16位
void setCursor(u16 xStart,u16 yStart,u16 xEnd,u16 yEnd);      // 设置显示区域
u16 readReg(u16 reg);



// 本地US粗略延时函数，减少移植时对外部文件依赖；
#if 0
static void delayUS( u32 times)
{
    times=times*7;      //  10us内用7;
    while(--times)         
        __nop();  
}
#endif 

// ms延时函数，减少移植时对外部文件依赖；
#if 1
static void delayMS(u32 ms)
{
    ms=ms*6500;                  
    for(u32 i=0; i<ms; i++);      // 72MHz系统时钟下，多少个空循环约耗时1ms
}
#endif




/*****************************************************************************
*函  数：LCD_Init
*功  能：初始化lcd
*参  数：为了简化理解、便于移植，参数(屏幕像素、显示方向)在h文件里进行修改
*返回值：无 
*备  注：
*****************************************************************************/
void LCD_Init(void)
{ 
    RCC->APB2ENR|=1<<3;       // 先使能外设PORTB时钟
    RCC->APB2ENR|=1<<4;       // 先使能外设PORTC时钟

    // 使能GPIO时针, 为减少调试过程忘了使能时钟而出错，把相关GPIO端口时钟，都使能了；
    RCC->APB2ENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD  ; 
    GPIO_InitTypeDef G;                     // 作用：配置引脚工作模式 
    G.GPIO_Pin  = GPIO_Pin_15;              // 引脚KEY_1, 闲时下拉(重要)， 按下时被置高电平
    G.GPIO_Mode = GPIO_Mode_Out_PP ;        // 闲时电平状态(使用芯片内部电阻进行电平上下拉)
    G.GPIO_Speed = GPIO_Speed_10MHz ;       // 输出电平反转速度；在输入状态时无效，但GPIO_Init函数需要用到；
    GPIO_Init(GPIOA, &G);                   // 初始化，调用引脚工作模式配置函数
    
    
    System_SwdMode ();
                                             
    //PORTC6~10复用推挽输出     
    GPIOC->CRH&=0XFFFFF000;
    GPIOC->CRH|=0X00000333;   // PB8 9 10     推挽输出
    GPIOC->CRL&=0X00FFFFFF;
    GPIOC->CRL|=0X33000000;   // PB6 7
    GPIOC->ODR|=0X07C0;       // PB6 7 8 9 10 全部上拉
    //PORTB 推挽输出     
    GPIOB->CRH=0X33333333;    // PB0~15       推挽输出
    GPIOB->CRL=0X33333333;      
    GPIOB->ODR=0XFFFF;        // PB0~15       全部上拉

    
    delayMS(100);            // delay 50 ms 

     //尝试9341 ID的读取        
    sendOrder(0XD3);          // 指令：读ID               
    readData();               // 第1个参数：dummy     
    readData();               // 第2个参数：IC版本号
    xLCD.id=readData();       // 第3个参数：IC名字(93)                                   
    xLCD.id<<=8;
    xLCD.id|=readData();      // 第4个参数：IC名字(41)    

    printf("显示屏 检测...        %x\r\n",xLCD.id);  // 打印LCD ID  
    if(xLCD.id !=0X9341)                             // 9341初始化失败
        return;

    sendOrder(0xCF);  
    sendDataShort(0x00); 
    sendDataShort(0xC1); 
    sendDataShort(0X30); 
    sendOrder(0xED);  
    sendDataShort(0x64); 
    sendDataShort(0x03); 
    sendDataShort(0X12); 
    sendDataShort(0X81); 
    sendOrder(0xE8);  
    sendDataShort(0x85); 
    sendDataShort(0x10); 
    sendDataShort(0x7A); 
    sendOrder(0xCB);  
    sendDataShort(0x39); 
    sendDataShort(0x2C); 
    sendDataShort(0x00); 
    sendDataShort(0x34); 
    sendDataShort(0x02); 
    sendOrder(0xF7);  
    sendDataShort(0x20); 
    sendOrder(0xEA);  
    sendDataShort(0x00); 
    sendDataShort(0x00); 
    sendOrder(0xC0);      // Power control 
    sendDataShort(0x1B);  // VRH[5:0] 
    sendOrder(0xC1);      // Power control 
    sendDataShort(0x01);  // SAP[2:0];BT[3:0] 
    sendOrder(0xC5);      // VCM control 
    sendDataShort(0x30);  // 3F
    sendDataShort(0x30);  // 3C
    sendOrder(0xC7);      // VCM control2 
    sendDataShort(0XB7); 
    sendOrder(0x36);      // Memory Access Control 
    sendDataShort(0x48); 
    sendOrder(0x3A);   
    sendDataShort(0x55); 
    sendOrder(0xB1);   
    sendDataShort(0x00);   
    sendDataShort(0x1A); 
    sendOrder(0xB6);      // Display Function Control 
    sendDataShort(0x0A); 
    sendDataShort(0xA2); 
    sendOrder(0xF2);      // 3Gamma Function Disable 
    sendDataShort(0x00); 
    sendOrder(0x26);      // Gamma curve selected 
    sendDataShort(0x01); 
    sendOrder(0xE0);      // Set Gamma 
    sendDataShort(0x0F); 
    sendDataShort(0x2A); 
    sendDataShort(0x28); 
    sendDataShort(0x08); 
    sendDataShort(0x0E); 
    sendDataShort(0x08); 
    sendDataShort(0x54); 
    sendDataShort(0XA9); 
    sendDataShort(0x43); 
    sendDataShort(0x0A); 
    sendDataShort(0x0F); 
    sendDataShort(0x00); 
    sendDataShort(0x00); 
    sendDataShort(0x00); 
    sendDataShort(0x00);          
    sendOrder(0XE1);      // Set Gamma 
    sendDataShort(0x00); 
    sendDataShort(0x15); 
    sendDataShort(0x17); 
    sendDataShort(0x07); 
    sendDataShort(0x11); 
    sendDataShort(0x06); 
    sendDataShort(0x2B); 
    sendDataShort(0x56); 
    sendDataShort(0x3C); 
    sendDataShort(0x05); 
    sendDataShort(0x10); 
    sendDataShort(0x0F); 
    sendDataShort(0x3F); 
    sendDataShort(0x3F); 
    sendDataShort(0x0F); 
    sendOrder(0x2B); 
    sendDataShort(0x00);
    sendDataShort(0x00);
    sendDataShort(0x01);
    sendDataShort(0x3f);
    sendOrder(0x2A); 
    sendDataShort(0x00);
    sendDataShort(0x00);
    sendDataShort(0x00);
    sendDataShort(0xef);     
    sendOrder(0x11);      // Exit Sleep
    delayMS(120);
    sendOrder(0x29);      // display on    
    
    LCD_DisplayDir(LCD_DIR);   // 设置显示方向
    LCD_Fill(0, 0, xLCD.width-1, xLCD.height-1, BLACK);
    LCD_LED=1;                 // 点亮背光
    xLCD.FlagInit =1;    
}          

// 底层函数-1，向LCD发送寄存器地址(指令)
void sendOrder(u16 data)
{ 
    
    LCD_RS_LOW;          // RS=0:指令，RS=1:数据
     LCD_CS_LOW;         // 片选信号拉低，选定设备
    GPIOB->ODR = data;   // 赋值数据
    LCD_WR_LOW;          // WR=0：设备可以读取数据，WR=1：设备等待读取数据
    LCD_WR_HIGH; 
     LCD_CS_HIGH;        // 片选信号拉高，
}

// 底层函数-2
// 写数据函数
// 可以替代sendDataShortX宏,拿时间换空间.
// data:寄存器值
#if 1
void sendDataShort(u16 data)
{
    LCD_RS_HIGH;        // RS高: 数据值， RS低: 寄存器地址值
    LCD_CS_LOW;         // 片选 
    GPIOB->ODR = data;  // PB0~15作为数据线
    LCD_WR_LOW;         // 写数据
    LCD_WR_HIGH;        // 写数据 
    LCD_CS_HIGH;        // 片选
}
#else
//写数据函数
#define sendDataShort(data)\
        {\
            LCD_RS_HIGH;\
            LCD_CS_LOW;\
            GPIOB->ODR = data;\
            LCD_WR_LOW;\
            LCD_WR_HIGH;\
            LCD_CS_HIGH;\
            } 
#endif

// 底层函数-3
// 读LCD数据
// 返回值:读到的值
u16 readData(void)
{
    // 把PB0~15设置为：上拉输入
    GPIOB->CRL=0X88888888; //PB0-7  上拉输入
    GPIOB->CRH=0X88888888; //PB8-15 上拉输入
    GPIOB->ODR=0X0000;     //全部输出0

    //读取数据 (读寄存器时,并不需要读2次)
    LCD_RS_HIGH;
    LCD_CS_LOW;    
    LCD_RD_LOW;
    //if(xLCD.id==0X8989) delay_us(2);//FOR 8989,延时2us                       
    u16 reg = GPIOB->IDR;  
    LCD_RD_HIGH;
    LCD_CS_HIGH; 

    // 把PB0~15设置为：上拉输入
    GPIOB->CRL=0X33333333; //PB0-7  上拉输出
    GPIOB->CRH=0X33333333; //PB8-15 上拉输出
    GPIOB->ODR=0XFFFF;     //全部输出高
    
    return reg;  
}

//读寄存器
//LCD_Reg:寄存器编号
//返回值:读到的值
u16 readReg(u16 reg)
{                                           
     sendOrder (reg);    // 寄存器  
    return readData(); 
} 

//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值
u16 LCD_BGR2RGB(u16 c)
{
    u16  r,g,b,rgb;   
    b=(c>>0)&0x1f;
    g=(c>>5)&0x3f;
    r=(c>>11)&0x1f;     
    rgb=(b<<11)+(g<<5)+(r<<0);         
    return(rgb);
}    
//当mdk -O1时间优化时需要设置
//延时i
void opt_delay(u8 i)
{
    while(i--);
}
//读取个某点的颜色值     
//x,y:坐标
//返回值:此点的颜色
u16 LCD_ReadPoint(u16 x,u16 y)
{
     u16 r,g,b;
    
    if(x>=xLCD.width||y>=xLCD.height)return 0;    //超过了范围,直接返回           
    setCursor(x,y, x+1, y+1);
    sendOrder(0X2E);//9341/6804/3510/1963 发送读GRAM指令
    GPIOB->CRL=0X88888888;                             //PB0-7  上拉输入
    GPIOB->CRH=0X88888888;                             //PB8-15 上拉输入
    GPIOB->ODR=0XFFFF;                                 //全部输出高

    LCD_RS_HIGH;
    LCD_CS_LOW;        
    //读取数据(读GRAM时,第一次为假读)    
    LCD_RD_LOW;           
    opt_delay(2);                                      //延时        
    r=GPIOB->IDR;                                      //实际坐标颜色 
    LCD_RD_HIGH;

     //dummy READ
    LCD_RD_LOW;                       
    opt_delay(2);    // 延时                       
     r=GPIOB->IDR;   // 实际坐标颜色
    LCD_RD_HIGH;

    LCD_RD_LOW;                       
    opt_delay(2);    // 延时               
    b=GPIOB->IDR;    // 读取蓝色值        
    LCD_RD_HIGH;
    g=r&0XFF;        // 对于9341,第一次读取的是RG的值,R在前,G在后,各占8位
    g<<=8;

    LCD_CS_HIGH;
    GPIOB->CRL=0X33333333;         // PB0-7  上拉输出
    GPIOB->CRH=0X33333333;         // PB8-15 上拉输出
    GPIOB->ODR=0XFFFF;             // 全部输出高  
    
    return (((r>>11)<<11)|((g>>10)<<5)|(b>>11)); // ILI9341/NT35310/NT35510需要公式转换一下
    
}         
//LCD开启显示
void LCD_DisplayOn(void)
{                       
    sendOrder(0X29);    //开启显示
    LCD_LED =1;

}     
//LCD关闭显示
void LCD_DisplayOff(void)
{       
    sendOrder(0X28);    //关闭显示
    LCD_LED =0;
}   

/*****************************************************************
 * 函  数：setCursor
 * 功  能：设置显示区域，在此区域写点数据自动换行
 * 参  数：xStart横坐标起点, yStart纵坐标起点
 *         xEnd横坐标线束点，yEnd纵坐标线束点
 * 返回值：无
 *
 * 备  注： 魔女开发板团队编写   淘宝 https://demoboard.taobao.com
 * 分  享： 不断更新于Q群文件夹：262901124  最后修改_2020年09月014日
******************************************************************/
void setCursor(u16 xStart,u16 yStart,u16 xEnd,u16 yEnd)
{             
    sendOrder (0X2A);                 // 发送指令：设置x坐标
    sendDataShort(xStart>>8);
    sendDataShort(xStart&0xFF);
    sendDataShort(xEnd>>8);
    sendDataShort(xEnd&0xFF);     
    
    sendOrder (0X2B); 
    sendDataShort(yStart>>8);
    sendDataShort(yStart&0xFF);
    sendDataShort(yEnd>>8);
    sendDataShort(yEnd&0xFF);     

    // 发送写GRAM指令
    sendOrder(0X2C);    
 
}          
 

// 设置LCD显示方向
// 四种显示方向，0-正竖屏，3-倒竖屏，5-正横屏, 6-倒横屏
void LCD_DisplayDir(u8 scanDir)
{
    u16 regval=0;
    
    if(scanDir==0||scanDir==3)     // 竖屏
    {
        xLCD.dir=0;          
        xLCD.width  = LCD_WIDTH;
        xLCD.height = LCD_HIGH ;
    }
    if(scanDir==5 || scanDir==6)   // 横屏
    {                      
        xLCD.dir=1;          
        xLCD.width  = LCD_HIGH;
        xLCD.height = LCD_WIDTH;
    }         
    
    if(scanDir==0) regval|=(0<<7)|(0<<6)|(0<<5); // 从左到右,从上到下
    if(scanDir==3) regval|=(1<<7)|(1<<6)|(0<<5); // 从右到左,从下到上
    if(scanDir==5) regval|=(0<<7)|(1<<6)|(1<<5); // 从上到下,从右到左
    if(scanDir==6) regval|=(1<<7)|(0<<6)|(1<<5); // 从下到上,从左到右           
    sendOrder (0X36);             // 读写方向，颜色模式
    sendDataShort(regval|0x08);          // 
}     

/*****************************************************************
 * 函  数：drawPoint
 * 功  能：画一个点
 * 参  数：x坐标，y坐标, 16位颜色值
 * 返回值：无
 * 
 * 备  注： 魔女开发板团队编写   淘宝 https://demoboard.taobao.com
 * 分  享： 不断更新于Q群文件夹：262901124  最后修改_2020年09月01日
******************************************************************/
void LCD_DrawPoint(u16 x, u16 y, u16 color)
{
    setCursor(x,y, x+1, y+1);        //设置光标位置 
    sendDataShort(color); 
}    

//在指定区域内填充指定颜色
//区域大小:(xend-xsta+1)*(yend-ysta+1)
//xsta
//color:要填充的颜色
void LCD_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 color)
{          
    u32 CNT=(ex+1-sx)*(ey+1-sy);
    setCursor(sx, sy, ex, ey);                  // 设置光标位置 
    
    LCD_RS_HIGH;             // RS高: 数据值， RS低: 寄存器地址值
    LCD_CS_LOW;              // 片选 
    while(CNT--)
    {
        GPIOB->ODR = color;  // PB0~15作为数据线
        LCD_WR_LOW;          // 写数据
        LCD_WR_HIGH;         // 写数据 
    }
    LCD_CS_HIGH;             // 片选
}  

//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void LCD_Line(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    u16 t; 
    int xerr=0,yerr=0,delta_x,delta_y,distance; 
    int incx,incy,uRow,uCol; 
    delta_x=x2-x1; //计算坐标增量 
    delta_y=y2-y1; 
    uRow=x1; 
    uCol=y1; 
    if(delta_x>0)incx=1; //设置单步方向 
    else if(delta_x==0)incx=0;//垂直线 
    else {incx=-1;delta_x=-delta_x;} 
    if(delta_y>0)incy=1; 
    else if(delta_y==0)incy=0;//水平线 
    else{incy=-1;delta_y=-delta_y;} 
    if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
    else distance=delta_y; 
    for(t=0;t<=distance+1;t++ )//画线输出 
    {  
        LCD_DrawPoint(uRow,uCol, color);//画点 
        xerr+=delta_x ; 
        yerr+=delta_y ; 
        if(xerr>distance) 
        { 
            xerr-=distance; 
            uRow+=incx; 
        } 
        if(yerr>distance) 
        { 
            yerr-=distance; 
            uCol+=incy; 
        } 
    }  
}    
//画矩形      
//(x1,y1),(x2,y2):矩形的对角坐标
void LCD_Rectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    LCD_Line(x1,y1,x2,y1, color);
    LCD_Line(x1,y1,x1,y2, color);
    LCD_Line(x1,y2,x2,y2, color);
    LCD_Line(x2,y1,x2,y2, color); 
}
//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void LCD_Circle(u16 x0,u16 y0,u8 r, u16 color)
{
    int a,b;
    int di;
    a=0;b=r;      
    di=3-(r<<1);             //判断下个点位置的标志
    while(a<=b)
    {
        LCD_DrawPoint(x0+a,y0-b, color);             //5
        LCD_DrawPoint(x0+b,y0-a, color);             //0           
        LCD_DrawPoint(x0+b,y0+a, color);             //4               
        LCD_DrawPoint(x0+a,y0+b, color);             //6 
        LCD_DrawPoint(x0-a,y0+b, color);             //1       
        LCD_DrawPoint(x0-b,y0+a, color);             
        LCD_DrawPoint(x0-a,y0-b, color);             //2             
        LCD_DrawPoint(x0-b,y0-a, color);             //7                  
        a++;
        //使用Bresenham算法画圆     
        if(di<0)di +=4*a+6;      
        else
        {
            di+=10+4*(a-b);   
            b--;
        }                             
    }
}                                       



/******************************************************************
 * 函数名： drawAscii
 * 功  能： 在指定位置显示一个字符    
 * 参  数： u16 x,y     起始坐标
 *          u8  num     要显示的字符:" "--->"~"
 *          u8  size    字体大小 12/16/24/32
 *          u32 fColor  字体颜色
 *          u32 bColor  背景颜色
 * 备  注： 参考原子哥和野火大神的代码后修改  资料存放Q群：262901124 
 *****************************************************************/
void drawAscii(u16 x,u16 y,u8 num,u8 size,u32 fColor, u32 bColor)
{              
    //spiInit();                                      // 防止SPI参数被其它设备修改了
     
    u8 temp;
    u16 y0=y;
    
    u8 csize=(size/8+((size%8)?1:0))*(size/2);           // 得到字体一个字符对应点阵集所占的字节数    
     num=num-' ';                                       // 得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
    for(u8 t=0;t<csize;t++)
    {   
        if(size==12)         temp=asc2_1206[num][t];   // 调用1206字体
        else if(size==16)    temp=asc2_1608[num][t];   // 调用1608字体
        else if(size==24)    temp=asc2_2412[num][t];   // 调用2412字体
        else if(size==32)    temp=asc2_3216[num][t];   // 调用3216字体
        else return;                                   // 没有的字库
        
        for(u8 t1=0; t1<8; t1++)
        {                
            if(temp&0x80) LCD_DrawPoint (x, y, fColor);// 字体 画点 
            else          LCD_DrawPoint (x, y, bColor);// 背景 画点
            temp<<=1;
            y++;
            if(y>=xLCD.height)    return;              // 超出屏幕高度(底)
            if((y-y0)==size)
            {
                y=y0;
                x++;
                if(x>=xLCD.width) return;              // 超出屏幕宽度(宽)
                break;
            }
        }       
    }                     
} 

/******************************************************************
 * 函数名： drawGBK
 * 功  能： 在指定位置显示一个字符    
 * 参  数： u16 x,y     起始坐标
 *          u8  num     要显示的字符:" "--->"~"
 *          u8  size    字体大小 12/16/24/32
 *          u32 fColor  字体颜色
 *          u32 bColor  背景颜色
 * 备  注： 参考原子哥和野火大神的代码后修改  资料存放Q群：262901124 
 *****************************************************************/
void drawGBK( u16 x, u16 y, u8* font, u8 size, u32 fColor, u32 bColor)
{    
    u8 temp;
    u16 y0=y;
    u8 GBK[128];   
    u8 csize=(size/8+((size%8)?1:0))*(size); // 得到字体一个字符对应点阵集所占的字节数     
    W25qxx_ReadGBK(font, size, GBK);         // 得到相应大小的点阵数据
    
    //spiInit();                             // 防止SPI参数被其它设备修改了    
    for(u8 t=0; t<csize; t++)
    {                                                      
        temp = GBK[t];                       // 得到GBK点阵数据                          
        for(u8 t1=0; t1<8; t1++)
        {
            if(temp&0x80)   LCD_DrawPoint (x, y, fColor);    
            else            LCD_DrawPoint (x, y, bColor);            
            temp<<=1;
            y++;
            if((y-y0)==size)
            {
                y=y0;
                x++;
                break;
            }
        }       
    }      
}



/******************************************************************************
 * 函  数： LCD_String
 * 功  能： 在LCD上显示字符串(支持英文、汉字)
 * 描  述： 英文：字模数据保存在font.h，编译后和代码一起保存在芯片内部Flash
 *          汉字：字模保存在外部Flash中，本函数字库在W25Q128中
 *                魔女开发板中W25Q128已烧录宋体4种字号大小字模数据
 * 参  数： u16   x      字体左上角X坐标
 *          u16   y      字体左上角y坐标
 *          char* pFont  要显示的字符串数据
 *          u8    size   字号大小：12 16 24 32
 *          u32   fColor 字体颜色
 *          u32   bColor 背景颜色
 * 返回值:  无
 * 备  注： 魔女开发板   资料更新Q群：262901124    最后修改_2020年05月1８日
 ******************************************************************************/  
void LCD_String(u16 x, u16 y, char* pFont, u8 size, u32 fColor, u32 bColor)
{
    if(xLCD .FlagInit ==0 ) return;
    
    u16 xStart = x;    

    if( size!=12 && size!=16 && size!=24 && size!=32 )       // 字体大小控制
        size=24;    
    
    while(*pFont != 0)                // 连续读取字符串数据，直到'\0'时停止
    {    
        if(x>(xLCD.width-size))       // 行位置判断，如果到了行末，就把光标换行
        {
            x=xStart;
            y=y+size;
        }
        if(y>(xLCD.height - size))    // 列位置判断，如果到了列末，就返回，不再输出
            return;        
        
        if(*pFont < 127)              // ASCII字符
        {
            drawAscii (x, y, *pFont, size, fColor, bColor);            
            pFont++;
            x+=size/2;            
        }
        else                          // 汉字显示
        {     
            // 重要: 如果用的不是魔女开发板的字库, 就要修改或注释下面这一行, 这样就不影响ASCII英文字符的输出            
            drawGBK(x, y, (u8*)pFont, size, fColor, bColor);     
            pFont = pFont+2;          // 下一个要显示的数据在内存中的位置              
            x=x+size;                 // 下一个要显示的数据在屏幕上的X位置    
        }
    }      
}



/******************************************************************
 * 函数名： LCD_Image
 * 功  能： 在指定区域内填充指定图片数据
 *          图片数据在font.h文件中.只适合少量图片保存
 *          Image2Lcd转换：水平扫描，16位真彩色
 * 参  数： u16 x,y     左上角起始坐标
 *          u16 width   图片宽度
 *          u16 height  图片高度
 *          u8* image   数据缓存地址
 * 备  注： 魔女开发板团队  资料存放Q群：262901124        最后修改_2020年09月2日
 *****************************************************************/
void LCD_Image(u16 x, u16 y, u16 width, u16 height, const u8 *image) 
{  
    //spiInit();                               // 防止SPI参数被其它设备修改了
    
    u16 colorData=0;
    u32 cnt=0;
    
     for(u16 i=0; i<height; i++)               // 一行一行地显示
    {
         setCursor(x, y+i, x+width, y+height); // 重新设置光标位置 
        for(u16 j=0; j<width; j++)             // 一行中，从左到事，逐个像素处理
        {            
            colorData=(image[cnt*2+1]<<8) | image[cnt*2]; 
            sendDataShort(colorData);          // 写入16位颜色数据 
            cnt++;    
        }
    }      
}  



/******************************************************************
 * 函数名： LCD_Cross
 * 功  能： 在指定点上绘制十字线，用于校准触摸屏
 * 参  数： uint16_t x  　   十字线的中心点坐标x
 *          uint16_t y  　   十字线的中心点坐标x
 *          uint16_t len     十字线的像素长度
 *          uint32_t fColor  颜色
 * 返　回： 无
 * 备  注： 
 *****************************************************************/
void LCD_Cross( uint16_t x, uint16_t y, uint16_t len, uint32_t fColor)
{
    uint16_t temp=len/2;                
                
    LCD_Line(x-temp, y, x+temp, y, fColor);   
    LCD_Line(x, y-temp, x, y+temp, fColor);       
}



/******************************************************************
 * 函数名： LCD_GUI
 * 功  能： 测试板载设备情况的LCD显示方案
 * 参  数：           
 * 返　回： 
 * 备  注： 
 *****************************************************************/
void LCD_GUI(void)
{
    char strTemp[30];
    
    LCD_Fill(0, 0,  xLCD.width,  xLCD.height, BLACK);
    //LCD_Image (0,0, 60, 60, imageLoge);  // 图片显示函数
    LCD_Line(  0, 49,  0,329,GREY);        // 左竖
    LCD_Line(119, 70,119,329,GREY);        // 中竖
    LCD_Line(239, 49,239,329,GREY);        // 右竖
    LCD_Fill(  0,287,239,329,WHITE);       // 底白
    LCD_Line(0,303,239,303,BLACK);
    LCD_Line(119,287,119,329,BLACK);
    LCD_Fill (0,49, 239,70,WHITE);    
    LCD_Line(119, 49,119, 70,BLACK);       // 中竖
    LCD_Fill (119,168, 239,188,WHITE);    
    
    LCD_String( 60,  0,"魔女开发板", 24, WHITE, BLACK);    
    LCD_String(  8, 28,"STM32F103RCT6 - 设备检测监控", 16, GREY, BLACK);        
    LCD_String(  6, 52,"板载设备", 16,  BLACK , WHITE);
    LCD_String(125, 52,"WiFi无线通信", 16, BLACK , WHITE);
    LCD_String(125,171,"CAN通信" , 16, BLACK , WHITE);    
    LCD_String(  6,290,"内部温度", 12, BLACK , WHITE);     // 内部温度
    LCD_String(  6,306,"启动次数", 12, BLACK , WHITE);     // 启动次数
    LCD_String(125,290,"触摸坐标", 12, BLACK , WHITE);     // 触摸坐标
    LCD_String(125,306,"运行时长", 12, BLACK , WHITE);     // 运行时长
    sprintf(strTemp, "第%d次", xW25Qxx .StartupTimes);
    LCD_String( 68,306,strTemp,12,BLUE,WHITE);
    
    u16 y=74;
    // UASRT1
    LCD_String( 6, y, "UART1配置",  12, YELLOW ,BLACK);  
    if(xUSART.USART1InitFlag ==1)  { LCD_String(90,y,"完成", 12, GREEN ,BLACK); }   else    { LCD_String(90,y,"失败", 12, RED ,BLACK); }  y=y+15;
    // SystemClock
    LCD_String( 6, y, "系统时钟",   12, YELLOW ,BLACK);   
    sprintf(strTemp,"%d",SystemCoreClock/1000000);
    LCD_String(84, y,strTemp,       12, GREEN ,BLACK);  
    LCD_String(96, y, "MHz", 12, GREEN ,BLACK);   y=y+15;
    // LED指示灯
    LCD_String( 6, y, "LED指示灯",  12, YELLOW ,BLACK);   
    LCD_String(90,y,"完成", 12, GREEN ,BLACK);    y=y+15;
    // 按键中断
    LCD_String( 6, y, "按键中断",   12, YELLOW ,BLACK);   
    LCD_String(90,y,"完成", 12, GREEN ,BLACK);    y=y+15;
    // FLASH存储器
    LCD_String( 6, y, "FLASH存储",  12, YELLOW ,BLACK);   
    if(xW25Qxx.FlagInit  ==1)  { LCD_String(71,y,xW25Qxx.type, 12, GREEN ,BLACK); }   else    { LCD_String(90,y,"失败", 12, RED ,BLACK); }  y=y+15;
    // 汉字字库
    LCD_String( 6, y, "汉字字库",   12, YELLOW ,BLACK);  
    if(xW25Qxx .FlagGBKStorage ==1)  { LCD_String(90,y,"正常", 12, GREEN ,BLACK); }   else    { LCD_String(90,y,"失败", 12, RED ,BLACK); }  y=y+15;
    // 显示屏
    LCD_String( 6, y, "显示屏芯片", 12, YELLOW ,BLACK);  
    sprintf(strTemp,"%X",xLCD.id);        
    if(xLCD.FlagInit  ==1)  { LCD_String(90,y,strTemp, 12, GREEN ,BLACK); }   else    { LCD_String(90,y,"失败", 12, RED ,BLACK); }  y=y+15;
           
}

