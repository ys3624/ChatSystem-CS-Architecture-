#include "Server.h"
#include "ServerAti.h"


bool StartUp()
{
	//请求协议版本
	WSAStartup(MAKEWORD(2,2),&wsaData);
	if (LOBYTE(wsaData.wVersion)!=2||HIBYTE(wsaData.wVersion)!=2)
	{
		printf("请求版本失败！\n");
		return false;
	}
	printf("请求版本成功！\n");



	// 创建socke
	serverSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (SOCKET_ERROR==serverSocket)
	{
		printf("创建SOCKET失败\n");
		WSACleanup();
		return false;
	}
	printf("请求SOCKET成功！\n");


	//创建协议地址族
	addr.sin_family=AF_INET;//协议版本
	//addr.sin_addr.S_un.S_addr=inet_addr("192.168.43.76");
	addr.sin_addr.S_un.S_addr= htons(INADDR_ANY);
	addr.sin_port=htons(10086);//0-65535   10000左右

	// 绑定端口
	int r=bind(serverSocket,(sockaddr*)&addr,sizeof addr);
	if (r==-1)
	{
		printf("bind失败！\n");
		closesocket(serverSocket);
		WSACleanup();
		return false;
	}
	printf("bind成功！\n");
	
	//连接数据库
	if(mysql.Connect())
	{
		printf("连接数据库成功！\n");
	}else
	{
		printf("连接数据库失败！\n");
		closesocket(serverSocket);
		WSACleanup();
		return false;
	}

	//监听
	r=listen(serverSocket,10);
	if (r==-1)
	{
		printf("listen失败！\n");
		closesocket(serverSocket);
		WSACleanup();
		return false;
	}
	printf("listen成功！\n");

	return true;
}


void WaitingConnection()
{
	while (1)
	{
		clientSocket[count]=accept(serverSocket,(sockaddr*)&cAddr,&len);
		if (SOCKET_ERROR==clientSocket[count])
		{
			printf("服务器错误！\n");
			closesocket(serverSocket);
			WSACleanup();
			return;
		}
		printf("有客户端连接:%s\n",inet_ntoa(cAddr.sin_addr));
		stack<HANDLE> Control;
		TreadControl.insert(make_pair(count,Control));
		HANDLE smain=CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)Service,(char*)count,NULL,NULL);
		
		///////////////////
		//cout<<smain<<endl;//进程管理测试用
		/////////////////

		NowF.insert(make_pair(count,smain));//初始化用户主线程
		stack<HANDLE> treadC;
		TreadControl.insert(make_pair(count,treadC));//初始化线程控制

		count++;
	}
}


void Service(int idx)
{
	while (1)
	{
		Send(idx,"1、登录 2、注册  3、修改密码 4、退出系统");
		string res=Receive(idx);
		if (res.compare("1")==0)
		{
			if (!LogOn(idx))
			{
				Send(idx,"登录失败！");
			}else
			{
				Send(idx,"登陆成功！");
				while(1)
				{
					Send(idx,"选择服务：0、挂起 1、双人会话 2、多人会议 3、退出登录");
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
						Send(idx,"请选择正确服务！");
					}else
					{
						while (1)
						{
							Send(idx,"挂起中...输入命令“#awake”退出挂起状态！");
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
			ChangePassword(idx);//函数内置提示

		}else if (res.compare("4")==0)
		{
			printf("有客户端退出:%s\n",inet_ntoa(cAddr.sin_addr));
			Send(idx,"#退出系统");
			closesocket(clientSocket[idx]);  // 这里应建议一个映射表将IP与socke编号进行对应，以便循环利用空间
			                                 //现在这样做造成了空间浪费
			break;
		}else
		{
			Send(idx,"请输入正确的选项代码！");
		}
	}	
}


void TreadReSet(int idx,HANDLE re)
{
	HANDLE now=NowF[idx];
	TreadControl[idx].push(now);  //不能申请中间变量，中间变量只能传递值
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
	//cout<<TreadControl[idx].top()<<endl; //测试用 
	////////////////

	ResumeThread(TreadControl[idx].top());
	NowF[idx]=TreadControl[idx].top();
	//ResumeThread(TreadControl[idx].top());
	TreadControl[idx].pop();
	//SetEvent(now);
}




bool LogOn(int idx)
{
	Send(idx,"请输入用户名：");
    string user=Receive(idx);
	if (!mysql.UserExist(user))
	{
		Send(idx,"用户不存在！");
		return false;
	}else
	{
		Send(idx,"请输入密码：");
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
				UserState.insert(make_pair(user,0));//初始化用户状态
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
	Send(idx,"需要与谁通话？");
	string reb=Receive(idx);
	if (!mysql.UserExist(reb))
	{
		Send(idx,"没有此用户！");
	}else if (UserLogOn.count(reb)==0)
	{
		Send(idx,"该用户不在线！");
	}else if (UserLogOn[reb]==-1)
	{
		Send(idx,"该用户不在线！");
	}else if (UserState[reb]==1)
	{
		Send(idx,"该用户忙！");
	}else
	{
		TreadReSet(UserLogOn[reb]);

		Send(UserLogOn[reb],UserLogOn2[idx]+"请求连接！1、接通 2、拒绝");
		Send(UserLogOn[reb],"#000");
		
		string back2=Receive(UserLogOn[reb]);
		if (back2.compare("1")==0)
		{
			Send(UserLogOn[reb],"已接通！输入“#exit”结束会话！");
			Send(idx,"已接通！输入“#exit”结束会话！");
			UserState[reb]=1;
			UserState[UserLogOn2[idx]]=1;

			txuser* a=new txuser;
			a->from=idx;
			a->to=UserLogOn[reb];
			txuser* b=new txuser;
			b->from=UserLogOn[reb];
			b->to=idx;
			HANDLE hThrd1=CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)tongxin,(LPVOID*)a,NULL,NULL);// 创建线程句柄
			HANDLE hThrd2=CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)tongxin,(LPVOID*)b,NULL,NULL);

			//TreadReSet(UserLogOn[reb],hThrd2);
			ChangeNowT(UserLogOn[reb],hThrd2);

			DWORD  exitCode1=0;//
			DWORD  exitCode2=0;

			GetExitCodeThread(hThrd1, &exitCode1);  // 刺探 指定线程(通过线程的HANDLE) 是否己经结束, 
			GetExitCodeThread(hThrd2, &exitCode2); //exitCode1中存储线程状态,若正在运行,值为STILL_ACTIVE 
				                                  //若己经结束,值为 线程的返回值(即 return 后的值)
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
			Send(UserLogOn[reb],"已拒绝！");
			Send(idx,"对方已拒绝！");
		}else
		{
			Send(UserLogOn[reb],"已拒绝！");
			Send(idx,"对方已拒绝！");
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
			string duankao="#会话结束";
			//send(clientSocket[info->from],duankao.c_str(),duankao.length(),NULL);
			send(clientSocket[info->to],duankao.c_str(),duankao.length(),NULL);
			break;
		}
		if (buff.compare("#OK")==0)
		{
			//string du="#成功结束会话！";
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
	UserLogOn2.erase(idx);//移除

	/*Send(idx,"请输入用户名：");
	string user=Receive(idx);
	Send(idx,"请输入密码：");
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
	Send(idx,"请输入用户名：");
	string username=Receive(idx);
	if (!mysql.UserExist(username))
	{
		Send(idx,"用户不存在！");
		return false;
	}else
	{
		while(1)
		{
			Send(idx,"请输入原密码：");
			string recpassword=Receive(idx);
			string mysqlres=mysql.Query(username,0);
			if (mysqlres.compare(recpassword)==0)
			{
				Send(idx,"请输入新密码：");
				string password1=Receive(idx);
				Send(idx,"请再次输入新密码：");
				string password2=Receive(idx);
				if (password1.compare(password2)==0)
				{
					if (mysql.UpdatePassword(username,password1))
					{
						Send(idx,"修改成功！");
						return true;
					}else
					{
						Send(idx,"修改失败！");
						return false;
					}	
				}else{
					Send(idx,"两次输入不一致!是否重新修改：Y/N");
					string ar=Receive(idx);
					if (ar.compare("N")==0)
					{
						return false;
					}
				}	
			}else
			{
				Send(idx,"原密码错误！是否重新输入：Y/N");
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
		Send(idx,"请输入新的用户名：");
		string username=Receive(idx);
		bool myres=mysql.UserExist(username);
		if (!myres)
		{
			Send(idx,"请输入密码：");
			string password1=Receive(idx);
			Send(idx,"请再次输入密码：");
			string password2=Receive(idx);
			if (password1.compare(password2)==0)
			{
				mysql.Insert(username,password1);
				Send(idx,"注册成功！");
				return true;
			}else{
				Send(idx,"两次输入不一致!是否重新注册：Y/N");
				string ar=Receive(idx);
				if (ar.compare("N")==0)
				{
					return false;
				}
			}
		}else
		{
			Send(idx,"用户名被占用！是否重新注册：Y/N");
			string ar=Receive(idx);
			if (ar.compare("N")==0)
			{
				return false;
			}
		}
	}	

}
