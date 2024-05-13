#ifndef _BASE64_H
#define _BASE64_H

#include "main.h"



typedef struct
{
    char str[100];                      //´æ´¢·Ö¸îµÄ×Ö·û´®
    
    uint8_t DNS;                        //ÓòÃû
    uint8_t Port;                       //¶Ë¿Ú
    uint8_t MountPoint;                 //¹ÒÔØµã
    uint8_t Account[100];                    //ÕËºÅ
    uint8_t Password[100];                   //ÃÜÂë
    
}CORS_Structure;
extern CORS_Structure CORS_Struct;

void ParseCORS(const char* string, int n);
int base64_encode(const unsigned char * sourcedata, char * base64);



#endif
