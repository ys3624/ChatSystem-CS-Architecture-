#include "Client.h"
#include "ClientAti.h"

void acceptSecv(){
	char recvBuff[1024];
	while(1)
	{
		int r=recv(clientSocket,recvBuff,1023,NULL);
		if (r>0)
		{
			recvBuff[r]=0;
			if (strcmp(recvBuff,"#会话结束")==0)
			{
				string rback="#OK";
				send(clientSocket,rback.c_str(),rback.length(),NULL);
				cout<<"#会话结束"<<endl;

			}else if (strcmp(recvBuff,"#退出系统")==0)
			{
				printf("感谢使用\n");
				exit(0);
			}else if(strcmp(recvBuff,"#000")==0)
			{
				string rback="0";
				send(clientSocket,rback.c_str(),rback.length(),NULL);
			}else
			{
				printf("%s\n",recvBuff);
			}
		}
	}
}


bool StartUp()
{
	//请求协议版本
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);
	if (LOBYTE(wsaData.wVersion)!=2||HIBYTE(wsaData.wVersion)!=2)
	{
		printf("请求版本失败！\n");
		return false;
	}
	printf("请求版本成功！\n");



	// 创建socke
	clientSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (SOCKET_ERROR==clientSocket)
	{
		printf("创建SOCKET失败\n");
		WSACleanup();
		return false;
	}
	printf("请求SOCKET成功！\n");


	//获取协议地址族
	SOCKADDR_IN addr={0};
	addr.sin_family=AF_INET;//协议版本
	addr.sin_addr.S_un.S_addr=inet_addr("192.168.43.76");
	addr.sin_port=htons(10086);//0-65535   10000左右


	// 连接服务器
	int r=connect(clientSocket,(sockaddr*)&addr,sizeof addr);
	if (r==-1)
	{
		printf("连接服务器失败!\n");
		closesocket(clientSocket);
		WSACleanup();
		return false;
	}
	printf("连接服务器成功！\n");

	//初始界面
	cout<<"********************************"<<endl;
	cout<<"欢迎使用帅讯聊天系统！"<<endl;
	//cout<<"1、登录 2、注册  3、修改密码 4、退出"<<endl;


	//通信
	CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)acceptSecv,NULL,NULL,NULL);
	char buff[1024];
	while (1)
	{
		memset(buff,0,1024);
	    //printf("我：");
		scanf("%s",buff);
		send(clientSocket,buff,strlen(buff),NULL);
	}
}

