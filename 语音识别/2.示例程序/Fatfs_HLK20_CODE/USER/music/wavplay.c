#include "app_task.h"

__wavctrl wavctrl;								//WAV���ƽṹ�壬����WAV��ʽ��ͷ����Ϣ
volatile unsigned char wavtransferend = 0;	//i2s������ɱ�־
volatile unsigned char wavwitchbuf    = 0;	//i2sbufxָʾ��־
/*********************************************************************************************************
 * �� �� �� : WavDecode_Init
 * ����˵�� : ����ָ��wav�ļ���ͷ����Ϣ
 * ��    �� : fname���ļ�·��+�ļ�����wavx��wavͷ����Ϣ��Žṹ��ָ��
 * �� �� ֵ : 0���ɹ���1�����ļ�ʧ�ܣ�2����wav�ļ���3��data����δ�ҵ�
 * ��    ע : wavר�ã���֧����������
*********************************************************************************************************/  
unsigned char Wav_DecodeInit(unsigned char *fname, __wavctrl* wavx)
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
	
	return 0;
}
/*********************************************************************************************************
* �� �� �� : Wav_BuffFill
* ����˵�� : ��ȡ�ļ����ݣ���仺����
* ��    �� : buf����������size���������
* �� �� ֵ : ��ȡ�������ݸ���
* ��    ע : ������һ֡������ʱ��ĩβ��0
*********************************************************************************************************/ 
unsigned int Wav_BuffFill(unsigned char *buf, unsigned short size)
{
	unsigned int bread = 0;
	unsigned short i = 0;

	f_read(audiodev.file, buf, size, (UINT *)&bread);	
	if(bread < size)									//����������,����0
		for(i=bread; i<size-bread; i++)
			buf[i] = 0; 
	return bread;
}  
/*********************************************************************************************************
* �� �� �� : Wav_I2sDmaTx_Callback
* ����˵�� : dma����ص����������Ϊdma���жϷ�����
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : dma�����������������������ݣ�ÿ����һ���������ᴥ��һ��
*********************************************************************************************************/ 
void Wav_I2sDmaTx_Callback(void) 
{   
	if(DMA1_Stream4->CR & 0x80000)		//��ǰʹ�õ��Ǵ洢��һ
	{
		wavwitchbuf = 0;
		if((audiodev.status&0X01) == 0)	//����ͣ����ʱ�������������0
			memset(audiodev.i2sbuf1, 0, WAV_I2S_TX_DMA_BUFSIZE);		
	}
	else 								//��ǰʹ�õ��Ǵ洢����
	{
		wavwitchbuf=1;
		if((audiodev.status&0X01) == 0)
			memset(audiodev.i2sbuf2, 0, WAV_I2S_TX_DMA_BUFSIZE);			
	}
	wavtransferend = 1;					//��Ǵ������
} 
/*********************************************************************************************************
* �� �� �� : Wav_GetCurtime
* ����˵�� : ��ȡ��ǰ����ʱ��
* ��    �� : fx���ļ�ָ�룬wavx��wav���ſ�����
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void Wav_GetCurtime(FIL *fx, __wavctrl *wavx)
{
	long long fpos = 0;  	
 	wavx->totsec = wavx->datasize / (wavx->bitrate/8);		//�����ܳ���(��λ:��) 
	fpos = fx->fptr - wavx->datastart; 						//�õ���ǰ�ļ����ŵ��ĵط� 
	wavx->cursec = fpos * wavx->totsec / wavx->datasize;	//��ǰ���ŵ��ڶ�������?	
}
/*********************************************************************************************************
* �� �� �� : Wav_PlaySong
* ����˵�� : ����wav��ʽ����Ƶ�ļ�
* ��    �� : fname���ļ�·��+�ļ���
* �� �� ֵ : 
* ��    ע : ��
*********************************************************************************************************/ 
u8 Wav_PlaySong(u8* fname)
{
	unsigned char res = 0;  
	unsigned int fillnum = 0; 
	
	taskENTER_CRITICAL();
	audiodev.file = (FIL*)mymalloc(SRAMIN, sizeof(FIL));			//�������ڴ洢�ļ����������ڴ�
	audiodev.i2sbuf1 = mymalloc(SRAMIN, WAV_I2S_TX_DMA_BUFSIZE);	//���뻺����1���ڴ棬4kb��С
	audiodev.i2sbuf2 = mymalloc(SRAMIN, WAV_I2S_TX_DMA_BUFSIZE);	//���뻺����2���ڴ棬4kb��С
	taskEXIT_CRITICAL();
	
	if(audiodev.file && audiodev.i2sbuf1 && audiodev.i2sbuf2)		//�ж���û������ɹ���ʧ��һ������ֱ�ӽ���
	{ 
		res = Wav_DecodeInit(fname, &wavctrl);	//�õ��ļ�����Ϣ
		if(res == 0)	//�����ļ��ɹ�
		{
			Audio_Stop();
			DMA1_Stream4->M0AR = (unsigned int)audiodev.i2sbuf1;	//�󶨻�����һ
			DMA1_Stream4->M1AR = (unsigned int)audiodev.i2sbuf2;	//�󶨻�������
			i2s_tx_callback = Wav_I2sDmaTx_Callback;				//�󶨻ص�����
			
			res = f_open(audiodev.file, (TCHAR *)fname, FA_READ);	//���ļ�
			if(res == 0)
			{
				f_lseek(audiodev.file, wavctrl.datastart);			//�����ļ�ͷ
				fillnum = Wav_BuffFill(audiodev.i2sbuf1, WAV_I2S_TX_DMA_BUFSIZE);
				fillnum = Wav_BuffFill(audiodev.i2sbuf2, WAV_I2S_TX_DMA_BUFSIZE);
				Audio_Start();  
				WM8978_SPKvol_Set(VOL);
				while(res == 0)
				{ 
					while(wavtransferend == 0);				//�ȴ�wav�������; 
					wavtransferend = 0;
					if(fillnum != WAV_I2S_TX_DMA_BUFSIZE)	//���Ž���?
					{
						res = 0;
						break;
					} 
 					if(wavwitchbuf)	fillnum = Wav_BuffFill(audiodev.i2sbuf2, WAV_I2S_TX_DMA_BUFSIZE);//���buf2
					else 			fillnum = Wav_BuffFill(audiodev.i2sbuf1, WAV_I2S_TX_DMA_BUFSIZE);//���buf1
					while(1)
					{
						if(status_dev.PlayState == PLAY_PAUSE)	//��ͣ
						{
							audiodev.status &= ~(1<<0);
							status_dev.PlayState = PLAY_CLEAR;
						}
						if(status_dev.PlayState == PLAY_PLAY)	//����
						{
							WM8978_SPKvol_Set(VOL);
							audiodev.status |= 0X01;
							status_dev.PlayState = PLAY_CLEAR;
						}
						if(status_dev.PlayState==PLAY_PREVIOUS || status_dev.PlayState==PLAY_NEXT)		//��һ��/��һ��
						{
							res = status_dev.PlayState;
							status_dev.PlayState = PLAY_CLEAR;
							break; 
						}
						Wav_GetCurtime(audiodev.file, &wavctrl);			//�õ���ʱ��͵�ǰ���ŵ�ʱ�� 
//						Audio_MsgShow(wavctrl.totsec, wavctrl.cursec);		//��ʾ����Ļ
						if(status_dev.PlayState == PLAY_STOP)	
						{
							res = 0XFF;
							status_dev.PlayState = PLAY_CLEAR;
							break;
						}
						if((audiodev.status & 0X01) == false)	Delay_Ms(10);
						else break;
					}
				}
				Audio_Stop(); 
			}else res=0XFF; 
		}else res=0XFF;
	}else res=0XFF; 
	
	taskENTER_CRITICAL();
	f_close(audiodev.file);
	myfree(SRAMIN, audiodev.file);		//�ͷ��ڴ� 
	myfree(SRAMIN, audiodev.i2sbuf1);	//�ͷ��ڴ�
	myfree(SRAMIN, audiodev.i2sbuf2);	//�ͷ��ڴ� 
	audiodev.file    = NULL;
	audiodev.i2sbuf1 = NULL;
	audiodev.i2sbuf2 = NULL;
	taskEXIT_CRITICAL();
	
	return res;
} 
	









