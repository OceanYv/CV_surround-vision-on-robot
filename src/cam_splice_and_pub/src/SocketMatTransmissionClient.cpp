#include "SocketMatTransmissionClient.h"
 
SocketMatTransmissionClient::SocketMatTransmissionClient(void){}
SocketMatTransmissionClient::~SocketMatTransmissionClient(void){}
 
int SocketMatTransmissionClient::socketConnect(const char* IP, int PORT)
{
	struct sockaddr_in    servaddr;
 
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
	if(image.cols != IMG_WIDTH || image.rows != IMG_HEIGHT || image.type() != CV_8UC3){
		printf("the image must satisfy : cols == IMG_WIDTH（%d）  rows == IMG_HEIGHT（%d） type == CV_8UC3\n\n", IMG_WIDTH, IMG_HEIGHT);
		return -1;
	}

	//编码，并存入数组pic_data
	data_inf.lenth = 0;
	std::vector<unsigned char> data_pic;
	std::vector<int> param = std::vector<int>(2);
	param[0] = 1;			//CV_IMWRITE_JPEG_QUALITY
	param[1] = 80; 											// default(95) 0-100
	cv::imencode(".jpg", image, data_pic,param);			//根据测试，800*600分辨率下，压缩后的大小在190000~450000范围内，稳定网络下（100	M宽带）帧率可达13
	//std::cout<<"压缩为jpg之后，大小为："<<data_pic.size()<<"        压缩率为："<<1.0*(data_pic.size())/(IMG_WIDTH*IMG_HEIGHT*3)<<std::endl;
	for(std::vector<unsigned char> ::iterator it =data_pic.begin(); it !=data_pic.end();it++){
		pic_data[data_inf.lenth] = *it;
		data_inf.lenth++;
	}

	//发送数据
	std::cout<<"发送信息中的大小为："<<data_inf.lenth<<std::endl;
	if (send(sockClient, (char *)(&data_inf),NEEDSED1, 0) < 0){				//发送储存图片信息的结构体
		printf("send image error: %s(errno: %d)\n", strerror(errno), errno);
		return -1;
	}
	if (send(sockClient, (char *)(pic_data), data_inf.lenth, 0) < 0){						//发送储存图片的结构体
		printf("send image error: %s(errno: %d)\n", strerror(errno), errno);
		return -1;
	}

	return 1;
}

void SocketMatTransmissionClient::socketDisconnect(void)
{
	close(sockClient);
}