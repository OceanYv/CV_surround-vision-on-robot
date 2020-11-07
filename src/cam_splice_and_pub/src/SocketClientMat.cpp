#include "SocketMatTransmissionClient.h"
 
#define V_WIDTH 800
#define V_HEIGHT 600

int main()
{
	SocketMatTransmissionClient socketMat;
	if (socketMat.socketConnect("192.168.0.2", 6666) < 0)
		return 0;
	
	cv::VideoCapture capture(2);
	cv::Mat image;

	capture.set(3,V_WIDTH);
    capture.set(4,V_HEIGHT);
 
	while (1){
		if (!capture.isOpened())
			return 0;
 
		capture >> image;

		if (image.empty())
			return 0;
 
		socketMat.transmit(image);
	}
 
	socketMat.socketDisconnect();
	return 0;
}
