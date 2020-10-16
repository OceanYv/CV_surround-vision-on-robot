#ifndef __SOCKETMATTRANSMISSIONCLIENT_H__
#define __SOCKETMATTRANSMISSIONCLIENT_H__
 
#include "opencv2/opencv.hpp"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
 
using namespace cv;

//待传输图像默认大小为 1024*768 4张拼接，分十六个包发送，可修改
#define V_WIDTH 1024
#define V_HEIGHT 768
#define IMG_WIDTH (V_WIDTH*2)
#define IMG_HEIGHT (V_HEIGHT*2)
#define PACKAGE_NUM 16
#define BUFFER_SIZE IMG_WIDTH*IMG_HEIGHT*3/PACKAGE_NUM		//默认格式为CV_8UC3
 
struct sentbuf
{
	char buf[BUFFER_SIZE];
	int flag;
};
 
class SocketMatTransmissionClient
{
public:
	SocketMatTransmissionClient(void);
	~SocketMatTransmissionClient(void);
 
private:
	int sockClient;
	struct sentbuf data;
 
public:
 
	// return : -1		连接失败
	//			1		连接成功
	int socketConnect(const char* IP, int PORT);
 
	// 传输图像
	// return : -1		传输失败
	//			1		传输成功
	int transmit(cv::Mat image);

	// 断开socket连接
	void socketDisconnect(void);
};
 
#endif