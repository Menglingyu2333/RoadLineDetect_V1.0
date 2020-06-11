#include "lcd.h"
#include "FONT.h"
//#include "stdlib.h"
//#include "usart.h"
//#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//测试硬件：单片机STM32F103RBT6,主频72M  单片机工作电压3.3V
//QDtech-TFT液晶驱动 for STM32 IO模拟
//xiao冯@ShenZhen QDtech co.,LTD
//公司网站:www.qdtech.net
//淘宝网站：http://qdtech.taobao.com
//我司提供技术支持，任何技术问题欢迎随时交流学习
//固话(传真) :+86 0755-23594567
//手机:15989313508（冯工）
//邮箱:QDtech2008@gmail.com
//Skype:QDtech2008
//技术交流QQ群:324828016
//创建日期:2013/5/13
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 深圳市全动电子技术有限公司 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************************************
//=======================================液晶屏数据线接线==========================================//
STM32 PB组接液晶屏DB0~DB16,举例依次为DB0接PB0,..DB15接PB15.
//=======================================液晶屏控制线接线==========================================//
//LCD_CS	接PC9	//片选信号
//LCD_RS	接PC8	//寄存器/数据选择信号
//LCD_WR	接PC7	//写信号
//LCD_RD	接PC6	//读信号
//LCD_RST	接PC5	//复位信号
//LCD_LED	接PC10	//背光控制信号(高电平点亮)
//=========================================触摸屏触接线=========================================//
//不使用触摸或者模块本身不带触摸，则可不连接
//MO(MISO)	接PC2	//SPI总线输出
//MI(MOSI)	接PC3	//SPI总线输入
//PEN		接PC1	//触摸屏中断信号
//TCS		接PC13	//触摸IC片选
//CLK		接PC0	//SPI总线时钟
**************************************************************************************************/

//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;

//画笔颜色,背景颜色
u16 POINT_COLOR = 0x0000,BACK_COLOR = 0xFFFF;
u16 DeviceCode;



//******************************************************************
//函数名：  LCD_WR_REG
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    向液晶屏总线写入写16位指令
//输入参数：Reg:待写入的指令值
//返回值：  无
//修改记录：无
//******************************************************************
void LCD_WR_REG(u8 data)
{
#if LCD_USE8BIT_MODEL==1//使用8位并行数据总线模式
	LCD_RS_CLR;
	LCD_CS_CLR;
	DATAOUT(data<<8);
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;

#else//使用16位并行数据总线模式
	LCD_RS_CLR;
	LCD_CS_CLR;
	DATAOUT(data);
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;

#endif
}

//******************************************************************
//函数名：  LCD_WR_DATA
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    向液晶屏总线写入写16位数据
//输入参数：Data:待写入的数据
//返回值：  无
//修改记录：无
//******************************************************************
void LCD_WR_DATA(u16 data)
{


#if LCD_USE8BIT_MODEL==1//使用8位并行数据总线模式
	LCD_RS_SET;
	LCD_CS_CLR;
	DATAOUT(data<<8);
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;

#else//使用16位并行数据总线模式
	LCD_RS_SET;
	LCD_CS_CLR;
	DATAOUT(data);
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;

#endif

}
//******************************************************************
//函数名：  LCD_DrawPoint_16Bit
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    8位总线下如何写入一个16位数据
//输入参数：(x,y):光标坐标
//返回值：  无
//修改记录：无
//******************************************************************
void LCD_DrawPoint_16Bit(u16 color)
{
#if LCD_USE8BIT_MODEL==1
	LCD_CS_CLR;
	LCD_RD_SET;
	LCD_RS_SET;//写地址
	DATAOUT(color);
	LCD_WR_CLR;
	LCD_WR_SET;
	DATAOUT(color<<8);
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;
#else
	LCD_WR_DATA(color);
#endif


}

//******************************************************************
//函数名：  LCD_WriteReg
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    写寄存器数据
//输入参数：LCD_Reg:寄存器地址
//			LCD_RegValue:要写入的数据
//返回值：  无
//修改记录：无
//******************************************************************
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{
	LCD_WR_REG(LCD_Reg);
	LCD_WR_DATA(LCD_RegValue);
}

//******************************************************************
//函数名：  LCD_WriteRAM_Prepare
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    开始写GRAM
//			在给液晶屏传送RGB数据前，应该发送写GRAM指令
//输入参数：无
//返回值：  无
//修改记录：无
//******************************************************************
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}

//******************************************************************
//函数名：  LCD_DrawLine
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    GUI画线
//输入参数：x1,y1:起点坐标
//        	x2,y2:终点坐标
//返回值：  无
//修改记录：无
//******************************************************************
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
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
		LCD_DrawPoint(uRow,uCol);//画点
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

//******************************************************************
//函数名：  LCD_DrawPoint
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    在指定位置写入一个像素点数据
//输入参数：(x,y):光标坐标
//返回值：  无
//修改记录：无
//******************************************************************
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);//设置光标位置
#if LCD_USE8BIT_MODEL==1
	LCD_CS_CLR;
	LCD_RD_SET;
	LCD_RS_SET;//写地址
	DATAOUT(POINT_COLOR);
	LCD_WR_CLR;
	LCD_WR_SET;
	DATAOUT(POINT_COLOR<<8);
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;
#else
	LCD_WR_DATA(POINT_COLOR);
#endif


}

//******************************************************************
//函数名：  LCD_Clear
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    LCD全屏填充清屏函数
//输入参数：Color:要清屏的填充色
//返回值：  无
//修改记录：无
//******************************************************************
void LCD_Clear(u16 Color)
{
	u32 index=0;
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);
	/*
#if LCD_USE8BIT_MODEL==1
	LCD_RS_SET;//写数据
	LCD_CS_CLR;
	for(index=0;index<lcddev.width*lcddev.height;index++)
	{
		DATAOUT(Color);
		LCD_WR_CLR;
		LCD_WR_SET;

		DATAOUT(Color<<8);
		LCD_WR_CLR;
		LCD_WR_SET;
	}
	LCD_CS_SET;
#else //16位模式
	for(index=0;index<lcddev.width*lcddev.height;index++)
	{
		LCD_WR_DATA(Color);
	}
#endif
	*/
		LCD_RS_SET;//写数据
	LCD_CS_CLR;
	for(index=0;index<lcddev.width*lcddev.height;index++)
	{
		DATAOUT(Color);
		LCD_WR_CLR;
		LCD_WR_SET;

		DATAOUT(Color<<8);
		LCD_WR_CLR;
		LCD_WR_SET;
	}
	LCD_CS_SET;

}

//******************************************************************
//函数名：  LCD_GPIOInit
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    液晶屏IO初始化，液晶初始化前要调用此函数
//输入参数：无
//返回值：  无
//修改记录：无
//******************************************************************
void LCD_GPIOInit(void)
{
	GPIO_InitType GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC|RCC_APB2PERIPH_GPIOE|RCC_APB2PERIPH_AFIO, ENABLE);
	GPIO_PinsRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);

	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10|GPIO_Pins_0|GPIO_Pins_1|GPIO_Pins_2|GPIO_Pins_3|GPIO_Pins_5;	   //GPIO_Pin_10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;  //推挽输出
	GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_10MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure); //GPIOC
	GPIO_SetBits(GPIOC,GPIO_Pins_10|GPIO_Pins_0|GPIO_Pins_1|GPIO_Pins_2|GPIO_Pins_3|GPIO_Pins_5);


	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_All;	//
	GPIO_Init(GPIOE, &GPIO_InitStructure); //GPIOB
	GPIO_SetBits(GPIOE,GPIO_Pins_All);
}

//******************************************************************
//函数名：  LCD_Reset
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    LCD复位函数，液晶初始化前要调用此函数
//输入参数：无
//返回值：  无
//修改记录：无
//******************************************************************
void LCD_RESET(void)
{
	LCD_RST_CLR;
	rt_thread_mdelay(100);
	LCD_RST_SET;
	rt_thread_mdelay(50);
}
u16 LCD_RD_DATA(void)
{
	u16 t;
 	GPIOE->CTRLL=0X88888888; //PB0-7  上拉输入
	GPIOE->CTRLH=0X88888888; //PB8-15 上拉输入
	GPIOE->OPTDT=0X0000;     //全部输出0

	LCD_RS_SET;
	LCD_CS_CLR;
	//读取数据(读寄存器时,并不需要读2次)
	LCD_RD_CLR;
	rt_hw_us_delay(2);//FOR 8989,延时2us
	t=DATAIN;
	LCD_RD_SET;
	LCD_CS_SET;

	GPIOE->CTRLL=0X33333333; //PB0-7  上拉输出
	GPIOE->CTRLH=0X33333333; //PB8-15 上拉输出
	GPIOE->OPTDT=0XFFFF;    //全部输出高
	return t;
}

//******************************************************************
//函数名：  LCD_Init
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    LCD初始化
//输入参数：无
//返回值：  无
//修改记录：无
//******************************************************************
void LCD_Init(void)
{

	LCD_GPIOInit();
 	LCD_RESET();

    LCD_WR_REG(0XD3);
    LCD_RD_DATA();              //dummy read
    LCD_RD_DATA();              //读到0X00
    lcddev.id=LCD_RD_DATA();    //读取93
    lcddev.id<<=8;
    lcddev.id|=LCD_RD_DATA();   //读取41

	//************* Start Initial Sequence **********//
	LCD_WR_REG(0xCF);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0xC1);
	LCD_WR_DATA(0X30);
	LCD_WR_REG(0xED);
	LCD_WR_DATA(0x64);
	LCD_WR_DATA(0x03);
	LCD_WR_DATA(0X12);
	LCD_WR_DATA(0X81);
	LCD_WR_REG(0xE8);
	LCD_WR_DATA(0x85);
	LCD_WR_DATA(0x10);
	LCD_WR_DATA(0x7A);
	LCD_WR_REG(0xCB);
	LCD_WR_DATA(0x39);
	LCD_WR_DATA(0x2C);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x34);
	LCD_WR_DATA(0x02);
	LCD_WR_REG(0xF7);
	LCD_WR_DATA(0x20);
	LCD_WR_REG(0xEA);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0xC0);    //Power control
	LCD_WR_DATA(0x1B);   //VRH[5:0]
	LCD_WR_REG(0xC1);    //Power control
	LCD_WR_DATA(0x01);   //SAP[2:0];BT[3:0]
	LCD_WR_REG(0xC5);    //VCM control
	LCD_WR_DATA(0x30); 	 //3F
	LCD_WR_DATA(0x30); 	 //3C
	LCD_WR_REG(0xC7);    //VCM control2
	LCD_WR_DATA(0XB7);
	LCD_WR_REG(0x36);    // Memory Access Control
	LCD_WR_DATA(0x48);
	LCD_WR_REG(0x3A);
	LCD_WR_DATA(0x55);
	LCD_WR_REG(0xB1);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x1A);
	LCD_WR_REG(0xB6);    // Display Function Control
	LCD_WR_DATA(0x0A);
	LCD_WR_DATA(0xA2);
	LCD_WR_REG(0xF2);    // 3Gamma Function Disable
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0x26);    //Gamma curve selected
	LCD_WR_DATA(0x01);
	LCD_WR_REG(0xE0);    //Set Gamma
	LCD_WR_DATA(0x0F);
	LCD_WR_DATA(0x2A);
	LCD_WR_DATA(0x28);
	LCD_WR_DATA(0x08);
	LCD_WR_DATA(0x0E);
	LCD_WR_DATA(0x08);
	LCD_WR_DATA(0x54);
	LCD_WR_DATA(0XA9);
	LCD_WR_DATA(0x43);
	LCD_WR_DATA(0x0A);
	LCD_WR_DATA(0x0F);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0XE1);    //Set Gamma
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x15);
	LCD_WR_DATA(0x17);
	LCD_WR_DATA(0x07);
	LCD_WR_DATA(0x11);
	LCD_WR_DATA(0x06);
	LCD_WR_DATA(0x2B);
	LCD_WR_DATA(0x56);
	LCD_WR_DATA(0x3C);
	LCD_WR_DATA(0x05);
	LCD_WR_DATA(0x10);
	LCD_WR_DATA(0x0F);
	LCD_WR_DATA(0x3F);
	LCD_WR_DATA(0x3F);
	LCD_WR_DATA(0x0F);
	LCD_WR_REG(0x2B);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x3f);
	LCD_WR_REG(0x2A);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0xef);
	LCD_WR_REG(0x11); //Exit Sleep
	rt_thread_mdelay(120);
	LCD_WR_REG(0x29); //display on


	LCD_SetParam();//设置LCD参数
	LCD_LED=1;//点亮背光
	//LCD_Clear(WHITE);
}

/*************************************************
函数名：LCD_SetWindows
功能：设置lcd显示窗口，在此区域写点数据自动换行
入口参数：xy起点和终点
返回值：无
*************************************************/
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
{
	LCD_WR_REG(lcddev.setxcmd);
	LCD_WR_DATA(xStar>>8);
	LCD_WR_DATA(0x00FF&xStar);
	LCD_WR_DATA(xEnd>>8);
	LCD_WR_DATA(0x00FF&xEnd);

	LCD_WR_REG(lcddev.setycmd);
	LCD_WR_DATA(yStar>>8);
	LCD_WR_DATA(0x00FF&yStar);
	LCD_WR_DATA(yEnd>>8);
	LCD_WR_DATA(0x00FF&yEnd);

	LCD_WriteRAM_Prepare();	//开始写入GRAM
}

/*************************************************
函数名：LCD_SetCursor
功能：设置光标位置
入口参数：xy坐标
返回值：无
*************************************************/
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
	LCD_WR_REG(lcddev.setxcmd);
	LCD_WR_DATA(Xpos>>8);
	LCD_WR_DATA(0x00FF&Xpos);


	LCD_WR_REG(lcddev.setycmd);
	LCD_WR_DATA(Ypos>>8);
	LCD_WR_DATA(0x00FF&Ypos);

	LCD_WriteRAM_Prepare();	//开始写入GRAM
}

//设置LCD参数
//方便进行横竖屏模式切换
void LCD_SetParam(void)
{
	lcddev.wramcmd=0x2C;
#if USE_HORIZONTAL==1	//使用横屏
	lcddev.dir=1;//横屏
	lcddev.width=320;
	lcddev.height=240;
	lcddev.setxcmd=0x2A;
	lcddev.setycmd=0x2B;
	LCD_WriteReg(0x36,0x6C);

#else//竖屏
	lcddev.dir=0;//竖屏
	lcddev.width=240;
	lcddev.height=320;
	lcddev.setxcmd=0x2A;
	lcddev.setycmd=0x2B;
	LCD_WriteReg(0x36,0xC9);
#endif
}



void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{

	LCD_WR_REG(0x2A);
	LCD_WR_DATA(sx>>8);
	LCD_WR_DATA(0x00FF&sx);
	LCD_WR_DATA((sx+width)>>8);
	LCD_WR_DATA(0x00FF&(sx+width));

	LCD_WR_REG(0x2B);
	LCD_WR_DATA(sy>>8);
	LCD_WR_DATA(0x00FF&sy);
	LCD_WR_DATA((sy+height)>>8);
	LCD_WR_DATA(0x00FF&(sy+height));

	LCD_WR_REG(0x2C);	//开始写入GRAM
}

void LCD_Drawbmp16(u16 x,u16 y,u16 width,u16 length,const unsigned char *p)
{
   int i;
   unsigned char picH,picL;
   LCD_SetWindows(x,y,x+width-1,y+length-1);//窗口设置
     for(i=0;i<width*length;i++)
   {
     picL=*(p+i*2);  //数据低位在前
     picH=*(p+i*2+1);
     LCD_DrawPoint_16Bit(picH<<8|picL);
   }
   LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复显示窗口为全屏

 }
void LCD_DrawBinImg(u16 x,u16 y,u16 width,u16 length,u8 threshold,const unsigned char *p)
{
  int i;
  LCD_SetWindows(x,y,x+width-1,y+length-1);//窗口设置
  for(i=0;i<width*length;i++)
  {
//    LCD_DrawPoint_16Bit(YUV_to_Bin(*(p+i),threshold));ColorCvt
    LCD_DrawPoint_16Bit(ColorCvt(*(p+i)));
  }
}

void LCD_DrawBinImg_8bit(u16 x,u16 y,u16 width,u16 length,const unsigned char *p)
{
  int i;
  u16 temp;
  LCD_SetWindows(x,y,x+width-1,y+length-1);//窗口设置
  for(i=0;i<width*length;i++)
  {
    temp = 0 ;
    temp=*(p+i);
    LCD_DrawPoint_16Bit((temp<<8 | temp));
  }
}

void LCD_DrawYVU8Img(u16 x,u16 y,u16 width,u16 length,const unsigned char *p)
{
   int i;
//    unsigned char picH,picL;
   LCD_SetWindows(x,y,x+width-1,y+length-1);//窗口设置
   for(i=0;i<width*length;i++)
   {
//      picL=*(p+i*2);  //数据低位在前
//      picH=*(p+i*2+1);
     LCD_DrawPoint_16Bit(yuv_8_to_RGBGray(*(p+i)));
   }
//    LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复显示窗口为全屏

 }


 //******************************************************************
 //函数名：  gui_circle
 //作者：    xiao冯@全动电子
 //日期：    2013-02-22
 //功能：    在指定位置画一个指定大小的圆(填充)
 //输入参数：(xc,yc) :圆中心坐标
 //          c:填充的颜色
 //      r:圆半径
 //      fill:填充判断标志，1-填充，0-不填充
 //返回值：  无
 //修改记录：无
 //******************************************************************
 void gui_circle(int xc, int yc,u16 c,int r, int fill)
 {
   int x = 0, y = r, yi, d;

   d = 3 - 2 * r;


   if (fill)
   {
     // 如果填充（画实心圆）
     while (x <= y) {
       for (yi = x; yi <= y; yi++)
         _draw_circle_8(xc, yc, x, yi, c);

       if (d < 0) {
         d = d + 4 * x + 6;
       } else {
         d = d + 4 * (x - y) + 10;
         y--;
       }
       x++;
     }
   } else
   {
     // 如果不填充（画空心圆）
     while (x <= y) {
       _draw_circle_8(xc, yc, x, y, c);
       if (d < 0) {
         d = d + 4 * x + 6;
       } else {
         d = d + 4 * (x - y) + 10;
         y--;
       }
       x++;
     }
   }
 }

 //******************************************************************
 //函数名：  LCD_ShowString
 //作者：    xiao冯@全动电子
 //日期：    2013-02-22
 //功能：    显示英文字符串
 //输入参数：x,y :起点坐标
 //      size:字体大小
 //      *p:字符串起始地址
 //      mode:模式 0,填充模式;1,叠加模式
 //返回值：  无
 //修改记录：无
 //******************************************************************
 void LCD_ShowString(u16 x,u16 y,u8 size,u8 *p,u8 mode)
 {
     while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
     {
     if(x>(lcddev.width-1)||y>(lcddev.height-1))
     return;
         LCD_ShowChar(x,y,POINT_COLOR,BACK_COLOR,*p,size,mode);
         x+=size/2;
         p++;
     }
 }

 //******************************************************************
 //函数名：  LCD_ShowNum
 //作者：    xiao冯@全动电子
 //日期：    2013-02-22
 //功能：    显示单个数字变量值
 //输入参数：x,y :起点坐标
 //      len :指定显示数字的位数
 //      size:字体大小(12,16)
 //      color:颜色
 //      num:数值(0~4294967295)
 //返回值：  无
 //修改记录：无
 //******************************************************************
 void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
 {
   u8 t,temp;
   u8 enshow=0;
   for(t=0;t<len;t++)
   {
     temp=(num/mypow(10,len-t-1))%10;
     if(enshow==0&&t<(len-1))
     {
       if(temp==0)
       {
         LCD_ShowChar(x+(size/2)*t,y,POINT_COLOR,BACK_COLOR,' ',size,0);
         continue;
       }else enshow=1;

     }
     LCD_ShowChar(x+(size/2)*t,y,POINT_COLOR,BACK_COLOR,temp+'0',size,0);
   }
 }

 //******************************************************************
 //函数名：  LCD_ShowChar
 //作者：    xiao冯@全动电子
 //日期：    2013-02-22
 //功能：    显示单个英文字符
 //输入参数：(x,y):字符显示位置起始坐标
 //          fc:前置画笔颜色
 //      bc:背景颜色
 //      num:数值（0-94）
 //      size:字体大小
 //      mode:模式  0,填充模式;1,叠加模式
 //返回值：  无
 //修改记录：无
 //******************************************************************
 void LCD_ShowChar(u16 x,u16 y,u16 fc, u16 bc, u8 num,u8 size,u8 mode)
 {
     u8 temp;
     u8 pos,t;
   u16 colortemp=POINT_COLOR;

   num=num-' ';//得到偏移后的值
   LCD_SetWindows(x,y,x+size/2-1,y+size-1);//设置单个文字显示窗口
   if(!mode) //非叠加方式
   {

     for(pos=0;pos<size;pos++)
     {
       if(size==12)temp=asc2_1206[num][pos];//调用1206字体
       else temp=asc2_1608[num][pos];     //调用1608字体
       for(t=0;t<size/2;t++)
         {
             if(temp&0x01)LCD_DrawPoint_16Bit(fc);
         else LCD_DrawPoint_16Bit(bc);
         temp>>=1;

         }

     }
   }else//叠加方式
   {
     for(pos=0;pos<size;pos++)
     {
       if(size==12)temp=asc2_1206[num][pos];//调用1206字体
       else temp=asc2_1608[num][pos];     //调用1608字体
       for(t=0;t<size/2;t++)
         {
         POINT_COLOR=fc;
             if(temp&0x01)LCD_DrawPoint(x+t,y+pos);//画一个点
             temp>>=1;
         }
     }
   }
   POINT_COLOR=colortemp;
   LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复窗口为全屏
 }

 //******************************************************************
 //函数名：  mypow
 //作者：    xiao冯@全动电子
 //日期：    2013-02-22
 //功能：    求m的n次方(gui内部调用)
 //输入参数：m:乘数
 //          n:幂
 //返回值：  m的n次方
 //修改记录：无
 //******************************************************************
 u32 mypow(u8 m,u8 n)
 {
   u32 result=1;
   while(n--)result*=m;
   return result;
 }

 //******************************************************************
 //函数名：  _draw_circle_8
 //作者：    xiao冯@全动电子
 //日期：    2013-02-22
 //功能：    8对称性画圆算法(内部调用)
 //输入参数：(xc,yc) :圆中心坐标
 //      (x,y):光标相对于圆心的坐标
 //          c:填充的颜色
 //返回值：  无
 //修改记录：无
 //******************************************************************
 void _draw_circle_8(int xc, int yc, int x, int y, u16 c)
 {
   GUI_DrawPoint(xc + x, yc + y, c);

   GUI_DrawPoint(xc - x, yc + y, c);

   GUI_DrawPoint(xc + x, yc - y, c);

   GUI_DrawPoint(xc - x, yc - y, c);

   GUI_DrawPoint(xc + y, yc + x, c);

   GUI_DrawPoint(xc - y, yc + x, c);

   GUI_DrawPoint(xc + y, yc - x, c);

   GUI_DrawPoint(xc - y, yc - x, c);
 }
 //////////////////////////////////////////////////////////////////////////////////
 //本程序只供学习使用，未经作者许可，不得用于其它任何用途
 //测试硬件：单片机STM32F103RBT6,主频72M  单片机工作电压3.3V
 //QDtech-TFT液晶驱动 for STM32 IO模拟
 //xiao冯@ShenZhen QDtech co.,LTD
 //公司网站:www.qdtech.net
 //淘宝网站：http://qdtech.taobao.com
 //我司提供技术支持，任何技术问题欢迎随时交流学习
 //固话(传真) :+86 0755-23594567
 //手机:15989313508（冯工）
 //邮箱:QDtech2008@gmail.com
 //Skype:QDtech2008
 //技术交流QQ群:324828016
 //创建日期:2013/5/13
 //版本：V1.1
 //版权所有，盗版必究。
 //Copyright(C) 深圳市全动电子技术有限公司 2009-2019
 //All rights reserved
 //////////////////////////////////////////////////////////////////////////////////
 //******************************************************************
 //函数名：  GUI_DrawPoint
 //作者：    xiao冯@全动电子
 //日期：    2013-02-22
 //功能：    GUI描绘一个点
 //输入参数：x:光标位置x坐标
 //          y:光标位置y坐标
 //      color:要填充的颜色
 //返回值：  无
 //修改记录：无
 //******************************************************************
 void GUI_DrawPoint(u16 x,u16 y,u16 color)
 {
   LCD_SetCursor(x,y);//设置光标位置
   LCD_DrawPoint_16Bit(color);
 }

