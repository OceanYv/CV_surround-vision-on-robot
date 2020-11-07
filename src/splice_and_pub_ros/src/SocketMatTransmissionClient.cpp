#include "splice_and_pub/SocketMatTransmissionClient.h"
 
SocketMatTransmissionClient::SocketMatTransmissionClient(void){}
SocketMatTransmissionClient::~SocketMatTransmissionClient(void){}
 
int SocketMatTransmissionClient::socketConnect(const char* IP, int PORT)
{
	struct sockaddr_in servaddr;
 
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
 
	if (connect(sockClient, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
		return -1;
	}
	else 
		printf("connect successful!\n");

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
	cv::imencode(".jpg", image, data_pic,param);
	//std::cout<<"压缩为jpg之后，大小为："<<data_pic.size()<<"        压缩率为："<<1.0*(data_pic.size())/(IMG_WIDTH*IMG_HEIGHT*3)<<std::endl;

	//发送图片大小数据包
	data_inf.lenth = data_pic.size();
	int packnum = floor(1.0*data_inf.lenth/BUFFER_LEN);
	//std::cout <<"发送信息中的大小为："<<data_inf.lenth<<",共需发送" << packnum+1 <<"个包"<<std::endl;
	if (send(sockClient, (char *)(&data_inf),NEEDSED1, 0) < 0){
		printf("send image error: %s(errno: %d)\n", strerror(errno), errno);
		return -1;
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
			return -1;
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
		printf("出错啦！！！\n");
		return -1;
	}
	if (send(sockClient, (char *)(pic_data), last_num, 0) < 0){		//发送包
		printf("send image error: %s(errno: %d)\n", strerror(errno), errno);
		return -1;
	}
	//std::cout << "    "<<packnum+1<<"个包发送完毕"<<std::endl;

	return 1;
}

void SocketMatTransmissionClient::socketDisconnect(void)
{
	close(sockClient);
}