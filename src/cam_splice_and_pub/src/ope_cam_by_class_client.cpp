#include "using_a_cam.hpp"
#include "SocketMatTransmissionClient.h"

//作为客户端
int main(void)
{
    cv::Mat combine1,combine2,combine;
    //cv::namedWindow("picture_combine_send",2);
    int num=1;

    using_a_cam cam_front("front",V_WIDTH,V_HEIGHT,false);
    using_a_cam cam_back("back",V_WIDTH,V_HEIGHT,false);
    using_a_cam cam_left("left",V_WIDTH,V_HEIGHT,false);
    using_a_cam cam_right("right",V_WIDTH,V_HEIGHT,false); 

    cv::Mat   Label1 = cv::imread("../bin/pic/front.png",1);
    cv::Mat   Label2 = cv::imread("../bin/pic/back.png",1);
    cv::Mat   Label3 = cv::imread("../bin/pic/left.png",1);
    cv::Mat   Label4 = cv::imread("../bin/pic/right.png",1);

    cv::Mat   Label1_mask = cv::imread("../bin/pic/front.png",0);
    cv::Mat   Label2_mask = cv::imread("../bin/pic/back.png",0);
    cv::Mat   Label3_mask = cv::imread("../bin/pic/left.png",0);
    cv::Mat   Label4_mask = cv::imread("../bin/pic/right.png",0);

    SocketMatTransmissionClient socketMat;
	if (socketMat.socketConnect("192.168.0.2", 6666) < 0)           //127.0.0.1  192.168.0.2
		return 0;

    while(1){
      
        cv::Mat pic_front = cam_front.get_a_picture();
        cv::Mat pic_back = cam_back.get_a_picture();
        cv::Mat pic_left = cam_left.get_a_picture();
        cv::Mat pic_right = cam_right.get_a_picture();

        cv::Mat imageROI1 = pic_front(cv::Rect(V_WIDTH-100, V_HEIGHT-70, 100, 70));
        cv::Mat imageROI2 = pic_back(cv::Rect(V_WIDTH-100, V_HEIGHT-70, 100, 70));
        cv::Mat imageROI3 = pic_left(cv::Rect(V_WIDTH-100, V_HEIGHT-70, 100, 70));
        cv::Mat imageROI4 = pic_right(cv::Rect(V_WIDTH-100, V_HEIGHT-70, 100, 70));

        Label1.copyTo(imageROI1,Label1_mask);
        Label2.copyTo(imageROI2,Label2_mask);
        Label3.copyTo(imageROI3,Label3_mask);
        Label4.copyTo(imageROI4,Label4_mask);

        hconcat(pic_front,pic_back,combine1);
        hconcat(pic_left,pic_right,combine2);
        vconcat(combine1,combine2,combine);
        //cv::imshow("picture_combine_send", combine);   //在窗口显示
        //cv::waitKey(30); 

        //std::cout<<"开始传输第"<< num <<"张图片"<<std::endl;
        socketMat.transmit(combine);
        //imwrite("../bin/save_pic/1.jpg", combine);
        cv::waitKey(20); 
        //std::cout<<"已发送"<< num <<"张图片"<<std::endl;
        num++;

    }
    return 0;
}