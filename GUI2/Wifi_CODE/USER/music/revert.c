#include "revert.h"
#include "app_task.h"

__wavctrl wavctrl1;		//WAV控制结构体，解析WAV格式的头部信息
__audiodev audiodev1;	//音乐播放控制器

unsigned char wavwitchbuf1 = 0;		//i2sbufx指示标志
unsigned char wavtransferend1 = 0;		//i2s传输完成标志

/*********************************************************************************************************
 * 函 数 名 : Wav_DecodeInit1
 * 功能说明 : 解析指定wav文件的头部信息
 * 形    参 : fname：文件路径+文件名，wavx：wav头部信息存放结构体指针
 * 返 回 值 : 0：成功，1：打开文件失败，2：非wav文件，3：data区域未找到
 * 备    注 : wav专用，不支持其他类型。注意：为了避免函数重入的问题，直接复制了一份，不想复制处理该问题即可
*********************************************************************************************************/  
unsigned char Wav_DecodeInit1(unsigned char *fname, __wavctrl *wavx)
{
	unsigned char res  = 0;
	unsigned int  br   = 0;
	unsigned char *buf = NULL; 
	FIL *ftemp = NULL;
	ChunkRIFF *riff = NULL;
	ChunkFMT  *fmt  = NULL;
	ChunkFACT *fact = NULL;
	ChunkDATA *data = NULL;
	
	taskENTER_CRITICAL();
	ftemp = (FIL *)mymalloc(SRAMCCM, sizeof(FIL));	//为文件描述符分配内存空间
	buf = mymalloc(SRAMCCM, 512);					//为data数据段分配内存空间
	taskEXIT_CRITICAL();

	if(ftemp && buf)								//申请失败则结束
	{
		res = f_open(ftemp, (TCHAR *)fname, FA_READ);	//只读的形式打开文件，避免误操作
		if(res == FR_OK)
		{
			f_read(ftemp, buf, 512, &br);		//读取512字节在数据
			riff = (ChunkRIFF *)buf;			//获取RIFF块，其数据格式是固定的，所以直接按照结构体内存分布强转即可
			if(riff->Format == 0X45564157)		//如果是WAV文件
			{
				fmt = (ChunkFMT *)(buf + 12);								//获取FMT块 
				fact = (ChunkFACT *)(buf + 12 + 8 + fmt->ChunkSize);		//读取FACT块
				if(fact->ChunkID==0X74636166 || fact->ChunkID==0X5453494C)	//具有fact/LIST块的时候(未测试)
					wavx->datastart = 12 + 8 + fmt->ChunkSize + 8 + fact->ChunkSize;	
				else wavx->datastart = 12 + 8 + fmt->ChunkSize;  
				data = (ChunkDATA *)(buf + wavx->datastart);				//读取DATA块
				if(data->ChunkID == 0X61746164)					//解析成功!
				{
					wavx->audioformat 	= fmt->AudioFormat;		//音频格式
					wavx->nchannels 	= fmt->NumOfChannels;	//通道数
					wavx->samplerate 	= fmt->SampleRate;		//采样率
					wavx->bitrate 		= fmt->ByteRate * 8;	//得到位速
					wavx->blockalign 	= fmt->BlockAlign;		//块对齐
					wavx->bps 			= fmt->BitsPerSample;	//位数,16/24/32位
					wavx->datasize		= data->ChunkSize;		//数据块大小
					wavx->datastart		= wavx->datastart + 8;	//数据流开始的地方. 
				}else res = 3;	//data区域未找到.
			}else res = 2;		//非wav文件
		}else res = 1;			//打开文件错误
	}
	
	taskENTER_CRITICAL();
	f_close(ftemp);				//关闭文件
	myfree(SRAMCCM, ftemp);		//释放内存
	myfree(SRAMCCM, buf); 
	taskEXIT_CRITICAL();
	
	return res;
}
/*********************************************************************************************************
* 函 数 名 : Wav_BuffFill
* 功能说明 : 读取文件数据，填充缓冲区
* 形    参 : buf：缓冲区，size：填充数量
* 返 回 值 : 读取到的数据个数
* 备    注 : 读不够一帧缓冲区时，末尾补0
*********************************************************************************************************/ 
static unsigned int Wav_BuffFill1(unsigned char *buf, unsigned short size)
{
	unsigned int bread = 0;
	unsigned short i = 0;
	
	f_read(audiodev1.file, buf, size, (UINT*)&bread);	
	if(bread < size)									//不够数据了,补充0
		for(i=bread; i<size-bread; i++)
			buf[i] = 0; 
	return bread;
}  
/*********************************************************************************************************
* 函 数 名 : Wav_I2sDmaTx_Callback1
* 功能说明 : dma传输回调，可以理解为dma的中断服务函数
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : dma轮流搬运两个缓冲区的数据，每传完一个缓冲区会触发一次
*********************************************************************************************************/ 
void Wav_I2sDmaTx_Callback1(void) 
{   
	if(DMA1_Stream4->CR & (1<<19))	wavwitchbuf1 = 0;
	else 							wavwitchbuf1 = 1;
	wavtransferend1 = 1;
} 
/*********************************************************************************************************
* 函 数 名 : Wav_PlaySong
* 功能说明 : 播放wav格式的音频文件
* 形    参 : fname：文件路径+文件名
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void Wav_PlayRevert(unsigned char *fname)
{
	unsigned char res = 0;  
	unsigned int fillnum = 0; 
	
	WM8978_SPKvol_Set(0);	
	
	if(status_dev.PlayMode == true)
		vTaskSuspend(PlayMusic_Handler);
	
	taskENTER_CRITICAL();
	audiodev1.file = (FIL*)mymalloc(SRAMIN, sizeof(FIL));			//申请用于存储文件描述符的内存
	audiodev1.i2sbuf1 = mymalloc(SRAMIN, WAV_I2S_TX_DMA_BUFSIZE);	//申请缓存区1的内存，4kb大小
	audiodev1.i2sbuf2 = mymalloc(SRAMIN, WAV_I2S_TX_DMA_BUFSIZE);	//申请缓存区2的内存，4kb大小

	if(audiodev1.file && audiodev1.i2sbuf1 && audiodev1.i2sbuf2)	//判断有没有申请成功，失败一个都会直接结束
	{
		res = Wav_DecodeInit1(fname, &wavctrl1);	//得到文件的信息
		if(res == 0)	//解析文件成功
		{ 
			I2S_Play_Stop();
			DMA1_Stream4->M0AR = (unsigned int)audiodev1.i2sbuf1;
			DMA1_Stream4->M1AR = (unsigned int)audiodev1.i2sbuf2;
			i2s_tx_callback = Wav_I2sDmaTx_Callback1;				//回调函数指wav_i2s_dma_callback
			res = f_open(audiodev1.file, (TCHAR*)fname, FA_READ);	//打开文件
			if(res == 0)
			{
				f_lseek(audiodev1.file, wavctrl1.datastart);		//跳过文件头
				fillnum = Wav_BuffFill1(audiodev1.i2sbuf1, WAV_I2S_TX_DMA_BUFSIZE);
				fillnum = Wav_BuffFill1(audiodev1.i2sbuf2, WAV_I2S_TX_DMA_BUFSIZE);
				I2S_Play_Start();
				WM8978_SPKvol_Set(VOL);
				while(res==0)
				{ 
					while(wavtransferend1 == 0);		//等待wav传输完成; 
					wavtransferend1 = 0;
					if(fillnum != WAV_I2S_TX_DMA_BUFSIZE)	//播放结束?
					{
						res=0;
						break;
					} 
 					if(wavwitchbuf1)	fillnum = Wav_BuffFill1(audiodev1.i2sbuf2, WAV_I2S_TX_DMA_BUFSIZE);	//填充buf2
					else 							fillnum = Wav_BuffFill1(audiodev1.i2sbuf1, WAV_I2S_TX_DMA_BUFSIZE);	//填充buf1
				}
				I2S_Play_Stop();
			}
		}
	}

	f_close(audiodev1.file);
	myfree(SRAMIN, audiodev1.file);		//释放内存 
	myfree(SRAMIN, audiodev1.i2sbuf1);	//释放内存
	myfree(SRAMIN, audiodev1.i2sbuf2);	//释放内存 
	audiodev1.file    = NULL;
	audiodev1.i2sbuf1 = NULL;
	audiodev1.i2sbuf2 = NULL;
	
	if(status_dev.PlayMode == true)
	{
		I2S_Play_Stop();
		DMA1_Stream4->M0AR = (unsigned int)audiodev.i2sbuf1;
		DMA1_Stream4->M1AR = (unsigned int)audiodev.i2sbuf2;
		i2s_tx_callback = Wav_I2sDmaTx_Callback;
		I2S_Play_Start(); 
		vTaskResume(PlayMusic_Handler);
	}
	taskEXIT_CRITICAL();
} 
	

