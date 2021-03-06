/**
 * @file    SerialPort.h
 * @brief   串口通信头文件
 * @author  xiadengma && storms
 * @date    2021.7.24
 * @version 1.2.0.0
 */

#ifndef SERIALPORT_H
#define SERIALPORT_H

///串口相关的头文件
#include "stdint.h"
#include <errno.h>  //错误号定义
#include <fcntl.h>  //文件控制定义
#include <semaphore.h>
#include <stdio.h>   //标准输入输出定义
#include <stdlib.h>  //标准函数库定义
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>  //PPSIX 终端控制定义
#include <unistd.h>   //Unix 标准函数定义

#include <iostream>
using namespace std;

#define UART_DEVICE_0 "/dev/ttyUSB0"
#define UART_DEVICE_1 "/dev/ttyUSB1"

#define BAUDRATE 115200  ///调参：波特率

///波特率相关
const static int speed_arr[] = {
    B921600, B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300, B38400, B19200, B9600, B4800, B2400, B1200, B300,
};
const static int name_arr[] = {
    921600, 115200, 38400, 19200, 9600, 4800, 2400, 1200, 300, 38400, 19200, 9600, 4800, 2400, 1200, 300,
};

/// NAME    :CRC8/MAXIM
/// POLY    :(0x31)->x8 + x5 + x4 + 1
/// WIDTH   :8
/// INIT    :00
/// REFIN   :true
/// REFOUT  :true
/// XOROUT  :00
const uint8_t CRC8_INIT     = 0xff;
const uint8_t CRC8_TAB[256] = {
    0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41, 0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
    0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62, 0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
    0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07, 0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
    0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24, 0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
    0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd, 0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
    0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee, 0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
    0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b, 0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
    0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8, 0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35,
};

typedef union {
    float         f;
    unsigned char c[8];
} float2uchar;

typedef struct
{
    float2uchar yaw;       ///偏航角
    float2uchar pitch;     ///俯仰角
    float2uchar distance;  ///距离 单位：mm
} HostComputerData;

typedef struct
{
    float2uchar   gain_yaw;          ///获取的yaw
    float2uchar   gain_pitch;        ///获取的pitch
    unsigned char mode;              /// 0:正常自瞄 1：击打能量机关 2：吊射模式 3：预测自瞄
    unsigned char color;             /// 0：击打红色 1：击打蓝色
    unsigned char speed;             ///射速: 步兵：15/18  英雄：16  单位：m/s
    float2uchar   lob_offset_yaw;    ///吊射微调yaw
    float2uchar   lob_offset_pitch;  ///吊射微调pitch
} GroundChassisData;

class Uart
{
public:
    Uart();

    int           InitSerial(int& fdcom);
    void          SetSpeed(int fd, int speed);
    int           SetBit(int fd, int databits, int stopbits, int parity);
    bool          SetUart(int fd, int speed, int flow, int databits, int stopbits, int parity);
    void          GetMode(int& fd, GroundChassisData& data);
    void          TransformTarPos(int& fd, const HostComputerData& data);
    unsigned char CRC8Check(unsigned char* addr, int len);
    void          CloseSerial(int fd);

private:
    int           fd;
    int           speed;
    unsigned char rdata[255];
    unsigned char Sdata[255];
};

extern Uart              InfoPort;
extern HostComputerData  SendData;
extern GroundChassisData ReceiveData;

#endif  // SERIALPORT_H
