#include "SocketMatTransmissionClient.h"
 
SocketMatTransmissionClient::SocketMatTransmissionClient(void){}
SocketMatTransmissionClient::~SocketMatTransmissionClient(void){}
 
int SocketMatTransmissionClient::socketConnect(const char* IP, int PORT)
{
	struct sockaddr_in    servaddr;
 
	if ((sockClient = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
		return -1;
	}
 
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	if (inet_pton(AF_INET, IP, &servaddr.sin_addr) <= 0) {
		printf("inet_pton error for %s\n", IP);
		return -1;
	}
 
	if (connect(sockClient, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
		return -1;
	}
	else 
		printf("connect successful!\n");

	return 1;
}
 
int SocketMatTransmissionClient::transmit(cv::Mat image)
{
	if (image.empty()){
		printf("empty image\n\n");
		return -1;
	}
 
	if(image.cols != IMG_WIDTH || image.rows != IMG_HEIGHT || image.type() != CV_8UC3){
		printf("the image must satisfy : cols == IMG_WIDTH（%d）  rows == IMG_HEIGHT（%d） type == CV_8UC3\n\n", IMG_WIDTH, IMG_HEIGHT);
		return -1;
	}
 
	for(int k = 0; k < PACKAGE_NUM; k++) {
		int num1 = IMG_HEIGHT / PACKAGE_NUM * k;		//该包发送的起始行
		for (int i = 0; i < IMG_HEIGHT / PACKAGE_NUM; i++){	//i为该包发送的总行数
			int num2 = i * IMG_WIDTH * 3;						//该包的字节数
			uchar* ucdata = image.ptr<uchar>(i + num1);
			for (int j = 0; j < IMG_WIDTH * 3; j++)
				data.buf[num2 + j] = ucdata[j];
		}
 
		if(k == PACKAGE_NUM - 1)			//最后一个包
			data.flag = 2;
		else
			data.flag = 1;
 
		if (send(sockClient, (char *)(&data), sizeof(data), 0) < 0){
			printf("send image error: %s(errno: %d)\n", strerror(errno), errno);
			return -1;
		}
	}

	return 1;
}

void SocketMatTransmissionClient::socketDisconnect(void)
{
	close(sockClient);
}