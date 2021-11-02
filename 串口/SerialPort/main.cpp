#include "SerialPort.h"
#include <opencv4/opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Uart InfoPort;          ///<串口
int  fd_serial0   = 0;  ///<串口设备
bool serial_state = 0;  ///<串口传输状态量

///信号量名称
sem_t emptysend;
sem_t senddata;
sem_t emptyimage;
sem_t image;

HostComputerData  SendData;     ///<传输给下位机的数据
GroundChassisData ReceiveData;  ///<接受下位机的数据

int main()
{
    serial_state = InfoPort.InitSerial(fd_serial0) + 1;  //初始化串口

    while (1)
    {
        //        if (1)  //调试使用
        if (serial_state)
        {
            ///数据传输信号量 等待处理完毕 开始传输
            sem_wait(&senddata);

            ///串口接收数据
            InfoPort.GetMode(fd_serial0, ReceiveData);

            ///传输数据给下位机
            InfoPort.TransformTarPos(fd_serial0, SendData);

            ///数据传输信号量 传输完毕
            sem_post(&emptysend);
        }
    }

    return 0;
}
