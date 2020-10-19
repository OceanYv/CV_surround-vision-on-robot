#ifndef __SOCKETMATTRANSMISSIONSEVER_H__
#define __SOCKETMATTRANSMISSIONSEVER_H__
 
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
 
#define V_WIDTH 800
#define V_HEIGHT 600
#define IMG_WIDTH (V_WIDTH*2)
#define IMG_HEIGHT (V_HEIGHT*2)
#define NEEDREC1 8			//sizeof(sentbuf_lenth)

struct sentbuf_lenth{
	long unsigned int lenth;
};
 
class SocketMatTransmissionServer
{
public:
	SocketMatTransmissionServer(void);
	~SocketMatTransmissionServer(void);
	int sockConn;
private:
	struct sentbuf_lenth data_inf;
	int pic_count;
	unsigned char pic_data[500000];
 
public:
 
	// 返回-1：连接失败，返回1，连接成功
	int socketConnect(int PORT);

	int receive(cv::Mat& image);
 
	// 断开socket连接
	void socketDisconnect(void);
};
 
#endif