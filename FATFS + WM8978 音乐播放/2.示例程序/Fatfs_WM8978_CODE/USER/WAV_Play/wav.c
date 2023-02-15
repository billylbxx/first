#include "WAV.h"
#include "ff.h"
#include "lcd.h"
#include "WAV.h" 
#include "usart1.h" 
#include "systick.h" 
#include "malloc.h"
#include "ff.h"
#include "i2s.h"
#include "wm8978.h"
#include "key.h"
#include "led.h"
#include "stdlib.h"

WAVINFO wavinfo;			//WAV文件信息
WAVBuf wavbuf;				//音乐播放缓冲区 
vu8 wav_transfer_flag=0;	//iis传输完成标志
vu8 wav_dma_buf_flag=0;		//dmabuf选择标志


//开始播放
void WAV_Start(void)
{
		DMA1_Stream4->CR|=1<<0;		//开始播放,开启DMA传输  	
} 
//关闭播放
void WAV_Stop(void)
{
	DMA1_Stream4->CR&=~(1<<0);	//结束播放	,停止DMA传输
} 


//WAV解码
//fname:文件路径+文件名
//wav:wav 信息存放结构体指针
//返回值:0,成功;1,打开文件失败
u8 WAV_Decode(u8* fname,WAVINFO* wav)
{
	FIL*ftemp;
	u8 *buf; 
	u32 br=0;
	u8 res=0;
	RIFFChunk *riff;
	FMTChunk *fmt;
	FACTChunk *fact;
	DATAChunk *data;
	ftemp=(FIL*)MemIn_Malloc(sizeof(FIL));
	buf=MemIn_Malloc(512);
	if(ftemp&&buf)	//内存申请成功
	{
		res=f_open(ftemp,(TCHAR*)fname,FA_READ);//打开文件
		if(res==FR_OK)
		{
			f_read(ftemp,buf,512,&br);	//读取512字节在数据
			riff=(RIFFChunk *)buf;		//获取RIFF块
			if(riff->WaveID==0X45564157)//是WAV文件    "WAVE"
			{
				fmt=(FMTChunk *)(buf+12);	//获取FMT块 因为RIFF块 占用了12个字节，所以+12
				fact=(FACTChunk *)(buf+12+8+fmt->ChunkSize);//读取FACT块
				if(fact->FACTID==0X74636166||fact->FACTID==0X5453494C)wav->OffBytes=12+8+fmt->ChunkSize+8+fact->FACTSize;//具有fact/LIST块的时候(未测试)
				else wav->OffBytes=12+8+fmt->ChunkSize;  
				data=(DATAChunk *)(buf+wav->OffBytes);	//读取DATA块
				if(data->DATAID==0X61746164)//解析成功!
				{
					wav->AudioFormat=fmt->AudioFormat;		//音频格式
					wav->NumOfChannels=fmt->NumOfChannels;		//通道数
					wav->SampleRate=fmt->SampleRate;		//采样率
					wav->BitRate=fmt->ByteRate*8;			//得到位速
					wav->BlockAlign=fmt->BlockAlign;		//块对齐
					wav->BitCount=fmt->BitsPerSample;			//位数,16/24/32位				
					wav->WAVSize=data->DATASize;			//数据块大小
					wav->OffBytes=wav->OffBytes+8;		//数据流开始的地方. 
					 
					printf("音频格式:%d\r\n",wav->AudioFormat);
					printf("通道数量:%d\r\n",wav->NumOfChannels);
					printf("采样率  :%d\r\n",wav->SampleRate);
					printf("位速率  :%d\r\n",wav->BitRate);
					printf("字节对齐:%d\r\n",wav->BlockAlign);
					printf("音频位数:%d\r\n",wav->BitCount);
					printf("数据大小:%d\r\n",wav->WAVSize);
					printf("字节偏移:%d\r\n",wav->OffBytes);  
				}else res=1;
			}else res=1;
		}else res=1;
	}
	f_close(ftemp); 	//关闭文件
	MemIn_Free(ftemp);//释放内存
	MemIn_Free(buf); 
	return res;
}

//WAV填充buf
//buf:数据区
//size:填充数据量
//bits:位数(16/24)
//返回值:读到的数据个数
u32 WAV_Fill_Buf(u8 *buf,u16 size,u8 bits)
{
	u16 readlen=0;
	u32 bread;
	u16 i;
	u8 *p;
	if(bits==24)//24bit音频,需要处理一下
	{
		readlen=(size/4)*3;							//读取四分之三的数据。此次要读取的字节数 因为SPI一次最多能发16个位。24位需要发送两次。
		f_read(wavbuf.file,wavbuf.buff,readlen,(UINT*)&bread);	//读取数据
		p=wavbuf.buff;
		for(i=0;i<size;) //把读到的三个字节转成四个字节 p－〉低 中 高 ；buf －〉中 高 NC 低 。一次发16个位
		{
			buf[i++]=p[1];
			buf[i]=p[2]; 
			i+=2;
			buf[i++]=p[0];
			p+=3;
		} 
		bread=(bread*4)/3;		//填充后的大小.
	}else 
	{
		f_read(wavbuf.file,buf,size,(UINT*)&bread);//16bit音频,直接读取数据  
		if(bread<size)//不够数据了,补充0
		{
			for(i=bread;i<size-bread;i++)buf[i]=0; 
		}
	}
	return bread;
}  
//WAV播放时,DMA传输完成标志
void DMA_TX_Over(void) 
{   
	if(DMA1_Stream4->CR&(1<<19))
	{
		wav_dma_buf_flag=0;  //传输buff1和buff2的标志
	}
	else  
	{
		wav_dma_buf_flag=1;
	}
	wav_transfer_flag=1;
} 

/********************************************************************
*  函数名： void WAV_Play(u8* fname)
*  功能描述: 播放音乐
*  输入参数:  fname：带路径文件名
*  输出参数: 无 
*  返回值:   无     
*  其他:        
*  作者:  
*********************************************************************/
void WAV_Play(u8* fname)
{
	u8 res;  
	u32 fillnum; 			//填充数目
	wavbuf.file=(FIL*)MemIn_Malloc(sizeof(FIL));
	wavbuf.dmabuf0=MemIn_Malloc(DMA_Tx_Size);
	wavbuf.dmabuf1=MemIn_Malloc(DMA_Tx_Size);
	wavbuf.buff		=MemIn_Malloc(DMA_Tx_Size);
	res=WAV_Decode(fname,&wavinfo);					//wav解码得到文件的信息
	if(res==0)//解析文件成功
	{
		if(wavinfo.BitCount==16)
		{
			WM8978_I2S_Mode(2,0);	//飞利浦标准,16位数据长度
			I2S2_Init(0,2,0,1);		//飞利浦标准,主机发送,时钟低电平有效,16位扩展帧长度
		}else if(wavinfo.BitCount==24)
		{
			WM8978_I2S_Mode(2,2);	//飞利浦标准,24位数据长度
			I2S2_Init(0,2,0,2);		//飞利浦标准,主机发送,时钟低电平有效,24位扩展帧长度
		}
		else if(wavinfo.BitCount==32)
		{
			res=0xff;
			goto end_play_song;
		}
		I2S2_SampleRate_Set(wavinfo.SampleRate);//设置采样率
		I2S2_TX_DMA_Init(wavbuf.dmabuf0,wavbuf.dmabuf1,DMA_Tx_Size/2); //配置TX DMA,只配置，还没开始传输
		WAV_Stop();
		res=f_open(wavbuf.file,(TCHAR*)fname,FA_READ);	//打开文件
		if(res==0)
		{
			f_lseek(wavbuf.file, wavinfo.OffBytes);		//跳过文件头
			fillnum=WAV_Fill_Buf(wavbuf.dmabuf0,DMA_Tx_Size,wavinfo.BitCount);//填充wavbuf.dmabuf0
			fillnum=WAV_Fill_Buf(wavbuf.dmabuf1,DMA_Tx_Size,wavinfo.BitCount);//填充wavbuf.dmabuf1
			WAV_Start();  
			while(res==0)
			{ 
				if(Scanf_Key(0)==1||Scanf_Key(0)==2||Scanf_Key(0)==3)
				{
					res=3;break;
				}
				while(wav_transfer_flag==0);//等待wav传输完成;
				wav_transfer_flag=0;
				if(fillnum!=DMA_Tx_Size)//播放结束
				{
					res=3;
					break;
				} 
				if(wav_dma_buf_flag)
				{
					fillnum=WAV_Fill_Buf(wavbuf.dmabuf1,DMA_Tx_Size,wavinfo.BitCount);//填充buf2
				}
				else
				{					
					fillnum=WAV_Fill_Buf(wavbuf.dmabuf0,DMA_Tx_Size,wavinfo.BitCount);//填充buf1
				}
			}
			WAV_Stop(); 
		} 
		f_close(wavbuf.file);
	}
	end_play_song:
	MemIn_Free(wavbuf.buff);	//释放内存
	MemIn_Free(wavbuf.dmabuf0);//释放内存
	MemIn_Free(wavbuf.dmabuf1);//释放内存 
	MemIn_Free(wavbuf.file);	//释放内存 
} 
	









