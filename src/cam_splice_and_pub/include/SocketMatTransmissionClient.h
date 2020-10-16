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

//待传输图像默认大小为 1024*768*4(4张拼接)，分8个包发送（按行划分）
#define V_WIDTH 800
#define V_HEIGHT 600
#define IMG_WIDTH (V_WIDTH*2)
#define IMG_HEIGHT (V_HEIGHT*2)
#define PACKAGE_NUM 8
#define BUFFER_SIZE IMG_WIDTH*IMG_HEIGHT*3/PACKAGE_NUM		//默认格式为CV_8UC3
 
struct sentbuf
{
	char buf[BUFFER_SIZE];
	int flag;			//是否为最后一个包的标志，2：为最后一个包，1：非最后一个包
};
 
class SocketMatTransmissionClient
{
public:
	SocketMatTransmissionClient(void);
	~SocketMatTransmissionClient(void);
 
private:
	int sockClient;
	struct sentbuf data;	//缓存区
 
public:
 
	//建立与server的连接，成功返回1，失败返回-1
	int socketConnect(const char* IP, int PORT);
 
	// 传输图像，成功返回1，失败返回-1
	int transmit(cv::Mat image);

	// 断开socket连接
	void socketDisconnect(void);
};
 
#endif