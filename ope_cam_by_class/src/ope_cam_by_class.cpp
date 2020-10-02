#include "using_a_cam.hpp"

  /* 根据测试： 
      *     3840*2160     ：几乎不可用
      *     1920*1080    ：3FPS
      *     1280*720       ：10FPS  
      *     1024*768        ：10FPS
      *     800*600           ：20FPS
      */
#define WEIGHT 1024
#define HEIGHT 768

int main(int argc, char *argv[])
{
    cv::Mat combine1,combine2,combine;
    cv::namedWindow("camera",2);

    using_a_cam cam_front("front",WEIGHT,HEIGHT,false);
    using_a_cam cam_back("back",WEIGHT,HEIGHT,false);
    using_a_cam cam_left("left",WEIGHT,HEIGHT,false);
    using_a_cam cam_right("right",WEIGHT,HEIGHT,false); 

    cv::Mat   Label1 = cv::imread("./pic/front.png",1);
    cv::Mat   Label2 = cv::imread("./pic/back.png",1);
    cv::Mat   Label3 = cv::imread("./pic/left.png",1);
    cv::Mat   Label4 = cv::imread("./pic/right.png",1);

    while(1){
      
        cv::Mat pic_front = cam_front.get_a_picture();
        cv::Mat pic_back = cam_back.get_a_picture();
        cv::Mat pic_left = cam_left.get_a_picture();
        cv::Mat pic_right = cam_right.get_a_picture();

        cv::Mat imageROI1 = pic_front(cv::Rect(WEIGHT-100, HEIGHT-70, 100, 70));
        cv::Mat imageROI2 = pic_back(cv::Rect(WEIGHT-100, HEIGHT-70, 100, 70));
        cv::Mat imageROI3 = pic_left(cv::Rect(WEIGHT-100, HEIGHT-70, 100, 70));
        cv::Mat imageROI4 = pic_right(cv::Rect(WEIGHT-100, HEIGHT-70, 100, 70));

        Label1.copyTo(imageROI1);
        Label2.copyTo(imageROI2);
        Label3.copyTo(imageROI3);
        Label4.copyTo(imageROI4);

        hconcat(pic_front,pic_back,combine1);
        hconcat(pic_left,pic_right,combine2);
        vconcat(combine1,combine2,combine);
        cv::imshow("camera", combine);   //在窗口显示
        cv::waitKey(20); 
    }
    return 0;
}