#include "bmp.h"

//BMP解码
//fname:文件路径+文件名
//wav:wav 信息存放结构体指针
//返回值:0,成功;1,打开文件失败
//u8 WAV_Decode(u8* fname,WAVINFO* wav)
//{
//	FIL*ftemp;
//	u8 *buf; 
//	u32 br=0;
//	u8 res=0;
//	RIFFChunk *riff;
//	FMTChunk *fmt;
//	FACTChunk *fact;
//	DATAChunk *data;
//	ftemp=(FIL*)MemIn_Malloc(sizeof(FIL));
//	buf=MemIn_Malloc(512);
//	if(ftemp&&buf)	//内存申请成功
//	{
//		res=f_open(ftemp,(TCHAR*)fname,FA_READ);//打开文件
//		if(res==FR_OK)
//		{
//			f_read(ftemp,buf,512,&br);	//读取512字节在数据
//			riff=(RIFFChunk *)buf;		//获取RIFF块
//			if(riff->WaveID==0X45564157)//是WAV文件    "WAVE"
//			{
//				fmt=(FMTChunk *)(buf+12);	//获取FMT块 因为RIFF块 占用了12个字节，所以+12
//				fact=(FACTChunk *)(buf+12+8+fmt->ChunkSize);//读取FACT块
//				if(fact->FACTID==0X74636166||fact->FACTID==0X5453494C)wav->OffBytes=12+8+fmt->ChunkSize+8+fact->FACTSize;//具有fact/LIST块的时候(未测试)
//				else wav->OffBytes=12+8+fmt->ChunkSize;  
//				data=(DATAChunk *)(buf+wav->OffBytes);	//读取DATA块
//				if(data->DATAID==0X61746164)//解析成功!
//				{
//					wav->AudioFormat=fmt->AudioFormat;		//音频格式
//					wav->NumOfChannels=fmt->NumOfChannels;		//通道数
//					wav->SampleRate=fmt->SampleRate;		//采样率
//					wav->BitRate=fmt->ByteRate*8;			//得到位速
//					wav->BlockAlign=fmt->BlockAlign;		//块对齐
//					wav->BitCount=fmt->BitsPerSample;			//位数,16/24/32位				
//					wav->WAVSize=data->DATASize;			//数据块大小
//					wav->OffBytes=wav->OffBytes+8;		//数据流开始的地方. 
//					 
//					printf("音频格式:%d\r\n",wav->AudioFormat);
//					printf("通道数量:%d\r\n",wav->NumOfChannels);
//					printf("采样率  :%d\r\n",wav->SampleRate);
//					printf("位速率  :%d\r\n",wav->BitRate);
//					printf("字节对齐:%d\r\n",wav->BlockAlign);
//					printf("音频位数:%d\r\n",wav->BitCount);
//					printf("数据大小:%d\r\n",wav->WAVSize);
//					printf("字节偏移:%d\r\n",wav->OffBytes);  
//				}else res=1;
//			}else res=1;
//		}else res=1;
//	}
//	f_close(ftemp); 	//关闭文件
//	MemIn_Free(ftemp);//释放内存
//	MemIn_Free(buf); 
//	return res;
//}