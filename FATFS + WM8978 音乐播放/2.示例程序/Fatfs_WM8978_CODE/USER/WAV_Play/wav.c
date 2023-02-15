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

WAVINFO wavinfo;			//WAV�ļ���Ϣ
WAVBuf wavbuf;				//���ֲ��Ż����� 
vu8 wav_transfer_flag=0;	//iis������ɱ�־
vu8 wav_dma_buf_flag=0;		//dmabufѡ���־


//��ʼ����
void WAV_Start(void)
{
		DMA1_Stream4->CR|=1<<0;		//��ʼ����,����DMA����  	
} 
//�رղ���
void WAV_Stop(void)
{
	DMA1_Stream4->CR&=~(1<<0);	//��������	,ֹͣDMA����
} 


//WAV����
//fname:�ļ�·��+�ļ���
//wav:wav ��Ϣ��Žṹ��ָ��
//����ֵ:0,�ɹ�;1,���ļ�ʧ��
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
	if(ftemp&&buf)	//�ڴ�����ɹ�
	{
		res=f_open(ftemp,(TCHAR*)fname,FA_READ);//���ļ�
		if(res==FR_OK)
		{
			f_read(ftemp,buf,512,&br);	//��ȡ512�ֽ�������
			riff=(RIFFChunk *)buf;		//��ȡRIFF��
			if(riff->WaveID==0X45564157)//��WAV�ļ�    "WAVE"
			{
				fmt=(FMTChunk *)(buf+12);	//��ȡFMT�� ��ΪRIFF�� ռ����12���ֽڣ�����+12
				fact=(FACTChunk *)(buf+12+8+fmt->ChunkSize);//��ȡFACT��
				if(fact->FACTID==0X74636166||fact->FACTID==0X5453494C)wav->OffBytes=12+8+fmt->ChunkSize+8+fact->FACTSize;//����fact/LIST���ʱ��(δ����)
				else wav->OffBytes=12+8+fmt->ChunkSize;  
				data=(DATAChunk *)(buf+wav->OffBytes);	//��ȡDATA��
				if(data->DATAID==0X61746164)//�����ɹ�!
				{
					wav->AudioFormat=fmt->AudioFormat;		//��Ƶ��ʽ
					wav->NumOfChannels=fmt->NumOfChannels;		//ͨ����
					wav->SampleRate=fmt->SampleRate;		//������
					wav->BitRate=fmt->ByteRate*8;			//�õ�λ��
					wav->BlockAlign=fmt->BlockAlign;		//�����
					wav->BitCount=fmt->BitsPerSample;			//λ��,16/24/32λ				
					wav->WAVSize=data->DATASize;			//���ݿ��С
					wav->OffBytes=wav->OffBytes+8;		//��������ʼ�ĵط�. 
					 
					printf("��Ƶ��ʽ:%d\r\n",wav->AudioFormat);
					printf("ͨ������:%d\r\n",wav->NumOfChannels);
					printf("������  :%d\r\n",wav->SampleRate);
					printf("λ����  :%d\r\n",wav->BitRate);
					printf("�ֽڶ���:%d\r\n",wav->BlockAlign);
					printf("��Ƶλ��:%d\r\n",wav->BitCount);
					printf("���ݴ�С:%d\r\n",wav->WAVSize);
					printf("�ֽ�ƫ��:%d\r\n",wav->OffBytes);  
				}else res=1;
			}else res=1;
		}else res=1;
	}
	f_close(ftemp); 	//�ر��ļ�
	MemIn_Free(ftemp);//�ͷ��ڴ�
	MemIn_Free(buf); 
	return res;
}

//WAV���buf
//buf:������
//size:���������
//bits:λ��(16/24)
//����ֵ:���������ݸ���
u32 WAV_Fill_Buf(u8 *buf,u16 size,u8 bits)
{
	u16 readlen=0;
	u32 bread;
	u16 i;
	u8 *p;
	if(bits==24)//24bit��Ƶ,��Ҫ����һ��
	{
		readlen=(size/4)*3;							//��ȡ�ķ�֮�������ݡ��˴�Ҫ��ȡ���ֽ��� ��ΪSPIһ������ܷ�16��λ��24λ��Ҫ�������Ρ�
		f_read(wavbuf.file,wavbuf.buff,readlen,(UINT*)&bread);	//��ȡ����
		p=wavbuf.buff;
		for(i=0;i<size;) //�Ѷ����������ֽ�ת���ĸ��ֽ� p������ �� �� ��buf ������ �� NC �� ��һ�η�16��λ
		{
			buf[i++]=p[1];
			buf[i]=p[2]; 
			i+=2;
			buf[i++]=p[0];
			p+=3;
		} 
		bread=(bread*4)/3;		//����Ĵ�С.
	}else 
	{
		f_read(wavbuf.file,buf,size,(UINT*)&bread);//16bit��Ƶ,ֱ�Ӷ�ȡ����  
		if(bread<size)//����������,����0
		{
			for(i=bread;i<size-bread;i++)buf[i]=0; 
		}
	}
	return bread;
}  
//WAV����ʱ,DMA������ɱ�־
void DMA_TX_Over(void) 
{   
	if(DMA1_Stream4->CR&(1<<19))
	{
		wav_dma_buf_flag=0;  //����buff1��buff2�ı�־
	}
	else  
	{
		wav_dma_buf_flag=1;
	}
	wav_transfer_flag=1;
} 

/********************************************************************
*  �������� void WAV_Play(u8* fname)
*  ��������: ��������
*  �������:  fname����·���ļ���
*  �������: �� 
*  ����ֵ:   ��     
*  ����:        
*  ����:  
*********************************************************************/
void WAV_Play(u8* fname)
{
	u8 res;  
	u32 fillnum; 			//�����Ŀ
	wavbuf.file=(FIL*)MemIn_Malloc(sizeof(FIL));
	wavbuf.dmabuf0=MemIn_Malloc(DMA_Tx_Size);
	wavbuf.dmabuf1=MemIn_Malloc(DMA_Tx_Size);
	wavbuf.buff		=MemIn_Malloc(DMA_Tx_Size);
	res=WAV_Decode(fname,&wavinfo);					//wav����õ��ļ�����Ϣ
	if(res==0)//�����ļ��ɹ�
	{
		if(wavinfo.BitCount==16)
		{
			WM8978_I2S_Mode(2,0);	//�����ֱ�׼,16λ���ݳ���
			I2S2_Init(0,2,0,1);		//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,16λ��չ֡����
		}else if(wavinfo.BitCount==24)
		{
			WM8978_I2S_Mode(2,2);	//�����ֱ�׼,24λ���ݳ���
			I2S2_Init(0,2,0,2);		//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,24λ��չ֡����
		}
		else if(wavinfo.BitCount==32)
		{
			res=0xff;
			goto end_play_song;
		}
		I2S2_SampleRate_Set(wavinfo.SampleRate);//���ò�����
		I2S2_TX_DMA_Init(wavbuf.dmabuf0,wavbuf.dmabuf1,DMA_Tx_Size/2); //����TX DMA,ֻ���ã���û��ʼ����
		WAV_Stop();
		res=f_open(wavbuf.file,(TCHAR*)fname,FA_READ);	//���ļ�
		if(res==0)
		{
			f_lseek(wavbuf.file, wavinfo.OffBytes);		//�����ļ�ͷ
			fillnum=WAV_Fill_Buf(wavbuf.dmabuf0,DMA_Tx_Size,wavinfo.BitCount);//���wavbuf.dmabuf0
			fillnum=WAV_Fill_Buf(wavbuf.dmabuf1,DMA_Tx_Size,wavinfo.BitCount);//���wavbuf.dmabuf1
			WAV_Start();  
			while(res==0)
			{ 
				if(Scanf_Key(0)==1||Scanf_Key(0)==2||Scanf_Key(0)==3)
				{
					res=3;break;
				}
				while(wav_transfer_flag==0);//�ȴ�wav�������;
				wav_transfer_flag=0;
				if(fillnum!=DMA_Tx_Size)//���Ž���
				{
					res=3;
					break;
				} 
				if(wav_dma_buf_flag)
				{
					fillnum=WAV_Fill_Buf(wavbuf.dmabuf1,DMA_Tx_Size,wavinfo.BitCount);//���buf2
				}
				else
				{					
					fillnum=WAV_Fill_Buf(wavbuf.dmabuf0,DMA_Tx_Size,wavinfo.BitCount);//���buf1
				}
			}
			WAV_Stop(); 
		} 
		f_close(wavbuf.file);
	}
	end_play_song:
	MemIn_Free(wavbuf.buff);	//�ͷ��ڴ�
	MemIn_Free(wavbuf.dmabuf0);//�ͷ��ڴ�
	MemIn_Free(wavbuf.dmabuf1);//�ͷ��ڴ� 
	MemIn_Free(wavbuf.file);	//�ͷ��ڴ� 
} 
	









