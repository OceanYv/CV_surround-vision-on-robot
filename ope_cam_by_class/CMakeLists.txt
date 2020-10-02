include_directories(./include)      #声明头文件存放的文件夹路径

add_library(using_a_cam src/using_a_cam.cpp)     #添加一个库
add_executable(ope_cam_by_class src/ope_cam_by_class.cpp)
target_link_libraries(ope_cam_by_class using_a_cam ${OpenCV_LIBS} )
