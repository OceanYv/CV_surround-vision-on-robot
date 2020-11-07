# 基于Opencv 3.2.0 C++ Socket实现的视频网络传输程序

## code_from_csdn: SCDN上找到的图像传输代码，除了增加一些注之外，尽量不会去动它  
    1. 要求至少有一个摄像头设备  
    2. 在两个头文件和客户端文件中修改图像尺寸、IP、端口参数  
    3.先启动server程序，再启动client程序  

## cam_splice_and_pub：基于code_from_csdn实现的图像合并与传输代码  
    1. 要求有四个摄像头设备，如果不足，需要在client文件中修改  
    2. 用于水印的图片文件放在./bin/cam_splice_and_pub/pic路径下  
    3. 在两个头文件和客户端文件中修改图像尺寸、IP、端口参数  
    4. 先启动server程序，再启动client程序  
    
    **2020.10.16**  
        四个相机的画面合并之后，直接以Mat的格式分多个包传输  

## splice_and_pub_ros: 将cam_splice_and_pub中的客户端部分迁移到ros中的代码实现
    1. 建立catkin工作空间后，放在其src目录下，并通过catkin指令进行编译
	eg：先设置自定义路径为当前路径
            $ mkdir catkin_ws/src
            $ cp 下载代码路径/splice_and_pub_ros 自定义路径/catkin_ws/src/splice_and_pub_ros
            $ cd 自定义路径/catkin_ws
            $ catkin_make
            $ echo "source 自定义路径/catkin_ws/devel/setup.bash" >> ~/.bashrc 
            打开一个新的终端
            $ roslaunch splice_and_pub_ros start_client_and_cams.launch
    2. 该代码的开发环境为ubuntu 18.04 + ros melodic, 运行设备为Jetson nano + 两个CSI板载相机、两个USB相机
    3. 迁移到ROS下的原因为：
        1）可以通过launch文件方便的配置相机参数，而无需进入代码进行修改和重新编译
        2）通过多个节点实现相机数据的读取、拼接、通讯，以多线程的方式大大提高画面的实时性（延时大约为0.5s）
