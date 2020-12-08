#include <vector>
#include <boost/thread.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>
#include "splice_and_pub/SocketMatTransmissionServer.h"
#define PORT 6666

boost::shared_mutex flag_lock;                      //读写锁
cv::Mat pic_combine;                                //订阅得到的图像
int rec_dateflag,send_dateflag,thread_num=0;        //是否已经更新图像数据，有多少个线程完成了新数据的发送，工作线程的个数

image_transport::ImageTransport *it;                //图像订阅与发布手柄
std::vector<image_transport::Subscriber> pic_sub;   //订阅者列表，目前还没啥作用
SocketMatTransmissionServer socketMat;              //用于网络通讯

void camCallback(const sensor_msgs::ImageConstPtr& msg){
        pic_combine = cv_bridge::toCvCopy(msg, "bgr8")->image;  //toCvShare  toCvCopy
        //ROS_INFO_STREAM("Thread 00"<<": 04 Receive picture from Splice node successfully!.");
        rec_dateflag=1;
        send_dateflag=0;
}

void workfunc(int sockCli){
    int num=1;
    ros::NodeHandle nh;

    while(nh.ok()){
        waitKey(5);
        //ROS_INFO_STREAM("Thread "<<sockCli<<": 01 trying to read lock flag_lock.");
        //ROS_INFO_STREAM("Thread "<<sockCli<<": 02 send_dateflag="<<send_dateflag<<" thread_num="<<thread_num);
        flag_lock.lock();
        if(send_dateflag == thread_num){
            rec_dateflag = 0;
	    //ROS_INFO_STREAM("Thread "<<sockCli<<": 03 waiting for the spin.");
            ros::spinOnce();
        }
        if(rec_dateflag == 1){
            //ROS_INFO_STREAM("Thread "<<sockCli<<": 05 begin to trans the "<< num <<" picture.");
            int ret=socketMat.transmit(sockCli,pic_combine);
            if(ret == -1){
                //ROS_INFO_STREAM("ERROR, the thread will be killed.");
                break;
            }
            else{
                //imwrite(save_path+"/combine.jpg", combine);
                ROS_INFO_STREAM("Thread "<<sockCli<<": 06 has trans "<<num<<" pictures.");
                num++;
            }
            send_dateflag++;
            flag_lock.unlock();
            //ROS_INFO_STREAM("Thread "<<sockCli<<": 07 flag_lock is read released. Send_dateflag="<<send_dateflag<<" thread_num="<<thread_num);
        }
        else{
	    flag_lock.unlock();
            //ROS_INFO_STREAM("Thread "<<sockCli<<": 07 flag_lock is read released. Send_dateflag="<<send_dateflag<<" thread_num="<<thread_num);
            }
    }
    thread_num--;
    //rec_dateflag=0;
    send_dateflag=thread_num;
    flag_lock.unlock();
    //ROS_INFO_STREAM("Thread "<<sockCli<<": 07 flag_lock is read released. Send_dateflag="<<send_dateflag<<" thread_num="<<thread_num);
    //ROS_INFO_STREAM("Thread "<<sockCli<<": 10 send_dateflag="<<send_dateflag<<" thread_num="<<thread_num);
    ROS_INFO_STREAM("-----------Thread "<<sockCli<<": is dead. Now we have "<<thread_num<<" worker threads.----------");
    socketMat.socketDisconnect(sockCli);
    return;
}

//作为服务器
int main(int argc, char *argv[])
{
    ros::init(argc, argv, "core_node");
    ros::NodeHandle nh;
    std::string pro_path,save_path;
    nh.param<std::string>("/pic_pro_path",pro_path,"/home/zju128");
    nh.param<std::string>("/pic_save_path",save_path,"/home/zju128");

    int port=PORT;
    if (socketMat.serverbind(port) < 0)
        return 0;

    it = new image_transport::ImageTransport(nh);
    image_transport::Subscriber sub_temp = it->subscribe("pic_combine", 1, camCallback);
    pic_sub.push_back(sub_temp);
    std::vector<boost::thread> thread_vec;   //线程列表

    waitKey(3100);
    while(nh.ok()){
        int sockClient=socketMat.connect(1);
        if (sockClient < 0)
            return 0;
        else{                                                   //向thread_vec中添加一个新的线程
            thread_num++;
            thread_vec.push_back(boost::thread(workfunc,sockClient));
            ROS_INFO_STREAM("A new thread is created. Now we have "<<thread_num<<" worker threads.");
            flag_lock.lock();
            //rec_dateflag=0;
            send_dateflag=thread_num;
            flag_lock.unlock();
            //ROS_INFO_STREAM("Main thread "<<": send_dateflag="<<send_dateflag<<" thread_num="<<thread_num);
        }
    }

    for (int i = 0; i < thread_vec.size(); ++i) {   //等待所有线程结束
		if(thread_vec[i].joinable())
			thread_vec[i].join();
	}

    socketMat.socketrelease();
    delete(it);
    return 1;
}
