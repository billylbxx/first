#ifndef __JSON_H
#define __JSON_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/*RGB��ɫ����*/
struct RGB
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
};

/*JSON��ֵ*/
typedef struct
{
	struct RGB Rgb;	 //�������͵�ֵ��ȡ
	char key[20];	 //����ȡ
	short value;	 //��ֵͨ��ȡ
}JSON;



char JsonString_Parsing(unsigned char *str, JSON *json);
void JsonString_Dispose(JSON *json);


#endif
