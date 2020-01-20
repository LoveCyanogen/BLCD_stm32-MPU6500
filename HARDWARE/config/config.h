#ifndef _CONFIG_H_	
#define _CONFIG_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include <math.h>
#include <string.h>			//�ַ����Ƚ�
#include <ctype.h>			//��дת��
#include <stdlib.h>
#include <stdio.h>

#include "stm32f4xx.h"		//STM32�̼���
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_can.h"
#include "stm32f4xx_usart.h"
#include "bsp.h"			//�弶��ʼ��
#include "sys.h"
#include "time2.h"
#include "delay.h"
#include "usart.h"
#include "current.h"
#include "ds18b20.h"
#include "mpu6500.h"
#include "led.h"
#include "pwm.h"
#include "can.h"
#include "key.h"
#include "adc.h"
#include "GPIO_Init.h"
#include "BLDCTIM_Init.h" 


//#include "usb_lib.h"
//#include "hw_config.h"
//#include "usb_pwr.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
typedef enum
{
  CW=0,  // ˳ʱ�뷽��(���ŵ���ῴ)��һ�㶨��Ϊ��ת
  CCW=1  // ��ʱ�뷽��(���ŵ���ῴ)��һ�㶨��Ϊ��ת
}MOTOR_DIR;

typedef enum 
{
  STOP=0,  // ͣ��
  RUN=1    // ����
}MOTOR_STATE;
//���ܿ�������
typedef struct SendControlCmd
{
	uint8_t GearPos;		//��λ
	uint8_t Value_L;		//���Ʊ仯��ֵ
	uint8_t Value_H;		//���Ʊ仯��ֵ
	uint8_t SpeedFlag;		//�ٶȵ�λ
}ControlCmdType;
extern ControlCmdType ControlDriveCmd;
//����ϴ���ECU��״̬��Ϣ
typedef struct MotorStateInformation
{
	uint8_t EnableState;	//���յ���ECUָ��ִ�е�״̬
	uint8_t	ModeState;		//������״̬����λ�źţ�
	uint8_t Value_L;		//ִ�е�ֵ
	uint8_t Value_H;		//ִ�е�ֵ
	uint8_t	ErrorCodeNum;	//���ֵĹ���
	uint8_t Reserved1;		//����ֵ
	uint8_t Reserved2;		//����ֵ
	uint8_t Reserved3;		//����ֵ
}MotorStateDataType;
typedef struct
{
  __IO int          motor_speed;        // ���ת��(RPM):0..3500
  __IO MOTOR_STATE  motor_state;        // �����ת״̬
  __IO MOTOR_DIR    motor_direction;    // ���ת������
  __IO uint32_t     step_counter;       // �������������������ڲ������ת��
  __IO uint16_t     stalling_count;     // ֹͣ��־�����ֵ����2000����Ϊ���ֹͣ��ת
}MOTOR_DEVICE;
extern MOTOR_DEVICE bldc_dev;
//����PID�ṹ��
typedef struct 
{
   __IO int      SetPoint;      //�趨Ŀ�� Desired Value
   __IO double   Proportion;    //�������� Proportional Const
   __IO double   Integral;      //���ֳ��� Integral Const
   __IO double   Derivative;    //΢�ֳ��� Derivative Const
   __IO int      LastError;     //Error[-1]
   __IO int      PrevError;     //Error[-2]
}PID;

/* ˽�к궨�� ----------------------------------------------------------------*/
/*************************************/
//����PID��غ�
//�����������趨�Ե������Ӱ��ǳ���
/*************************************/
#define  P_DATA                   0.5               //P����
#define  I_DATA                   0.06              //I����
#define  D_DATA                   0                 //D����

//#define  MOTOR_RATED_SPEED        2500              //ȷ��ת��
#define  MOTOR_MAX_SPEED          10              //����ת��
#define  MOTOR_MIN_SPEED          1             
#define  MOTOR_POLE_PAIR_NUM      4                 //������(8 �ż���)
#define  DRIVE_ID			0x44	//�������ID


/************************************/
#define LED_RED_OFF()   	GPIO_ResetBits(GPIOB, GPIO_Pin_7 );  	   //LED1 ��������ָʾ��
#define LED_RED_ON()  		GPIO_SetBits(GPIOB, GPIO_Pin_7 ); 	  

#define LED_GREEN_OFF()   	GPIO_SetBits(GPIOB, GPIO_Pin_8 );  	   //LED2 
#define LED_GREEN_ON()  		GPIO_ResetBits(GPIOB, GPIO_Pin_8 ); 	  

#define LED_YELLOW_OFF()   	GPIO_ResetBits(GPIOB, GPIO_Pin_9 );  //LED3  ��ECUʧ��ָʾ�� 
#define LED_YELLOW_ON()  		 GPIO_SetBits(GPIOB, GPIO_Pin_9 );  	  

#define Ctrl_Brake_OFF()   	GPIO_SetBits(GPIOD, GPIO_Pin_7 );  	  
#define Ctrl_Brake_ON()  	GPIO_ResetBits(GPIOD, GPIO_Pin_7 ); 	  //�رյ�Դ����


extern const unsigned char Ascill_16[];

#define Enable_PrintfDebug

void DebugPrintf(const char *fmt,...);
void TIM1_PWM_Init(void);
void SendControlCmd(void);

#ifdef __cplusplus		   //�����CPP����C���� //��������
}

#endif

#endif
