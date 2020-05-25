/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-01-08     shelton      first version
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

int temp;
#define SAMPLE_UART_NAME "uart1" /* �� �� �� �� �� �� */
static rt_device_t serial; /* �� �� �� �� �� �� */
static void UART_Init()
{
    serial = rt_device_find("uart1");
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
}

//#define LCD_THREAD_STACK_SIZE 1024
//#define LCD_THREAD_PRIORITY 20
//#define LCD_THREAD_TIMESLICE 10
//static rt_thread_t tid_LCD = RT_NULL;
//static void LCD_thread_entry(void* parameter)
//{
//    LCD_Init();
//
//    LCD_LED=1;//��������
////    main_test(); 		//����������
////    rt_thread_mdelay(1000);
////    Touch_Test();       //��������д����
//    while(1)
//    {
//        Gui_Drawbmp16(0,0,ov2640_framebuf);
//        rt_thread_mdelay(100);
//    }
//
//}

#define Camera_THREAD_STACK_SIZE 1024
#define Camera_THREAD_PRIORITY 10
#define Camera_THREAD_TIMESLICE 10
static rt_thread_t tid_Camera = RT_NULL;

uint16_t RGB_PixelBuf_16b;

static void Camera_thread_entry(void* parameter)
{

    uint32_t i=0;
//    uint32_t jpeglen=0;
//    uint32_t lineLength=0;


    LCD_SetWindows(0,0,OV2640_JPEG_WIDTH-1,OV2640_JPEG_HEIGHT-1);//��������
    for(i=0;i<10;i++)   //����10֡���ȴ�OV2640�Զ����ںã��ع��ƽ��֮��ģ�
    {
     while(OV2640_VSYNC==1);
     while(OV2640_VSYNC==0);
    }
    rt_hw_interrupt_disable();

    while(1)
    {
//      jpeglen=0;
      LCD_SetCursor(0, 0);

     while(OV2640_VSYNC==1)  //��ʼ�ɼ�jpeg����
     {
      while(OV2640_HREF)
      {
        while(OV2640_PCLK==0);
//        ov2640_framebuf[jpeglen]=OV2640_DATA;
        RGB_PixelBuf_16b=OV2640_DATA;
        while(OV2640_PCLK==1);
        while(OV2640_PCLK==0);
        RGB_PixelBuf_16b|=(OV2640_DATA)<<8;

        RGB_PixelBuf_16b=yuv422_to_Gray(RGB_PixelBuf_16b);
        RGB_PixelBuf_16b=yuv422_y_to_bitmap(60,RGB_PixelBuf_16b);

        LCD_RS_SET;
        LCD_CS_CLR;
        DATAOUT(RGB_PixelBuf_16b);
        LCD_WR_CLR;
        LCD_WR_SET;
        LCD_CS_SET;
        while(OV2640_PCLK==1);
      }
     }
//     LCD_Drawbmp16(0,0,OV2640_JPEG_WIDTH,OV2640_JPEG_HEIGHT,ov2640_framebuf);
    }

}


int main(void)
{
    UART_Init();
//    APP_GPIO_Init();

    LCD_Init();
    rt_thread_mdelay(100);
    OV2640_Init();

//    /* ���� LCD �߳�*/
//    tid_LCD = rt_thread_create("t_LCD",
//                    LCD_thread_entry, (void*)0,
//                    LCD_THREAD_STACK_SIZE,
//                    LCD_THREAD_PRIORITY, LCD_THREAD_TIMESLICE);
//    if (tid_LCD != RT_NULL)
//    rt_thread_startup(tid_LCD);

    /* ���� Camera �߳�*/
    tid_Camera = rt_thread_create("t_Camera",
                    Camera_thread_entry, (void*)0,
                    Camera_THREAD_STACK_SIZE,
                    Camera_THREAD_PRIORITY, Camera_THREAD_TIMESLICE);
    if (tid_Camera != RT_NULL)
    rt_thread_startup(tid_Camera);



    return 0;
}
MSH_CMD_EXPORT(main, thread sample);

