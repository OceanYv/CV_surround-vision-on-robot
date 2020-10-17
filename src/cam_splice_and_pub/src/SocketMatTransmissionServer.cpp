#include "SocketMatTransmissionServer.h"
 
SocketMatTransmissionServer::SocketMatTransmissionServer(void){}
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
	int returnflag = 0;
	count = 0;
	memset(&data,0,sizeof(data));

	//接收信息数据
	memset(&data_inf,0,needrec1);
	int rec_sum=0,rec_single=0;			//接收储存图片信息的结构体
	while(rec_sum<needrec1){
		rec_single = recv(sockConn, (char*)(&data_inf) + rec_sum, needrec1 - rec_sum, 0);
		rec_sum +=rec_single;
	}

	//接收图像数据
	rec_sum=0,rec_single=0;
	//std::cout<<"接下来应接收的数据大小为："<<data_inf.lenth<<std::endl;
	while(rec_sum<data_inf.lenth){
		rec_single = recv(sockConn, (char*)(pic_data)+rec_sum, data_inf.lenth, 0);
		rec_sum +=rec_single;
		//std::cout<<"data_inf.lenth - rec_sum/rec_sum/data_inf.lenth = "<<data_inf.lenth - rec_sum<<"/"<<rec_sum<<"/"<<data_inf.lenth<<std::endl;
	}

	//解码
	std::vector<unsigned char> data_pic;
	for(int i=0;i<data_inf.lenth;i++)
		data_pic.push_back(pic_data[i]);
	if ((pic_data[0] != 0xFF || pic_data[1] != 0xD8)){
		data_pic.insert(data_pic.begin(), 0xFF);
		data_pic.insert(data_pic.begin()+1, 0xD8);
	}
	image = cv::imdecode(data_pic, 1);

	return 1;
}

void SocketMatTransmissionServer::socketDisconnect(void)
{
	close(sockConn);
}