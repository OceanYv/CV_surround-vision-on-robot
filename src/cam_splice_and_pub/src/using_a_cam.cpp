#include "using_a_cam.hpp"

    using_a_cam::using_a_cam(std::string cam_name, double width, double height,bool window):
        cam_name_(cam_name),
        height_(height),
        width_(width),
        window_(window),
        fresh_notic_flag_(false)
      {
        if(cam_name_ == "front")
            cam_num_ =  FRONT;
        else if(cam_name_== "back")
            cam_num_ = BACK;
        else if(cam_name_== "left")
            cam_num_ = LEFT;
        else if(cam_name_ == "right")
            cam_num_ = RIGHT;
        else{
            std::cout<<"There is no such a camera named "<<cam_name_<<std::endl;
            return;
        }
        
        //配置媒体流管道
        std::string pipeline;
        if(cam_num_ == FRONT || cam_num_ == BACK)
            pipeline=get_usb_pipeline(cam_num_, width_, height_, 10);
        else
            pipeline=get_csi_pipeline(cam_num_, width_, height_, 10);
	    std::cout << "Using pipeline: \n\t" << pipeline << "\n";

        //打开摄像头
        capture_.open(pipeline, cv::CAP_GSTREAMER);
        if(capture_.isOpened())  
            std::cout<<"camera at " << cam_name_ <<" is opened"<<std::endl;
        else {
            std::cout<<"camera at " << cam_name_ <<" can't be opened"<<std::endl;
            return;
        }

        std::cout<<"--------------camera " << cam_name_ <<"-----------------"<<std::endl;
        std::cout<<"    宽度×高度：" << capture_.get(3)<<"*"<<capture_.get(4)<<std::endl;
        std::cout<<"    帧      率：" << capture_.get(5)<<std::endl;
        if(window_ == true)
            cv::namedWindow("camera"+cam_name_,0);
    }

    using_a_cam::~using_a_cam(){
        capture_.release();
        std::cout<<"cam "<<cam_num_<<" has been closed!!!"<<std::endl;
    }

    cv::Mat using_a_cam::get_a_picture(){
        capture_>>frame_;
        return frame_;
    }

    void using_a_cam::fresh_the_window(){
        int tem;
        if(window_ ==  false){
            if(fresh_notic_flag_ == false){
                std::cout  << "你指定了"<<cam_name_<<"相机不可通过窗口显示，如果想要显示，请输入1，否则输入0：";         
                std::cin >>  tem;
                if( tem == 1){
                    cv::namedWindow("camera"+cam_name_,0);
                    window_ = true;
                }
                fresh_notic_flag_ = true;
            }
            return;
        }

        capture_>>frame_;
        if (!frame_.empty())
            cv::imshow("camera"+cam_name_, frame_);   //在窗口显示

        cv::waitKey(10); 
    }

std::string using_a_cam::get_csi_pipeline(int cam_id,int width, int height, int fps) {
    return "nvarguscamerasrc sensor-id="+ std::to_string(cam_id) +" ! video/x-raw(memory:NVMM), width=3264, height=2464, format=NV12, framerate=" + std::to_string(fps) + "/1 ! " +
    "nvvidconv flip-method=0 ! video/x-raw, width="+ std::to_string(width) +", height="+ std::to_string(height) +", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}

std::string using_a_cam::get_usb_pipeline(int cam_id,int width, int height, int fps) {
    return "v4l2src device=/dev/video"+ std::to_string(cam_id) +" ! video/x-raw, width="+std::to_string(width)+", height="+ std::to_string(height) +", format=YUY2, framerate=" + std::to_string(fps) + "/1 ! " +
    "videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}
