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
 	/*建立连接，或断连后重新建立连接
	 *	输入值flag:
	 *		１:建立一个新的链接；
	 *		２:重建当前链接；
	 *  返回值：客户端的套接字
	 */
	int connect(int flag);

	//绑定端口，获取服务器套接字，成功返回1，失败返回-1
	int serverbind(int PORT);
 
	// 向指定客户端传输图像，成功返回1，失败返回-1
	int transmit(int sockClient,cv::Mat image);

	// 断开socket连接
	void socketDisconnect(int sockClient);//客户端
	void socketrelease(void);//服务器
};
 
#endif
