#include "base64.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t account[] = "cedr205"; 		//CORS�˺�
uint8_t password[] = "fyx34854"; 	//CORS�˺����� 

const char base64char[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char padding_char = '=';

CORS_Structure CORS_Struct;



////�ü��ַ���
//static char* substring(char* dst, char* src, int start, int len)
//{
//    int length = 100;//��󳤶�
//    if (start >= length || start < 0)
//        return NULL;
//    if (len > length)
//        len = length - start;
//    src += start;
//    while (len--)
//    {
//        *(dst++) = *(src++);
//    }
//    *(dst++) = '\0';
//    return dst;
//}

void ParseCORS(const char* string, int n)
{
    char result[200];
    unsigned char max_length = 200;
    const char* start = string;

    int xor_sum = 0;//У��ֵ
    int i = 0;
    unsigned char len = 0;//�����ַ�������
    char checksum_str[3];

    //���㳤��
    while (string[len] != '\0')len++;
    // �������ݵ�����
    for (i = 1; i < len; i++)
    {
        if (string[i] == '*')
            break;
        xor_sum ^= string[i];
    }
    sprintf(checksum_str, "%02X", xor_sum);
    //if (strncmp(checksum_str, (const char*)&string[i + 1], 2) == 0) //ͨ�� 
    {
        // ��λ����n������
        for (int i = 0; i < n; i++) {
            start = strchr(start, ',');
            if (!start) {
                // ������Ų���n�����������Ϊ���ַ���
                result[0] = '\0';
                return;
            }

            // �ƶ�����һ���ַ�
            start++;
        }

        // ���㶺�ź��ַ����ĳ���
        const char* end = strchr(start, ',');
        size_t length = end ? (size_t)(end - start) : strlen(start);

        // ��ȡ�ַ��������Ƶ����
        if (length < max_length - 1) {
            strncpy(result, start, length);
            // �ֶ���� null ��β
            result[length] = '\0';
        }
        else {
            // Ŀ�곤�Ȳ��㣬�ض��ַ���
            strncpy(result, start, max_length - 1);
            result[max_length - 1] = '\0';
        }

        switch (n)
        {
        case(1):
            CORS_Struct.DNS = result[0];
            break;
        case(2):
            CORS_Struct.Port = result[0];
            break;
        case(3):
            CORS_Struct.MountPoint = atoi(result);
            break;
        case(4):
            sprintf((char*)CORS_Struct.Account, "%s", result);
            break;
        case(5):
            sprintf((char*)CORS_Struct.Password, "%s", result);
            break;
        }
    }
}

/*  BASE64���ܺ��� */

/*�������
* const unsigned char * sourcedata�� Դ����
* char * base64 �����ֱ���
*/
int base64_encode(const unsigned char * sourcedata, char * base64)
{
    int i=0, j=0;
    unsigned char trans_index=0;    // ������8λ�����Ǹ���λ��Ϊ0
    const int datalength = strlen((const char*)sourcedata);
    for (; i < datalength; i += 3){
        // ÿ����һ�飬���б���
        // Ҫ��������ֵĵ�һ��
        trans_index = ((sourcedata[i] >> 2) & 0x3f);
        base64[j++] = base64char[(int)trans_index];
        // �ڶ���
        trans_index = ((sourcedata[i] << 4) & 0x30);
        if (i + 1 < datalength){
            trans_index |= ((sourcedata[i + 1] >> 4) & 0x0f);
            base64[j++] = base64char[(int)trans_index];
        }else{
            base64[j++] = base64char[(int)trans_index];

            base64[j++] = padding_char;

            base64[j++] = padding_char;

            break;   // �����ܳ��ȣ�����ֱ��break
        }
        // ������
        trans_index = ((sourcedata[i + 1] << 2) & 0x3c);
        if (i + 2 < datalength){ // �еĻ���Ҫ����2��
            trans_index |= ((sourcedata[i + 2] >> 6) & 0x03);
            base64[j++] = base64char[(int)trans_index];

            trans_index = sourcedata[i + 2] & 0x3f;
            base64[j++] = base64char[(int)trans_index];
        }
        else{
            base64[j++] = base64char[(int)trans_index];

            base64[j++] = padding_char;

            break;
        }
    }

    base64[j] = '\0'; 
    
    return j;
}



