#ifndef __SOCKETMATTRANSMISSIONCLIENT_H__
#define __SOCKETMATTRANSMISSIONCLIENT_H__
 
#include "opencv2/opencv.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
 
using namespace cv;

#define NEEDREC1 8			//sizeof(sentbuf_lenth)
#define BUFFER_LEN 10000
#define WAITIME -1

struct sentbuf_lenth{
	long unsigned int lenth;
};
 
class SocketMatTransmissionClient
{
public:
	SocketMatTransmissionClient(void);
	~SocketMatTransmissionClient(void);
	
private:
	struct sentbuf_lenth data_inf;
	int pic_count;
	unsigned char pic_data[BUFFER_LEN];
 
public:
 	int sockClient;

	// 返回-1：连接失败，返回1，连接成功
	int socketConnect(const char* IP, int PORT);

	int receive(cv::Mat& image);
 
	// 断开socket连接
	void socketDisconnect(void);
};
 
#endif
