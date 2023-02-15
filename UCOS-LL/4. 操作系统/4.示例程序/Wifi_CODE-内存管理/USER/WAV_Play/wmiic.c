#include "wmiic.h"
#include "systick.h"

void WM_IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;             
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9;	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;    //ͨ�����
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;    //��©
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  //�ٶ�����
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL; //���գ�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);            //��ʼ��
	
	WM_IIC_SCL = 1;
	WM_IIC_SDA = 1;
}
/*��ֹ�źţ�SCLΪ�ߵ�ƽ�ڼ䣬SDA�ɸߵ�ƽ��͵�ƽ�����䡣*/
void WM_IIC_Start(void)
{
	WM_IIC_SDA_OUT();	//���ģʽ
	WM_IIC_SCL = 1;	//ʱ����Ϊ��
	WM_IIC_SDA = 1;	//������Ϊ��
	Delay_Us(5);	//���֣�Ϊ�˲������صĶ���
	WM_IIC_SDA = 0;	//SCLΪ�ߵ�ƽ�ڼ䣬����SDA��������ֹ�ź�
	Delay_Us(5);
	WM_IIC_SCL = 0;	//ռ�����ߣ�׼����������
	Delay_Us(5);
}
/*��ֹ�źţ�SCLΪ�ߵ�ƽ�ڼ䣬SDA�ɵ͵�ƽ��ߵ�ƽ�����䡣*/
void WM_IIC_Stop(void)
{
	WM_IIC_SDA_OUT();	//���ģʽ
	WM_IIC_SCL = 0;	//SCLΪ�͵�ƽʱ���ܸı�SDA��״̬
	WM_IIC_SDA = 0;	//����SDA��Ϊ����������׼��
	Delay_Us(5);
	WM_IIC_SCL = 1;	//��SCLΪ�ߵ�ƽ�ڼ�
	WM_IIC_SDA = 1;	//SDA�ɵ��������
	Delay_Us(5);
}
/*Ӧ���źţ�ÿ���ֽڣ�8bit��������ɺ����һ��ʱ���źţ�*/
/*��SCLΪ�ߵ�ƽ�ڼ䣬SDAΪ�ͣ����ʾһ��Ӧ���źţ�ACK����*/
void WM_IIC_ACK(void)
{	
	WM_IIC_SCL = 0;	//SCLΪ�͵�ƽʱ���ܸı�SDA��״̬
	WM_IIC_SDA_OUT();	//���ģʽ
	WM_IIC_SDA = 0;	//����Ӧ���ź�
	Delay_Us(5);
	WM_IIC_SCL = 1;	//����SCL���ӻ���ȡӦ���ź�
	Delay_Us(5);
	WM_IIC_SCL = 0;	//����SCL��׼��������һ������
}
/*Ӧ���źţ�ÿ���ֽڣ�8bit��������ɺ����һ��ʱ���źţ�*/
/*��SCLΪ�ߵ�ƽ�ڼ䣬SDAΪ�ߣ����ʾһ����Ӧ���źţ�NACK����*/
void WM_IIC_NACK(void)
{
	WM_IIC_SCL = 0;	//SCLΪ�͵�ƽʱ���ܸı�SDA��״̬
	WM_IIC_SDA_OUT();	//���ģʽ
	WM_IIC_SDA = 1;	//������Ӧ���ź�
	Delay_Us(5);
	WM_IIC_SCL = 1;	//����SCL���ӻ���ȡӦ���ź�
	Delay_Us(5);
	WM_IIC_SCL = 0;	//����SCL��׼��������һ������
}
/*����0��Ӧ�𣬷���1����Ӧ��*/
unsigned char WM_IIC_Wait_ACK(void)
{
	WM_IIC_SDA_IN();	//����ģʽ
	WM_IIC_SCL = 1;	//����SCL��SDA�����ȶ�
	Delay_Us(5);
	if(WM_IIC_SDAIN)	//���������Ϊ�ߣ���ô��ʾ��Ӧ���ź�
	{
		WM_IIC_Stop();
		return 1;
	}
	WM_IIC_SCL = 0;	//����SCL��׼����һ�����ݵĽ���
	return 0;
}
/*MSB����λ��ǰ��*/
void WM_IIC_WriteByte(unsigned char data)
{
	unsigned char count = 0;
	WM_IIC_SDA_OUT();	//���ģʽ
	for(count=0;count<8;count++)
	{
		WM_IIC_SCL = 0;	//SCLΪ�͵�ƽʱ���ܸı�SDA��״̬
		if(data&0x80)	WM_IIC_SDA = 1;
		else			WM_IIC_SDA = 0;
		data <<= 1;
		Delay_Us(5);
		WM_IIC_SCL = 1;	//������׼���ã�֪ͨ�ӻ���ȡ
		Delay_Us(5);
	}
	WM_IIC_SCL = 0;
	Delay_Us(5);
}
/*��ȡһ���ֽڣ�ack = 1������Ӧ���źţ�ack = 0�����ͷ�Ӧ���ź�*/
unsigned char WM_IIC_ReadByte(unsigned char ack)
{
	unsigned char count = 0, data = 0;
	WM_IIC_SDA_IN();	//����ģʽ
	for(count=0;count<8;count++)
	{
		WM_IIC_SCL = 0;	//֪ͨ�ӻ�׼������
		Delay_Us(5);
		WM_IIC_SCL = 1;	//�ӻ��Ѿ�׼����������
		data <<= 1;		//׼��һ����λ
		data |= !!WM_IIC_SDAIN;	//��ֵ����ȷ�����ֻ��1��0
		Delay_Us(5);
	}
	if(ack)	WM_IIC_ACK();		//�����βξ�������Ӧ��ͷ�Ӧ���ź�
	else	WM_IIC_NACK();	
	return data;
}
