#ifndef _WAV_H_
#define _WAV_H_
#include "stm32f4xx.h"
#include "io_bit.h"
#include "ff.h"
#define DMA_Tx_Size    8192		
 
/*
*********************************************************************************************************
    WAVE文件作为多媒体中使用的声波文件格式之一，它是以RIFF格式为标准的。RIFF是英文Resource Interchange 
File Format的缩写，每个WAVE文件的头四个字节便是“RIFF”。
    WAVE文件是由若干个Chunk组成的。按照在文件中的出现位置包括：RIFF WAVE Chunk, Format Chunk, Fact Chunk
(可选), Data Chunk。具体见下图：

		------------------------------------------------
		|             RIFF WAVE Chunk                  |
		|             ID  = 'RIFF'                     |
		|             RiffType = 'WAVE'                |
		------------------------------------------------
		|             Format Chunk                     |
		|             ID = 'fmt '                      |
		------------------------------------------------
		|             Fact Chunk(optional)             |
		|             ID = 'fact'                      |
		------------------------------------------------
		|             Data Chunk                       |
		|             ID = 'data'                      |
		------------------------------------------------
					 Wav格式包含Chunk示例

    其中除了Fact Chunk外，其他三个Chunk是必须的。每个Chunk有各自的ID，位于Chunk最开始位置，作为标示，而且
均为4个字节。并且紧跟在ID后面的是Chunk大小（去除ID和Size所占的字节数后剩下的其他字节数目），4个字节表示，
低字节表示数值低位，高字节表示数值高位。下面具体介绍各个Chunk内容。
*********************************************************************************************************
*/

/*
*********************************************************************************************************
1. RIFF WAVE Chunk
    ==================================
    |       |所占字节数|  具体内容   |
    ==================================
    | ID    |  4 Bytes |   'RIFF'    |
    ----------------------------------
    | Size  |  4 Bytes |             |
    ----------------------------------
    | Type  |  4 Bytes |   'WAVE'    |
    ----------------------------------
	
	以'RIFF'作为标示，然后紧跟着为size字段，该size是整个wav文件大小减去ID和Size所占用的字节数，
即FileLen - 8 = Size。然后是Type字段，为'WAVE'，表示是wav文件。
*********************************************************************************************************
*/
/* 添加__packed关键字表示结构体成员间紧密排列 */
__packed typedef  struct
{
    u32 RiffID;		   	//RiffID;这里固定为"RIFF",即0X46464952
    u32 ChunkSize ;		   	//集合大小;文件总大小-8
    u32 WaveID;	   			//格式;WAVE,即0X45564157
}RIFFChunk ;

/*
*********************************************************************************************************
2. Format Chunk
    ====================================================================
    |               |   字节数  |              具体内容                |
    ====================================================================
    | ID            |  4 Bytes  |   'fmt '                             |
    --------------------------------------------------------------------
    | Size          |  4 Bytes  | 数值为16或18，18则最后又附加信息     |
    --------------------------------------------------------------------  ----
    | AudioFormat   |  2 Bytes  | 编码方式，一般为0x0001               |     |
    --------------------------------------------------------------------     |
    | Channels      |  2 Bytes  | 声道数目，1--单声道；2--双声道       |     |
    --------------------------------------------------------------------     |
    | SampleRate	  |  4 Bytes  | 采样频率                             |     |
    --------------------------------------------------------------------     |
    | ByteRate      |  4 Bytes  | 每秒所需字节数                       |     |===> WAVE_FORMAT
    --------------------------------------------------------------------     |
    | BlockAlign    |  2 Bytes  | 数据块对齐单位(每个采样需要的字节数) |     |
    --------------------------------------------------------------------     |
    | BitsPerSample |  2 Bytes  | 每个采样需要的bit数                  |     |
    --------------------------------------------------------------------     |
    | ByteExtraData |  2 Bytes  | 附加信息（可选，通过Size来判断有无） |     |
    --------------------------------------------------------------------  ----
	
	以'fmt '作为标示。一般情况下Size为16，此时最后附加信息没有；如果为18则最后多了2个字节的附加信息。
主要由一些软件制成的wav格式中含有该2个字节的.
*********************************************************************************************************
*/
__packed typedef  struct
{
  u32 FmtID	;		   			//FmtID ;这里固定为"fmt ",即0X20746D66
  u32 ChunkSize ;		   	//数值为16或18，18则最后又附加信息
  u16 AudioFormat;	  	//音频格式;0X01,表示线性PCM;0X11表示IMA ADPCM
	u16 NumOfChannels;		//通道数量;1,表示单声道;2,表示双声道;
	u32 SampleRate;				//采样率;0X1F40,表示8Khz
	u32 ByteRate;					//每秒所需字节数
	u16 BlockAlign;				//数据块对齐单位(每个采样需要的字节数) 
	u16 BitsPerSample;		//每个采样需要的bit数  
//	u16 ByteExtraData;	//附加信息（可选，通过Size来判断有无）
}FMTChunk;	

/*
*********************************************************************************************************
3. Fact Chunk
    ==================================
    |       |所占字节数|  具体内容   |
    ==================================
    | ID    |  4 Bytes |   'fact'    |
    ----------------------------------
    | Size  |  4 Bytes |   数值为4   |
    ----------------------------------
    | data  |  4 Bytes |             |
    ----------------------------------

	Fact Chunk是可选字段，一般当wav文件由某些软件转化而成，则包含该Chunk。
*********************************************************************************************************
*/
__packed typedef  struct 
{
    u32 FACTID;		   	//FACTID;这里固定为"fact",即0X74636166;
    u32 FACTSize ;		   	//子集合大小(不包括ID和Size);这里为:4.
    u32 NumOfSamples;	  	//采样的数量; 
}FACTChunk;


/*
*********************************************************************************************************
4. Data Chunk
    ==================================
    |       |所占字节数|  具体内容   |
    ==================================
    | ID    |  4 Bytes |   'data'    |
    ----------------------------------
    | Size  |  4 Bytes |             |
    ----------------------------------
    | data  |          |             |
    ----------------------------------

    Data Chunk是真正保存wav数据的地方，以'data'作为该Chunk的标示。然后是数据的大小。紧接着就是wav数据。
根据Format Chunk中的声道数以及采样bit数，wav数据的bit位置可以分成以下几种形式：

    ---------------------------------------------------------------------
    |   单声道  |    取样1    |    取样2    |    取样3    |    取样4    |
    |           |--------------------------------------------------------
    |  8bit量化 |    声道0    |    声道0    |    声道0    |    声道0    |
    ---------------------------------------------------------------------
    |   双声道  |          取样1            |           取样2           |
    |           |--------------------------------------------------------
    |  8bit量化 |  声道0(左)  |  声道1(右)  |  声道0(左)  |  声道1(右)  |
    ---------------------------------------------------------------------
    |           |          取样1            |           取样2           |
    |   单声道  |--------------------------------------------------------
    | 16bit量化 |    声道0    |  声道0      |    声道0    |  声道0      |
    |           | (低位字节)  | (高位字节)  | (低位字节)  | (高位字节)  |
    ---------------------------------------------------------------------
    |           |                         取样1                         |
    |   双声道  |--------------------------------------------------------
    | 16bit量化 |  声道0(左)  |  声道0(左)  |  声道1(右)  |  声道1(右)  |
    |           | (低位字节)  | (高位字节)  | (低位字节)  | (高位字节)  |
    ---------------------------------------------------------------------

*********************************************************************************************************
*/

//LIST块 
__packed typedef  struct 
{
    u32 LISTID;		   	//chunk id;这里固定为"LIST",即0X5453494C
    u32 LISTSize ;		  //子集合大小(不包括ID和Size);这里为:4. 
}LISTChunk;


__packed typedef  struct 
{
    u32 DATAID;		   	//DATAID;这里固定为"data",即0X74636166
    u32 DATASize ;		 //子集合大小(不包括ID和Size) 
}DATAChunk;






//WAV 数据块结构体
typedef  struct
{ 
	RIFFChunk riff;	//riff块
	FMTChunk fmt;  	//fmt块
	FACTChunk fact;	//fact块 线性PCM,没有这个结构体	 
	DATAChunk data;	//data块		 
}WAVChuuk; 

//WAV 文件信息
typedef  struct
{ 
  u16 AudioFormat;			//音频格式;0X01,表示线性PCM;0X11表示IMA ADPCM
	u16 NumOfChannels;		//通道数量;1,表示单声道;2,表示双声道; 
	u16 BlockAlign;				//块对齐(字节);  
	u32 WAVSize;				//WAV数据大小 
  u32 TotTime ;				//整首歌时长,单位:秒
  u32 CurTime ;				//当前播放时长	
  u32 BitRate;	   			//位速率
	u32 SampleRate;				//采样率 
	u16 BitCount;					//位数bps,比如16bit,24bit,32bit
	u32 OffBytes;			 //数据帧开始的位置(在文件里面的偏移)
}WAVINFO; 

// 播放音乐所需缓冲区
typedef  struct
{  
	u8 *dmabuf0;  //dma 存储缓冲区0
	u8 *dmabuf1;  //dma 存储缓冲区1
	u8 *buff;			//在24bit解码的时候需要用到
	FIL *file;		//音频文件指针
}WAVBuf; 

extern WAVBuf wavbuf;	
extern int songTotalCount;
u8 WAV_Decode(u8* fname,WAVINFO* wav);
u32 WAV_Fill_Buf(u8 *buf,u16 size,u8 bits);
void DMA_TX_Over(void) ;
int WAV_Play(u8* fname);
void wavPlay(char *dir);
#endif
