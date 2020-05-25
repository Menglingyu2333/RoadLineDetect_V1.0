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


#define SAMPLE_UART_NAME "uart1" /* 串 口 设 备 名 称 */
static rt_device_t serial; /* 串 口 设 备 句 柄 */
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
    /* 线 程 1 采 用 低 优 先 级 运 行， 一 直 打 印 计 数 值 */
    rt_kprintf("thread1 count: %d\n", count ++);
    rt_thread_mdelay(500);
  }
}
ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[1024];
static struct rt_thread thread2;
/* 线 程 2 入 口 */
static void thread2_entry(void *param)
{
  rt_uint32_t count = 0;
  /* 线 程 2 拥 有 较 高 的 优 先 级， 以 抢 占 线 程 1 而 获 得 执 行 */
  for (count = 0; count < 10 ; count++)
  {
    /* 线 程 2 打 印 计 数 值 */
    rt_kprintf("thread2 count: %d\n", count);
  }
  rt_kprintf("thread2 exit\n");
  /* 线 程 2 运 行 结 束 后 也 将 自 动 被 系 统 脱 离 */
}

int main(void)
{
    UART_Init();
    APP_GPIO_Init();
    /* 创 建 线 程 1， 名 称 是 thread1， 入 口 是 thread1_entry*/
    tid1 = rt_thread_create("thread1",
                            thread1_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY, THREAD_TIMESLICE);
    /* 如 果 获 得 线 程 控 制 块， 启 动 这 个 线 程 */
    if (tid1 != RT_NULL)
    rt_thread_startup(tid1);

    /* 初 始 化 线 程 2， 名 称 是 thread2， 入 口 是 thread2_entry */
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
//////    	DrawTestPage("全动电子综合测试程序");
//////
////    	Gui_StrCenter(0,30,RED,BLUE,"全动电子",16,1);//居中显示
////    	Gui_StrCenter(0,60,RED,BLUE,"综合测试程序",16,1);//居中显示
////    	Gui_StrCenter(0,90,YELLOW,BLUE,"2.8' ILI9341 240X320",16,1);//居中显示
////    	Gui_StrCenter(0,120,BLUE,BLUE,"xiaoFeng@QDtech 2014-02-25",16,1);//居中显示
////        rt_thread_mdelay(5000);
//
//
//    }
    return 0;
}
MSH_CMD_EXPORT(main, thread sample);

