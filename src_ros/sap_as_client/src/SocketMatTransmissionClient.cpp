#include "splice_and_pub/SocketMatTransmissionClient.h"
 
SocketMatTransmissionClient::SocketMatTransmissionClient(void){}
SocketMatTransmissionClient::~SocketMatTransmissionClient(void){}
 
int SocketMatTransmissionClient::socketConnect(const char* IP, int PORT)
{

	if ((sockClient = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
		return -1;
	}
 
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	if (inet_pton(AF_INET, IP, &servaddr.sin_addr) <= 0) {
		printf("inet_pton error for %s\n", IP);
		return -1;
	}
 
	int flag=0,waitime;
	nh_.param("/tcp/waitime",waitime,10);
	if(waitime == -1)
		waitime = 100000;
	waitime = ceil(1.0*waitime/2);
	while(flag < waitime){
		if (connect(sockClient, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
			printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
			printf("Retrying...\n");
			flag++;
			cv::waitKey(2000);
		}
		else{
			printf("connect successful!\n");
			break;
		}
 	}
	 if(flag == waitime){
		 printf("Fail to connect to server %s for five times.", IP);
		 return 0;
	 }
 	return 1;
}
 
int SocketMatTransmissionClient::transmit(cv::Mat image)
{
	if (image.empty()){
		printf("empty image\n\n");
		return -1;
	}
	if(image.type() != CV_8UC3){
		printf("the image must satisfy : type == CV_8UC3\n\n");
		return -1;
	}

	//编码，并存入数组pic_data
	std::vector<unsigned char> data_pic;
	std::vector<int> param = std::vector<int>(2);
	param[0] = 1;			//CV_IMWRITE_JPEG_QUALITY
	param[1] = 90; 											// default(95) 0-100
        ROS_INFO_STREAM("Thread "<<sockClient<<": 21 begin to yasuo.");
	cv::imencode(".jpg", image, data_pic,param);
	ROS_INFO_STREAM("    压缩为jpg之后，大小为："<<data_pic.size()<<" 压缩率为："<<1.0*(data_pic.size())/(1280*720*3));

	//发送图片大小数据包
	data_inf.lenth = data_pic.size();
	int packnum = floor(1.0*data_inf.lenth/BUFFER_LEN);
	//std::cout <<"发送信息中的大小为："<<data_inf.lenth<<",共需发送" << packnum+1 <<"个包"<<std::endl;
	if (send(sockClient, (char *)(&data_inf),NEEDSED1, 0) < 0){
		printf("send image error: %s(errno: %d)\n", strerror(errno), errno);
		return reconnect();
	}

	//发送图片内容数据包2
	std::vector<unsigned char> ::iterator it=data_pic.begin();				//vector的迭代器
	for(int i=0 ; i<packnum ; i++){												//发送前packnum个包
		int p = 0;
		while(p<BUFFER_LEN){
			pic_data[p] = *it;
			it++;p++;
		}
		if (send(sockClient, (char *)(pic_data), BUFFER_LEN, 0) < 0){
			printf("send image error: %s(errno: %d)\n", strerror(errno), errno);
			return reconnect();
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
		return reconnect();
	}
	//std::cout << "    "<<packnum+1<<"个包发送完毕"<<std::endl;

	return 1;
}

int SocketMatTransmissionClient::reconnect(void)
{
	int flag=0,waitime,port;
	std::string ip;
	nh_.param("/tcp/waitime",waitime,30);
	nh_.param<std::string>("/tcp/ip",ip,"192.168.0.1");
	nh_.param("/tcp/port",port,6666);
	
	close(sockClient);
	printf("Try connect to server again, it maybe will take 5~10 seconds...\n");
	cv::waitKey(3000);

	if(waitime == -1)
		waitime = 100000;
        printf("waitime = %d\n",waitime);

	if ((sockClient = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
		return -1;
	}
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	if (inet_pton(AF_INET, ip.c_str(), &servaddr.sin_addr) <= 0) {
		printf("inet_pton error for %s\n", ip.c_str());
		return -1;
	}

	while(flag < waitime && nh_.ok()){
		if (connect(sockClient, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
			cv::waitKey(1000);
			flag++;
			//printf("Try connect to server again...\n");
		}	
		else{
			printf("Recovery connection successfully!\n");
			break;
		}
 	}
	 if(flag == waitime){
		 printf("fail to reconnect to server, the Client will be closed.\n");
		 socketDisconnect();
		 return -1;
	 }
	 return 2;
}

void SocketMatTransmissionClient::socketDisconnect(void)
{
	close(sockClient);
}
