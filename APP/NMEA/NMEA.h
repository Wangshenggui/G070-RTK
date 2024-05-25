#ifndef _NMEA_H
#define _NMEA_H

#include "rtk.h"
#include "main.h"

typedef struct
{
    char str[100];                      //存储分割的字符串

    /*时间*/
    uint8_t Hour;
    uint8_t Min;
    uint8_t Sec;

    uint8_t PosStatus;                  //位置状态

    double Latitude;                    //纬度（度）
    uint8_t LatitudeDir;                //纬度方向

    double Longitude;                   //经度（度）
    uint8_t LongitudeDir;               //经度方向

    double GroundSpeed;                 //地面速率（节）

    double CourseAngle;                 //航向角（度）

    /*日期*/
    uint8_t Year;
    uint8_t Month;
    uint8_t Day;

    double MagneticDeclination;         //磁偏角（度）

    uint8_t DirOfMagneticDeclination;   //磁偏角方向

    uint8_t ModeIndicator;              //定位模式

    uint8_t ModeStatus;                 //模式状态
}GPRMC_Structure;

typedef struct
{
    char str[100];                      //存储分割的字符串

    /*时间*/
    uint8_t Hour;
    uint8_t Min;
    uint8_t Sec;

    double Latitude;                    //纬度（度）
    uint8_t LatitudeDir;                //纬度方向

    double Longitude;                   //经度（度）
    uint8_t LongitudeDir;               //经度方向

    uint8_t ModeIndicator;              //定位模式

    uint8_t SpaceVehicles;              //使用的卫星数

    double HDOP;                        //水平精度因子

    double Altitude;                    //海拔高度（单位:m）

    double HeightAboveEllipsoid;        //地球椭球面相对大地水准面的高度（单位:m）

    uint16_t DGPS_ID;                   //差分基站 ID
}GPGGA_Structure;

extern //用于保存GPRMC数据，通过串口发送到G431
uint8_t OutGNxxxData[100];

void ParseGPRMC(const char* string, int n);
void ParseGPRMCH(const char* string, int n);
void ParseGPGGA(const char* string, int n);
float ParseGPTHS(const char* string);
void copyRMCData(void);
void BluetoothCopyRMCData(void);







#endif
