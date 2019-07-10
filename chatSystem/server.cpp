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
			//�㲥����
			for (int i=0;i<count;i++ )
			{
				send(clientSocket[i],buff,strlen(buff),NULL);
			}
		}
	}
}

int main()
{
	//����Э��汾
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);
	if (LOBYTE(wsaData.wVersion)!=2||HIBYTE(wsaData.wVersion)!=2)
	{
		printf("����汾ʧ�ܣ�\n");
		return -1;
	}
	printf("����汾�ɹ���\n");



	// ����socke
	SOCKET serverSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (SOCKET_ERROR==serverSocket)
	{
		printf("����SOCKETʧ��\n");
		WSACleanup();
		return -2;
	}
	printf("����SOCKET�ɹ���\n");


	//����Э���ַ��
	SOCKADDR_IN addr={0};
	addr.sin_family=AF_INET;//Э��汾
	addr.sin_addr.S_un.S_addr=inet_addr("192.168.43.76");
	addr.sin_port=htons(10086);//0-65535   10000����

	// �󶨶˿�
	int r=bind(serverSocket,(sockaddr*)&addr,sizeof addr);
	if (r==-1)
	{
		printf("bind�ɹ���\n");
		closesocket(serverSocket);
		WSACleanup();
		return -3;
	}
	printf("bind�ɹ���\n");

	//����
	r=listen(serverSocket,10);
	if (r==-1)
	{
		printf("listen�ɹ���\n");
		closesocket(serverSocket);
		WSACleanup();
		return -3;
	}
	printf("listen�ɹ���\n");


	//�ȴ��ͻ������� ����
	//�ͻ���Э���ַ��
	SOCKADDR_IN cAddr={0};
	int len=sizeof(cAddr);
	while (1)
	{
		clientSocket[count]=accept(serverSocket,(sockaddr*)&cAddr,&len);
		if (SOCKET_ERROR==clientSocket[count])
		{
			printf("����������\n");
			closesocket(serverSocket);
			WSACleanup();
			return -4;
		}
		printf("�пͻ�������:%s\n",inet_ntoa(cAddr.sin_addr));
		
		CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)tongxin,(char*)count,NULL,NULL);

		
		count++;
	}
	


	////ͨѶ
	//char buff[1024];
	//while(1)
	//{
	//	r=recv(clientSocket[],buff,1023,NULL);
	//	if (r>0)
	//	{
	//		buff[r]=0;// ���\0	
	//		printf(">>%s\n",buff);
	//	}

	//}

	return 0;
}