#ifndef SERVERATI_H
#define SERVERATI_H

SOCKADDR_IN cAddr={0};
int len=sizeof(cAddr);
SOCKET clientSocket[1024];
int count=0;

MySQL_Operation mysql;

WSADATA wsaData;
SOCKET serverSocket;
SOCKADDR_IN addr={0};
map<string,int> UserLogOn;
map<int,string> UserLogOn2;
map<int,stack<HANDLE>> TreadControl;
map<int,HANDLE> NowF;
map<string,int> UserState;

#endif
