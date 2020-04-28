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

/* defined the LED2 pin: PD13 */
#define LED2_PIN    GET_PIN(D, 13)
/* defined the LED3 pin: PD14 */
#define LED3_PIN    GET_PIN(D, 14)
/* defined the LED4 pin: PD15 */
#define LED4_PIN    GET_PIN(D, 15)
#define SAMPLE_UART_NAME "uart1" /* �� �� �� �� �� �� */

int main(void)
{
    uint32_t Speed = 200;
    char str[] = "hello RT-Thread!\r\n";
    static rt_device_t serial; /* �� �� �� �� �� �� */
    /* set LED2 pin mode to output */
    rt_pin_mode(LED2_PIN, PIN_MODE_OUTPUT);
    /* set LED3 pin mode to output */
    rt_pin_mode(LED3_PIN, PIN_MODE_OUTPUT);
    /* set LED4 pin mode to output */
    rt_pin_mode(LED4_PIN, PIN_MODE_OUTPUT);
    serial = rt_device_find(SAMPLE_UART_NAME);
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
    rt_device_write(serial, 0, str, (sizeof(str) - 1));

    while (1)
    {
        rt_pin_write(LED2_PIN, PIN_LOW);
        rt_thread_mdelay(Speed);
        rt_pin_write(LED3_PIN, PIN_LOW);
        rt_thread_mdelay(Speed);
        rt_pin_write(LED4_PIN, PIN_LOW);
        rt_thread_mdelay(Speed);
        rt_pin_write(LED2_PIN, PIN_HIGH);
        rt_thread_mdelay(Speed);
        rt_pin_write(LED3_PIN, PIN_HIGH);
        rt_thread_mdelay(Speed);
        rt_pin_write(LED4_PIN, PIN_HIGH);
        rt_thread_mdelay(Speed);
    }
}
