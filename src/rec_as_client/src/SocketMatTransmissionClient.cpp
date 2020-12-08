#include "SocketMatTransmissionClient.h"
 
SocketMatTransmissionClient::SocketMatTransmissionClient(void){
	pic_count = 0;
	//std::cout<<"sizeof(sentbuf_lenth):"<<needrec1<<std::endl;
}
SocketMatTransmissionClient::~SocketMatTransmissionClient(void){}
 
int SocketMatTransmissionClient::socketConnect(const char* IP, int PORT){
	
	struct sockaddr_in    servaddr; 
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	if (inet_pton(AF_INET, IP, &servaddr.sin_addr) <= 0) {
		printf("inet_pton error for %s\n", IP);
		return -1;
	}

	if ((sockClient = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
		return -1;
	}

	int to_flag=0,waitime=WAITIME;
	if (waitime ==-1)
		waitime=10000;
	waitime=ceil(1.0*waitime/2);
	while(to_flag<waitime){
		if (connect(sockClient, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
			printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
			printf("Retrying...\n");
			to_flag++;
			cv::waitKey(2000);
		}
		else {
			printf("connect successful!\n");
			break;
		}
	}
	if(to_flag==waitime){
		printf("Fail to connect to the server at %s of %d s.\n",IP,WAITIME);
		return -1;
	}
	return 1;
}

int SocketMatTransmissionClient::receive(cv::Mat& image)
{
	//接收信息数据
	pic_count++;
	memset(&data_inf,0,NEEDREC1);
	int rec_sum=0,rec_single=0;			//接收储存图片信息的结构体
	while(rec_sum<NEEDREC1){
		rec_single = recv(sockClient, (char*)(&data_inf) + rec_sum, NEEDREC1 - rec_sum, 0);
		if(rec_single==0)
			return -1;
		rec_sum +=rec_single;
	}

	//接收图像数据
	int packnum = floor(1.0*data_inf.lenth/BUFFER_LEN);
	//std::cout<<"第"<<pic_count<<"张图片的大小为："<<data_inf.lenth<<",分为"<<packnum+1<<"个包接收"<<std::endl;
	std::vector<unsigned char> data_pic;

	for(int n=0;n<packnum;n++){			//接收前packnum个包
		rec_sum=0,rec_single=0;
		while(rec_sum<BUFFER_LEN){
			rec_single = recv(sockClient, (char*)(pic_data)+rec_sum, BUFFER_LEN-rec_sum, 0);
			if(rec_single==0)
				return -1;
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
		rec_single = recv(sockClient, (char*)(pic_data)+rec_sum, last_num-rec_sum, 0);
		if(rec_single==0)
			return -1;
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
	//cv::waitKey(5);
	//send(sockClient,"FINISH_RECEIVE!",16,0);
	//解码
	image = cv::imdecode(data_pic, 1);
	//std::cout<<"图像解码完毕!"<<std::endl;
	return 1;
}

void SocketMatTransmissionClient::socketDisconnect(void)
{
	close(sockClient);
}