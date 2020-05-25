
/////////////////////////////////////用户配置区///////////////////////////////////
//支持横竖屏快速定义切换，支持8/16位模式切换
#define USE_HORIZONTAL  	1	//定义是否使用横屏 		0,不使用.1,使用.
#define LCD_USE8BIT_MODEL   0    //定义数据总线是否使用8位模式 0,使用16位模式.1,使用8位模式
//////////////////////////////////////////////////////////////////////////////////
//定义LCD的尺寸
#if USE_HORIZONTAL==1	//使用横屏
#define LCD_W 320
#define LCD_H 240
#else
#define LCD_W 240
#define LCD_H 320
#endif

#define LCD_BL      GET_PIN(C,  5)
#define LCD_BL_ON   rt_pin_write(LCD_BL, PIN_HIGH);
#define LCD_BL_OFF  rt_pin_write(LCD_BL, PIN_LOW);

//TFTLCD部分外要调用的函数
extern u16  POINT_COLOR;//默认红色
extern u16  BACK_COLOR; //背景颜色.默认为白色

#define	LCD_CS_SET  GPIOA->BSRE=1<<2    //片选端口  	PA2
#define	LCD_RS_SET	GPIOC->BSRE=1<<0    //数据/命令 PC0
#define	LCD_WR_SET	GPIOC->BSRE=1<<1    //写数据		PC1
#define	LCD_RD_SET	GPIOC->BSRE=1<<2    //读数据		PC2
#define	LCD_RST_SET	GPIOC->BSRE=1<<3    //复位	    PC3

#define	LCD_CS_CLR  GPIOA->BRE=1<<2     //片选端口  	PA2
#define	LCD_RS_CLR	GPIOC->BRE=1<<0     //数据/命令 PC0
#define	LCD_WR_CLR	GPIOC->BRE=1<<1     //写数据		PC1
#define	LCD_RD_CLR	GPIOC->BRE=1<<2     //读数据		PC2
#define	LCD_RST_CLR	GPIOC->BRE=1<<3     //复位		PC3

#define DATAOUT(x) GPIOE->OPTDT=x; //数据输出
#define DATAIN     GPIOE->IPTDT;   //数据输入

#define	LCD_LED_ON   //LCD背光    		 PC10


typedef struct
{
	u16 width;			//LCD 宽度
	u16 height;			//LCD 高度
	u16 id;				//LCD ID
	u8  dir;			//横屏还是竖屏控制：0，竖屏；1，横屏。
	u16	wramcmd;		//开始写gram指令
	u16 setxcmd;		//设置x坐标指令
	u16  setycmd;		//设置y坐标指令
}_lcd_dev;

//LCD参数
extern _lcd_dev lcddev;	//管理LCD重要参数


//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000
#define BLUE         	 0x001F
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色

void LCD_Init(void);
void LCD_RESET(void);
//void LCD_DisplayOn(void);
//void LCD_DisplayOff(void);
void LCD_Clear(u16 Color);
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_DrawPoint(u16 x,u16 y);//画点
//u16  LCD_ReadPoint(u16 x,u16 y); //读点
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd);
void LCD_DrawPoint_16Bit(u16 color);
u16 LCD_RD_DATA(void);//读取LCD数据
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue);
void LCD_WR_DATA(u16 data);
u16 LCD_ReadReg(u16 LCD_Reg);
void LCD_WriteRAM_Prepare(void);
//void LCD_WriteRAM(u16 RGB_Code);
//u16 LCD_ReadRAM(void);
//u16 LCD_BGR2RGB(u16 c);
void LCD_SetParam(void);

//void GUI_DrawPoint(u16 x,u16 y,u16 color);
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);
//void Draw_Circle(u16 x0,u16 y0,u16 fc,u8 r);
void LCD_ShowChar(u16 x,u16 y,u16 fc, u16 bc, u8 num,u8 size,u8 mode);
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);
//void LCD_Show2Num(u16 x,u16 y,u16 num,u8 len,u8 size,u8 mode);
void LCD_ShowString(u16 x,u16 y,u8 size,u8 *p,u8 mode);
void GUI_DrawFont16(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode);
void GUI_DrawFont24(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode);
void GUI_DrawFont32(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode);
void Show_Str(u16 x, u16 y, u16 fc, u16 bc, u8 *str,u8 size,u8 mode);
void Gui_Drawbmp16(u16 x,u16 y,const unsigned char *p); //显示40*40 QQ图片
void gui_circle(int xc, int yc,u16 c,int r, int fill);
void Gui_StrCenter(u16 x, u16 y, u16 fc, u16 bc, u8 *str,u8 size,u8 mode);
void LCD_DrawFillRectangle(u16 x1, u16 y1, u16 x2, u16 y2);

void GUI_DrawPoint(u16 x,u16 y,u16 color);
u32 mypow(u8 m,u8 n);
void UART_Printf(char str[]);
void rt_hw_us_delay(rt_uint32_t us);

