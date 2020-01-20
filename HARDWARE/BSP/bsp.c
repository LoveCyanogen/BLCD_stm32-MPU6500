#include "config.h"
/****************************************************************************
* ����:void  BSP_Init(void)
* ����:������ʼ��
* ��ڲ���:��
* ���ڲ���:��
* ˵��:��
****************************************************************************/


void BSP_Init(void)
{
	SystemInit();
	TIM2_Init();	
	delay_init(168);
	GPIO_Config();
	HALL_GPIO_Init();
	current_Init();
	CAN_Config();
	BLDC_TIMx_PWM_Init();
	LED_Init();
	mpu6500_init();
  uart_init(115200);  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

