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
#define THREAD_PRIORITY 25
#define THREAD_STACK_SIZE 512
#define THREAD_TIMESLICE 5

static rt_thread_t tid1 = RT_NULL;

static void thread1_entry(void *parameter)
{
  rt_uint32_t count = 0;
  while (1)
  {
    /* �� �� 1 �� �� �� �� �� �� �� �У� һ ֱ �� ӡ �� �� ֵ */
    rt_kprintf("thread1 count: %d\n", count ++);
    rt_thread_mdelay(500);
  }
}
ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[1024];
static struct rt_thread thread2;
/* �� �� 2 �� �� */
static void thread2_entry(void *param)
{
  rt_uint32_t count = 0;
  /* �� �� 2 ӵ �� �� �� �� �� �� ���� �� �� ռ �� �� 1 �� �� �� ִ �� */
  for (count = 0; count < 10 ; count++)
  {
    /* �� �� 2 �� ӡ �� �� ֵ */
    rt_kprintf("thread2 count: %d\n", count);
  }
  rt_kprintf("thread2 exit\n");
  /* �� �� 2 �� �� �� �� �� Ҳ �� �� �� �� ϵ ͳ �� �� */
}

int main(void)
{
    UART_Init();
    APP_GPIO_Init();
    /* �� �� �� �� 1�� �� �� �� thread1�� �� �� �� thread1_entry*/
    tid1 = rt_thread_create("thread1",
                            thread1_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY, THREAD_TIMESLICE);
    /* �� �� �� �� �� �� �� �� �飬 �� �� �� �� �� �� */
    if (tid1 != RT_NULL)
    rt_thread_startup(tid1);

    /* �� ʼ �� �� �� 2�� �� �� �� thread2�� �� �� �� thread2_entry */
    rt_thread_init(&thread2, "thread2",
                    thread2_entry,
                    RT_NULL, &thread2_stack[0],
                    sizeof(thread2_stack),
                    THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    rt_thread_startup(&thread2);




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

