#include "SocketMatTransmissionServer.h"
 
int main()
{
	SocketMatTransmissionServer socketMat;
	if (socketMat.socketConnect(6666) < 0)
		return 0;
	
	cv::namedWindow("picture_combine_receive",0);
	cv::Mat image;
	while (1){
		if(socketMat.receive(image) > 0){
			cv::imshow("picture_combine_receive",image);
			cv::waitKey(25);
		}
		else{
			std::cout<<"通过socket获取图像失败！"<<std::endl;
		}
	}
 
	socketMat.socketDisconnect();
	return 0;
}
