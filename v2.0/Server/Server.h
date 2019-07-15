#ifndef SERVER_H
#define SERVER_H

#include "MySQL_Operation.h"

#include <stdio.h>
#include <Windows.h>
#include <string>
#include <iostream>
#include <map>
#include <stack>


#pragma comment(lib,"ws2_32.lib")
using namespace std;


typedef struct TXUSER
{
	int from;
	int to;
}txuser;


void MainService(int idx);
bool StartUp();
void WaitingConnection();
void Service(int idx);
bool LogOn(int idx);
void LogOut(int idx);
string Receive(int idx);
bool Send(int idx,string data);
void TwoPersonCnversation(int idx);
void tongxin(LPVOID* lpParam);
bool ChangePassword(int idx);
bool Register(int idx);

void TreadReSet(int idx,HANDLE re);
void TreadReSet(int idx);
void TreadSet(int idx);
void ChangeNowT(int idx,HANDLE re);

#endif
