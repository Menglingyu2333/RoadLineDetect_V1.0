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
#include "lcd.h"

/* defined the LED2 pin: PD13 */
#define LED2_PIN    GET_PIN(D, 13)
/* defined the LED3 pin: PD14 */
#define LED3_PIN    GET_PIN(D, 14)
/* defined the LED4 pin: PD15 */
#define LED4_PIN    GET_PIN(D, 15)


#define SAMPLE_UART_NAME "uart1" /* �� �� �� �� �� �� */
static rt_device_t serial; /* �� �� �� �� �� �� */
void UART_Init(void);
void UART_Init()
{
    serial = rt_device_find("uart1");
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
}

void APP_GPIO_Init(void);
void APP_GPIO_Init()
{
    /* set LED2 pin mode to output */
    rt_pin_mode(LED2_PIN, PIN_MODE_OUTPUT);
    /* set LED3 pin mode to output */
    rt_pin_mode(LED3_PIN, PIN_MODE_OUTPUT);
    /* set LED4 pin mode to output */
    rt_pin_mode(LED4_PIN, PIN_MODE_OUTPUT_OD);
    rt_pin_write(LED4_PIN, PIN_HIGH);

//    rt_pin_mode(LCD_BL, PIN_MODE_OUTPUT);
}
void LED2_StartBlink()
{
    rt_pin_write(LED2_PIN, PIN_LOW);
    rt_thread_mdelay(200);
    rt_pin_write(LED2_PIN, PIN_HIGH);
    rt_thread_mdelay(200);
    rt_pin_write(LED2_PIN, PIN_LOW);
    rt_thread_mdelay(200);
    rt_pin_write(LED2_PIN, PIN_HIGH);
    rt_thread_mdelay(200);
    rt_pin_write(LED2_PIN, PIN_LOW);
    rt_thread_mdelay(200);
    rt_pin_write(LED2_PIN, PIN_HIGH);
    rt_thread_mdelay(200);
}
#define THREAD_STACK_SIZE 1024
#define THREAD_PRIORITY 20
#define THREAD_TIMESLICE 10
/* �� �� ÿ �� �� �� �� �� �� �� */
volatile rt_uint32_t count[2];
/* �� �� 1��2 �� �� һ �� �� �ڣ� �� �� �� �� �� �� ͬ */
static void thread_entry(void* parameter)
{
  rt_uint32_t value;
  value = (rt_uint32_t)parameter;
  while (1)
  {
    rt_kprintf("thread %d is running\n", value);
    rt_thread_mdelay(1000); // �� ʱ һ �� ʱ ��
  }
}
static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;
static void hook_of_scheduler(struct rt_thread* from, struct rt_thread* to)
{
  rt_kprintf("from: %s --> to: %s \n", from->name , to->name);
}
int main(void)
{
    UART_Init();
    APP_GPIO_Init();
   /* �� �� �� �� �� �� �� */
    rt_scheduler_sethook(hook_of_scheduler);
    /* �� �� �� �� 1 */
    tid1 = rt_thread_create("thread1",
    thread_entry, (void*)1,
    THREAD_STACK_SIZE,
    THREAD_PRIORITY, THREAD_TIMESLICE);
    if (tid1 != RT_NULL)
    rt_thread_startup(tid1);
    /* �� �� �� �� 2 */
    tid2 = rt_thread_create("thread2",
    thread_entry, (void*)2,
    THREAD_STACK_SIZE,
    THREAD_PRIORITY,THREAD_TIMESLICE - 5);
    if (tid2 != RT_NULL)
    rt_thread_startup(tid2);

    //LCD_Init();
//    while (1)
//    {
//        LED2_StartBlink();
//        LCD_Clear(BRED);
//        LED2_StartBlink();
//        LCD_Clear(GREEN);
////        LCD_Fill(0,0,lcddev.width,20,BLUE);
////        LCD_Fill(0,lcddev.height-20,lcddev.width,lcddev.height,BLUE);
////        POINT_COLOR=WHITE;
//////    	DrawTestPage("ȫ�������ۺϲ��Գ���");
//////
////    	Gui_StrCenter(0,30,RED,BLUE,"ȫ������",16,1);//������ʾ
////    	Gui_StrCenter(0,60,RED,BLUE,"�ۺϲ��Գ���",16,1);//������ʾ
////    	Gui_StrCenter(0,90,YELLOW,BLUE,"2.8' ILI9341 240X320",16,1);//������ʾ
////    	Gui_StrCenter(0,120,BLUE,BLUE,"xiaoFeng@QDtech 2014-02-25",16,1);//������ʾ
////        rt_thread_mdelay(5000);
//
//
//    }
    return 0;
}
MSH_CMD_EXPORT(main, thread sample);

