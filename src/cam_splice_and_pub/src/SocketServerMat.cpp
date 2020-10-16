#include "SocketMatTransmissionServer.h"
 
int main()
{
	SocketMatTransmissionServer socketMat;
	if (socketMat.socketConnect(6666) < 0)
		return 0;
 
	cv::Mat image;
	while (1){
		if(socketMat.receive(image) > 0){
			cv::imshow("picture_combine_receive",image);
			cv::waitKey(20);
		}
	}
 
	socketMat.socketDisconnect();
	return 0;
}