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
			if (strcmp(recvBuff,"#�Ự����")==0)
			{
				string rback="#OK";
				send(clientSocket,rback.c_str(),rback.length(),NULL);
				cout<<"#�Ự����"<<endl;

			}else if (strcmp(recvBuff,"#�˳�ϵͳ")==0)
			{
				printf("��лʹ��\n");
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
	//����Э��汾
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);
	if (LOBYTE(wsaData.wVersion)!=2||HIBYTE(wsaData.wVersion)!=2)
	{
		printf("����汾ʧ�ܣ�\n");
		return false;
	}
	printf("����汾�ɹ���\n");



	// ����socke
	clientSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (SOCKET_ERROR==clientSocket)
	{
		printf("����SOCKETʧ��\n");
		WSACleanup();
		return false;
	}
	printf("����SOCKET�ɹ���\n");


	//��ȡЭ���ַ��
	SOCKADDR_IN addr={0};
	addr.sin_family=AF_INET;//Э��汾
	addr.sin_addr.S_un.S_addr=inet_addr("192.168.43.76");
	addr.sin_port=htons(10086);//0-65535   10000����


	// ���ӷ�����
	int r=connect(clientSocket,(sockaddr*)&addr,sizeof addr);
	if (r==-1)
	{
		printf("���ӷ�����ʧ��!\n");
		closesocket(clientSocket);
		WSACleanup();
		return false;
	}
	printf("���ӷ������ɹ���\n");

	//��ʼ����
	cout<<"********************************"<<endl;
	cout<<"��ӭʹ��˧Ѷ����ϵͳ��"<<endl;
	//cout<<"1����¼ 2��ע��  3���޸����� 4���˳�"<<endl;


	//ͨ��
	CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)acceptSecv,NULL,NULL,NULL);
	char buff[1024];
	while (1)
	{
		memset(buff,0,1024);
	    //printf("�ң�");
		scanf("%s",buff);
		send(clientSocket,buff,strlen(buff),NULL);
	}
}

