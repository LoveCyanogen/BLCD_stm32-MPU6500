#include "config.h"
s_mpu6500 mpu6500_info;


void SPI2Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure; 
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //����ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);  
	
	//Ƭѡ�ܽ�,���������
	SPI_NSS_HIGH;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//spi�ܽ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2); //�����Ÿ��ù���
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2);
	
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //ȫ˫��ģʽ
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;   //��Ϊ����ʹ��
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;   //���ݳ���8
    SPI_InitStructure.SPI_CPOL  = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;   //�������NSS����
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2,&SPI_InitStructure);
    SPI_Cmd(SPI2,ENABLE);
}

s16 SpiReadWrite16Bits(s16 TxData)
{
	while( (SPI2->SR&0x02) == 0);//�ȴ�ֱ�����ͼĴ�����
	SPI2->DR = TxData;
	while((SPI2->SR&0x01) == 0);//�ȴ��յ�����
	return SPI2->DR;
}

//��̬������MPU6500��ʼ������
void mpu6500_init(void)
{
	delay_ms(500);
	SPI2Init();
	
	SPI_NSS_LOW;
	SpiReadWrite16Bits(106<<8|0x10);
	SPI_NSS_HIGH;
	delay_ms(1);
	
	SPI_NSS_LOW;
	SpiReadWrite16Bits(0x6b<<8|0x00);		//���ַx6bд��0x00  //ʹ���¶ȴ�����,ʹ��PLL
	SPI_NSS_HIGH;
	delay_ms(1);
	
	SPI_NSS_LOW;
	SpiReadWrite16Bits(0x19<<8|0x00);		//���ݲ���������Ϊ	1KHZ
	SPI_NSS_HIGH;
	delay_ms(1);
	
	SPI_NSS_LOW;
	SpiReadWrite16Bits(0x1a<<8|0x03);	
	SPI_NSS_HIGH;
	delay_ms(1);
	
	SPI_NSS_LOW;
	SpiReadWrite16Bits(0x1b<<8|0x18);		//���ٶ�������ֵ  +-2000/s
	SPI_NSS_HIGH;
	delay_ms(1);
	
	SPI_NSS_LOW;
	SpiReadWrite16Bits(0x1c<<8|0x10);		//���ٶ�������ֵ 	+-8g
	SPI_NSS_HIGH;
	delay_ms(1);
}
