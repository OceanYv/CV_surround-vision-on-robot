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

/*1280*720 :最高10帧（0.4MB/帧-网速5MB/s）
  1024*768 :最高10帧（0.32MB/帧）
  800*600  :最高15帧（0.2MB/帧）
  640*480  :最高20帧（0.14MB/帧）
 */
#define V_WIDTH 1280
#define V_HEIGHT 720
#define IMG_WIDTH (V_WIDTH*2)
#define IMG_HEIGHT (V_HEIGHT*2)
#define NEEDREC1 8			//sizeof(sentbuf_lenth)
#define BUFFER_LEN 10000

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
	unsigned char pic_data[BUFFER_LEN];
 
public:
 
	// 返回-1：连接失败，返回1，连接成功
	int socketConnect(int PORT);

	int receive(cv::Mat& image);
 
	// 断开socket连接
	void socketDisconnect(void);
};
 
#endif
