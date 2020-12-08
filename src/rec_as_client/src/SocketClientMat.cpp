#include "SocketMatTransmissionClient.h"
#include <time.h>
 
int main()
{
	SocketMatTransmissionClient socketMat;
	if (socketMat.socketConnect("192.168.0.2",6666) < 0)
		return 0;
	
	cv::namedWindow("picture_combine_receive",0);
	cv::Mat image;
	while (1){
		//接收图像
		if(socketMat.receive(image) > 0){
			cv::imshow("picture_combine_receive",image);
			cv::waitKey(25);
		}
		else{
			std::cout<<"通过socket获取图像失败,尝试重新建立连接..."<<std::endl;
			socketMat.socketDisconnect();
			cv::waitKey(1000);
			if (socketMat.socketConnect("192.168.0.2",6666) < 0)
				return 0;
		}
	}
	socketMat.socketDisconnect();
	return 0;
}