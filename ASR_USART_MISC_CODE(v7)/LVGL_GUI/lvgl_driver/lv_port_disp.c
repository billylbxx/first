#include "lv_port_disp.h"
#include "lcd.h"
#include "mymalloc.h"

#define MY_DISP_HOR_RES 240
#define MY_DISP_VER_RES 240

static lv_disp_drv_t *disp_drv_p;

static lv_color_t *lv_disp_buf1;
static lv_color_t *lv_disp_buf2;	//�ڴ湻���Կ�������Խ��Խ��
//static lv_color_t lv_disp_buf2[MY_DISP_HOR_RES * 20];	

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
/*********************************************************************************************************
* �� �� �� : DisPlay_SPI_DMA_Init
* ����˵�� : SPI3 DMA1��ʼ��
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : DMA1_Stream7������ʾ���ݵ�SPI3��DR�Ĵ���
*********************************************************************************************************/ 
static void DisPlay_SPI_DMA_Init()
{
    DMA_InitTypeDef  DMA_InitStructure	= {0};
	NVIC_InitTypeDef NVIC_InitStruct	= {0};
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); //DMA1ʱ��ʹ��
    DMA_DeInit(DMA1_Stream7);
    while(DMA_GetCmdStatus(DMA1_Stream7) != DISABLE) {}  //�ȴ�DMA������

    /* ���� DMA Stream */
    DMA_InitStructure.DMA_Channel 				= DMA_Channel_0;  				//ͨ��ѡ��
    DMA_InitStructure.DMA_PeripheralBaseAddr 	= (unsigned int)&SPI3->DR; 		//DMA�����ַ
    DMA_InitStructure.DMA_Memory0BaseAddr 		= (unsigned int)lv_disp_buf1;	//DMA �洢��0��ַ
    DMA_InitStructure.DMA_DIR 					= DMA_DIR_MemoryToPeripheral;	//�洢��������ģʽ
    DMA_InitStructure.DMA_BufferSize 			= sizeof(lv_disp_buf1);			//���ݴ�����
    DMA_InitStructure.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;	//���������ģʽ
    DMA_InitStructure.DMA_MemoryInc 			= DMA_MemoryInc_Enable;			//�洢������ģʽ
    DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;	//�������ݳ���:8λ
    DMA_InitStructure.DMA_MemoryDataSize 		= DMA_MemoryDataSize_Byte;		//�洢�����ݳ���:8λ
    DMA_InitStructure.DMA_Mode 					= DMA_Mode_Normal;				//ʹ����ͨģʽ
    DMA_InitStructure.DMA_Priority 				= DMA_Priority_High;			//�е����ȼ�
    DMA_InitStructure.DMA_FIFOMode 				= DMA_FIFOMode_Disable;			//��ʹ��fifo
    DMA_InitStructure.DMA_FIFOThreshold 		= DMA_FIFOThreshold_Full;		//fifoȫ����
    DMA_InitStructure.DMA_MemoryBurst 			= DMA_MemoryBurst_Single;		//�洢��ͻ�����δ���
    DMA_InitStructure.DMA_PeripheralBurst 		= DMA_PeripheralBurst_Single;	//����ͻ�����δ���
    DMA_Init(DMA1_Stream7, &DMA_InitStructure);									//��ʼ��DMA Stream
        
    SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE); // SPI3ʹ��DMA����
    
	NVIC_InitStruct.NVIC_IRQChannel = DMA1_Stream7_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
    DMA_ITConfig(DMA1_Stream7, DMA_IT_TC, ENABLE);
	
	DMA_Cmd(DMA1_Stream7, DISABLE);
}
/*********************************************************************************************************
* �� �� �� : DisPlay_SPI_DMA_Enable
* ����˵�� : ����DMA������һ�δ���
* ��    �� : buf����Ҫ���˵����ݵ�ָ�룻size�����˵�������
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void DisPlay_SPI_DMA_Enable(void *buf, unsigned int size)
{
	DMA1_Stream7->CR &= ~(0x01);
	while((DMA1_Stream7->CR&0X1)){}
    DMA1_Stream7->M0AR = (unsigned int)buf;
    DMA1_Stream7->NDTR = size;
	DMA1_Stream7->CR |= (0x01);	
}
/*********************************************************************************************************
* �� �� �� : DMA1_Stream7_IRQHandler
* ����˵�� : DMA1_Stream7��������ж�
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void DMA1_Stream7_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_Stream7, DMA_IT_TCIF7) != RESET)
	//if(DMA1->HISR & (1<<27))
    {
		DMA_ClearITPendingBit(DMA1_Stream7, DMA_IT_TCIF7);
		//DMA1->HIFCR |= (1<<27);
		LCD_CS = 1;
		SPI3->DR;	
        lv_disp_flush_ready(disp_drv_p);	/* tell lvgl that flushing is done */
    }
}
/*********************************************************************************************************
* �� �� �� : lv_port_disp_init
* ����˵�� : lvgl��ʾ�ӿڳ�ʼ��
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ����lvgl��ʾ�ӿ�
*********************************************************************************************************/ 
void lv_port_disp_init(void)
{
    DisPlay_SPI_DMA_Init();

    static lv_disp_draw_buf_t draw_buf_dsc_1;
	lv_disp_buf1 = mymalloc(SRAMIN ,MY_DISP_HOR_RES * 20 * 2);
	lv_disp_buf2 = mymalloc(SRAMIN ,MY_DISP_HOR_RES * 20 * 2);
    lv_disp_draw_buf_init(&draw_buf_dsc_1, lv_disp_buf1, lv_disp_buf2, MY_DISP_HOR_RES * 20);  //����Ĵ�СԽ��Խ�ã��ڴ��㹻����ʹ��˫������Ҵ���

    static lv_disp_drv_t disp_drv;                  /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;
    disp_drv.flush_cb = disp_flush;
    disp_drv.draw_buf = &draw_buf_dsc_1;

    lv_disp_drv_register(&disp_drv);
}
/*********************************************************************************************************
* �� �� �� : disp_flush
* ����˵�� : ˢ��ҳ��ӿں���
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    unsigned int size = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) * 2;

	disp_drv_p = disp_drv;
	/*�Ÿ��ʱ���е㻨����Ӧ����dma�����*/
	LCD_Address_Set(area->x1, area->y1, area->x2, area->y2+1);	
	LCD_CS = 0;
	DisPlay_SPI_DMA_Enable(color_p, size);

//	LCD_Color_Fill(area->x1, area->y1, area->x2-area->x1, area->y2-area->y1+1, (unsigned short *)color_p);
//	lv_disp_flush_ready(disp_drv_p);	/* tell lvgl that flushing is done */
}


