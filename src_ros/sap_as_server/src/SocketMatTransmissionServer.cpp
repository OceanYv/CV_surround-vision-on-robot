#include "splice_and_pub/SocketMatTransmissionServer.h"
 
SocketMatTransmissionServer::SocketMatTransmissionServer(void){}
SocketMatTransmissionServer::~SocketMatTransmissionServer(void){}
 
int SocketMatTransmissionServer::serverbind(int PORT)
{
	ROS_INFO("Trying bind!");
	server_sockfd = socket(AF_INET,SOCK_STREAM, 0);		//服务器端套接字
 
	struct sockaddr_in server_sockaddr;
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(PORT);
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	int on=1;
 	if(setsockopt(server_sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(int))<0)
		ROS_ERROR("setsockopt error\n");
	if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1){
		ROS_ERROR("bind error");
		return -1;
	}

	if(listen(server_sockfd,5) == -1){
		ROS_ERROR("listen error");
		return -1;
	}
	ROS_INFO("Listening!");
}

int SocketMatTransmissionServer::connect(){
	ROS_INFO("Waiting for a new connection...");
	
	struct sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);
	sockClient = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);
	if(sockClient<0){
		ROS_ERROR("connect!");
		return -1;
	}
	else{
		ROS_INFO("connect successfully!");
		return sockClient;
	}
}

int SocketMatTransmissionServer::transmit(int sockClient,std::vector<unsigned char> &data_pic)
{
	//发送图片大小数据包
	data_inf.lenth = data_pic.size();
	int packnum = floor(1.0*data_inf.lenth/BUFFER_LEN);
	if (send(sockClient, (char *)(&data_inf),NEEDSED1, 0) < 0){
		ROS_ERROR("send image error: %s(errno: %d)", strerror(errno), errno);
		return -1;
	}

	//发送图片内容数据包
	//ROS_INFO_STREAM("    begin to send.");
	std::vector<unsigned char> ::iterator it=data_pic.begin();
	for(int i=0 ; i<packnum ; i++){												//发送前packnum个包
		int p = 0;
		while(p<BUFFER_LEN){
			pic_data[p] = *it;
			it++;p++;
		}
		if (send(sockClient, (char *)(pic_data), BUFFER_LEN, 0) < 0){
			printf("send image error: %s(errno: %d)\n", strerror(errno), errno);
			return -1;
		}
		//ROS_INFO_STREAM("    已发送" << i+1 <<"个包");
	}

	//发送最后一个数据包
	int last_num = data_inf.lenth%BUFFER_LEN;
	//ROS_INFO_STREAM("  最后一个包的大小为" << last_num);
	for(int i=0;i<last_num;i++){
		pic_data[i] = *it;
		it++;
	}
	if(it !=data_pic.end()){
		ROS_ERROR("出错啦,发送的字节数不对！！！");
		return -1;
	}
	if (send(sockClient, (char *)(pic_data), last_num, 0) < 0){		//发送包
		ROS_ERROR("send image error: %s(errno: %d)", strerror(errno), errno);
		return -1;
	}

	//等待客户端返回“FINISH_RECEIVE!”
	/*char ret[16];
	memset(ret,0,16);
	recv(sockClient,ret,16,0);
	if(strcmp(ret,"FINISH_RECEIVE!")!=0){
		std::cout<<"Didn't receive proper response"<<std::endl;
		return -1;
	}*/
	//ROS_INFO_STREAM("    Finish to send.");
	return 1;
}

void SocketMatTransmissionServer::socketDisconnect(int sockClient)
{
	close(sockClient);
}

void SocketMatTransmissionServer::socketrelease(void){
	close(server_sockfd);
}
