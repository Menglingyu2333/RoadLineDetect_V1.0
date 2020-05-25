
/* defined the LED2 pin: PD13 */
#define LED2_PIN    GET_PIN(D, 13)
/* defined the LED3 pin: PD14 */
#define LED3_PIN    GET_PIN(D, 14)
/* defined the LED4 pin: PD15 */
#define LED4_PIN    GET_PIN(D, 15)

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

void EXTI9_5_IRQHandler(void)
{
//  GPIO_WriteBit(GPIOD, GPIO_Pins_10, temp);
  if(temp==0) temp=1;
    else temp=0;
  EXTI->PND = EXTI_Line6;
}

void APP_GPIO_Init()
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

