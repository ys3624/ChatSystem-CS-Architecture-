#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <Windows.h>
#include <string>
#include <iostream>
//#include <graphics.h>//easyX
#pragma comment(lib,"ws2_32.lib")
using namespace std;


void acceptSecv();
bool StartUp();
void SendMassage();

#endif
