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

/*1280*720 :最高10帧（0.4MB/帧-网速5MB/s）
  1024*768 :最高10帧（0.32MB/帧）
  800*600  :最高15帧（0.2MB/帧）
  640*480  :最高20帧（0.14MB/帧）
 */
#define V_WIDTH 1280
#define V_HEIGHT 720
#define IMG_WIDTH (V_WIDTH*2)
#define IMG_HEIGHT (V_HEIGHT*2)
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
	struct sentbuf_lenth data_inf;
	unsigned char pic_data[BUFFER_LEN];
 
public:
 
	//建立与server的连接，成功返回1，失败返回-1
	int socketConnect(const char* IP, int PORT);
 
	// 传输图像，成功返回1，失败返回-1
	int transmit(cv::Mat image);

	// 断开socket连接
	void socketDisconnect(void);
};
 
#endif