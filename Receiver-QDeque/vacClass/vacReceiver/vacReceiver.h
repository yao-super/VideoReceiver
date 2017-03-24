#ifndef VACVIDEORECEIVER_H
#define VACVIDEORECEIVER_H

#include <iostream>
#include <vector>
#include <deque>
#include <WinSock2.h>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <QDialog>
#include <QThread>
#include <QMutex>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
using namespace cv;
# define PACKAGE_SIZE 1024

typedef struct{
	unsigned char packagebuf[PACKAGE_SIZE];
	int flag;
	int pkgs;
	int size;
} DataPackage;

//static QMutex mutex;
static deque<DataPackage> m_picque1;
static deque<DataPackage> m_picque2;
static int flag1 = 0;
static int flag2 = 1;

class vacVideoReceiver:public QThread
{
public:
	vacVideoReceiver();
	~vacVideoReceiver();
private:
	void InitalVideoSocket();
	void storeVideo(deque<DataPackage>& m_picque);
	SOCKET m_VideoSocketReceiver;
	sockaddr_in m_VideoSendAddr;
	sockaddr_in m_VideoRecvAddrR;
	DataPackage m_RecvPackage;
	int m_sendaddrLen;
	//vector<unsigned char> m_Imgresult;

protected:
	void run();
};

class vacVideoShow:public QThread
{

private:
	void ShowVideo(deque<DataPackage>& m_picque);
	DataPackage m_RecvPackage;
	vector<unsigned char> m_Imgresult;

protected:
	void run();
};


#endif //VACVIDEORECEIVER_H