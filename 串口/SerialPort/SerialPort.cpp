/**
 * @file    SerialPort.cpp
 * @brief   串口通信
 * @author  xiadengma && storms
 * @date    2021.7.28
 * @version 1.8.2.0
 */

#include "SerialPort.h"

Uart::Uart() {}

/**
 * @brief GetMode 接收数据
 * @param fd      打开串口的文件句柄
 * @param data    接收的数据
 * @note    buffer：| 数据1 | 数据2 | 无效数据3 | 数据4 | ---> 我们应该获取缓冲区中最新的数据，即数据4，则从后往前校验
 *          | 0x5a | gain_yaw | gain_pitch | mode | color | speed | lob_offset_yaw | lob_offset_pitch |CRC8 check |
 */
void Uart::GetMode(int& fd, GroundChassisData& data)
{
    int bytes;

    bytes = read(fd, rdata, 255);  //先读取缓冲区中255个数据

    if (bytes == -1)
    {
        cout << "从串口读取数据失败" << endl;
        return;
    }
    else if (bytes == 0)
    {
        cout << "未从串口读取到数据" << endl;
        return;
    }
    else
    {
        cout << "bytes= " << bytes << endl;
    }

    ///在读取到的数据中从后往前依次进行校验
    /// 1. bytes-n   :n为总数据长度
    /// 2. rdata[i+x]:x为有效数据长度+1CRC
    /// 3. CRC(...,y):y为有效数据长度
    for (uint8_t i = bytes - 13; i != 0; i--)
    {
        if (rdata[i] == 0x5a && rdata[i + 12] == CRC8Check(&rdata[i + 1], 11))
        {
            data.gain_yaw.c[0] = rdata[i + 1];
            data.gain_yaw.c[1] = rdata[i + 2];
            data.gain_yaw.c[2] = rdata[i + 3];
            data.gain_yaw.c[3] = rdata[i + 4];

            data.gain_pitch.c[0] = rdata[i + 5];
            data.gain_pitch.c[1] = rdata[i + 6];
            data.gain_pitch.c[2] = rdata[i + 7];
            data.gain_pitch.c[3] = rdata[i + 8];

            data.mode  = rdata[i + 9];
            data.color = rdata[i + 10];
            data.speed = rdata[i + 11];

            return;
        }
    }
}

/**
 * @brief TransformTarPos 传输数据
 * @param fd              打开串口的文件句柄
 * @param data            发送的数据
 * @note | 0xA5 | Pitch | Yaw | Distance | CRC8 check |
 */
void Uart::TransformTarPos(int& fd, const HostComputerData& data)
{
    ///帧头
    Sdata[0] = 0xA5;

    ///有效数据
    Sdata[1]  = data.pitch.c[0];
    Sdata[2]  = data.pitch.c[1];
    Sdata[3]  = data.pitch.c[2];
    Sdata[4]  = data.pitch.c[3];
    Sdata[5]  = data.yaw.c[0];
    Sdata[6]  = data.yaw.c[1];
    Sdata[7]  = data.yaw.c[2];
    Sdata[8]  = data.yaw.c[3];
    Sdata[9]  = data.distance.c[0];
    Sdata[10] = data.distance.c[1];
    Sdata[11] = data.distance.c[2];
    Sdata[12] = data.distance.c[3];

    /// CRC8校验位
    Sdata[13] = CRC8Check(&Sdata[1], 12);

    write(fd, Sdata, 14);
}

/**
 * @brief InitSerial 初始化串口
 * @param fdcom       打开串口的文件句柄
 * @return
 */
int Uart::InitSerial(int& fdcom)
{
    fdcom = open(UART_DEVICE_0, O_RDWR | O_NONBLOCK);  //打开串口文件　读写＆非堵塞

    if (fdcom == -1)
    {
        fdcom = open(UART_DEVICE_1, O_RDWR | O_NONBLOCK);
    }

    if (fdcom == -1)
    {
        cout << "串口打开失败" << endl;
        return -1;
    }
    else
    {
        cout << "串口打开成功->串口数= " << fdcom << endl;
    }

    //设置串口属性
    if (!SetUart(fdcom, BAUDRATE, 0, 8, 1, 'N'))
    {
        return -1;
    }

    return 0;
}

/**
 * @brief CRC8Check 计算crc校验值
 * @param addr      数据首位地址
 * @param len       数据长度
 * @return          计算得出的CRC值
 */
unsigned char Uart::CRC8Check(uint8_t* addr, int len)
{
    unsigned char temp;
    unsigned char crc = CRC8_INIT;  // CRC8初始值

    ///相当于模2除法求余数
    for (; len > 0; len--)
    {
        temp = *addr++;
        crc  = crc ^ temp;  //与CRC初始值异或
        crc  = CRC8_TAB[crc];
    }

    return crc;  //返回最终校验值
}

/**
 * @brief SetUart  设置串口属性
 * @param fd       打开串口的文件句柄
 * @param speed    波特率
 * @param flow     数据流类型
 * @param databits 数据位
 * @param stopbits 停止位
 * @param parity   校验类型
 * @return
 */
bool Uart::SetUart(int fd, int speed, int flow, int databits, int stopbits, int parity)
{
    struct termios options;

    ///获取终端属性
    if (tcgetattr(fd, &options) != 0)
    {
        cout << "获取终端属性失败" << endl;
        return false;
    }

    ///设置波特率
    for (size_t i = 0; i < sizeof(speed_arr) / sizeof(int); i++)
    {
        if (speed == name_arr[i])
        {
            cfsetispeed(&options, speed_arr[i]);  //设定输入波特率
            cfsetospeed(&options, speed_arr[i]);  //设定输出波特率
        }
    }

    ///设置串口本地连接和接收使能<-启用接收且无视串口信号线状态
    options.c_cflag |= (CLOCAL | CREAD);

    ///设置数据流控制
    switch (flow)
    {
    case 0:  //不使用流控制
        options.c_cflag &= ~CRTSCTS;
        break;
    case 1:  //使用硬件流控制
        options.c_cflag |= CRTSCTS;
        break;
    case 2:  //使用软件流控制
        options.c_cflag |= (IXON | IXOFF | IXANY);
        break;
    default:
        cout << "设置数据流控制错误" << endl;
        return false;
    }

    ///设置数据位
    options.c_cflag &= ~CSIZE;  //清除数据位掩码
    switch (databits)
    {
    case 5:  // 5位数据位
        options.c_cflag |= CS5;
        break;
    case 6:  // 6位数据位
        options.c_cflag |= CS6;
        break;
    case 7:  // 7位数据位
        options.c_cflag |= CS7;
        break;
    case 8:  // 8位数据位
        options.c_cflag |= CS8;
        break;
    default:
        cout << "设置数据位错误" << endl;
        return false;
    }

    ///设置停止位
    switch (stopbits)
    {
    case 1:
        options.c_cflag &= ~CSTOPB;
        break;
    case 2:
        options.c_cflag |= CSTOPB;
        break;
    default:
        cout << "设置停止位错误" << endl;
        return false;
    }

    //设置校验位
    switch (parity)
    {
    case 'n':
    case 'N':  //无奇偶校验位
        options.c_cflag &= ~PARENB;
        options.c_iflag &= ~INPCK;
        break;
    case 'o':
    case 'O':  //设置为奇校验
        options.c_cflag |= (PARODD | PARENB);
        options.c_iflag |= INPCK;
        break;
    case 'e':
    case 'E':  //设置为偶校验
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
        options.c_iflag |= INPCK;
        break;
    case 's':
    case 'S':  //设置为空格
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        break;
    default:
        cout << "设置校验位错误" << endl;
        return false;
    }

    options.c_oflag &= ~OPOST;                           //输出标志：设置原始输出
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  //本地标志：选择原始模式

    ///设置等待时间和最小接收字符
    /// VTIME=0 如果有数据可用，则read最多返回所要求的字节数
    /// VMIN =0 如果无数据可用，则read立即返回0
    options.c_cc[VTIME] = 0;
    options.c_cc[VMIN]  = 0;

    ///设置串口属性
    tcflush(fd, TCIFLUSH);  //刷新输入数据
    if (tcsetattr(fd, TCSANOW, &options) != 0)
    {
        cout << "设置串口属性失败" << endl;
        return false;
    }

    return true;
}

/**
 * @brief CloseSerial 关闭串口
 * @param fd          打开串口的文件句柄
 */
void Uart::CloseSerial(int fd)
{
    close(fd);
}
