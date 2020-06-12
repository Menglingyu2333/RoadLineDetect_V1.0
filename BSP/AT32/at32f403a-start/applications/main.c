

/*
* Copyright (c) 2006-2018, RT-Thread Development Team
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date			 Author 	  Notes
* 2020-01-08	 shelton	  first version
*/
#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"
//#include "drv_gpio.h"
#include "sys.h"
#include "lcd.h"
#include "touch.h"
#include "test.h"

#include "ov2640.h"
#include "LaneDetect.h"

#include "rthw.h"

int 			temp;

#define SAMPLE_UART_NAME		"uart1" /* �� �� �� �� �� �� */
static rt_device_t serial; /* �� �� �� �� �� �� */


static void UART_Init()
{
	serial				= rt_device_find("uart1");
	rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
}

u8 *			ImageBuf;
u8 *			ImageBufProc;
u8				ImagePtr_W, ImagePtr_H;

#define ImageData(H, W) 		*(u8*)(ImageBuf+H*ImageWidth+W)

#define Camera_THREAD_STACK_SIZE 1024
#define Camera_THREAD_PRIORITY	12
#define Camera_THREAD_TIMESLICE 10
static rt_thread_t tid_Camera = RT_NULL;
static void Camera_thread_entry(void * parameter)
{

	LCD_SetWindows(0, 0, ImageWidth - 1, ImageHeight - 1); //LCD��������

  //��ȡͼ�񻺴�
	ImageBuf			= rt_malloc(ImageWidth * ImageHeight);
	if(ImageBuf == NULL) {
		rt_kprintf("get memory FAILED!\n");
		return;
	}
	ImageBufProc  = rt_malloc(ImageWidth * ImageHeight);
	if(ImageBufProc == NULL) {
	  rt_free(ImageBuf);
		rt_kprintf("get memory FAILED!\n");
		return;
	}

  rt_hw_interrupt_disable();//��ʼ����ͼ���ر�����ϵͳ�жϣ�����ᵼ�½���ʱ�����

  while(1)
  {
  	ImagePtr_W			= 0;
  	ImagePtr_H			= 0;
  	LCD_SetCursor(0, 0);
  	while(OV2640_VSYNC == 1);//�������䵽һ���ͼ��
  	while(OV2640_VSYNC == 0);
  	while(OV2640_VSYNC == 1) //��ʼ�ɼ�jpeg����
  	{
  		ImagePtr_W			= 0;

  		while(OV2640_HREF == 0 && OV2640_VSYNC == 1);

  		while(OV2640_HREF)
      {
  			while(OV2640_PCLK == 0);
  			while(OV2640_PCLK == 1);
  			while(OV2640_PCLK == 0);

  			ImageData(ImagePtr_H, ImagePtr_W) = OV2640_DATA;

  			LCD_RS_SET;
  			LCD_CS_CLR;
  			DATAOUT(yuv_8_to_RGBGray(ImageData(ImagePtr_H, ImagePtr_W)));
  			LCD_WR_CLR;
  			LCD_WR_SET;
  			LCD_CS_SET;

  			while(OV2640_PCLK == 1);

  			++ImagePtr_W;
  		}
  		++ImagePtr_H;
  	}

    //��ֵ�˲�
  	Filter(ImageBuf, ImageWidth, ImageHeight, 3);
    //��Ե���
  	EdgeDetect(ImageBuf, ImageBufProc, ImageWidth, ImageWidth, ImageHeight);
    //�Ҷ�ͼ���ֵ��
  	Image_YUV2Bitmap(ImageBufProc, ImageBufProc, ImageWidth, ImageHeight, 120);
  	//����任���ֱ��
  	HoughLineDetect(ImageBufProc);
    //��ʾ���մ������ͼ��
  	LCD_DrawBinImg(0, ImageHeight, ImageWidth, ImageHeight, 120, ImageBufProc);
  }
}


int main(void)
{
	UART_Init();

	//	  APP_GPIO_Init();
	LCD_Init();
	rt_thread_mdelay(100);
	OV2640_Init();

	/* ���� Camera �߳�*/
	tid_Camera			= rt_thread_create("t_Camera",
		Camera_thread_entry, (void *) 0,
		Camera_THREAD_STACK_SIZE,
		Camera_THREAD_PRIORITY, Camera_THREAD_TIMESLICE);

	if(tid_Camera != RT_NULL)
		rt_thread_startup(tid_Camera);

	return RT_EOK;
}


MSH_CMD_EXPORT(main, thread sample);

