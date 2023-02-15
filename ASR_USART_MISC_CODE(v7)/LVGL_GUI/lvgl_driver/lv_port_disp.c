#include "lv_port_disp.h"
#include "lcd.h"
#include "mymalloc.h"

#define MY_DISP_HOR_RES 240
#define MY_DISP_VER_RES 240

static lv_disp_drv_t *disp_drv_p;

static lv_color_t *lv_disp_buf1;
static lv_color_t *lv_disp_buf2;	//内存够可以开两个，越大越好
//static lv_color_t lv_disp_buf2[MY_DISP_HOR_RES * 20];	

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
/*********************************************************************************************************
* 函 数 名 : DisPlay_SPI_DMA_Init
* 功能说明 : SPI3 DMA1初始化
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : DMA1_Stream7搬运显示数据到SPI3的DR寄存器
*********************************************************************************************************/ 
static void DisPlay_SPI_DMA_Init()
{
    DMA_InitTypeDef  DMA_InitStructure	= {0};
	NVIC_InitTypeDef NVIC_InitStruct	= {0};
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); //DMA1时钟使能
    DMA_DeInit(DMA1_Stream7);
    while(DMA_GetCmdStatus(DMA1_Stream7) != DISABLE) {}  //等待DMA可配置

    /* 配置 DMA Stream */
    DMA_InitStructure.DMA_Channel 				= DMA_Channel_0;  				//通道选择
    DMA_InitStructure.DMA_PeripheralBaseAddr 	= (unsigned int)&SPI3->DR; 		//DMA外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr 		= (unsigned int)lv_disp_buf1;	//DMA 存储器0地址
    DMA_InitStructure.DMA_DIR 					= DMA_DIR_MemoryToPeripheral;	//存储器到外设模式
    DMA_InitStructure.DMA_BufferSize 			= sizeof(lv_disp_buf1);			//数据传输量
    DMA_InitStructure.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;	//外设非增量模式
    DMA_InitStructure.DMA_MemoryInc 			= DMA_MemoryInc_Enable;			//存储器增量模式
    DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;	//外设数据长度:8位
    DMA_InitStructure.DMA_MemoryDataSize 		= DMA_MemoryDataSize_Byte;		//存储器数据长度:8位
    DMA_InitStructure.DMA_Mode 					= DMA_Mode_Normal;				//使用普通模式
    DMA_InitStructure.DMA_Priority 				= DMA_Priority_High;			//中等优先级
    DMA_InitStructure.DMA_FIFOMode 				= DMA_FIFOMode_Disable;			//不使用fifo
    DMA_InitStructure.DMA_FIFOThreshold 		= DMA_FIFOThreshold_Full;		//fifo全容量
    DMA_InitStructure.DMA_MemoryBurst 			= DMA_MemoryBurst_Single;		//存储器突发单次传输
    DMA_InitStructure.DMA_PeripheralBurst 		= DMA_PeripheralBurst_Single;	//外设突发单次传输
    DMA_Init(DMA1_Stream7, &DMA_InitStructure);									//初始化DMA Stream
        
    SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE); // SPI3使能DMA发送
    
	NVIC_InitStruct.NVIC_IRQChannel = DMA1_Stream7_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
    DMA_ITConfig(DMA1_Stream7, DMA_IT_TC, ENABLE);
	
	DMA_Cmd(DMA1_Stream7, DISABLE);
}
/*********************************************************************************************************
* 函 数 名 : DisPlay_SPI_DMA_Enable
* 功能说明 : 配置DMA并启动一次传输
* 形    参 : buf：需要搬运的数据的指针；size：搬运的数据量
* 返 回 值 : 无
* 备    注 : 无
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
* 函 数 名 : DMA1_Stream7_IRQHandler
* 功能说明 : DMA1_Stream7发送完成中断
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 无
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
* 函 数 名 : lv_port_disp_init
* 功能说明 : lvgl显示接口初始化
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 配置lvgl显示接口
*********************************************************************************************************/ 
void lv_port_disp_init(void)
{
    DisPlay_SPI_DMA_Init();

    static lv_disp_draw_buf_t draw_buf_dsc_1;
	lv_disp_buf1 = mymalloc(SRAMIN ,MY_DISP_HOR_RES * 20 * 2);
	lv_disp_buf2 = mymalloc(SRAMIN ,MY_DISP_HOR_RES * 20 * 2);
    lv_disp_draw_buf_init(&draw_buf_dsc_1, lv_disp_buf1, lv_disp_buf2, MY_DISP_HOR_RES * 20);  //缓存的大小越大越好，内存足够可以使用双缓冲兵乓传输

    static lv_disp_drv_t disp_drv;                  /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;
    disp_drv.flush_cb = disp_flush;
    disp_drv.draw_buf = &draw_buf_dsc_1;

    lv_disp_drv_register(&disp_drv);
}
/*********************************************************************************************************
* 函 数 名 : disp_flush
* 功能说明 : 刷新页面接口函数
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    unsigned int size = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) * 2;

	disp_drv_p = disp_drv;
	/*放歌的时候有点花屏，应该是dma引起的*/
	LCD_Address_Set(area->x1, area->y1, area->x2, area->y2+1);	
	LCD_CS = 0;
	DisPlay_SPI_DMA_Enable(color_p, size);

//	LCD_Color_Fill(area->x1, area->y1, area->x2-area->x1, area->y2-area->y1+1, (unsigned short *)color_p);
//	lv_disp_flush_ready(disp_drv_p);	/* tell lvgl that flushing is done */
}


