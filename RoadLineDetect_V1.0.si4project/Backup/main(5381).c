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


#define SAMPLE_UART_NAME "uart1" /* ¥Æ ø⁄ …Ë ±∏ √˚ ≥∆ */
static rt_device_t serial; /* ¥Æ ø⁄ …Ë ±∏ æ‰ ±˙ */
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
#define LCD_THREAD_STACK_SIZE 10*1024
#define LCD_THREAD_PRIORITY 20
#define LCD_THREAD_TIMESLICE 10
static rt_thread_t tid_LCD = RT_NULL;
static void LCD_thread_entry(void* parameter)
{
    LCD_Init();

    LCD_LED=1;//µ„¡¡±≥π‚
    main_test(); 		//≤‚ ‘÷˜ΩÁ√Ê
//    rt_thread_mdelay(1000);
    Touch_Test();       //¥•√˛∆¡ ÷–¥≤‚ ‘
}

void uart_init(uint32_t bound){
  //GPIO∂Àø⁄…Ë÷√
  GPIO_InitType GPIO_InitStructure;
	USART_InitType USART_InitStructure;
	NVIC_InitType NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_USART1|RCC_APB2PERIPH_GPIOA, ENABLE);	// πƒ‹USART1£¨GPIOA ±÷”

	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_9; //PA.9
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//∏¥”√Õ∆ÕÏ ‰≥ˆ
  GPIO_Init(GPIOA, &GPIO_InitStructure);//≥ı ºªØGPIOA.9

  //USART1_RX	  GPIOA.10≥ı ºªØ
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//∏°ø’ ‰»Î
  GPIO_Init(GPIOA, &GPIO_InitStructure);//≥ı ºªØGPIOA.10

  //Usart1 NVIC ≈‰÷√
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//«¿’º”≈œ»º∂3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//◊””≈œ»º∂3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQÕ®µ¿ πƒ‹
	NVIC_Init(&NVIC_InitStructure);	//∏˘æ›÷∏∂®µƒ≤Œ ˝≥ı ºªØVICºƒ¥Ê∆˜

   //USART ≥ı ºªØ…Ë÷√

	USART_InitStructure.USART_BaudRate = bound;//¥Æø⁄≤®Ãÿ¬ 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//◊÷≥§Œ™8Œª ˝æ›∏Ò Ω
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//“ª∏ˆÕ£÷πŒª
	USART_InitStructure.USART_Parity = USART_Parity_No;//Œﬁ∆Ê≈º–£—ÈŒª
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//Œﬁ”≤º˛ ˝æ›¡˜øÿ÷∆
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	// ’∑¢ƒ£ Ω

  USART_Init(USART1, &USART_InitStructure); //≥ı ºªØ¥Æø⁄1
  USART_INTConfig (USART1, USART_INT_RDNE, ENABLE);//ø™∆Ù¥Æø⁄Ω” ‹÷–∂œ
  USART_Cmd(USART1, ENABLE);                    // πƒ‹¥Æø⁄1

}

#define Camera_THREAD_STACK_SIZE 60*1024
#define Camera_THREAD_PRIORITY 10
#define Camera_THREAD_TIMESLICE 10
static rt_thread_t tid_Camera = RT_NULL;
static void Camera_thread_entry(void* parameter)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//…Ë÷√÷–∂œ”≈œ»º∂∑÷◊ÈŒ™◊È2£∫2Œª«¿’º”≈œ»º∂£¨2ŒªœÏ”¶”≈œ»º∂
    uart_init(921600);	 	//¥Æø⁄≥ı ºªØŒ™921600
    printf("OV2640 ERRORÛ!\r\n");
    my_mem_init(SRAMIN);        //≥ı ºªØƒ⁄≤øƒ⁄¥Ê≥ÿ
    ov2640_framebuf=mymalloc(SRAMIN,52*1024);//…Í«Î÷°ª∫¥Ê
    ov2640_speed_ctrl();

    rt_thread_mdelay(1000);


    while(1)
    {
        while(OV2640_Init())            //≥ı ºªØOV2640
        {
//          printf("OV2640 ¥ÌŒÛ!\r\n");
            rt_thread_mdelay(200);
        }
        rt_thread_mdelay(1500);
        OV2640_OutSize_Set(OV2640_JPEG_WIDTH,OV2640_JPEG_HEIGHT);
        ov2640_speed_ctrl();

        while(1)
        {

            while(OV2640_VSYNC)         //µ»¥˝÷°–≈∫≈
            {
                  //JPG≈ƒ’’
                OV2640_OutSize_Set(OV2640_JPEG_WIDTH,OV2640_JPEG_HEIGHT);
                ov2640_speed_ctrl();
                ov2640_jpg_photo();
            }
        }
    }


}

int main(void)
{
    UART_Init();
    APP_GPIO_Init();
//
//    /* ¥¥Ω® LCD œﬂ≥Ã*/
//    tid_LCD = rt_thread_create("thread_LCD",
//                    LCD_thread_entry, (void*)0,
//                    LCD_THREAD_STACK_SIZE,
//                    LCD_THREAD_PRIORITY, LCD_THREAD_TIMESLICE);
//    if (tid_LCD != RT_NULL)
//    rt_thread_startup(tid_LCD);

    /* ¥¥Ω® Camera œﬂ≥Ã*/
    tid_Camera = rt_thread_create("thread_Camera",
                    Camera_thread_entry, (void*)0,
                    Camera_THREAD_STACK_SIZE,
                    Camera_THREAD_PRIORITY, Camera_THREAD_TIMESLICE);
    printf("tid_Camera=%02X",tid_Camera);
    if (tid_Camera != RT_NULL)
    rt_thread_startup(tid_Camera);



    return 0;
}
MSH_CMD_EXPORT(main, thread sample);

