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

/* defined the LED2 pin: PD13 */
#define LED2_PIN    GET_PIN(D, 13)
/* defined the LED3 pin: PD14 */
#define LED3_PIN    GET_PIN(D, 14)
/* defined the LED4 pin: PD15 */
#define LED4_PIN    GET_PIN(D, 15)


#define SAMPLE_UART_NAME "uart1" /* �� �� �� �� �� �� */
static rt_device_t serial; /* �� �� �� �� �� �� */
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
#define LCD_THREAD_STACK_SIZE 1024
#define LCD_THREAD_PRIORITY 20
#define LCD_THREAD_TIMESLICE 10
static rt_thread_t tid_LCD = RT_NULL;
static void LCD_thread_entry(void* parameter)
{
    LCD_Init();

    LCD_LED=1;//��������
//    main_test(); 		//����������
//    rt_thread_mdelay(1000);
//    Touch_Test();       //��������д����
    while(1)
    {
        Gui_Drawbmp16(0,0,ov2640_framebuf);
        rt_thread_mdelay(100);
    }

}

#define Camera_THREAD_STACK_SIZE 1024
#define Camera_THREAD_PRIORITY 10
#define Camera_THREAD_TIMESLICE 10
static rt_thread_t tid_Camera = RT_NULL;
static void Camera_thread_entry(void* parameter)
{

//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
//    uart_init(921600);	 	//���ڳ�ʼ��Ϊ921600
    printf("OV2640 ERROR?\r\n");
    my_mem_init(SRAMIN);        //��ʼ���ڲ��ڴ��
    ov2640_framebuf=mymalloc(SRAMIN,10*1024);//����֡����
    ov2640_speed_ctrl();

    rt_thread_mdelay(1000);


    while(1)
    {
        while(OV2640_Init())            //��ʼ��OV2640
        {
//          printf("OV2640 ����!\r\n");
            rt_thread_mdelay(200);
        }
        rt_thread_mdelay(1500);
        OV2640_OutSize_Set(OV2640_JPEG_WIDTH,OV2640_JPEG_HEIGHT);
        ov2640_speed_ctrl();

        while(1)
        {

            while(OV2640_VSYNC)         //�ȴ�֡�ź�
            {
                  //JPG����
                OV2640_OutSize_Set(OV2640_JPEG_WIDTH,OV2640_JPEG_HEIGHT);
                ov2640_speed_ctrl();
                ov2640_jpg_photo();
                rt_thread_mdelay(1000);
            }
        }
    }


}

int main(void)
{
    UART_Init();
    APP_GPIO_Init();

    /* ���� LCD �߳�*/
    tid_LCD = rt_thread_create("t_LCD",
                    LCD_thread_entry, (void*)0,
                    LCD_THREAD_STACK_SIZE,
                    LCD_THREAD_PRIORITY, LCD_THREAD_TIMESLICE);
    if (tid_LCD != RT_NULL)
    rt_thread_startup(tid_LCD);

    /* ���� Camera �߳�*/
    tid_Camera = rt_thread_create("t_Camera",
                    Camera_thread_entry, (void*)0,
                    Camera_THREAD_STACK_SIZE,
                    Camera_THREAD_PRIORITY, Camera_THREAD_TIMESLICE);
    printf("tid_Camera=%02X",tid_Camera);
    if (tid_Camera != RT_NULL)
    rt_thread_startup(tid_Camera);



    return 0;
}
MSH_CMD_EXPORT(main, thread sample);

