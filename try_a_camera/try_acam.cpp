#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

int main(int argc, char *argv[])
{
    cv::VideoCapture capture1;
    capture1.open(0);  //打开摄像头，只要一个摄像头，默认是0; 笔记本上有自带的摄像头,再插入usb摄像头应该是1
    if(capture1.isOpened())  //打开摄像头
        std::cout<<"video1 is opened";

    cv::VideoCapture capture2;
    capture2.open(1);  //打开摄像头，只要一个摄像头，默认是0; 笔记本上有自带的摄像头,再插入usb摄像头应该是1
    if(capture2.isOpened())  //打开摄像头
        std::cout<<"video2 is open";

    cv::VideoCapture capture3;
    capture3.open(2);  //打开摄像头，只要一个摄像头，默认是0; 笔记本上有自带的摄像头,再插入usb摄像头应该是1
    if(capture3.isOpened())  //打开摄像头
        std::cout<<"video3 is open";

    cv::VideoCapture capture4;
    capture4.open(3);  //打开摄像头，只要一个摄像头，默认是0; 笔记本上有自带的摄像头,再插入usb摄像头应该是1
    if(capture4.isOpened())  //打开摄像头
        std::cout<<"video4 is open";
    
    cv::Mat frame1,frame2,frame3,frame4; //定义一个矩阵接受帧
    cv::Mat combine1,combine2,combine; //定义一个矩阵接受帧
    cv::namedWindow("cam_combine",0);  //定义显示帧，窗口自由大小 

    while(1){
        capture1>>frame1; //取出一帧
        capture2>>frame2; //取出一帧
        capture3>>frame3; //取出一帧
        capture4>>frame4; //取出一帧

        hconcat(frame1,frame2,combine1);
        hconcat(frame3,frame4,combine2);
        vconcat(combine1,combine2,combine);

        if (!combine.empty())
            cv::imshow("cam_combine", combine);   //在窗口显示
        cv::waitKey(30);
    }
    
    return 0;
}