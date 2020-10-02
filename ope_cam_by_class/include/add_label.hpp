//这个函数还有问题，没有在用
#ifndef ADD_LABEL
#define ADD_LABEL

#include "using_a_cam.hpp"

cv::Mat add_label(using_a_cam* cam,cv::Mat* label){
        cv::Mat pic = cam->get_a_picture();
        cv::Mat imageROI = pic(cv::Rect(pic.rows-label->rows,  pic.cols-label->cols, label->rows,  label->cols));
        label->copyTo(imageROI);

        return pic;
}

#endif