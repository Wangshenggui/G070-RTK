#include "nmea.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


GPRMC_Structure GPRMC_Struct;
GPRMC_Structure GPRMCH_Struct;
GPGGA_Structure GPGGA_Struct;

//���ڱ���GPRMC���ݣ�ͨ�����ڷ��͵�G431
uint8_t OutGNxxxData[100];


//�ü��ַ���
static char* substring(char* dst, char* src, int start, int len)
{
    int length = 100;//��󳤶�
    if (start >= length || start < 0)
        return NULL;
    if (len > length)
        len = length - start;
    src += start;
    while (len--)
    {
        *(dst++) = *(src++);
    }
    *(dst++) = '\0';
    return dst;
}

//$GNGGA,075410.80,2623.01755888,N,10636.51535335,E,1,13,2.3,1223.1814,M,-29.1520,M,,*6B
//$GNRMC,075410.80,A,2623.01755888,N,10636.51535335,E,0.168,199.3,100424,2.0,W,A,C*5E
void ParseGPRMC(const char* string, int n)
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
    if (strncmp(checksum_str, (const char*)&string[i + 1], 2) == 0) //ͨ�� 
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
        case(1)://ʱ��
            //����ʱ��
            substring(GPRMC_Struct.str, result, 0, 2);
            GPRMC_Struct.Hour = (uint8_t)atoi(GPRMC_Struct.str) + 8;
            substring(GPRMC_Struct.str, result, 2, 2);
            GPRMC_Struct.Min = (uint8_t)atoi(GPRMC_Struct.str);
            substring(GPRMC_Struct.str, result, 4, 2);
            GPRMC_Struct.Sec = (uint8_t)atoi(GPRMC_Struct.str);
            break;
        case(2)://λ��״̬
            GPRMC_Struct.PosStatus = result[0];
            break;
        case(3)://γ��
            substring(GPRMC_Struct.str, result, 0, strlen(result));
            GPRMC_Struct.Latitude = atof(GPRMC_Struct.str);
            break;
        case(4)://N/S
            GPRMC_Struct.LatitudeDir = result[0];
            break;
        case(5)://����
            substring(GPRMC_Struct.str, result, 0, strlen(result));
            GPRMC_Struct.Longitude = atof(GPRMC_Struct.str);
            break;
        case(6)://E/W
            GPRMC_Struct.LongitudeDir = result[0];
            break;
        case(7)://�����ٶ�
            GPRMC_Struct.GroundSpeed = (double)atof(result);
            break;
        case(8)://�����
            GPRMC_Struct.CourseAngle = (double)atof(result);
            break;
        case(9)://����
            //��������
            substring(GPRMC_Struct.str, result, 0, 2);
            GPRMC_Struct.Day = (uint8_t)atoi(GPRMC_Struct.str);
            substring(GPRMC_Struct.str, result, 2, 2);
            GPRMC_Struct.Month = (uint8_t)atoi(GPRMC_Struct.str);
            substring(GPRMC_Struct.str, result, 4, 2);
            GPRMC_Struct.Year = (uint8_t)atoi(GPRMC_Struct.str);
            break;
        case(10)://��ƫ��
            GPRMC_Struct.MagneticDeclination = (double)atof(result);
            break;
        case(11)://��ƫ�Ƿ���
            GPRMC_Struct.DirOfMagneticDeclination = result[0];
            break;
        case(12)://��λģʽ
            GPRMC_Struct.ModeIndicator = result[0];
            break;
        case(13)://ģʽ״̬
            GPRMC_Struct.ModeStatus = result[0];
            break;
        }
    }
}

void ParseGPRMCH(const char* string, int n)
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
    if (strncmp(checksum_str, (const char*)&string[i + 1], 2) == 0) //ͨ�� 
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
        case(1)://ʱ��
            //����ʱ��
            substring(GPRMCH_Struct.str, result, 0, 2);
            GPRMCH_Struct.Hour = (uint8_t)atoi(GPRMCH_Struct.str) + 8;
            substring(GPRMCH_Struct.str, result, 2, 2);
            GPRMCH_Struct.Min = (uint8_t)atoi(GPRMCH_Struct.str);
            substring(GPRMCH_Struct.str, result, 4, 2);
            GPRMCH_Struct.Sec = (uint8_t)atoi(GPRMCH_Struct.str);
            break;
        case(2)://λ��״̬
            GPRMCH_Struct.PosStatus = result[0];
            break;
        case(3)://γ��
            substring(GPRMCH_Struct.str, result, 0, strlen(result));
            GPRMCH_Struct.Latitude = atof(GPRMCH_Struct.str);
            break;
        case(4)://N/S
            GPRMCH_Struct.LatitudeDir = result[0];
            break;
        case(5)://����
            substring(GPRMCH_Struct.str, result, 0, strlen(result));
            GPRMCH_Struct.Longitude = atof(GPRMCH_Struct.str);
            break;
        case(6)://E/W
            GPRMCH_Struct.LongitudeDir = result[0];
            break;
        case(7)://�����ٶ�
            GPRMCH_Struct.GroundSpeed = (double)atof(result);
            break;
        case(8)://�����
            GPRMCH_Struct.CourseAngle = (double)atof(result);
            break;
        case(9)://����
            //��������
            substring(GPRMCH_Struct.str, result, 0, 2);
            GPRMCH_Struct.Day = (uint8_t)atoi(GPRMCH_Struct.str);
            substring(GPRMCH_Struct.str, result, 2, 2);
            GPRMCH_Struct.Month = (uint8_t)atoi(GPRMCH_Struct.str);
            substring(GPRMCH_Struct.str, result, 4, 2);
            GPRMCH_Struct.Year = (uint8_t)atoi(GPRMCH_Struct.str);
            break;
        case(10)://��ƫ��
            GPRMCH_Struct.MagneticDeclination = (double)atof(result);
            break;
        case(11)://��ƫ�Ƿ���
            GPRMCH_Struct.DirOfMagneticDeclination = result[0];
            break;
        case(12)://��λģʽ
            GPRMCH_Struct.ModeIndicator = result[0];
            break;
        case(13)://ģʽ״̬
            GPRMCH_Struct.ModeStatus = result[0];
            break;
        }
    }
}

void ParseGPGGA(const char* string, int n)
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
    if (strncmp(checksum_str, (const char*)&string[i + 1], 2) == 0) //ͨ�� 
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
        case(1)://ʱ��
            //����ʱ��
            substring(GPGGA_Struct.str, result, 0, 2);
            GPGGA_Struct.Hour = (uint8_t)atoi(GPGGA_Struct.str) + 8;
            substring(GPGGA_Struct.str, result, 2, 2);
            GPGGA_Struct.Min = (uint8_t)atoi(GPGGA_Struct.str);
            substring(GPGGA_Struct.str, result, 4, 2);
            GPGGA_Struct.Sec = (uint8_t)atoi(GPGGA_Struct.str);
            break;
        case(2)://γ��
            substring(GPGGA_Struct.str, result, 0, strlen(result));
            GPGGA_Struct.Latitude = atof(GPGGA_Struct.str);
            break;
        case(3)://N/S
            GPGGA_Struct.LatitudeDir = result[0];
            break;
        case(4)://����
            substring(GPGGA_Struct.str, result, 0, strlen(result));
            GPGGA_Struct.Longitude = atof(GPGGA_Struct.str);
            break;
        case(5)://E/W
            GPGGA_Struct.LongitudeDir = result[0];
            break;
        case(6)://��λģʽ
            GPGGA_Struct.ModeIndicator = result[0];
            break;
        case(7)://ʹ�õ���������
            substring(GPGGA_Struct.str, result, 0, strlen(result));
            GPGGA_Struct.SpaceVehicles = (int)atoi(GPGGA_Struct.str);
            break;
        case(8)://ˮƽ��������
            substring(GPGGA_Struct.str, result, 0, strlen(result));
            GPGGA_Struct.HDOP = atof(GPGGA_Struct.str);
            break;
        case(9)://���θ߶�
            substring(GPGGA_Struct.str, result, 0, strlen(result));
            GPGGA_Struct.Altitude = atof(GPGGA_Struct.str);
            break;
        case(11)://������������Դ��ˮ׼��ĸ߶�
            substring(GPGGA_Struct.str, result, 0, strlen(result));
            GPGGA_Struct.HeightAboveEllipsoid = atof(GPGGA_Struct.str);
            break;
        case(14)://��ֻ�վID
            substring(GPGGA_Struct.str, result, 0, strlen(result));
            GPGGA_Struct.DGPS_ID = atoi(GPGGA_Struct.str);
            break;
        }
    }
}

float ParseGPTHS(const char* string)
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
    if (strncmp(checksum_str, (const char*)&string[i + 1], 2) == 0) //ͨ�� 
    {
        start = strchr(start, ',');
        if (!start) {
            // ������Ų���n�����������Ϊ���ַ���
            result[0] = '\0';
            return 0;
        }
        // �ƶ�����һ���ַ�
        start++;

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

        return atof(result);
    }
    return 0;
}

//�����ݸ��Ƶ�OutGNRMCData
void copyRMCData()
{
    //֡ͷ
    OutGNxxxData[0] = 0xAB;
    
    //ʱ������
    OutGNxxxData[1] = GPRMC_Struct.Hour;
    OutGNxxxData[2] = GPRMC_Struct.Min;
    OutGNxxxData[3] = GPRMC_Struct.Sec;
    
    //������Ч״̬
    OutGNxxxData[4] = GPRMC_Struct.PosStatus;
    
    union
    {
        double v;
        unsigned char bit[8];
    }GPS_Speed;
    GPS_Speed.v = GPRMC_Struct.GroundSpeed;
    //�����ٶ�
    OutGNxxxData[5]=GPS_Speed.bit[0];
    OutGNxxxData[6]=GPS_Speed.bit[1];
    OutGNxxxData[7]=GPS_Speed.bit[2];
    OutGNxxxData[8]=GPS_Speed.bit[3];
    OutGNxxxData[9]=GPS_Speed.bit[4];
    OutGNxxxData[10]=GPS_Speed.bit[5];
    OutGNxxxData[11]=GPS_Speed.bit[6];
    OutGNxxxData[12]=GPS_Speed.bit[7];
    
    union
    {
        double v;
        unsigned char bit[8];
    }GPS_CourseAngle;
    GPS_CourseAngle.v=GPRMC_Struct.CourseAngle;
    //�����
    OutGNxxxData[13] = GPS_CourseAngle.bit[0];
    OutGNxxxData[14] = GPS_CourseAngle.bit[1];
    OutGNxxxData[15] = GPS_CourseAngle.bit[2];
    OutGNxxxData[16] = GPS_CourseAngle.bit[3];
    OutGNxxxData[17] = GPS_CourseAngle.bit[4];
    OutGNxxxData[18] = GPS_CourseAngle.bit[5];
    OutGNxxxData[19] = GPS_CourseAngle.bit[6];
    OutGNxxxData[20] = GPS_CourseAngle.bit[7];
    
    //����
    OutGNxxxData[21] = GPRMC_Struct.Year;
    OutGNxxxData[22] = GPRMC_Struct.Month;
    OutGNxxxData[23] = GPRMC_Struct.Day;
    
    //��λģʽ
    OutGNxxxData[24] = GPRMC_Struct.ModeIndicator;
    
    union
    {
        double v;
        unsigned char bit[8];
    }GPS_Longitude;
    //����
    GPS_Longitude.v=GPRMC_Struct.Longitude;
    OutGNxxxData[25] = GPS_Longitude.bit[0];
    OutGNxxxData[26] = GPS_Longitude.bit[1];
    OutGNxxxData[27] = GPS_Longitude.bit[2];
    OutGNxxxData[28] = GPS_Longitude.bit[3];
    OutGNxxxData[29] = GPS_Longitude.bit[4];
    OutGNxxxData[30] = GPS_Longitude.bit[5];
    OutGNxxxData[31] = GPS_Longitude.bit[6];
    OutGNxxxData[32] = GPS_Longitude.bit[7];
    
    union
    {
        double v;
        unsigned char bit[8];
    }GPS_Latitude;
    //γ��
    GPS_Latitude.v=GPRMC_Struct.Latitude;
    OutGNxxxData[33] = GPS_Latitude.bit[0];
    OutGNxxxData[34] = GPS_Latitude.bit[1];
    OutGNxxxData[35] = GPS_Latitude.bit[2];
    OutGNxxxData[36] = GPS_Latitude.bit[3];
    OutGNxxxData[37] = GPS_Latitude.bit[4];
    OutGNxxxData[38] = GPS_Latitude.bit[5];
    OutGNxxxData[39] = GPS_Latitude.bit[6];
    OutGNxxxData[40] = GPS_Latitude.bit[7];
    
    //ʹ�õ���������
    OutGNxxxData[41] = GPGGA_Struct.SpaceVehicles;
    
    union
    {
        double v;
        unsigned char bit[8];
    }GPS_HDOP;
    //ˮƽ��������
    GPS_HDOP.v = GPGGA_Struct.HDOP;
    OutGNxxxData[42] = GPS_HDOP.bit[0];
    OutGNxxxData[43] = GPS_HDOP.bit[1];
    OutGNxxxData[44] = GPS_HDOP.bit[2];
    OutGNxxxData[45] = GPS_HDOP.bit[3];
    OutGNxxxData[46] = GPS_HDOP.bit[4];
    OutGNxxxData[47] = GPS_HDOP.bit[5];
    OutGNxxxData[48] = GPS_HDOP.bit[6];
    OutGNxxxData[49] = GPS_HDOP.bit[7];
    
    union
    {
        double v;
        unsigned char bit[8];
    }GPS_Altitude;
    //���θ߶�
    GPS_Altitude.v = GPGGA_Struct.Altitude;
    OutGNxxxData[50] = GPS_Altitude.bit[0];
    OutGNxxxData[51] = GPS_Altitude.bit[1];
    OutGNxxxData[52] = GPS_Altitude.bit[2];
    OutGNxxxData[53] = GPS_Altitude.bit[3];
    OutGNxxxData[54] = GPS_Altitude.bit[4];
    OutGNxxxData[55] = GPS_Altitude.bit[5];
    OutGNxxxData[56] = GPS_Altitude.bit[6];
    OutGNxxxData[57] = GPS_Altitude.bit[7];
    
    //��ֻ�վID
    OutGNxxxData[58] = GPGGA_Struct.DGPS_ID >> 8;
    OutGNxxxData[59] = GPGGA_Struct.DGPS_ID;
    
    
    GPS_Longitude.v=GPRMCH_Struct.Longitude;
    OutGNxxxData[60] = GPS_Longitude.bit[0];
    OutGNxxxData[61] = GPS_Longitude.bit[1];
    OutGNxxxData[62] = GPS_Longitude.bit[2];
    OutGNxxxData[63] = GPS_Longitude.bit[3];
    OutGNxxxData[64] = GPS_Longitude.bit[4];
    OutGNxxxData[65] = GPS_Longitude.bit[5];
    OutGNxxxData[66] = GPS_Longitude.bit[6];
    OutGNxxxData[67] = GPS_Longitude.bit[7];
    
    GPS_Latitude.v=GPRMCH_Struct.Latitude;
    OutGNxxxData[68] = GPS_Latitude.bit[0];
    OutGNxxxData[69] = GPS_Latitude.bit[1];
    OutGNxxxData[70] = GPS_Latitude.bit[2];
    OutGNxxxData[71] = GPS_Latitude.bit[3];
    OutGNxxxData[72] = GPS_Latitude.bit[4];
    OutGNxxxData[73] = GPS_Latitude.bit[5];
    OutGNxxxData[74] = GPS_Latitude.bit[6];
    OutGNxxxData[75] = GPS_Latitude.bit[7];

    //֡β��
    OutGNxxxData[76] = 0xeb;
    OutGNxxxData[77] = 0x90;
}





