#include <stdio.h>
#include <Windows.h>
#include <graphics.h>//easyX
#pragma comment(lib,"ws2_32.lib")



SOCKET clientSocket;
HWND hWnd;
int count;

void acceptSecv(){
	char recvBuff[1024];
	while(1)
	{
		int r=recv(clientSocket,recvBuff,1023,NULL);
		if (r>0)
		{
			recvBuff[r]=0;
			outtextxy(0,count*20,recvBuff);
			count++;
		}
	}
}




int main()
{
	//初始化界面
	hWnd=initgraph(300,400,SHOWCONSOLE);



	//请求协议版本
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);
	if (LOBYTE(wsaData.wVersion)!=2||HIBYTE(wsaData.wVersion)!=2)
	{
		printf("请求版本失败！\n");
		return -1;
	}
	printf("请求版本成功！\n");



	// 创建socke
    clientSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (SOCKET_ERROR==clientSocket)
	{
		printf("创建SOCKET失败\n");
		WSACleanup();
		return -2;
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
		return -1;
	}
	printf("连接服务器成功！\n");


	//通信
	char buff[1024];
	CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)acceptSecv,NULL,NULL,NULL);
	while (1)
	{
		memset(buff,0,1024);
		printf("说点啥：");
		scanf("%s",buff);
		send(clientSocket,buff,strlen(buff),NULL);
	}

	return 0;
}