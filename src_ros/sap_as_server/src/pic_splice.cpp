#include "ros/ros.h"
#include <iostream>
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <image_transport/image_transport.h>//实现图像的发布和订阅
#include <cv_bridge/cv_bridge.h>//完成opencv图像格式转化为ROS图像格式
#include "splice_and_pub/SocketMatTransmissionServer.h"


cv::Mat pic_front,pic_back,pic_left,pic_right;
int updateflag[4]={0,0,0,0};

void camCallback_front(const sensor_msgs::ImageConstPtr& msg){
    pic_front = cv_bridge::toCvCopy(msg, "bgr8")->image;
    //ROS_INFO("Receive picture from camera_front at Splice function successfully!/n");
    updateflag[2]=1;
}

void camCallback_back(const sensor_msgs::ImageConstPtr& msg){
    pic_back = cv_bridge::toCvCopy(msg, "bgr8")->image;
    //ROS_INFO("Receive picture from camera_back at Splice function successfully!/n");
    updateflag[3]=1;
}

void camCallback_left(const sensor_msgs::ImageConstPtr& msg){
    pic_left = cv_bridge::toCvCopy(msg, "bgr8")->image;
    //ROS_INFO("Receive picture from camera_left at Splice function successfully!/n");
    updateflag[0]=1;
}

void camCallback_right(const sensor_msgs::ImageConstPtr& msg){
    pic_right = cv_bridge::toCvCopy(msg, "bgr8")->image;
    //ROS_INFO("Receive picture from camera_right at Splice function successfully!/n");
    updateflag[1]=1;
}

//作为客户端
int main(int argc, char *argv[])
{
    ros::init(argc, argv, "pic_splice");
    ros::NodeHandle nh;

    double width,height;
    cv::Mat combine1,combine2,combine;
    int num=1,cam_nums,img_show;
    std::string pro_path,save_path;

    //读取参数服务器（图像大小）
    nh.param("/cam_conf/width",width,1280.0);
    nh.param("/cam_conf/height",height,720.0);
    nh.param("/pic_splice/cam_nums",cam_nums,4);
    nh.param("/pic_splice/img_show",img_show,0);
    nh.param<std::string>("/pic_pro_path",pro_path,"/home/zju128");
    nh.param<std::string>("/pic_save_path",save_path,"/home/zju128");
    //std::cout<<"already read params"<<std::endl;

    if(img_show==1){
        cv::namedWindow("picture_combine_send",2);
        std::cout<<"create the window"<<std::endl;
    }

    //创建订阅者
    image_transport::ImageTransport it(nh);//用声明的节点句柄初始化it
    image_transport::Subscriber sub_front = it.subscribe("cam_front", 1, camCallback_front);
    image_transport::Subscriber sub_back = it.subscribe("cam_back", 1, camCallback_back);
    image_transport::Subscriber sub_left = it.subscribe("cam_left", 1, camCallback_left);
    image_transport::Subscriber sub_right = it.subscribe("cam_right", 1, camCallback_right);
    image_transport::Publisher pub = it.advertise("pic_combine", 1,true);
    sensor_msgs::ImagePtr msg;
    //std::cout<<"create subscribers"<<std::endl;

    //用于添加水印
    cv::Mat   Label1 = cv::imread(pro_path+"/front.png",1);
    cv::Mat   Label2 = cv::imread(pro_path+"/back.png",1);
    cv::Mat   Label3 = cv::imread(pro_path+"/left.png",1);
    cv::Mat   Label4 = cv::imread(pro_path+"/right.png",1);
    cv::Mat   Label1_mask = cv::imread(pro_path+"/front.png",0);
    cv::Mat   Label2_mask = cv::imread(pro_path+"/back.png",0);
    cv::Mat   Label3_mask = cv::imread(pro_path+"/left.png",0);
    cv::Mat   Label4_mask = cv::imread(pro_path+"/right.png",0);

    //无画面窗口的图像显示
    cv::Mat   no_picture = cv::imread(pro_path+"/no_picture.jpg",1);
    cv::resize(no_picture,no_picture,Size(width,height));
    pic_front=pic_back=pic_left=pic_right=no_picture;
    std::cout<<"already read white pictures"<<std::endl;

    while(nh.ok()){
        ros::spinOnce();
	    int flagsum=updateflag[0]+updateflag[1]+updateflag[2]+updateflag[3];
	    //std::cout<<updateflag[0]<<" "<<updateflag[1]<<" "<<updateflag[2]<<" "<<updateflag[3]<<std::endl;

        //相机的图片都完成了更新
        if(flagsum == cam_nums){
            cv::Mat imageROI1 = pic_front(cv::Rect(width-100, height-70, 100, 70));
            cv::Mat imageROI2 = pic_back(cv::Rect(width-100, height-70, 100, 70));
            cv::Mat imageROI3 = pic_left(cv::Rect(width-100, height-70, 100, 70));
            cv::Mat imageROI4 = pic_right(cv::Rect(width-100, height-70, 100, 70));
            Label1.copyTo(imageROI1,Label1_mask);
            Label2.copyTo(imageROI2,Label2_mask);
            Label3.copyTo(imageROI3,Label3_mask);
            Label4.copyTo(imageROI4,Label4_mask);
            hconcat(pic_front,pic_back,combine1);
            hconcat(pic_left,pic_right,combine2);
            vconcat(combine1,combine2,combine);
            if(img_show==1){
                cv::imshow("picture_combine_send", combine);   //在窗口显示
                cv::waitKey(25);  
            }

            msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", combine).toImageMsg();
            pub.publish(msg);
            updateflag[0]=updateflag[1]=updateflag[2]=updateflag[3]=0;
        }
    }
    //cv::destroyWindow("picture_combine_send");
    return 1;
}
