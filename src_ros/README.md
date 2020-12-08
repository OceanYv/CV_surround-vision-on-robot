## splice_and_pub_ros: ros中四相机画面获取、合并与发布的代码实现
    1. 建立catkin工作空间后，放在其src目录下，并通过catkin指令进行编译
	eg：先设置自定义路径为当前路径
            $ mkdir catkin_ws/src
            $ cp 下载代码路径/SAP_as_client 自定义路径/catkin_ws/src/SAP_as_client
            $ cd 自定义路径/catkin_ws
            $ catkin_make
            $ echo "source 自定义路径/catkin_ws/devel/setup.bash" >> ~/.bashrc 
            打开一个新的终端
            $ roslaunch SAP_as_client start_client_and_cams.launch
    2. 该代码的开发环境为ubuntu 18.04 + ros melodic, 运行设备为Jetson nano + 两个CSI板载相机、两个USB相机
    3. ROS下实现的优点：
        1）可以通过launch文件方便的配置相机参数，而无需进入代码进行修改和重新编译
        2）通过多个节点实现相机数据的读取、拼接、通讯，以多线程的方式大大提高画面的实时性（延时大约为0.5s）
