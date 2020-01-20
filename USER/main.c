#include "config.h"

MOTOR_DEVICE bldc_dev  ={1000,STOP,CCW,0,0}; //���״̬����ṹ��
static PID bldc_pid;
static unsigned char PID_Status = 0;

const unsigned char  Ascill_16[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

unsigned int Program_Cycle_Nums = 0;
short temperature; 
void PrintCommandsList(void);
void KeyControl(void);
void HALL_EXTI_Callback(void);
void HAL_SYSTICK_Callback(void);
void IncPIDInit(void);
int IncPIDCalc(int NextPoint);
unsigned int Current_filter(unsigned int advalue);
unsigned int SpeedVolt_filter(unsigned int advalue);
extern u8  TIM5CH1_CAPTURE_STA;		//���벶��״̬		    				
extern u32	TIM5CH1_CAPTURE_VAL;	//���벶��ֵ 
u8 time=0;
u8 i=0,t=0;
u8 cnt=0;
u8 res;
u8 canbuf[8];//can���ݻ�����
u8 canres[8];//can����ֵ
long long temp=0;  
int main(void)
{
  BSP_Init();
	/************************************/  
	printf("*** BLDC Control board ***\r\n");
	printf("Date:20200203 V1.0 \r\n");
	while(1)
	{		
			KeyControl();
			delay_ms(10);
			temperature=DS18B20_Get_Temp();	
			printf("DS18B20���¶�Ϊ��%d",temperature);
}
}



//��������
void KeyControl(void)
{
	
	if(KEY_RUNBRAKE == ON)  //run  //PD8����͵�ƽ��������ť����
	{
	  if(bldc_dev.motor_state==STOP) // �������
	  {
		bldc_dev.motor_state=RUN;
		bldc_dev.step_counter=0;//�������������������ת��0
		bldc_dev.stalling_count=0;//ֹͣ��־
		IncPIDInit();
		speed_duty=200; //%20 start  �ٶ�ռ�ձ�
		NVIC_EnableIRQ(HALL_UEXTI_IRQn);//�ⲿ�ж�3
		NVIC_EnableIRQ(HALL_VEXTI_IRQn);//�ⲿ�ж�4
		NVIC_EnableIRQ(HALL_WEXTI_IRQn);//�ⲿ�ж�9_5
		HALL_EXTI_Callback();
	  } 
	
	}else // stop
	{
		BLDC_PHASE_STOP();
		while(KEY_RUNBRAKE);
	}
	
	if(ControlDriveCmd.SpeedFlag == ON)   //�ٶȵ�λ
	{
		if(ControlDriveCmd.Value_H > MOTOR_MAX_SPEED)		//������ٶȳ���max speed ������ֵ�ٶ�
			ControlDriveCmd.Value_H = MOTOR_MAX_SPEED;
		bldc_dev.motor_speed=ControlDriveCmd.Value_H;
	}else
	{
		if(ControlDriveCmd.Value_L > MOTOR_MIN_SPEED)		//����С�ٶȵ���main speed ������ֵ�ٶ�
			ControlDriveCmd.Value_L = MOTOR_MIN_SPEED;
		bldc_dev.motor_speed=ControlDriveCmd.Value_L;
		bldc_pid.SetPoint=bldc_dev.motor_speed;							//PID�趨�ٶ�
	}
	
	if(ControlDriveCmd.GearPos == ON)		//����
	{
		bldc_dev.motor_direction=CCW;
	}
	else
	{
		bldc_dev.motor_direction=CW;
	}
	time++;
	if(time==100)
	{   
		if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)		//û�н��յ�����,ֱ���˳� 
			BLDC_PHASE_STOP();
			LED_YELLOW_ON();
		  bldc_dev.motor_state=STOP;
			bldc_dev.motor_speed=0;
			bldc_dev.stalling_count=0;
			time=0;
	}
	
	//CH3 �ܵ���������
	if(Current_filter(ADCPrimevalValue[1]) > 3000)//���������
	{
		Ctrl_Brake_ON();//PB12��ƽ��
		BLDC_PHASE_STOP();//�ر���������
		LED_RED_ON();//GPIOB7�ߵ�ƽ,��
		while(1);
	}
		
}
/**
  * ��������: �ⲿ�жϷ�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void HALL_EXTI_Callback(void)
{
  __IO uint8_t uwStep = 0;
  static __IO int8_t uwStep0=0;
  
  uint16_t hall_read=(HALL_GPIO->IDR)&0x000E; // ��ȡ����������״̬ pin1 2 3
  
  if(bldc_dev.motor_state==STOP)return;
  
  uwStep=hall_read>>1; //�˴���Ҫ����ʵ�ʸ���˳��
  if(bldc_dev.motor_direction==CW) // �����ж�  
	uwStep=7-uwStep;
  
  BLDC_PHASE_CHANGE(uwStep);   // ��������
  if(uwStep0!=uwStep) // ȷ������Ѿ�����
  {    
    bldc_dev.step_counter++;
    bldc_dev.stalling_count=0;
  }
  uwStep0=uwStep;
}


void EXTI3_IRQHandler(void)		//�����ж�
{
	/* ȷ���Ƿ������EXTI LINE�ж�*/
	if(EXTI_GetITStatus(HALL_U_EXITLINE) != RESET)
	{
	 HALL_EXTI_Callback();
	}
	/* ����жϱ�־λ	*/
	EXTI_ClearITPendingBit(HALL_U_EXITLINE); 
}
void EXTI4_IRQHandler(void)		//�����ж�
{
	/* ȷ���Ƿ������EXTI LINE�ж�*/
	if(EXTI_GetITStatus(HALL_V_EXITLINE) != RESET)
	{
	HALL_EXTI_Callback();
	}
	/* ����жϱ�־λ	*/
	EXTI_ClearITPendingBit(HALL_V_EXITLINE); 
}
void EXTI9_5_IRQHandler(void)		//�����ж�
{
	/* ȷ���Ƿ������EXTI LINE�ж�*/
	if(EXTI_GetITStatus(HALL_W_EXITLINE) != RESET)
	{
	HALL_EXTI_Callback();
	}
	/* ����жϱ�־λ	*/
	EXTI_ClearITPendingBit(HALL_V_EXITLINE|HALL_W_EXITLINE); 
}

/**************PID������ʼ��********************************/
void IncPIDInit(void) 
{
  bldc_pid.LastError=0;                    //Error[-1]
  bldc_pid.PrevError=0;                    //Error[-2]
  bldc_pid.Proportion=P_DATA;              //�������� Proportional Const
  bldc_pid.Integral=I_DATA;                //���ֳ���  Integral Const
  bldc_pid.Derivative=D_DATA;              //΢�ֳ��� Derivative Const
  bldc_pid.SetPoint=bldc_dev.motor_speed;  //�趨Ŀ��Desired Value
}
/********************����ʽPID�������************************************/
int IncPIDCalc(int NextPoint) 
{
  int iError,iIncpid;                                       //��ǰ���
  iError=bldc_pid.SetPoint - NextPoint;                     //��������
  iIncpid=(bldc_pid.Proportion * iError)                    //E[k]��
              -(bldc_pid.Integral * bldc_pid.LastError)     //E[k-1]��
              +(bldc_pid.Derivative * bldc_pid.PrevError);  //E[k-2]��
              
  bldc_pid.PrevError=bldc_pid.LastError;                    //�洢�������´μ���
  bldc_pid.LastError=iError;
  return(iIncpid);                                    //��������ֵ
}
/**
  * ��������:	ʹ��TIM2��ʱ���жϻص�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: 1ms���иú���һ��
  */
void Tim2_VCP_Callback(void)
{
  static uint16_t time_count=0;
  
  bldc_dev.stalling_count++;  
  if(bldc_dev.motor_state==RUN)
  {
    time_count++;
    if(bldc_dev.stalling_count>2000) // �����ס��ʱ
      bldc_dev.motor_state=STOP;    
  }
  else
  {
    time_count=0;
  }
  if(time_count>50) // 50ms
  {
    int temp;
    int pid_result;
    // bldc_dev.step_counter��¼������������50msʱ���ڲ���������������������תһȦ�����ܹ�
    // (MOTOR_POLE_PAIR_NUM*6)������,ʹ�� n=bldc_dev.step_counter/(MOTOR_POLE_PAIR_NUM*6) Ϊ
    // 50ms�ڵ��ת��Ȧ����Ϊ����Ϊ��ת/����(rpm)��
    //  n/50 = x/(60*1000) --> x=bldc_dev.step_counter*50
	if(PID_Status == 1)
	{
		temp=bldc_dev.step_counter*50;
		pid_result=IncPIDCalc(temp);     // ��������
		pid_result =pid_result*10/25;    // *10/25Ϊת�ٺ�ռ�ձ�һ��ת����ת�٣�0~2500����ռ�ձȣ�0~1000��
	    if((pid_result+speed_duty)<70)
		  speed_duty =70;
		else if((pid_result+speed_duty)>1000)
		  speed_duty =1000;
		else
		  speed_duty +=pid_result;   
	}
	else
	{
		speed_duty=bldc_dev.motor_speed*10/25;// ����
		if(speed_duty < 70)
		  speed_duty =70;
		else if(speed_duty > 1000)
		  speed_duty =1000;
	}
	
    time_count=0;
    bldc_dev.step_counter=0;
  }
}

unsigned int Current_filter(unsigned int advalue)
{
	static unsigned int valuelist[5];
	static unsigned char index=0;
	unsigned char i;
	unsigned long value=0;
	
	valuelist[index++] = advalue;
	if(index==5) index =0;
	
	value =0;
	for(i=0;i<5;i++)
	{
		value +=valuelist[i];
	}
	return value/5;
}
