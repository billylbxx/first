#include "bmp.h"

//BMP����
//fname:�ļ�·��+�ļ���
//wav:wav ��Ϣ��Žṹ��ָ��
//����ֵ:0,�ɹ�;1,���ļ�ʧ��
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
//	if(ftemp&&buf)	//�ڴ�����ɹ�
//	{
//		res=f_open(ftemp,(TCHAR*)fname,FA_READ);//���ļ�
//		if(res==FR_OK)
//		{
//			f_read(ftemp,buf,512,&br);	//��ȡ512�ֽ�������
//			riff=(RIFFChunk *)buf;		//��ȡRIFF��
//			if(riff->WaveID==0X45564157)//��WAV�ļ�    "WAVE"
//			{
//				fmt=(FMTChunk *)(buf+12);	//��ȡFMT�� ��ΪRIFF�� ռ����12���ֽڣ�����+12
//				fact=(FACTChunk *)(buf+12+8+fmt->ChunkSize);//��ȡFACT��
//				if(fact->FACTID==0X74636166||fact->FACTID==0X5453494C)wav->OffBytes=12+8+fmt->ChunkSize+8+fact->FACTSize;//����fact/LIST���ʱ��(δ����)
//				else wav->OffBytes=12+8+fmt->ChunkSize;  
//				data=(DATAChunk *)(buf+wav->OffBytes);	//��ȡDATA��
//				if(data->DATAID==0X61746164)//�����ɹ�!
//				{
//					wav->AudioFormat=fmt->AudioFormat;		//��Ƶ��ʽ
//					wav->NumOfChannels=fmt->NumOfChannels;		//ͨ����
//					wav->SampleRate=fmt->SampleRate;		//������
//					wav->BitRate=fmt->ByteRate*8;			//�õ�λ��
//					wav->BlockAlign=fmt->BlockAlign;		//�����
//					wav->BitCount=fmt->BitsPerSample;			//λ��,16/24/32λ				
//					wav->WAVSize=data->DATASize;			//���ݿ��С
//					wav->OffBytes=wav->OffBytes+8;		//��������ʼ�ĵط�. 
//					 
//					printf("��Ƶ��ʽ:%d\r\n",wav->AudioFormat);
//					printf("ͨ������:%d\r\n",wav->NumOfChannels);
//					printf("������  :%d\r\n",wav->SampleRate);
//					printf("λ����  :%d\r\n",wav->BitRate);
//					printf("�ֽڶ���:%d\r\n",wav->BlockAlign);
//					printf("��Ƶλ��:%d\r\n",wav->BitCount);
//					printf("���ݴ�С:%d\r\n",wav->WAVSize);
//					printf("�ֽ�ƫ��:%d\r\n",wav->OffBytes);  
//				}else res=1;
//			}else res=1;
//		}else res=1;
//	}
//	f_close(ftemp); 	//�ر��ļ�
//	MemIn_Free(ftemp);//�ͷ��ڴ�
//	MemIn_Free(buf); 
//	return res;
//}