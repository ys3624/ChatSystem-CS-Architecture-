#ifndef MY_SQL_OPERATION_H
#define MY_SQL_OPERATION_H

//#include<WinSock.h>
#include <WinSock2.h>
#include <mysql.h>
#include <string>
#include <iostream>
using namespace std;
#pragma comment(lib,"libmysql.lib")


class MySQL_Operation
{
public:
   bool Connect();
   bool FreeConnect();
   string Query(string qus,int target);
   bool Insert(string name,string password);
   bool Insert(string name);
   bool UpdatePassword(string name,string password);
   bool UserExist(string name);
private:
	MYSQL con;
	MYSQL_RES *res;//是否可以不作为类属性
};




#endif MY_SQL_OPERATION_H
