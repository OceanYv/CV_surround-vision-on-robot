#include "using_a_cam.hpp"

    using_a_cam::using_a_cam(std::string cam_name, double height, double weight,bool window):
        cam_name_(cam_name),
        height_(height),
        weight_(weight),
        window_(window),
        fresh_notic_flag_(false)
      {
        if(cam_name_ == "front")
            cam_num_ = front;
        else if(cam_name_== "back")
            cam_num_ = back;
        else if(cam_name_== "left")
            cam_num_ = left;
        else if(cam_name_ == "right")
            cam_num_ = right;
        else
            std::cout<<"There is no such a camera named "<<cam_name_<<std::endl;
            
        capture_.open(cam_num_);
        if(capture_.isOpened())  //打开摄像头
            std::cout<<"camera at " << cam_name_ <<" is opened"<<std::endl;
        else 
            std::cout<<"camera at " << cam_name_ <<" can't be opened"<<std::endl;

        capture_.set(3,height_);
        capture_.set(4,weight_);

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