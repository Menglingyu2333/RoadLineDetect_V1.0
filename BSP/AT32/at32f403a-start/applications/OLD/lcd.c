
#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"
#include "drv_gpio.h"
#include "lcd.h"
#include "FONT.H"


_lcd_dev lcddev;
char PrintStr [100];


u16  POINT_COLOR;//Ĭ�Ϻ�ɫ
u16  BACK_COLOR; //������ɫ.Ĭ��Ϊ��ɫ


void rt_hw_us_delay(rt_uint32_t us)
{
    rt_uint32_t delta;/* �� �� �� ʱ �� �� �� tick �� */
    us = us * (SysTick->LOAD/(1000000/RT_TICK_PER_SECOND));
    /* �� �� �� ǰ ʱ �� */
    delta = SysTick->VAL; /* ѭ �� �� �� �� ǰ ʱ �䣬 ֱ �� �� �� ָ �� �� ʱ �� �� �� �� ѭ �� */
    while (delta - SysTick->VAL< us);
}

//���ܣ�    ��Һ��������д��д16λָ��
//���������Reg:��д���ָ��ֵ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_WR_REG(u8 data)
{
#if LCD_USE8BIT_MODEL==1//ʹ��8λ������������ģʽ
	LCD_RS_CLR;
	LCD_CS_CLR;
	DATAOUT(data<<8);
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;

#else//ʹ��16λ������������ģʽ
	LCD_RS_CLR;
	LCD_CS_CLR;
	DATAOUT(data);
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;

#endif
}

void LCD_WR_DATA(u16 data)
{


#if LCD_USE8BIT_MODEL==1//ʹ��8λ������������ģʽ
	LCD_RS_SET;
	LCD_CS_CLR;
	DATAOUT(data<<8);
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;

#else//ʹ��16λ������������ģʽ
	LCD_RS_SET;
	LCD_CS_CLR;
	DATAOUT(data);
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;

#endif

}

//д�Ĵ���
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{
	LCD_WR_REG(LCD_Reg);
	LCD_WR_DATA(LCD_RegValue);
}

//���Ĵ���
u16 LCD_ReadReg(u16 LCD_Reg)
{
 	LCD_WR_REG(LCD_Reg);  //д��Ҫ���ļĴ�����
	return LCD_RD_DATA();
}

u16 LCD_RD_DATA(void)
{
	u16 t;
 	GPIOE->CTRLL=0X88888888; //PB0-7  ��������
	GPIOE->CTRLH=0X88888888; //PB8-15 ��������
	GPIOE->OPTDT=0X0000;     //ȫ�����0

	LCD_RS_SET;
	LCD_CS_CLR;
	//��ȡ����(���Ĵ���ʱ,������Ҫ��2��)
	LCD_RD_CLR;
	if(lcddev.id==0X8989) rt_hw_us_delay(2);//FOR 8989,��ʱ2us
	t=DATAIN;
//  printf("DATAIN=%04X\x00",t);
	LCD_RD_SET;
	LCD_CS_SET;

	GPIOE->CTRLL=0X33333333; //PB0-7  �������
	GPIOE->CTRLH=0X33333333; //PB8-15 �������
	GPIOE->OPTDT=0XFFFF;    //ȫ�������
	return t;
}


void LCD_WR_DATAX(u16 data)
{
	LCD_RS_SET;
	LCD_CS_CLR;
	DATAOUT(data);
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;
}

//����LCD����
//������к�����ģʽ�л�
void LCD_SetParam(void)
{
	lcddev.wramcmd=0x2C;
#if USE_HORIZONTAL==1	//ʹ�ú���
	lcddev.dir=1;//����
	lcddev.width=320;
	lcddev.height=240;
	lcddev.setxcmd=0x2A;
	lcddev.setycmd=0x2B;
	LCD_WriteReg(0x36,0x6C);

#else//����
	lcddev.dir=0;//����
	lcddev.width=240;
	lcddev.height=320;
	lcddev.setxcmd=0x2A;
	lcddev.setycmd=0x2B;
	LCD_WriteReg(0x36,0xC9);
#endif
}

//��ʼ��lcd
//�ó�ʼ���������Գ�ʼ������ILI93XXҺ��,�������������ǻ���ILI9320��!!!
//�������ͺŵ�����оƬ��û�в���!

void LCD_Init(void)
{
    GPIO_InitType GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC|RCC_APB2PERIPH_GPIOE|RCC_APB2PERIPH_AFIO, ENABLE); //ʹ��PORTB,Cʱ�Ӻ�AFIOʱ��
    GPIO_PinsRemapConfig (GPIO_Remap_SWJ_JTAGDisable , ENABLE);//����SWD��ʧ��JTAG

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_InitStructure.GPIO_Pins = (GPIO_Pins_0|GPIO_Pins_1|GPIO_Pins_2|GPIO_Pins_3|
      GPIO_Pins_4|GPIO_Pins_5|GPIO_Pins_10|GPIO_Pins_11|GPIO_Pins_12);     ///PORTC6~10�����������
    GPIO_Init(GPIOC, &GPIO_InitStructure); //GPIOC

    GPIO_SetBits(GPIOC,GPIO_Pins_0|GPIO_Pins_1|GPIO_Pins_2|GPIO_Pins_3|
      GPIO_Pins_4|GPIO_Pins_5|GPIO_Pins_10|GPIO_Pins_11|GPIO_Pins_12);

    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_All; //  PORTE�������
    GPIO_Init(GPIOE, &GPIO_InitStructure); //GPIOE

    GPIO_SetBits(GPIOE,GPIO_Pins_All);

    LCD_RESET();

    rt_thread_mdelay(50);

//    LCD_WriteReg(0x0000,0x0001);
//    rt_thread_mdelay(50);
//    lcddev.id = LCD_ReadReg(0x0000);
        //����9341 ID�Ķ�ȡ
    LCD_WR_REG(0XD3);
    LCD_RD_DATA();              //dummy read
    LCD_RD_DATA();              //����0X00
    lcddev.id=LCD_RD_DATA();    //��ȡ93
    lcddev.id<<=8;
    lcddev.id|=LCD_RD_DATA();   //��ȡ41

//    printf(" LCD ID:%04X  \r\n\x00", lcddev.id);

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


	LCD_SetParam();//����LCD����	 
    LCD_BL_ON;                  //��������
//    LCD_Clear(BLUE);
}


//��������
//color:Ҫ���������ɫ
void LCD_Clear(u16 color)
{
	u32 index=0;
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 			//�õ��ܵ���
	if((lcddev.id==0X6804)&&(lcddev.dir==1))//6804������ʱ�����⴦��
	{
 		lcddev.dir=0;
 		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;
		LCD_SetCursor(0x00,0x0000);		//���ù��λ��
 		lcddev.dir=1;
  		lcddev.setxcmd=0X2B;
		lcddev.setycmd=0X2A;
 	}else LCD_SetCursor(0x00,0x0000);	//���ù��λ��
	LCD_WriteRAM_Prepare();     		//��ʼд��GRAM
	for(index=0;index<totalpoint;index++)LCD_WR_DATA(color);
}
/*************************************************
��������LCD_SetCursor
���ܣ����ù��λ��
��ڲ�����xy����
����ֵ����
*************************************************/
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
	LCD_WR_REG(lcddev.setxcmd);
	LCD_WR_DATA(Xpos>>8);
	LCD_WR_DATA(0x00FF&Xpos);


	LCD_WR_REG(lcddev.setycmd);
	LCD_WR_DATA(Ypos>>8);
	LCD_WR_DATA(0x00FF&Ypos);

	LCD_WriteRAM_Prepare();	//��ʼд��GRAM
}

//******************************************************************
//��������  LCD_WriteRAM_Prepare
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    ��ʼдGRAM
//			�ڸ�Һ��������RGB����ǰ��Ӧ�÷���дGRAMָ��
//�����������
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}
void LCD_RESET(void)
{
	LCD_RST_CLR;
	rt_thread_mdelay(100);
	LCD_RST_SET;
	rt_thread_mdelay(120);
}
//LCD������ʾ
//void LCD_DisplayOn(void)
//{
//	LCD_WriteReg(R7, 0x0173); //26��ɫ��ʾ����
//}
////LCD�ر���ʾ
//void LCD_DisplayOff(void)
//{
//	LCD_WriteReg(R7, 0x0);//�ر���ʾ
//}

/*************************************************
��������LCD_SetWindows
���ܣ�����lcd��ʾ���ڣ��ڴ�����д�������Զ�����
��ڲ�����xy�����յ�
����ֵ����
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

	LCD_WriteRAM_Prepare();	//��ʼд��GRAM
}

//******************************************************************
//��������  LCD_DrawPoint_16Bit
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    8λ���������д��һ��16λ����
//���������(x,y):�������
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_DrawPoint_16Bit(u16 color)
{
#if LCD_USE8BIT_MODEL==1
	LCD_CS_CLR;
	LCD_RD_SET;
	LCD_RS_SET;//д��ַ
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
//���ܣ�    ��ָ�������������ɫ
//���������sx:ָ������ʼ��x����
//        	sy:ָ������ʼ��y����
//			ex:ָ�����������x����
//			ey:ָ�����������y����
//        	color:Ҫ������ɫ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{

	u16 i,j;
	u16 width=ex-sx+1; 		//�õ����Ŀ��
	u16 height=ey-sy+1;		//�߶�
	LCD_SetWindows(sx,sy,ex-1,ey-1);//������ʾ����

#if LCD_USE8BIT_MODEL==1
	LCD_RS_SET;//д����
	LCD_CS_CLR;
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		{
			DATAOUT(color);
			LCD_WR_CLR;
			LCD_WR_SET;

			DATAOUT(color<<8);
			LCD_WR_CLR;
			LCD_WR_SET;
		}
	}
	LCD_CS_SET;
#else //16λģʽ
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		LCD_WR_DATA(color);	//д������
	}
#endif
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//�ָ���������Ϊȫ��
}

//���ܣ�    ��ʾһ���ַ���,������Ӣ����ʾ
//���������x,y :�������
// 			fc:ǰ�û�����ɫ
//			bc:������ɫ
//			str :�ַ���
//			size:�����С
//			mode:ģʽ	0,���ģʽ;1,����ģʽ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void Show_Str(u16 x, u16 y, u16 fc, u16 bc, u8 *str,u8 size,u8 mode)
{
	u16 x0=x;
  	u8 bHz=0;     //�ַ���������
    while(*str!=0)//����δ����
    {
        if(!bHz)
        {
			if(x>(lcddev.width-size/2)||y>(lcddev.height-size))
			return;
	        if(*str>0x80)bHz=1;//����
	        else              //�ַ�
	        {
		        if(*str==0x0D)//���з���
		        {
		            y+=size;
					x=x0;
		            str++;
		        }
		        else
				{
					if(size>16)//�ֿ���û�м���12X24 16X32��Ӣ������,��8X16����
					{
					LCD_ShowChar(x,y,fc,bc,*str,16,mode);
					x+=8; //�ַ�,Ϊȫ�ֵ�һ��
					}
					else
					{
					LCD_ShowChar(x,y,fc,bc,*str,size,mode);
					x+=size/2; //�ַ�,Ϊȫ�ֵ�һ��
					}
				}
				str++;

	        }
        }else//����
        {
			if(x>(lcddev.width-size)||y>(lcddev.height-size))
			return;
            bHz=0;//�к��ֿ�
			if(size==32)
			GUI_DrawFont32(x,y,fc,bc,str,mode);
			else if(size==24)
			GUI_DrawFont24(x,y,fc,bc,str,mode);
			else
			GUI_DrawFont16(x,y,fc,bc,str,mode);

	        str+=2;
	        x+=size;//��һ������ƫ��
        }
    }
}

//���ܣ�    ��ʾ����Ӣ���ַ�
//���������(x,y):�ַ���ʾλ����ʼ����
//        	fc:ǰ�û�����ɫ
//			bc:������ɫ
//			num:��ֵ��0-94��
//			size:�����С
//			mode:ģʽ  0,���ģʽ;1,����ģʽ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_ShowChar(u16 x,u16 y,u16 fc, u16 bc, u8 num,u8 size,u8 mode)
{
    u8 temp;
    u8 pos,t;
	u16 colortemp=POINT_COLOR;

	num=num-' ';//�õ�ƫ�ƺ��ֵ
	LCD_SetWindows(x,y,x+size/2-1,y+size-1);//���õ���������ʾ����
	if(!mode) //�ǵ��ӷ�ʽ
	{

		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//����1206����
			else temp=asc2_1608[num][pos];		 //����1608����
			for(t=0;t<size/2;t++)
		    {
		        if(temp&0x01)LCD_DrawPoint_16Bit(fc);
				else LCD_DrawPoint_16Bit(bc);
				temp>>=1;

		    }

		}
	}else//���ӷ�ʽ
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//����1206����
			else temp=asc2_1608[num][pos];		 //����1608����
			for(t=0;t<size/2;t++)
		    {
				POINT_COLOR=fc;
		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos);//��һ����
		        temp>>=1;
		    }
		}
	}
	POINT_COLOR=colortemp;
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//�ָ�����Ϊȫ��
}
//******************************************************************
//��������  GUI_DrawFont16
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    ��ʾ����16X16��������
//���������x,y :�������
//			fc:ǰ�û�����ɫ
//			bc:������ɫ
//			s:�ַ�����ַ
//			mode:ģʽ	0,���ģʽ;1,����ģʽ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void GUI_DrawFont16(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode)
{
	u8 i,j;
	u16 k;
	u16 HZnum;
	u16 x0=x;
	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//�Զ�ͳ�ƺ�����Ŀ


	for (k=0;k<HZnum;k++)
	{
	  if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
	  { 	LCD_SetWindows(x,y,x+16-1,y+16-1);
		    for(i=0;i<16*2;i++)
		    {
				for(j=0;j<8;j++)
		    	{
					if(!mode) //�ǵ��ӷ�ʽ
					{
						if(tfont16[k].Msk[i]&(0x80>>j))	LCD_DrawPoint_16Bit(fc);
						else LCD_DrawPoint_16Bit(bc);
					}
					else
					{
						POINT_COLOR=fc;
						if(tfont16[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//��һ����
						x++;
						if((x-x0)==16)
						{
							x=x0;
							y++;
							break;
						}
					}

				}

			}


		}
		continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
	}

	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//�ָ�����Ϊȫ��
}

//******************************************************************
//��������  GUI_DrawFont24
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    ��ʾ����24X24��������
//���������x,y :�������
//			fc:ǰ�û�����ɫ
//			bc:������ɫ
//			s:�ַ�����ַ
//			mode:ģʽ	0,���ģʽ;1,����ģʽ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void GUI_DrawFont24(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode)
{
	u8 i,j;
	u16 k;
	u16 HZnum;
	u16 x0=x;
	HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);	//�Զ�ͳ�ƺ�����Ŀ

			for (k=0;k<HZnum;k++)
			{
			  if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
			  { 	LCD_SetWindows(x,y,x+24-1,y+24-1);
				    for(i=0;i<24*3;i++)
				    {
							for(j=0;j<8;j++)
							{
								if(!mode) //�ǵ��ӷ�ʽ
								{
									if(tfont24[k].Msk[i]&(0x80>>j))	LCD_DrawPoint_16Bit(fc);
									else LCD_DrawPoint_16Bit(bc);
								}
							else
							{
								POINT_COLOR=fc;
								if(tfont24[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//��һ����
								x++;
								if((x-x0)==24)
								{
									x=x0;
									y++;
									break;
								}
							}
						}
					}


				}
				continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
			}

	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//�ָ�����Ϊȫ��
}

//���ܣ�    ��ʾ����32X32��������
//���������x,y :�������
//			fc:ǰ�û�����ɫ
//			bc:������ɫ
//			s:�ַ�����ַ
//			mode:ģʽ	0,���ģʽ;1,����ģʽ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void GUI_DrawFont32(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode)
{
	u8 i,j;
	u16 k;
	u16 HZnum;
	u16 x0=x;
	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	//�Զ�ͳ�ƺ�����Ŀ
	for (k=0;k<HZnum;k++)
			{
			  if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
			  { 	LCD_SetWindows(x,y,x+32-1,y+32-1);
				    for(i=0;i<32*4;i++)
				    {
						for(j=0;j<8;j++)
				    	{
							if(!mode) //�ǵ��ӷ�ʽ
							{
								if(tfont32[k].Msk[i]&(0x80>>j))	LCD_DrawPoint_16Bit(fc);
								else LCD_DrawPoint_16Bit(bc);
							}
							else
							{
								POINT_COLOR=fc;
								if(tfont32[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//��һ����
								x++;
								if((x-x0)==32)
								{
									x=x0;
									y++;
									break;
								}
							}
						}
					}


				}
				continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
			}

	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//�ָ�����Ϊȫ��
}

void Gui_StrCenter(u16 x, u16 y, u16 fc, u16 bc, u8 *str,u8 size,u8 mode)
{
	u16 len=strlen((const char *)str);
	u16 x1=(lcddev.width-len*8)/2;
	Show_Str(x+x1,y,fc,bc,str,size,mode);
}
//���ܣ�    ��ָ��λ��д��һ�����ص�����
//���������(x,y):�������
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);//���ù��λ��
#if LCD_USE8BIT_MODEL==1
	LCD_CS_CLR;
	LCD_RD_SET;
	LCD_RS_SET;//д��ַ
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
//��������  LCD_DrawLine
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    GUI����
//���������x1,y1:�������
//        	x2,y2:�յ�����
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;

	delta_x=x2-x1; //������������
	delta_y=y2-y1;
	uRow=x1;
	uCol=y1;
	if(delta_x>0)incx=1; //���õ�������
	else if(delta_x==0)incx=0;//��ֱ��
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if(delta_y==0)incy=0;//ˮƽ��
	else{incy=-1;delta_y=-delta_y;}
	if( delta_x>delta_y)distance=delta_x; //ѡȡ��������������
	else distance=delta_y;
	for(t=0;t<=distance+1;t++ )//�������
	{
		LCD_DrawPoint(uRow,uCol);//����
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
//��������  LCD_DrawRectangle
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    GUI������(�����)
//���������(x1,y1),(x2,y2):���εĶԽ�����
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}

//******************************************************************
//��������  LCD_DrawFillRectangle
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    GUI������(���)
//���������(x1,y1),(x2,y2):���εĶԽ�����
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_DrawFillRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_Fill(x1,y1,x2,y2,POINT_COLOR);

}

//******************************************************************
//��������  _draw_circle_8
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    8�Գ��Ի�Բ�㷨(�ڲ�����)
//���������(xc,yc) :Բ��������
// 			(x,y):��������Բ�ĵ�����
//         	c:������ɫ
//����ֵ��  ��
//�޸ļ�¼����
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

//******************************************************************
//��������  gui_circle
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    ��ָ��λ�û�һ��ָ����С��Բ(���)
//���������(xc,yc) :Բ��������
//         	c:������ɫ
//		 	r:Բ�뾶
//		 	fill:����жϱ�־��1-��䣬0-�����
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void gui_circle(int xc, int yc,u16 c,int r, int fill)
{
	int x = 0, y = r, yi, d;

	d = 3 - 2 * r;


	if (fill)
	{
		// �����䣨��ʵ��Բ��
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
		// �������䣨������Բ��
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

//���ܣ�    GUI���һ����
//���������x:���λ��x����
//        	y:���λ��y����
//			color:Ҫ������ɫ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void GUI_DrawPoint(u16 x,u16 y,u16 color)
{
	LCD_SetCursor(x,y);//���ù��λ��
	LCD_DrawPoint_16Bit(color);
}

//���ܣ�    ��ʾ�������ֱ���ֵ
//���������x,y :�������
//			len :ָ����ʾ���ֵ�λ��
//			size:�����С(12,16)
//			color:��ɫ
//			num:��ֵ(0~4294967295)
//����ֵ��  ��
//�޸ļ�¼����
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
//���ܣ�    ��m��n�η�(gui�ڲ�����)
//���������m:����
//	        n:��
//����ֵ��  m��n�η�
//�޸ļ�¼����
//******************************************************************
u32 mypow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)result*=m;
	return result;
}
//���ܣ�    ��ʾӢ���ַ���
//���������x,y :�������
//			size:�����С
//			*p:�ַ�����ʼ��ַ
//			mode:ģʽ	0,���ģʽ;1,����ģʽ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_ShowString(u16 x,u16 y,u8 size,u8 *p,u8 mode)
{
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {
		if(x>(lcddev.width-1)||y>(lcddev.height-1))
		return;
        LCD_ShowChar(x,y,POINT_COLOR,BACK_COLOR,*p,size,mode);
        x+=size/2;
        p++;
    }
}

//���ܣ�    ��ʾһ��16λBMPͼ��
//���������x,y :�������
// 			*p :ͼ��������ʼ��ַ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void Gui_Drawbmp16(u16 x,u16 y,const unsigned char *p) //��ʾ40*40 QQͼƬ
{
  	int i;
	unsigned char picH,picL;
	LCD_SetWindows(x,y,x+40-1,y+40-1);//��������
    for(i=0;i<40*40;i++)
	{
	 	picL=*(p+i*2);	//���ݵ�λ��ǰ
		picH=*(p+i*2+1);
		LCD_DrawPoint_16Bit(picH<<8|picL);
	}
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//�ָ���ʾ����Ϊȫ��

}

