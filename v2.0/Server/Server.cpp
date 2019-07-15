#include "Server.h"
#include "ServerAti.h"


bool StartUp()
{
	//����Э��汾
	WSAStartup(MAKEWORD(2,2),&wsaData);
	if (LOBYTE(wsaData.wVersion)!=2||HIBYTE(wsaData.wVersion)!=2)
	{
		printf("����汾ʧ�ܣ�\n");
		return false;
	}
	printf("����汾�ɹ���\n");



	// ����socke
	serverSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (SOCKET_ERROR==serverSocket)
	{
		printf("����SOCKETʧ��\n");
		WSACleanup();
		return false;
	}
	printf("����SOCKET�ɹ���\n");


	//����Э���ַ��
	addr.sin_family=AF_INET;//Э��汾
	//addr.sin_addr.S_un.S_addr=inet_addr("192.168.43.76");
	addr.sin_addr.S_un.S_addr= htons(INADDR_ANY);
	addr.sin_port=htons(10086);//0-65535   10000����

	// �󶨶˿�
	int r=bind(serverSocket,(sockaddr*)&addr,sizeof addr);
	if (r==-1)
	{
		printf("bindʧ�ܣ�\n");
		closesocket(serverSocket);
		WSACleanup();
		return false;
	}
	printf("bind�ɹ���\n");
	
	//�������ݿ�
	if(mysql.Connect())
	{
		printf("�������ݿ�ɹ���\n");
	}else
	{
		printf("�������ݿ�ʧ�ܣ�\n");
		closesocket(serverSocket);
		WSACleanup();
		return false;
	}

	//����
	r=listen(serverSocket,10);
	if (r==-1)
	{
		printf("listenʧ�ܣ�\n");
		closesocket(serverSocket);
		WSACleanup();
		return false;
	}
	printf("listen�ɹ���\n");

	return true;
}


void WaitingConnection()
{
	while (1)
	{
		clientSocket[count]=accept(serverSocket,(sockaddr*)&cAddr,&len);
		if (SOCKET_ERROR==clientSocket[count])
		{
			printf("����������\n");
			closesocket(serverSocket);
			WSACleanup();
			return;
		}
		printf("�пͻ�������:%s\n",inet_ntoa(cAddr.sin_addr));
		stack<HANDLE> Control;
		TreadControl.insert(make_pair(count,Control));
		HANDLE smain=CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)Service,(char*)count,NULL,NULL);
		
		///////////////////
		//cout<<smain<<endl;//���̹��������
		/////////////////

		NowF.insert(make_pair(count,smain));//��ʼ���û����߳�
		stack<HANDLE> treadC;
		TreadControl.insert(make_pair(count,treadC));//��ʼ���߳̿���

		count++;
	}
}


void Service(int idx)
{
	while (1)
	{
		Send(idx,"1����¼ 2��ע��  3���޸����� 4���˳�ϵͳ");
		string res=Receive(idx);
		if (res.compare("1")==0)
		{
			if (!LogOn(idx))
			{
				Send(idx,"��¼ʧ�ܣ�");
			}else
			{
				Send(idx,"��½�ɹ���");
				while(1)
				{
					Send(idx,"ѡ�����0������ 1��˫�˻Ự 2�����˻��� 3���˳���¼");
					string back1=Receive(idx);
					if (back1.compare("1")==0)
					{
						TwoPersonCnversation(idx);

					}else if(back1.compare("2")==0)
					{
					}else if(back1.compare("3")==0)
					{
						LogOut(idx);
						break;
					}else if (!back1.compare("0")==0)
					{
						Send(idx,"��ѡ����ȷ����");
					}else
					{
						while (1)
						{
							Send(idx,"������...�������#awake���˳�����״̬��");
							if (Receive(idx).compare("#awake")==0)
							{
								break;
							}
						}
					}
				}
			}
		}else if (res.compare("2")==0)
		{
			Register(idx);
			
		}else if (res.compare("3")==0)
		{
			ChangePassword(idx);//����������ʾ

		}else if (res.compare("4")==0)
		{
			printf("�пͻ����˳�:%s\n",inet_ntoa(cAddr.sin_addr));
			Send(idx,"#�˳�ϵͳ");
			closesocket(clientSocket[idx]);  // ����Ӧ����һ��ӳ���IP��socke��Ž��ж�Ӧ���Ա�ѭ�����ÿռ�
			                                 //��������������˿ռ��˷�
			break;
		}else
		{
			Send(idx,"��������ȷ��ѡ����룡");
		}
	}	
}


void TreadReSet(int idx,HANDLE re)
{
	HANDLE now=NowF[idx];
	TreadControl[idx].push(now);  //���������м�������м����ֻ�ܴ���ֵ
	SuspendThread(now);
	//ResetEvent(now);
	NowF[idx]=re;

}

void TreadReSet(int idx)
{
	HANDLE now=NowF[idx];
	SuspendThread(now);
	//ResetEvent(now);
}
void ChangeNowT(int idx,HANDLE re)
{
	HANDLE now=NowF[idx];
	TreadControl[idx].push(now);
	//ResetEvent(now);
	NowF[idx]=re;
}

void TreadSet(int idx)
{
	/////////////////
	//cout<<TreadControl[idx].top()<<endl; //������ 
	////////////////

	ResumeThread(TreadControl[idx].top());
	NowF[idx]=TreadControl[idx].top();
	//ResumeThread(TreadControl[idx].top());
	TreadControl[idx].pop();
	//SetEvent(now);
}




bool LogOn(int idx)
{
	Send(idx,"�������û�����");
    string user=Receive(idx);
	if (!mysql.UserExist(user))
	{
		Send(idx,"�û������ڣ�");
		return false;
	}else
	{
		Send(idx,"���������룺");
		string password=Receive(idx);
		string key=mysql.Query(user,0);
		if (key.compare(password))
		{
			return false;
		}else
		{
			if (UserLogOn.count(user)==0)
			{
				UserLogOn.insert(make_pair(user,idx));
				UserLogOn2.insert(make_pair(idx,user));
				UserState.insert(make_pair(user,0));//��ʼ���û�״̬
			}else
			{
				UserLogOn[user]=idx;
				UserLogOn2.insert(make_pair(idx,user));
				UserState[user]=0;
			}
			return true;
		}
	}	
}

string Receive(int idx)
{
	char buff[1024];
	int r;
	r=recv(clientSocket[idx],buff,1023,NULL);
	if (r>0)
	{
		buff[r]=0;
		string res=buff;
		return res;
	}
	return "#404F";
}

bool Send(int idx,string data)
{
	if (send(clientSocket[idx],data.c_str(),data.length(),NULL)>0)
	{
		return true;
	}else
	{
		return false;
	}
	
}

void TwoPersonCnversation(int idx)
{
	Send(idx,"��Ҫ��˭ͨ����");
	string reb=Receive(idx);
	if (!mysql.UserExist(reb))
	{
		Send(idx,"û�д��û���");
	}else if (UserLogOn.count(reb)==0)
	{
		Send(idx,"���û������ߣ�");
	}else if (UserLogOn[reb]==-1)
	{
		Send(idx,"���û������ߣ�");
	}else if (UserState[reb]==1)
	{
		Send(idx,"���û�æ��");
	}else
	{
		TreadReSet(UserLogOn[reb]);

		Send(UserLogOn[reb],UserLogOn2[idx]+"�������ӣ�1����ͨ 2���ܾ�");
		Send(UserLogOn[reb],"#000");
		
		string back2=Receive(UserLogOn[reb]);
		if (back2.compare("1")==0)
		{
			Send(UserLogOn[reb],"�ѽ�ͨ�����롰#exit�������Ự��");
			Send(idx,"�ѽ�ͨ�����롰#exit�������Ự��");
			UserState[reb]=1;
			UserState[UserLogOn2[idx]]=1;

			txuser* a=new txuser;
			a->from=idx;
			a->to=UserLogOn[reb];
			txuser* b=new txuser;
			b->from=UserLogOn[reb];
			b->to=idx;
			HANDLE hThrd1=CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)tongxin,(LPVOID*)a,NULL,NULL);// �����߳̾��
			HANDLE hThrd2=CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)tongxin,(LPVOID*)b,NULL,NULL);

			//TreadReSet(UserLogOn[reb],hThrd2);
			ChangeNowT(UserLogOn[reb],hThrd2);

			DWORD  exitCode1=0;//
			DWORD  exitCode2=0;

			GetExitCodeThread(hThrd1, &exitCode1);  // ��̽ ָ���߳�(ͨ���̵߳�HANDLE) �Ƿ񼺾�����, 
			GetExitCodeThread(hThrd2, &exitCode2); //exitCode1�д洢�߳�״̬,����������,ֵΪSTILL_ACTIVE 
				                                  //����������,ֵΪ �̵߳ķ���ֵ(�� return ���ֵ)
			while(exitCode1==STILL_ACTIVE||exitCode2==STILL_ACTIVE)
			{
				GetExitCodeThread(hThrd1, &exitCode1);  
				GetExitCodeThread(hThrd2, &exitCode2);	
			}
			UserState[reb]=0;
			UserState[UserLogOn2[idx]]=0;
			TreadSet(UserLogOn[reb]);

		}else if(back2.compare("2")==0)
		{
			Send(UserLogOn[reb],"�Ѿܾ���");
			Send(idx,"�Է��Ѿܾ���");
		}else
		{
			Send(UserLogOn[reb],"�Ѿܾ���");
			Send(idx,"�Է��Ѿܾ���");
		}
	}
}


void tongxin(LPVOID* lpParam)
{
	txuser* info=(txuser*)lpParam;
	string buff;
	string name;
	name=UserLogOn2[info->from];
	while (1)
	{
		buff=Receive(info->from);
		//r=recv(clientSocket[info->from],buff,1023,NULL);
		//buff[r]=0;
		if (buff.compare("#exit")==0)
		{
			string duankao="#�Ự����";
			//send(clientSocket[info->from],duankao.c_str(),duankao.length(),NULL);
			send(clientSocket[info->to],duankao.c_str(),duankao.length(),NULL);
			break;
		}
		if (buff.compare("#OK")==0)
		{
			//string du="#�ɹ������Ự��";
			//send(clientSocket[info->from],du.c_str(),du.length(),NULL);
			break;
		}
		buff=name+":"+buff;
		Send(info->to,buff);
		//send(clientSocket[info->to],buff,strlen(buff),NULL);		
	}
	
}

void LogOut(int idx)
{
	string name=UserLogOn2[idx];
	UserLogOn[name]=-1;
	UserLogOn2.erase(idx);//�Ƴ�

	/*Send(idx,"�������û�����");
	string user=Receive(idx);
	Send(idx,"���������룺");
	string password=Receive(idx);
	string key=mysql.Query(user,0);
	if (key.compare(password))
	{
		return false;
	}else
	{
		if (UserLogOn.count(user)==0)
		{
			UserLogOn.insert(make_pair(user,idx));
			UserLogOn2.insert(make_pair(idx,user));
		}else
		{
			UserLogOn[user]=idx;
			UserLogOn2.insert(make_pair(idx,user));
		}
		return true;
	}*/

}

bool ChangePassword(int idx)
{
	Send(idx,"�������û�����");
	string username=Receive(idx);
	if (!mysql.UserExist(username))
	{
		Send(idx,"�û������ڣ�");
		return false;
	}else
	{
		while(1)
		{
			Send(idx,"������ԭ���룺");
			string recpassword=Receive(idx);
			string mysqlres=mysql.Query(username,0);
			if (mysqlres.compare(recpassword)==0)
			{
				Send(idx,"�����������룺");
				string password1=Receive(idx);
				Send(idx,"���ٴ����������룺");
				string password2=Receive(idx);
				if (password1.compare(password2)==0)
				{
					if (mysql.UpdatePassword(username,password1))
					{
						Send(idx,"�޸ĳɹ���");
						return true;
					}else
					{
						Send(idx,"�޸�ʧ�ܣ�");
						return false;
					}	
				}else{
					Send(idx,"�������벻һ��!�Ƿ������޸ģ�Y/N");
					string ar=Receive(idx);
					if (ar.compare("N")==0)
					{
						return false;
					}
				}	
			}else
			{
				Send(idx,"ԭ��������Ƿ��������룺Y/N");
				string ar=Receive(idx);
				if (ar.compare("N")==0)
				{
					return false;
				}
			}
		}

	}
	
}


bool Register(int idx){
	while (1)
	{
		Send(idx,"�������µ��û�����");
		string username=Receive(idx);
		bool myres=mysql.UserExist(username);
		if (!myres)
		{
			Send(idx,"���������룺");
			string password1=Receive(idx);
			Send(idx,"���ٴ��������룺");
			string password2=Receive(idx);
			if (password1.compare(password2)==0)
			{
				mysql.Insert(username,password1);
				Send(idx,"ע��ɹ���");
				return true;
			}else{
				Send(idx,"�������벻һ��!�Ƿ�����ע�᣺Y/N");
				string ar=Receive(idx);
				if (ar.compare("N")==0)
				{
					return false;
				}
			}
		}else
		{
			Send(idx,"�û�����ռ�ã��Ƿ�����ע�᣺Y/N");
			string ar=Receive(idx);
			if (ar.compare("N")==0)
			{
				return false;
			}
		}
	}	

}
