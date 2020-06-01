

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

//#include "pic.h"
int 			temp;

#define SAMPLE_UART_NAME		"uart1" /* 串 口 设 备 名 称 */
static rt_device_t serial; /* 串 口 设 备 句 柄 */


static void UART_Init()
{
	serial				= rt_device_find("uart1");
	rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
}


//#define LCD_THREAD_STACK_SIZE 1024
//#define LCD_THREAD_PRIORITY 20
//#define LCD_THREAD_TIMESLICE 10
//static rt_thread_t tid_LCD = RT_NULL;
//static void LCD_thread_entry(void* parameter)
//{
//	  LCD_Init();
//
//	  LCD_LED=1;//点亮背光
////	main_test();		//测试主界面
////	rt_thread_mdelay(1000);
////	Touch_Test();		//触摸屏手写测试
//	  while(1)
//	  {
//		  Gui_Drawbmp16(0,0,ov2640_framebuf);
//		  rt_thread_mdelay(100);
//	  }
//
//}
#define Camera_THREAD_STACK_SIZE 1024
#define Camera_THREAD_PRIORITY	10
#define Camera_THREAD_TIMESLICE 10
static rt_thread_t tid_Camera = RT_NULL;

uint16_t		RGB_PixelBuf_16b;
u8 *			ImageBuf;
u8				ImagePtr_W, ImagePtr_H;

#define ImageData(H,W) *(u8*)(ImageBuf+H*ImageWidth+W)


static void Camera_thread_entry(void * parameter)
{
	uint32_t		i	= 0;

	ImageBuf			= rt_malloc(ImageWidth * ImageHeight);

	if (ImageBuf == NULL)
		{
		rt_kprintf("get memory FAILED!\n");
		return;
		}

	LCD_SetWindows(0, 0, ImageWidth - 1, ImageHeight - 1); //窗口设置

	for (i = 0; i < 10; i++) //丢弃10帧，等待OV2640自动调节好（曝光白平衡之类的）
		{
		while (OV2640_VSYNC == 1)
			;

		while (OV2640_VSYNC == 0)
			;
		}

	rt_hw_interrupt_disable();

	while (1)
		{
		ImagePtr_W			= 0;
		ImagePtr_H			= 0;
		LCD_SetCursor(0, 0);

		while (OV2640_VSYNC == 1) //开始采集jpeg数据
			{
			ImagePtr_W			= 0;
			while (OV2640_HREF)
				{
				while (OV2640_PCLK == 0)
					;

				while (OV2640_PCLK == 1)
					;

				while (OV2640_PCLK == 0)
					;

				ImageData(ImagePtr_H,ImagePtr_W)		= OV2640_DATA;

//				LCD_RS_SET;
//				LCD_CS_CLR;
//				DATAOUT(yuv_8_to_RGBGray(ImageData(ImagePtr_H,ImagePtr_W)));
//				LCD_WR_CLR;
//				LCD_WR_SET;
//				LCD_CS_SET;

				while (OV2640_PCLK == 1)
					;

				++ImagePtr_W;
				}
//      rt_kprintf("After VSYNC:ImagePtr_H=%d,ImagePtr_W=%d\n",ImagePtr_H,ImagePtr_W);
	    ++ImagePtr_H;
			}
      rt_kprintf("\nAfter VSYNC:ImagePtr_H=%d,ImagePtr_W=%d\n",ImagePtr_H,ImagePtr_W);
    LCD_DrawYVU8Img(0,0,ImageWidth,ImageHeight,ImageBuf);

		}

}


#define THREAD_PRIORITY 		25
#define THREAD_STACK_SIZE		512
#define THREAD_TIMESLICE		5
rt_thread_t 	tid = RT_NULL;

/* 线 程 入 口 */
void thread1_entry(void * parameter)
{
	int 			i;
	char *			ptr = RT_NULL;					/* 内 存 块 的 指 针 */

	ptr 				= rt_malloc(76800);

	/* 如 果 分 配 成 功 */
	if (ptr != RT_NULL)
		{
		rt_kprintf("get memory :%d byte\n", (1 << i));

		/* 释 放 内 存 块 */
		rt_free(ptr);
		rt_kprintf("free memory :%d byte\n", (1 << i));
		ptr 				= RT_NULL;
		}
	else
		{
		rt_kprintf("try to get %d byte memory failed!\n", (1 << i));
		return;
		}
}


int main(void)
{
	UART_Init();

	//	  APP_GPIO_Init();
	LCD_Init();
	rt_thread_mdelay(100);
	OV2640_Init();

	//	  /* 创建 LCD 线程*/
	//	  tid_LCD = rt_thread_create("t_LCD",
	//					  LCD_thread_entry, (void*)0,
	//					  LCD_THREAD_STACK_SIZE,
	//					  LCD_THREAD_PRIORITY, LCD_THREAD_TIMESLICE);
	//	  if (tid_LCD != RT_NULL)
	//	  rt_thread_startup(tid_LCD);

	/* 创建 Camera 线程*/
	tid_Camera			= rt_thread_create("t_Camera",
		Camera_thread_entry, (void *) 0,
		Camera_THREAD_STACK_SIZE,
		Camera_THREAD_PRIORITY, Camera_THREAD_TIMESLICE);

	if (tid_Camera != RT_NULL)
		rt_thread_startup(tid_Camera);

	/* 创 建 线 程 1 */
	//	tid 				= rt_thread_create("thread1",
	//		thread1_entry, RT_NULL,
	//		THREAD_STACK_SIZE,
	//		THREAD_PRIORITY,
	//		THREAD_TIMESLICE);
	//
	//	if (tid != RT_NULL)
	//		rt_thread_startup(tid);
	return 0;
}


MSH_CMD_EXPORT(main, thread sample);

