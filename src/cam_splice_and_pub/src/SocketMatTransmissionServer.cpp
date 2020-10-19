#include "SocketMatTransmissionServer.h"
 
SocketMatTransmissionServer::SocketMatTransmissionServer(void){
	pic_count = 0;
	//std::cout<<"sizeof(sentbuf_lenth):"<<needrec1<<std::endl;
}
SocketMatTransmissionServer::~SocketMatTransmissionServer(void){}
 
int SocketMatTransmissionServer::socketConnect(int PORT){
	int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);
 
	struct sockaddr_in server_sockaddr;
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(PORT);
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
 
	if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1){
		perror("bind");
		return -1;
	}
 
	if(listen(server_sockfd,5) == -1){
		perror("listen");
		return -1;
	}
 
	struct sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);
 
	sockConn = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);
	if(sockConn<0){
		perror("connect");
		return -1;
	}
	else{
		printf("connect successful!\n");
		return 1;
	}
	
	close(server_sockfd);
}
 

int SocketMatTransmissionServer::receive(cv::Mat& image)
{
	//接收信息数据
	pic_count++;
	memset(&data_inf,0,NEEDREC1);
	int rec_sum=0,rec_single=0;			//接收储存图片信息的结构体
	while(rec_sum<NEEDREC1){
		rec_single = recv(sockConn, (char*)(&data_inf) + rec_sum, NEEDREC1 - rec_sum, 0);
		rec_sum +=rec_single;
	}

	//接收图像数据
	int packnum = floor(1.0*data_inf.lenth/BUFFER_LEN);
	//std::cout<<"第"<<pic_count<<"张图片的大小为："<<data_inf.lenth<<",分为"<<packnum+1<<"个包接收"<<std::endl;
	std::vector<unsigned char> data_pic;

	for(int n=0;n<packnum;n++){			//接收前packnum个包
		rec_sum=0,rec_single=0;
		while(rec_sum<BUFFER_LEN){
			rec_single = recv(sockConn, (char*)(pic_data)+rec_sum, BUFFER_LEN-rec_sum, 0);
			rec_sum +=rec_single;
			//std::cout<<"        还要接收/已接受/共需接收 = "<<BUFFER_LEN - rec_sum<<"/"<<rec_sum<<"/"<<BUFFER_LEN<<std::endl;
		}
		for(int i=0;i<BUFFER_LEN;i++)
			data_pic.push_back(pic_data[i]);
		//std::cout<<"    已接收"<<n+1<<"个包"<<std::endl;
	}

	//接收最后一个包
	int last_num = data_inf.lenth%BUFFER_LEN;
	//std::cout<<"  最后一个包的大小为"<<last_num<<std::endl;
	rec_sum=0;
	while(rec_sum<last_num){
		rec_single = recv(sockConn, (char*)(pic_data)+rec_sum, last_num-rec_sum, 0);
		rec_sum +=rec_single;
		//std::cout<<"        本次接收/还要接收/已接受/共需接收 = "<<rec_single<<"/"<<last_num - rec_sum<<"/"<<rec_sum<<"/"<<last_num<<std::endl;
	}
	for(int i=0;i<last_num;i++)
		data_pic.push_back(pic_data[i]);

	if(data_pic.size()!=data_inf.lenth){
		printf("出错啦！！！\n");
		return -1;
	}
	//std::cout<<"    "<<packnum+1<<"个包接收完毕"<<std::endl;

	//解码
	image = cv::imdecode(data_pic, 1);
	//std::cout<<"图像解码完毕!"<<std::endl;
	return 1;
}

void SocketMatTransmissionServer::socketDisconnect(void)
{
	close(sockConn);
}