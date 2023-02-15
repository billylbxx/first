#ifndef __JSON_H
#define __JSON_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/*RGB颜色参数*/
struct RGB
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
};

/*JSON键值*/
typedef struct
{
	struct RGB Rgb;	 //数组类型的值读取
	char key[20];	 //键读取
	short value;	 //普通值读取
}JSON;



char JsonString_Parsing(unsigned char *str, JSON *json);
void JsonString_Dispose(JSON *json);


#endif
