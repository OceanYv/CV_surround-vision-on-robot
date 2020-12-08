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
#include "ros/ros.h"
 
using namespace cv;

#define NEEDSED1 8					//sizeof(sentbuf_lenth)
#define BUFFER_LEN 10000			//分包发送时，每个包的大小

struct sentbuf_lenth{
	long unsigned int lenth;
};
 
class SocketMatTransmissionClient
{
public:
	SocketMatTransmissionClient(void);
	~SocketMatTransmissionClient(void);
 
private:
	int sockClient;
	ros::NodeHandle nh_;
	struct sockaddr_in servaddr;
	struct sentbuf_lenth data_inf;
	unsigned char pic_data[BUFFER_LEN];

	// 断开之后重建链接
	int reconnect(void);
 
public:
 
	//建立与server的连接，成功返回1，失败返回-1
	int socketConnect(const char* IP, int PORT);
 
	// 传输图像，成功返回1，失败返回-1
	int transmit(cv::Mat image);

	// 断开socket连接
	void socketDisconnect(void);
};
 
#endif