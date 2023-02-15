/**
 * @file lv_port_disp_templ.c
 *
 */

 /*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp_template.h"
#include "lcd.h"
/*********************
 *      DEFINES
 *********************/
#define MY_DISP_HOR_RES 240
#define MY_DISP_VER_RES 240
/**********************
 *      TYPEDEFS
 **********************/
static lv_disp_drv_t *disp_drv_p;
 static lv_color_t buf_2_1[MY_DISP_HOR_RES * 10];                        /*A buffer for 10 rows*/
 static lv_color_t buf_2_2[MY_DISP_VER_RES * 10];                        /*An other buffer for 10 rows*/
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();
    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/
		static lv_disp_draw_buf_t draw_buf_dsc_2;
    lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, MY_DISP_HOR_RES * 10);   /*Initialize the display buffer*/
    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
     */

//    /* Example for 1) */
//    static lv_disp_draw_buf_t draw_buf_dsc_1;
//    static lv_color_t buf_1[MY_DISP_HOR_RES * 10];                          /*A buffer for 10 rows*/
//    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * 10);   /*Initialize the display buffer*/

    /* Example for 2) */
//    static lv_disp_draw_buf_t draw_buf_dsc_2;
//    static lv_color_t buf_2_1[MY_DISP_HOR_RES * 10];                        /*A buffer for 10 rows*/
//    static lv_color_t buf_2_1[MY_DISP_HOR_RES * 10];                        /*An other buffer for 10 rows*/
//    lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_1, MY_DISP_HOR_RES * 10);   /*Initialize the display buffer*/

//    /* Example for 3) also set disp_drv.full_refresh = 1 below*/
//    static lv_disp_draw_buf_t draw_buf_dsc_3;
//    static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*A screen sized buffer*/
//    static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*An other screen sized buffer*/
//    lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2, MY_DISP_VER_RES * LV_VER_RES_MAX);   /*Initialize the display buffer*/

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    static lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc_2;

    /*Required for Example 3)*/
    //disp_drv.full_refresh = 1

    /* Fill a memory array with a color if you have GPU.
     * Note that, in lv_conf.h you can enable GPUs that has built-in support in LVGL.
     * But if you have a different GPU you can use with this callback.*/
    //disp_drv.gpu_fill_cb = gpu_fill;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    /*You code here*/

	DisPlay_SPI_DMA_Init();
}

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/

//    int32_t x;
//    int32_t y;
//    for(y = area->y1; y <= area->y2; y++) {
//        for(x = area->x1; x <= area->x2; x++) {
//            /*Put a pixel to the display. For example:*/
//            /*put_px(x, y, *color_p)*/
//            color_p++;
//        }
//    }

	
	  unsigned int size = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) * 2;

		disp_drv_p = disp_drv;

		LCD_Address_Set(area->x1, area->y1, area->x2, area->y2+1);	
		LCD_CS = 0;
		DisPlay_SPI_DMA_Enable(color_p, size);
    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
//    lv_disp_flush_ready(disp_drv);
}


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
    DMA_InitStructure.DMA_Memory0BaseAddr 		= (unsigned int)buf_2_1;		//DMA 存储器0地址
    DMA_InitStructure.DMA_DIR 					= DMA_DIR_MemoryToPeripheral;	//存储器到外设模式
    DMA_InitStructure.DMA_BufferSize 			= sizeof(buf_2_2);				//数据传输量
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
/*OPTIONAL: GPU INTERFACE*/

/*If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color*/
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//                    const lv_area_t * fill_area, lv_color_t color)
//{
//    /*It's an example code which should be done by your GPU*/
//    int32_t x, y;
//    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/
//
//    for(y = fill_area->y1; y <= fill_area->y2; y++) {
//        for(x = fill_area->x1; x <= fill_area->x2; x++) {
//            dest_buf[x] = color;
//        }
//        dest_buf+=dest_width;    /*Go to the next line*/
//    }
//}


#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
