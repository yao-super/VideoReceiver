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

void vacVideoReceiver::run()
{
	
	while (true)
	{
		int n = recvfrom(m_VideoSocketReceiver,(char*)(&m_RecvPackage),sizeof(m_RecvPackage),0,(sockaddr*)&m_VideoSendAddr,&m_sendaddrLen);
		if(n == -1)
		{
			cout<<"RecvVideo failed"<<endl;
			return;
		}

		if (m_RecvPackage.flag!=0)
			continue;
		else
		{
			mutex.lock();
			m_picque.push_back(m_RecvPackage);
			mutex.unlock();
			
			int Paknum = m_RecvPackage.pkgs;
			bool mark = true;
			for(int i=1;i<Paknum && mark;++i)
			{
				recvfrom(m_VideoSocketReceiver,(char*)(&m_RecvPackage),sizeof(m_RecvPackage),0,(sockaddr*)&m_VideoSendAddr,&m_sendaddrLen);
				if(i==m_RecvPackage.flag)
				{
					mutex.lock();
					m_picque.push_back(m_RecvPackage);
					mutex.unlock();
				}
				else
				  break;
			}
        }
}
}
vacVideoReceiver::~vacVideoReceiver()
{
	closesocket(m_VideoSocketReceiver);
	WSACleanup();
}

void vacVideoShow::run()
{
	string temp;
	string resuit;
	
	while (true)
	{
	 bool mark =true;
	 mutex.lock();
	 while (m_picque.empty())
	 {
		 mutex.unlock();
		 Sleep(2);
	 }
	 
	 m_RecvPackage = m_picque.front();
	 m_picque.pop_front();
	 mutex.unlock();
	 temp.assign((const char*)m_RecvPackage.packagebuf,m_RecvPackage.size);
	 resuit+=temp;
	 int Paknum = m_RecvPackage.pkgs;
	 for(int i=1;i<Paknum;++i)
	 {
		 mutex.lock();
		 while (m_picque.empty())
		 {
			 mutex.unlock();
			 Sleep(2);
		 }
		 m_RecvPackage = m_picque.front();
		 m_picque.pop_front();
		 mutex.unlock();
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
}