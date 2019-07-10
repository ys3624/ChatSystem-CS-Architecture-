#include <stdio.h>
#include <Windows.h>

#pragma comment(lib,"ws2_32.lib")
SOCKADDR_IN cAddr={0};
int len=sizeof(cAddr);
SOCKET clientSocket[1024];
int count=0;

void tongxin(int idx)
{
	char buff[1024];
	int r;
	while (1)
	{
		r=recv(clientSocket[idx],buff,1023,NULL);
		if (r>0)
		{
			buff[r]=0;
			printf("%d:%s\n",idx,buff);
			//广播数据
			for (int i=0;i<count;i++ )
			{
				send(clientSocket[i],buff,strlen(buff),NULL);
			}
		}
	}
}

int main()
{
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
	SOCKET serverSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (SOCKET_ERROR==serverSocket)
	{
		printf("创建SOCKET失败\n");
		WSACleanup();
		return -2;
	}
	printf("请求SOCKET成功！\n");


	//创建协议地址族
	SOCKADDR_IN addr={0};
	addr.sin_family=AF_INET;//协议版本
	addr.sin_addr.S_un.S_addr=inet_addr("192.168.43.76");
	addr.sin_port=htons(10086);//0-65535   10000左右

	// 绑定端口
	int r=bind(serverSocket,(sockaddr*)&addr,sizeof addr);
	if (r==-1)
	{
		printf("bind成功！\n");
		closesocket(serverSocket);
		WSACleanup();
		return -3;
	}
	printf("bind成功！\n");

	//监听
	r=listen(serverSocket,10);
	if (r==-1)
	{
		printf("listen成功！\n");
		closesocket(serverSocket);
		WSACleanup();
		return -3;
	}
	printf("listen成功！\n");


	//等待客户端连接 阻塞
	//客户端协议地址族
	SOCKADDR_IN cAddr={0};
	int len=sizeof(cAddr);
	while (1)
	{
		clientSocket[count]=accept(serverSocket,(sockaddr*)&cAddr,&len);
		if (SOCKET_ERROR==clientSocket[count])
		{
			printf("服务器错误！\n");
			closesocket(serverSocket);
			WSACleanup();
			return -4;
		}
		printf("有客户端连接:%s\n",inet_ntoa(cAddr.sin_addr));
		
		CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)tongxin,(char*)count,NULL,NULL);

		
		count++;
	}
	


	////通讯
	//char buff[1024];
	//while(1)
	//{
	//	r=recv(clientSocket[],buff,1023,NULL);
	//	if (r>0)
	//	{
	//		buff[r]=0;// 添加\0	
	//		printf(">>%s\n",buff);
	//	}

	//}

	return 0;
}