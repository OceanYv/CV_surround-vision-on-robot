cmake_minimum_required(VERSION 3.0.0)
project(Simple_Splicing VERSION 0.1.0)

# 寻找OpenCV库
find_package(OpenCV REQUIRED)

# 添加头文件
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories("/usr/include/eigen3")
include_directories(./include)

add_subdirectory(try_a_camera)
add_subdirectory(ope_cam_by_class)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
