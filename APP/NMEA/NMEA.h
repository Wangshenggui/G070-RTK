#ifndef _NMEA_H
#define _NMEA_H

#include "rtk.h"
#include "main.h"

typedef struct
{
    char str[100];                      //�洢�ָ���ַ���

    /*ʱ��*/
    uint8_t Hour;
    uint8_t Min;
    uint8_t Sec;

    uint8_t PosStatus;                  //λ��״̬

    double Latitude;                    //γ�ȣ��ȣ�
    uint8_t LatitudeDir;                //γ�ȷ���

    double Longitude;                   //���ȣ��ȣ�
    uint8_t LongitudeDir;               //���ȷ���

    double GroundSpeed;                 //�������ʣ��ڣ�

    double CourseAngle;                 //����ǣ��ȣ�

    /*����*/
    uint8_t Year;
    uint8_t Month;
    uint8_t Day;

    double MagneticDeclination;         //��ƫ�ǣ��ȣ�

    uint8_t DirOfMagneticDeclination;   //��ƫ�Ƿ���

    uint8_t ModeIndicator;              //��λģʽ

    uint8_t ModeStatus;                 //ģʽ״̬
}GPRMC_Structure;

typedef struct
{
    char str[100];                      //�洢�ָ���ַ���

    /*ʱ��*/
    uint8_t Hour;
    uint8_t Min;
    uint8_t Sec;

    double Latitude;                    //γ�ȣ��ȣ�
    uint8_t LatitudeDir;                //γ�ȷ���

    double Longitude;                   //���ȣ��ȣ�
    uint8_t LongitudeDir;               //���ȷ���

    uint8_t ModeIndicator;              //��λģʽ

    uint8_t SpaceVehicles;              //ʹ�õ�������

    double HDOP;                        //ˮƽ��������

    double Altitude;                    //���θ߶ȣ���λ:m��

    double HeightAboveEllipsoid;        //������������Դ��ˮ׼��ĸ߶ȣ���λ:m��

    uint16_t DGPS_ID;                   //��ֻ�վ ID
}GPGGA_Structure;

extern //���ڱ���GPRMC���ݣ�ͨ�����ڷ��͵�G431
uint8_t OutGNxxxData[100];

void ParseGPRMC(const char* string, int n);
void ParseGPRMCH(const char* string, int n);
void ParseGPGGA(const char* string, int n);
float ParseGPTHS(const char* string);
void copyRMCData(void);
void BluetoothCopyRMCData(void);







#endif
