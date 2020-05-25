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
    rt_pin_mode(LED4_PIN, PIN_MODE_OUTPUT);

//    rt_pin_mode(LCD_BL, PIN_MODE_OUTPUT);
}
void rt_hw_us_delay(rt_uint32_t us)
{
    rt_uint32_t delta;/* �� �� �� ʱ �� �� �� tick �� */
    us = us * (SysTick->LOAD/(1000000/RT_TICK_PER_SECOND));
    /* �� �� �� ǰ ʱ �� */
    delta = SysTick->VAL; /* ѭ �� �� �� �� ǰ ʱ �䣬 ֱ �� �� �� ָ �� �� ʱ �� �� �� �� ѭ �� */
    while (delta - SysTick->VAL< us);
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
int main(void)
{
    UART_Init();
    APP_GPIO_Init();
    LCD_Init();
    while (1)
    {
        
        LED2_StartBlink();
        LCD_Fill(0,0,lcddev.width,20,BLUE);
        LCD_Fill(0,lcddev.height-20,lcddev.width,lcddev.height,BLUE);
        POINT_COLOR=WHITE;
//    	DrawTestPage("ȫ�������ۺϲ��Գ���");
//
    	Gui_StrCenter(0,30,RED,BLUE,"ȫ������",16,1);//������ʾ
//    	Gui_StrCenter(0,60,RED,BLUE,"�ۺϲ��Գ���",16,1);//������ʾ
//    	Gui_StrCenter(0,90,YELLOW,BLUE,"2.8' ILI9341 240X320",16,1);//������ʾ
//    	Gui_StrCenter(0,120,BLUE,BLUE,"xiaoFeng@QDtech 2014-02-25",16,1);//������ʾ
//        rt_thread_mdelay(5000);


    }
}
