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
#include "drv_gpio.h"
#include "sys.h"
#include "lcd.h"
#include "touch.h"
#include "gui.h"
#include "test.h"

#include "ov2640.h"
#include "malloc.h"
//#include "pic.h"

/* defined the LED2 pin: PD13 */
#define LED2_PIN    GET_PIN(D, 13)
/* defined the LED3 pin: PD14 */
#define LED3_PIN    GET_PIN(D, 14)
/* defined the LED4 pin: PD15 */
#define LED4_PIN    GET_PIN(D, 15)


int temp;
#define SAMPLE_UART_NAME "uart1" /* 串 口 设 备 名 称 */
static rt_device_t serial; /* 串 口 设 备 句 柄 */
static void UART_Init()
{
    serial = rt_device_find("uart1");
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
}

static void APP_GPIO_Init()
{
    /* set LED2 pin mode to output */
    rt_pin_mode(LED2_PIN, PIN_MODE_OUTPUT);
    /* set LED3 pin mode to output */
    rt_pin_mode(LED3_PIN, PIN_MODE_OUTPUT);
    /* set LED4 pin mode to output */
    rt_pin_mode(LED4_PIN, PIN_MODE_OUTPUT);
//    rt_pin_write(LED4_PIN, PIN_HIGH);

//    rt_pin_mode(LCD_BL, PIN_MODE_OUTPUT);
}
//#define LCD_THREAD_STACK_SIZE 1024
//#define LCD_THREAD_PRIORITY 20
//#define LCD_THREAD_TIMESLICE 10
//static rt_thread_t tid_LCD = RT_NULL;
//static void LCD_thread_entry(void* parameter)
//{
//    LCD_Init();
//
//    LCD_LED=1;//点亮背光
////    main_test(); 		//测试主界面
////    rt_thread_mdelay(1000);
////    Touch_Test();       //触摸屏手写测试
//    while(1)
//    {
//        Gui_Drawbmp16(0,0,ov2640_framebuf);
//        rt_thread_mdelay(100);
//    }
//
//}

void NVIC_cfg()
{

    EXTI_InitType EXTI_InitStructure;
    GPIO_InitType   GPIO_InitStructure;

    NVIC_InitType NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                          //选择中断分组2


    NVIC_InitStructure.NVIC_IRQChannel= EXTI9_5_IRQn;  //选择中断通道5

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 2; //抢占式中断优先级设置为2

    NVIC_InitStructure.NVIC_IRQChannelSubPriority= 2;  //响应式中断优先级设置为2

    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                                   //使能中断

    NVIC_Init(&NVIC_InitStructure);


   RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC|RCC_APB2PERIPH_GPIOD, ENABLE);

   /* Configure PA.00 pin as input floating */
   GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;
   GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
   GPIO_Init(GPIOC, &GPIO_InitStructure);

   /* Connect EXTI0 Line to PA.10 pin */
   GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,(GPIO_PinsSource6));//PA10对中断线10

   EXTI_InitStructure.EXTI_Line=EXTI_Line6;
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
   EXTI_InitStructure.EXTI_LineEnable = ENABLE;
   EXTI_Init(&EXTI_InitStructure);   //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

   /* Configure PA.00 pin as input floating */
   GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
   GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
   GPIO_Init(GPIOD, &GPIO_InitStructure);

}

//void EXTI6_Init(void)
//{
//}
void EXTI9_5_IRQHandler(void)
{
//  GPIO_WriteBit(GPIOD, GPIO_Pins_10, temp);
  if(temp==0) temp=1;
    else temp=0;
  EXTI->PND = EXTI_Line6;
}
#define RGB565_RED      0xf800
#define RGB565_GREEN    0x07e0
#define RGB565_BLUE     0x001f

#define Camera_THREAD_STACK_SIZE 1024
#define Camera_THREAD_PRIORITY 10
#define Camera_THREAD_TIMESLICE 10
static rt_thread_t tid_Camera = RT_NULL;

//把灰度值转换为RGB565格式
u16 Gray_to_RGB565Gray(u16 Gray)
{
  u16 RGB565Gray;
  RGB565Gray=((0x001F&Gray)<<11)|((0x003F&(Gray<<1))<<5)|(0x001F&Gray);
	return RGB565Gray;
}
u16 RGB565_to_Gray(u16 RGB)
{
  u16 Gray;
  u16 R  = (RGB & RGB565_RED)    >> 3+8;
  u16 G  = (RGB & RGB565_GREEN)  >> 5;
  u16 B  = (RGB & RGB565_BLUE)   ;
  Gray=(u16)(( R*77 +  G * 150 +  B * 29 +128 )/256);
  return Gray;
}

uint16_t RGB565_framebuf;
void OV2640_YUV422_Mode(void)
{
	//éè??:YUV422??ê?
	SCCB_WR_Reg(0xFF, 0x00);//éè??:YUV422ê?3?
	SCCB_WR_Reg(0xDA, 0x01);//YUV422.YVYU
}
u16 yuv422_to_Gray(u16 yuv422)
{
	u16 Gray;
	Gray =(((yuv422>>(8+3))<<11)|((yuv422>>(8+2))<<5)|((yuv422>>(8+3))<<0));
	return Gray;
}


u16 yuv422_y_to_bitmap(u8 threshold,u16 yuv422)
{
	u16 bitmap;
	u8 temp;

	temp = (u8)(yuv422>>8);

	if(temp >= threshold)
	{
		bitmap =0xffff;
	}
	else
	{
		bitmap =0x0000;
	}

	return bitmap;
}

static void Camera_thread_entry(void* parameter)
{

    uint32_t i=0;
//    uint32_t jpeglen=0;
//    uint32_t lineLength=0;

//    OV2640_YUV422_Mode();

    LCD_SetWindows(0,0,OV2640_JPEG_WIDTH-1,OV2640_JPEG_HEIGHT-1);//窗口设置
    for(i=0;i<10;i++)   //丢弃10帧，等待OV2640自动调节好（曝光白平衡之类的）
    {
     while(OV2640_VSYNC==1);
     while(OV2640_VSYNC==0);
    }
    rt_hw_interrupt_disable();

    while(1)
    {
//      jpeglen=0;
      LCD_SetCursor(0, 0);

     while(OV2640_VSYNC==1)  //开始采集jpeg数据
     {
      while(OV2640_HREF)
      {
        while(OV2640_PCLK==0);
//        ov2640_framebuf[jpeglen]=OV2640_DATA;
        RGB565_framebuf=OV2640_DATA;
        while(OV2640_PCLK==1);
        while(OV2640_PCLK==0);
        RGB565_framebuf|=(OV2640_DATA)<<8;
        RGB565_framebuf=RGB565_to_Gray(RGB565_framebuf);
        RGB565_framebuf=Gray_to_RGB565Gray(RGB565_framebuf);

//        RGB565_framebuf=yuv422_to_Gray(RGB565_framebuf);
//        RGB565_framebuf=yuv422_y_to_bitmap(60,RGB565_framebuf);

        LCD_RS_SET;
        LCD_CS_CLR;
        DATAOUT(RGB565_framebuf);
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
    APP_GPIO_Init();

    LCD_Init();
    rt_thread_mdelay(100);
    OV2640_Init();

//    /* 创建 LCD 线程*/
//    tid_LCD = rt_thread_create("t_LCD",
//                    LCD_thread_entry, (void*)0,
//                    LCD_THREAD_STACK_SIZE,
//                    LCD_THREAD_PRIORITY, LCD_THREAD_TIMESLICE);
//    if (tid_LCD != RT_NULL)
//    rt_thread_startup(tid_LCD);

    /* 创建 Camera 线程*/
    tid_Camera = rt_thread_create("t_Camera",
                    Camera_thread_entry, (void*)0,
                    Camera_THREAD_STACK_SIZE,
                    Camera_THREAD_PRIORITY, Camera_THREAD_TIMESLICE);
    if (tid_Camera != RT_NULL)
    rt_thread_startup(tid_Camera);



    return 0;
}
MSH_CMD_EXPORT(main, thread sample);

