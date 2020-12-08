#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>

std::string get_csi_pipeline(int cam_id,int width, int height, int fps) {
    return "nvarguscamerasrc sensor-id="+ std::to_string(cam_id) +" ! video/x-raw(memory:NVMM), width=3264, height=2464, format=NV12, framerate=" + std::to_string(fps) + "/1 ! " +
    "nvvidconv flip-method=0 ! video/x-raw, width="+ std::to_string(width) +", height="+ std::to_string(height) +", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}

std::string get_usb_pipeline(int cam_id,int width, int height, int fps) {
    return "v4l2src device=/dev/video"+ std::to_string(cam_id) +" ! video/x-raw, width="+std::to_string(width)+", height="+ std::to_string(height) +", format=YUY2, framerate=" + std::to_string(fps) + "/1 ! " +
    "videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "image_publisher");
  ros::NodeHandle nh_ex;

  std::string name,topic_name,save_path;
  double width,height;
  cv::Mat image;
  int cam_num,fps;

  //读取输入参数
  if(argc != 3){
    ROS_ERROR("You should put in 2 args!!!");
    return 0;
  }
  else{
    name = argv[1];
    cam_num = ((int)(argv[2][0]))-48;
  }

  if(cam_num != 0  && cam_num != 1  && cam_num != 2  && cam_num != 3){
    ROS_ERROR("Do not have such camera!!!");
    return 0;
  }

  //读取相机参数（分辨率、帧率）
  nh_ex.param("/cam_conf/width",width,1280.0);
  nh_ex.param("/cam_conf/height",height,720.0);
  nh_ex.param("/cam_conf/fps",fps,10);
  nh_ex.param<std::string>("/pic_save_path",save_path,"/home/zju128");

  //创建topic发布者
  topic_name="cam_"+name;
  image_transport::ImageTransport it(nh_ex);
  image_transport::Publisher pub = it.advertise(topic_name.c_str(), 1);
  sensor_msgs::ImagePtr msg;

  //创建相机读取管道
  std::string pipeline;
  if(cam_num == 2 || cam_num == 3)
    pipeline=get_usb_pipeline(cam_num, width, height, fps);
  else
    pipeline=get_csi_pipeline(cam_num, width, height, fps);
  std::cout << "Using pipeline: \n\t" << pipeline << "\n";

  //打开摄像头
  cv::VideoCapture capture;
  capture.open(pipeline, cv::CAP_GSTREAMER);
  if(capture.isOpened())  
    std::cout<<"camera at " << name <<" is opened"<<std::endl;
  else {
    std::cout<<"camera at " << name <<" can't be opened"<<std::endl;
    return 0;
  }
  std::cout<<"--------------camera " << name <<"-----------------"<<std::endl;
  std::cout<<"    宽度×高度：" << capture.get(3)<<"*"<<capture.get(4)<<std::endl;
  std::cout<<"    帧      率：" << capture.get(5)<<std::endl;
  std::cout<<"    Save image at: "<<save_path+"/"+topic_name+".jpg\n"<<std::endl; 

  //循环读取并发布图像
  ros::Rate loop_rate(fps);
  while (nh_ex.ok()) {
    capture>>image;
    //imwrite(save_path+"/"+topic_name+".jpg", image);
    msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", image).toImageMsg();   //将image转换成ROS所支持的消息类型
    pub.publish(msg);
    loop_rate.sleep();
  }
}
