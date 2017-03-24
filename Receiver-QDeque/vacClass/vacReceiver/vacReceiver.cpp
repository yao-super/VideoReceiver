#include "vacReceiver.h"

vacVideoReceiver::vacVideoReceiver()
{
	InitalVideoSocket();
};

void vacVideoReceiver::InitalVideoSocket()
{
	WSADATA wsaData;

	if(WSAStartup(MAKEWORD(2,2),&wsaData) != 0)
	{
		cout<<"WSAStartup failed!"<<endl;
		exit(-1);
	}

	m_VideoSocketReceiver=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(m_VideoSocketReceiver == INVALID_SOCKET)
	{
		cout<<"socket failed!"<<endl;
		WSACleanup();
		exit(-1);
	}

	m_sendaddrLen = sizeof(m_VideoSendAddr);
	m_VideoRecvAddrR.sin_family = AF_INET;
	m_VideoRecvAddrR.sin_port = htons(9999);
	m_VideoRecvAddrR.sin_addr.s_addr = htonl(INADDR_ANY);
	int retval = bind(m_VideoSocketReceiver,(sockaddr*)&m_VideoRecvAddrR,sizeof(m_VideoRecvAddrR));
	if(SOCKET_ERROR == retval)
	{
		cout<<"bind error !"<<endl;
		closesocket(m_VideoSocketReceiver);
		return;
	}
}

void vacVideoReceiver::storeVideo(deque<DataPackage>& m_picque)
{
		int n = recvfrom(m_VideoSocketReceiver,(char*)(&m_RecvPackage),sizeof(m_RecvPackage),0,(sockaddr*)&m_VideoSendAddr,&m_sendaddrLen);
		if(n == -1)
		{
			cout<<"RecvVideo failed"<<endl;
			return;
		}

		if (m_RecvPackage.flag!=0)
			;
		else
		{
    		m_picque.push_back(m_RecvPackage);
			int Paknum = m_RecvPackage.pkgs;
			bool mark = true;
			for(int i=1;i<Paknum && mark;++i)
			{
				recvfrom(m_VideoSocketReceiver,(char*)(&m_RecvPackage),sizeof(m_RecvPackage),0,(sockaddr*)&m_VideoSendAddr,&m_sendaddrLen);
				if(i==m_RecvPackage.flag)
				{					
					m_picque.push_back(m_RecvPackage);
				}
				else
					break;
			}
		}
	
}

void vacVideoReceiver::run()
{
	while(true)
	{
		if(flag1==0)
			storeVideo(m_picque1);
		else
			storeVideo(m_picque2);
	}
}
vacVideoReceiver::~vacVideoReceiver()
{
	closesocket(m_VideoSocketReceiver);
	WSACleanup();
}

void vacVideoShow::run()
{
	Sleep(2);
	while (true)
	{
		if(flag2==1)
			ShowVideo(m_picque2);
		else
			ShowVideo(m_picque1);
	}
	
}

void vacVideoShow::ShowVideo(deque<DataPackage>& m_picque)
{
	string temp;
	string resuit;

	bool mark =true;

	while (m_picque.empty())
	{
		swap(flag1,flag2);
	}

	m_RecvPackage = m_picque.front();
	m_picque.pop_front();

	temp.assign((const char*)m_RecvPackage.packagebuf,m_RecvPackage.size);
	resuit+=temp;
	int Paknum = m_RecvPackage.pkgs;
		for(int i=1;i<Paknum;++i)
		{

			while (m_picque.empty())
			{
				swap(flag1,flag2);
			}
			m_RecvPackage = m_picque.front();
			m_picque.pop_front();

			if(i==m_RecvPackage.flag)
			{
				temp.assign((const char*)m_RecvPackage.packagebuf,m_RecvPackage.size);
				resuit+=temp;
			}
			else
			{
				mark = false;
				break;
			}
		}
		if(mark==true)
		{ 
			m_Imgresult.assign(resuit.begin(),resuit.end());
			Mat imgresult = imdecode(Mat(m_Imgresult),CV_LOAD_IMAGE_COLOR);
			cvNamedWindow("imshowReceiver",0);
			cvResizeWindow("imshowReceiver",640,480);
			imshow("imshowReceiver",imgresult); 
			cvWaitKey(33);
		}
		resuit="";
		m_Imgresult.clear();
	
}