#ifndef __SOCKETMATTRANSMISSIONSERVER_H__
#define __SOCKETMATTRANSMISSIONSERVER_H__
 
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
 
class SocketMatTransmissionServer
{
public:
	SocketMatTransmissionServer(void);
	~SocketMatTransmissionServer(void);
 	int sockClient,server_sockfd;
private:
	
	struct sentbuf_lenth data_inf;
	unsigned char pic_data[BUFFER_LEN];
 
public:
	// 服务器绑定套接字与端口
	int serverbind(int PORT);

 	// 建立连接，成功返回客户端套接字，失败返回-1
	int connect(void);
 
	// 传输图像，成功返回1，失败返回-1
	int transmit(int sockClient,std::vector<unsigned char> &data_pic);

	// 断开socket连接
	void socketDisconnect(int sockClient);
	void socketrelease(void);
};
 
#endif
