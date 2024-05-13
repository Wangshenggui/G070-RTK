#ifndef _BASE64_H
#define _BASE64_H

#include "main.h"



typedef struct
{
    char str[100];                      //�洢�ָ���ַ���
    
    uint8_t DNS;                        //����
    uint8_t Port;                       //�˿�
    uint8_t MountPoint;                 //���ص�
    uint8_t Account[100];                    //�˺�
    uint8_t Password[100];                   //����
    
}CORS_Structure;
extern CORS_Structure CORS_Struct;

void ParseCORS(const char* string, int n);
int base64_encode(const unsigned char * sourcedata, char * base64);



#endif
