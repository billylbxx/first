#include "revert.h"
#include "app_task.h"

__wavctrl wavctrl1;		//WAV���ƽṹ�壬����WAV��ʽ��ͷ����Ϣ
__audiodev audiodev1;	//���ֲ��ſ�����

unsigned char wavwitchbuf1 = 0;		//i2sbufxָʾ��־
unsigned char wavtransferend1 = 0;		//i2s������ɱ�־

/*********************************************************************************************************
 * �� �� �� : Wav_DecodeInit1
 * ����˵�� : ����ָ��wav�ļ���ͷ����Ϣ
 * ��    �� : fname���ļ�·��+�ļ�����wavx��wavͷ����Ϣ��Žṹ��ָ��
 * �� �� ֵ : 0���ɹ���1�����ļ�ʧ�ܣ�2����wav�ļ���3��data����δ�ҵ�
 * ��    ע : wavר�ã���֧���������͡�ע�⣺Ϊ�˱��⺯����������⣬ֱ�Ӹ�����һ�ݣ����븴�ƴ�������⼴��
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
	ftemp = (FIL *)mymalloc(SRAMCCM, sizeof(FIL));	//Ϊ�ļ������������ڴ�ռ�
	buf = mymalloc(SRAMCCM, 512);					//Ϊdata���ݶη����ڴ�ռ�
	taskEXIT_CRITICAL();

	if(ftemp && buf)								//����ʧ�������
	{
		res = f_open(ftemp, (TCHAR *)fname, FA_READ);	//ֻ������ʽ���ļ������������
		if(res == FR_OK)
		{
			f_read(ftemp, buf, 512, &br);		//��ȡ512�ֽ�������
			riff = (ChunkRIFF *)buf;			//��ȡRIFF�飬�����ݸ�ʽ�ǹ̶��ģ�����ֱ�Ӱ��սṹ���ڴ�ֲ�ǿת����
			if(riff->Format == 0X45564157)		//�����WAV�ļ�
			{
				fmt = (ChunkFMT *)(buf + 12);								//��ȡFMT�� 
				fact = (ChunkFACT *)(buf + 12 + 8 + fmt->ChunkSize);		//��ȡFACT��
				if(fact->ChunkID==0X74636166 || fact->ChunkID==0X5453494C)	//����fact/LIST���ʱ��(δ����)
					wavx->datastart = 12 + 8 + fmt->ChunkSize + 8 + fact->ChunkSize;	
				else wavx->datastart = 12 + 8 + fmt->ChunkSize;  
				data = (ChunkDATA *)(buf + wavx->datastart);				//��ȡDATA��
				if(data->ChunkID == 0X61746164)					//�����ɹ�!
				{
					wavx->audioformat 	= fmt->AudioFormat;		//��Ƶ��ʽ
					wavx->nchannels 	= fmt->NumOfChannels;	//ͨ����
					wavx->samplerate 	= fmt->SampleRate;		//������
					wavx->bitrate 		= fmt->ByteRate * 8;	//�õ�λ��
					wavx->blockalign 	= fmt->BlockAlign;		//�����
					wavx->bps 			= fmt->BitsPerSample;	//λ��,16/24/32λ
					wavx->datasize		= data->ChunkSize;		//���ݿ��С
					wavx->datastart		= wavx->datastart + 8;	//��������ʼ�ĵط�. 
				}else res = 3;	//data����δ�ҵ�.
			}else res = 2;		//��wav�ļ�
		}else res = 1;			//���ļ�����
	}
	
	taskENTER_CRITICAL();
	f_close(ftemp);				//�ر��ļ�
	myfree(SRAMCCM, ftemp);		//�ͷ��ڴ�
	myfree(SRAMCCM, buf); 
	taskEXIT_CRITICAL();
	
	return res;
}
/*********************************************************************************************************
* �� �� �� : Wav_BuffFill
* ����˵�� : ��ȡ�ļ����ݣ���仺����
* ��    �� : buf����������size���������
* �� �� ֵ : ��ȡ�������ݸ���
* ��    ע : ������һ֡������ʱ��ĩβ��0
*********************************************************************************************************/ 
static unsigned int Wav_BuffFill1(unsigned char *buf, unsigned short size)
{
	unsigned int bread = 0;
	unsigned short i = 0;
	
	f_read(audiodev1.file, buf, size, (UINT*)&bread);	
	if(bread < size)									//����������,����0
		for(i=bread; i<size-bread; i++)
			buf[i] = 0; 
	return bread;
}  
/*********************************************************************************************************
* �� �� �� : Wav_I2sDmaTx_Callback1
* ����˵�� : dma����ص����������Ϊdma���жϷ�����
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : dma�����������������������ݣ�ÿ����һ���������ᴥ��һ��
*********************************************************************************************************/ 
void Wav_I2sDmaTx_Callback1(void) 
{   
	if(DMA1_Stream4->CR & (1<<19))	wavwitchbuf1 = 0;
	else 							wavwitchbuf1 = 1;
	wavtransferend1 = 1;
} 
/*********************************************************************************************************
* �� �� �� : Wav_PlaySong
* ����˵�� : ����wav��ʽ����Ƶ�ļ�
* ��    �� : fname���ļ�·��+�ļ���
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void Wav_PlayRevert(unsigned char *fname)
{
	unsigned char res = 0;  
	unsigned int fillnum = 0; 
	
	WM8978_SPKvol_Set(0);	
	
	if(status_dev.PlayMode == true)
		vTaskSuspend(PlayMusic_Handler);
	
	taskENTER_CRITICAL();
	audiodev1.file = (FIL*)mymalloc(SRAMIN, sizeof(FIL));			//�������ڴ洢�ļ����������ڴ�
	audiodev1.i2sbuf1 = mymalloc(SRAMIN, WAV_I2S_TX_DMA_BUFSIZE);	//���뻺����1���ڴ棬4kb��С
	audiodev1.i2sbuf2 = mymalloc(SRAMIN, WAV_I2S_TX_DMA_BUFSIZE);	//���뻺����2���ڴ棬4kb��С

	if(audiodev1.file && audiodev1.i2sbuf1 && audiodev1.i2sbuf2)	//�ж���û������ɹ���ʧ��һ������ֱ�ӽ���
	{
		res = Wav_DecodeInit1(fname, &wavctrl1);	//�õ��ļ�����Ϣ
		if(res == 0)	//�����ļ��ɹ�
		{ 
			I2S_Play_Stop();
			DMA1_Stream4->M0AR = (unsigned int)audiodev1.i2sbuf1;
			DMA1_Stream4->M1AR = (unsigned int)audiodev1.i2sbuf2;
			i2s_tx_callback = Wav_I2sDmaTx_Callback1;				//�ص�����ָwav_i2s_dma_callback
			res = f_open(audiodev1.file, (TCHAR*)fname, FA_READ);	//���ļ�
			if(res == 0)
			{
				f_lseek(audiodev1.file, wavctrl1.datastart);		//�����ļ�ͷ
				fillnum = Wav_BuffFill1(audiodev1.i2sbuf1, WAV_I2S_TX_DMA_BUFSIZE);
				fillnum = Wav_BuffFill1(audiodev1.i2sbuf2, WAV_I2S_TX_DMA_BUFSIZE);
				I2S_Play_Start();
				WM8978_SPKvol_Set(VOL);
				while(res==0)
				{ 
					while(wavtransferend1 == 0);		//�ȴ�wav�������; 
					wavtransferend1 = 0;
					if(fillnum != WAV_I2S_TX_DMA_BUFSIZE)	//���Ž���?
					{
						res=0;
						break;
					} 
 					if(wavwitchbuf1)	fillnum = Wav_BuffFill1(audiodev1.i2sbuf2, WAV_I2S_TX_DMA_BUFSIZE);	//���buf2
					else 							fillnum = Wav_BuffFill1(audiodev1.i2sbuf1, WAV_I2S_TX_DMA_BUFSIZE);	//���buf1
				}
				I2S_Play_Stop();
			}
		}
	}

	f_close(audiodev1.file);
	myfree(SRAMIN, audiodev1.file);		//�ͷ��ڴ� 
	myfree(SRAMIN, audiodev1.i2sbuf1);	//�ͷ��ڴ�
	myfree(SRAMIN, audiodev1.i2sbuf2);	//�ͷ��ڴ� 
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
	

