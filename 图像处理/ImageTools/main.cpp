#include <opencv4/opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
    Mat src_img = imread("/home/xiadengma/图片/init.jpg");

    imshow("src_img", src_img);
    waitKey(0);
    return 0;
}
