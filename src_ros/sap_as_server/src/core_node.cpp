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

cv::Mat pic_combine;                  //订阅得到的图像
std::vector<unsigned char> data_pic;  //压缩图片
SocketMatTransmissionServer socketMat;                                        //用于网络通讯
boost::shared_mutex flag_lock,pic_lock,vector_lock;                           //读写锁
int compress_flag=0,callback_flag=0,rec_dateflag,send_dateflag,thread_num=0;   //vec初始化标志，压缩线程运行标志，是否已经更新图像数据，有多少个线程完成了新数据的发送，工作线程的个数

void compress_fun(){
    compress_flag=1;
    cv::Mat pic_combine_temp;                       //为了减少读写锁占用的时间，设置temp变量
    std::vector<unsigned char> data_pic_temp;

    std::vector<int> param = std::vector<int>(2);
    param[0] = 1;			//CV_IMWRITE_JPEG_QUALITY
    param[1] = 50; 			// default(95) 0-100
    //ROS_INFO_STREAM("Thread 00: begin to compress.");
        pic_lock.lock();
        pic_combine_temp = pic_combine;
        pic_lock.unlock();
        cv::imencode(".jpg", pic_combine_temp, data_pic_temp,param);                  //这一行运行效率低!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        vector_lock.lock();
        data_pic = data_pic_temp;
        vector_lock.unlock();
    //ROS_INFO_STREAM("    finish compress. rate of compression:"<<1.0*(data_pic.size())/(1280*720*4*3));
    compress_flag=0;rec_dateflag=1;
}

void camCallback(const sensor_msgs::ImageConstPtr& msg){
    callback_flag=1;
    pic_lock.lock();
    pic_combine = cv_bridge::toCvCopy(msg, "bgr8")->image;  //toCvShare  toCvCopy
    //ROS_INFO_STREAM("Thread 00"<<": 04 Receive picture from Splice node successfully!.");

    if(pic_combine.empty()){
        printf("empty image\n\n");
        return;}
    if(pic_combine.type() != CV_8UC3){
        printf("the image must satisfy : type == CV_8UC3\n\n");
        return;}
    pic_lock.unlock();
    //ROS_INFO_STREAM("Thread 00: try to call compress_fun.");
    if(compress_flag==0)
        boost::thread temp(compress_fun);  //图像压缩线程
}

void workfunc(int sockCli){
    int num=1;
    ros::NodeHandle nh;

    while(nh.ok()){
        //waitKey(5);
        //ROS_INFO_STREAM("Thread "<<sockCli<<": 01 trying to read lock flag_lock.");
        flag_lock.lock();

        //ROS_INFO_STREAM("Thread "<<sockCli<<": 6666666666666666.");
        if(send_dateflag == thread_num){//最新的图像已经发送给所有客户端，准备更新图像
            rec_dateflag = 0;send_dateflag=0;
            //ROS_INFO_STREAM("Thread "<<sockCli<<": 02 waiting for the spin.");
            while(callback_flag==0){
                ros::spinOnce();
            }
            callback_flag=0;
        }

        if(rec_dateflag == 1){
            //ROS_INFO_STREAM("Thread "<<sockCli<<": 03 begin to trans the "<< num <<" picture.");
            vector_lock.lock();
            int ret=socketMat.transmit(sockCli,data_pic);
            vector_lock.unlock();
                if(ret == -1){
                    ROS_INFO_STREAM("ERROR, the thread will be killed.");
                    break;}
                else{
                    //imwrite(save_path+"/combine.jpg", combine);
                    //ROS_INFO_STREAM("Thread "<<sockCli<<": 06 has trans "<<num<<" pictures.");
                    num++;} 

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
    send_dateflag=thread_num;
    flag_lock.unlock();
    socketMat.socketDisconnect(sockCli);
    ROS_INFO_STREAM("-----------Thread "<<sockCli<<": is dead. Now we have "<<thread_num<<" worker threads.----------");
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

    image_transport::ImageTransport it(nh);
    image_transport::Subscriber sub_temp = it.subscribe("pic_combine", 1, camCallback);
    std::vector<boost::thread> thread_vec;   //线程列表

    waitKey(3000);
    while(nh.ok()){
        int sockClient=socketMat.connect();   //阻塞
        if (sockClient < 0)
            return 0;
        else{                                                   //向thread_vec中添加一个新的线程
            thread_num++;
            thread_vec.push_back(boost::thread(workfunc,sockClient));
            ROS_INFO_STREAM("A new thread is created. Now we have "<<thread_num<<" worker threads.");

            flag_lock.lock();
            send_dateflag=thread_num;
            flag_lock.unlock();
            ROS_INFO_STREAM("Main thread "<<": send_dateflag="<<send_dateflag<<" thread_num="<<thread_num);
        }
    }

    for (int i = 0; i < thread_vec.size(); ++i) {   //等待所有线程结束
		if(thread_vec[i].joinable())
			thread_vec[i].join();
	}
    socketMat.socketrelease();
    return 1;
}
