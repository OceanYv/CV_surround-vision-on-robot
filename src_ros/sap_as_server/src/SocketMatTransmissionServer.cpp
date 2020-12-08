#include "splice_and_pub/SocketMatTransmissionServer.h"
 
SocketMatTransmissionServer::SocketMatTransmissionServer(void){}
SocketMatTransmissionServer::~SocketMatTransmissionServer(void){}
 
int SocketMatTransmissionServer::serverbind(int PORT)
{
	printf("Trying to build the connection!\n");
	server_sockfd = socket(AF_INET,SOCK_STREAM, 0);		//服务器端套接字
 
	struct sockaddr_in server_sockaddr;
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(PORT);
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	int on=1;
 	if(setsockopt(server_sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(int))<0)
		ROS_ERROR("setsockopt error\n");
	if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1){
		perror("bind");
		return -1;
	}
	printf("bind successfully!\n");

	if(listen(server_sockfd,5) == -1){
		perror("listen");
		return -1;
	}
	printf("Listening!\n");
}

int SocketMatTransmissionServer::transmit(int sockClient,cv::Mat image)
{
	if (image.empty()){
		printf("empty image\n\n");
		return -1;
	}
	if(image.type() != CV_8UC3){
		printf("the image must satisfy : type == CV_8UC3\n\n");
		return -1;
	}

	std::vector<unsigned char> data_pic;
	std::vector<int> param = std::vector<int>(2);
	param[0] = 1;			//CV_IMWRITE_JPEG_QUALITY
	param[1] = 70; 											// default(95) 0-100
	//ROS_INFO_STREAM("Thread "<<sockClient<<": 21 begin to yasuo.");
	cv::imencode(".jpg", image, data_pic,param);
	//ROS_INFO_STREAM("    压缩为jpg之后，大小为："<<data_pic.size()<<" 压缩率为："<<1.0*(data_pic.size())/(1280*720*4*3));

	//发送图片大小数据包
	data_inf.lenth = data_pic.size();
	int packnum = floor(1.0*data_inf.lenth/BUFFER_LEN);
	//std::cout <<"发送信息中的大小为："<<data_inf.lenth<<",共需发送" << packnum+1 <<"个包"<<std::endl;
	if (send(sockClient, (char *)(&data_inf),NEEDSED1, 0) < 0){
		printf("send image error: %s(errno: %d)\n", strerror(errno), errno);
		return -1;
	}

	//发送图片内容数据包
	std::vector<unsigned char> ::iterator it=data_pic.begin();
    //ROS_INFO_STREAM("    begin to send.");
	for(int i=0 ; i<packnum ; i++){												//发送前packnum个包
		int p = 0;
		while(p<BUFFER_LEN){
			pic_data[p] = *it;
			it++;p++;
		}
		if (send(sockClient, (char *)(pic_data), BUFFER_LEN, 0) < 0){
			printf("send image error: %s(errno: %d)\n", strerror(errno), errno);
			return -1;//connect(2);
		}
		//std::cout << "    已发送" << i+1 <<"个包"<<std::endl;
	}

	//发送最后一个数据包
	int last_num = data_inf.lenth%BUFFER_LEN;
    //std::cout << "  最后一个包的大小为" << last_num<<std::endl;
	for(int i=0;i<last_num;i++){
		pic_data[i] = *it;
		it++;
	}
	if(it !=data_pic.end()){
		printf("出错啦,发送的字节数不对！！！\n");
		return -1;
	}
	if (send(sockClient, (char *)(pic_data), last_num, 0) < 0){		//发送包
		printf("send image error: %s(errno: %d)\n", strerror(errno), errno);
		return -1;
	}

	//等待客户端返回“FINISH_RECEIVE!”
	/*char ret[16];
	memset(ret,0,16);
	recv(sockClient,ret,16,0);
	if(strcmp(ret,"FINISH_RECEIVE!")!=0){
		std::cout<<"Didn't receive proper response"<<std::endl;
		return -1;//connect(2);
	}*/
	//ROS_INFO_STREAM("    finish to send.");
	return 1;
}

int SocketMatTransmissionServer::connect(int flag){
	if(flag ==1)
		printf("Waiting for a new connection...\n");
	else{
		close(sockClient);
		printf("Try connect to server again, it maybe will take 5~10 seconds...\n");
		cv::waitKey(3000);
	}
	struct sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);
	sockClient = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);
	if(sockClient<0){
		perror("connect");
		return -1;
	}
	else{
		printf("connect successful!\n");
		return sockClient;
	}
}

void SocketMatTransmissionServer::socketDisconnect(int sockClient)
{
	close(sockClient);
}

void SocketMatTransmissionServer::socketrelease(void){
	close(server_sockfd);
}
