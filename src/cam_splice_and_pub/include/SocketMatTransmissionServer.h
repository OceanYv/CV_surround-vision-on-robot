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
#define PACKAGE_NUM 8
#define BLOCKSIZE IMG_WIDTH*IMG_HEIGHT*3/PACKAGE_NUM
 
struct recvBuf
{
	char buf[BLOCKSIZE];
	int flag;
};
 
 
class SocketMatTransmissionServer
{
public:
	SocketMatTransmissionServer(void);
	~SocketMatTransmissionServer(void);
	int sockConn;
private:
	struct recvBuf data;
 
	int needRecv;
	int count;
 
public:
 
	// 返回-1：连接失败，返回1，连接成功
	int socketConnect(int PORT);
 
	// 传输图像
	// return : -1		接收失败
	//			1		接收成功
	int receive(cv::Mat& image);
 
	// 断开socket连接
	void socketDisconnect(void);
};
 
#endif