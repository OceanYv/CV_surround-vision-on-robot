***********************基于Opencv 3.2.0 C++ Socket实现的视频网络传输程序************************  

# code_from_csdn: SCDN上找到的图像传输代码，除了增加一些注之外，尽量不会去动它  
    1. 要求至少有一个摄像头设备  
    2. 在两个头文件和客户端文件中修改图像尺寸、IP、端口参数  
    3.先启动server程序，再启动client程序  

# cam_splice_and_pub：基于code_from_csdn实现的图像合并与传输代码  
    1. 要求有四个摄像头设备，如果不足，需要在client文件中修改  
    2. 用于水印的图片文件放在./bin/cam_splice_and_pub/pic路径下  
    3. 在两个头文件和客户端文件中修改图像尺寸、IP、端口参数  
    4. 先启动server程序，再启动client程序  
    
    ***2020.10.16***  
        四个相机的画面合并之后，直接以Mat的格式分多个包传输  
