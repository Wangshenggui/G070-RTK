#include "base64.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t account[] = "cedr205"; 		//CORS账号
uint8_t password[] = "fyx34854"; 	//CORS账号密码 

const char base64char[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char padding_char = '=';

CORS_Structure CORS_Struct;



////裁剪字符串
//static char* substring(char* dst, char* src, int start, int len)
//{
//    int length = 100;//最大长度
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

    int xor_sum = 0;//校验值
    int i = 0;
    unsigned char len = 0;//计算字符串长度
    char checksum_str[3];

    //计算长度
    while (string[len] != '\0')len++;
    // 计算数据的异或和
    for (i = 1; i < len; i++)
    {
        if (string[i] == '*')
            break;
        xor_sum ^= string[i];
    }
    sprintf(checksum_str, "%02X", xor_sum);
    //if (strncmp(checksum_str, (const char*)&string[i + 1], 2) == 0) //通过 
    {
        // 定位到第n个逗号
        for (int i = 0; i < n; i++) {
            start = strchr(start, ',');
            if (!start) {
                // 如果逗号不够n个，将结果设为空字符串
                result[0] = '\0';
                return;
            }

            // 移动到下一个字符
            start++;
        }

        // 计算逗号后字符串的长度
        const char* end = strchr(start, ',');
        size_t length = end ? (size_t)(end - start) : strlen(start);

        // 截取字符串并复制到结果
        if (length < max_length - 1) {
            strncpy(result, start, length);
            // 手动添加 null 结尾
            result[length] = '\0';
        }
        else {
            // 目标长度不足，截断字符串
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

/*  BASE64加密函数 */

/*编码代码
* const unsigned char * sourcedata， 源数组
* char * base64 ，码字保存
*/
int base64_encode(const unsigned char * sourcedata, char * base64)
{
    int i=0, j=0;
    unsigned char trans_index=0;    // 索引是8位，但是高两位都为0
    const int datalength = strlen((const char*)sourcedata);
    for (; i < datalength; i += 3){
        // 每三个一组，进行编码
        // 要编码的数字的第一个
        trans_index = ((sourcedata[i] >> 2) & 0x3f);
        base64[j++] = base64char[(int)trans_index];
        // 第二个
        trans_index = ((sourcedata[i] << 4) & 0x30);
        if (i + 1 < datalength){
            trans_index |= ((sourcedata[i + 1] >> 4) & 0x0f);
            base64[j++] = base64char[(int)trans_index];
        }else{
            base64[j++] = base64char[(int)trans_index];

            base64[j++] = padding_char;

            base64[j++] = padding_char;

            break;   // 超出总长度，可以直接break
        }
        // 第三个
        trans_index = ((sourcedata[i + 1] << 2) & 0x3c);
        if (i + 2 < datalength){ // 有的话需要编码2个
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



