#ifndef __USING_A_CAM__
#define __USING_A_CAM__

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define FRONT 3
#define BACK 2
#define LEFT 1
#define RIGHT 0

class  using_a_cam
{
    public:
        /*构造函数
         *输入：
          *  cam_name：可选择front、back 、left、right
          *  height、weight，可选择：
          *    3840*2160   1920*1080   1280*720    
          *    2592*1944   2048*1536   1600*1200
          *    1280*960      1024*768      800*600
          *    640*480         320*240    
          *  window：是否创建输出窗口，可选择 true/false    
         *功能：打开对应摄像头、设置一个对应的窗口
         */
        using_a_cam(std::string cam_name,double width, double height,bool window);
    
        /*析构函数
          *功能：释放相机
          */
        ~using_a_cam();

        /*获取图像
          *功能：通过该相机捕获一张图片并返回
          */
        cv::Mat get_a_picture();

        /*更新窗口
          *功能：通过该相机捕获一张图片并通过窗口显示出来
          */
        void fresh_the_window();

    private:
        int cam_num_;   //通过名称确定的相机编号
        double height_,width_;
        std::string cam_name_;          //如果对应不起来可以修改上方宏定义的值
        cv::VideoCapture capture_; 
        cv::Mat frame_;
        bool window_,fresh_notic_flag_;

        std::string get_csi_pipeline(int cam_id,int width, int height, int fps);
        std::string get_usb_pipeline(int cam_id,int width, int height, int fps);
};

#endif
