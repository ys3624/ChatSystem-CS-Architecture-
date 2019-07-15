#include "MySQL_Operation.h"


bool MySQL_Operation::Connect()
{
	//MYSQL con;
    this->con=*(mysql_init((MYSQL*)0));
	/*const string host="localhost";
	const string user="root";
	const string password="123456";
	const string dbname="user";*/
    
	const char host[]="localhost";
	const char user[]="root";
	const char password[]="123";
	const char dbname[]="user_info";
	const int port=3306;

	if (mysql_real_connect(&this->con,host,user,password,dbname,port,NULL,0))
	{
		//cout<<"���ݿ����ӳɹ���"<<endl;
		return true;
	}else
	{
		//cout<<"���ݿ�����ʧ��"<<endl;
		const char* error=mysql_error(&con);
		cout<<error<<endl;
		return false;
	}
}

bool MySQL_Operation::FreeConnect()
{
	try
   {
	mysql_free_result(res);
	mysql_close(&this->con);
   }catch(...)
   {
	   cout<<"�Ͽ�����ʧ�ܣ�"<<endl;
	   return false;
   }
   return true;
}

string MySQL_Operation::Query(string qus,int target)
{
	string squery;
	if (target==0)
	{
		squery="select password from user where name=\""+qus+"\"";
		//cout<<squery<<endl;
	}else if (target==1)
	{
		squery="select id from user where name=\""+qus+"\"";
		//cout<<squery<<endl;
	}else
	{
		return "#";
	}
	mysql_query(&this->con,"set names gbk");
	mysql_query(&this->con,squery.c_str());
	res=mysql_store_result(&this->con);

	////////////
	/*char* str_field;
	str_field=mysql_fetch_field(res)->name;
	cout<<"str_field:"<<str_field<<endl;*/

	MYSQL_ROW column;
	/*while (column=mysql_fetch_row(res))
	{
	cout<<column[0]<<endl;
	}*/
	column=mysql_fetch_row(res);
	string res;
	res=column[0];
	return res;
}

bool MySQL_Operation::Insert(string name)
{
	string sinsert;
	sinsert="insert into user(name) values(\""+name+"\")";
	mysql_query(&this->con,sinsert.c_str());
	return true;
}

bool MySQL_Operation::Insert(string name,string password)
{
	string sinsert;
	sinsert="insert into user(name,password) values(\""+name+"\","+"\""+password+"\")";
	if (mysql_query(&this->con,sinsert.c_str()))
	{
		cout<<mysql_error(&this->con)<<endl;
		return false;
	}else
	{
        return true;
	}	
}

bool MySQL_Operation::UpdatePassword(string name,string password)
{
	/*string supdate;
	string newpassword1;
	string newpassword2;
	newpassword1=MySQL_Operation::Query(name,0);
	if (newpassword1.compare(password))
	{
		cout<<"�������"<<endl;
		return false;
	}else
	{
		cout<<"�����������룺";
		cin>>newpassword1;
	}
	cout<<"���ٴ��������룺";
	cin>>newpassword2;
	if (newpassword1.compare(newpassword2))
	{
		cout<<"���벻һ�£�"<<endl;
		return false;
	}else
	{
		supdate="update user set password="+password+"\""+"where name=\""+name+"\"";
		mysql_query(&this->con,supdate.c_str());
	}*/
	string supdate;
	supdate="update user set password=\""+password+"\""+" where name=\""+name+"\"";
	if (mysql_query(&this->con,supdate.c_str()))
	{
		cout<<mysql_error(&this->con)<<endl;
		return false;
	}else
	{
		return true;
	}	
}

bool MySQL_Operation::UserExist(string name)
{
	string squery;
	squery="select count(*) from user where name=\""+name+"\""+"limit 1";
	
	mysql_query(&this->con,"set names gbk");
	mysql_query(&this->con,squery.c_str());
	res=mysql_store_result(&this->con);

	////////////
	/*char* str_field;
	str_field=mysql_fetch_field(res)->name;
	cout<<"str_field:"<<str_field<<endl;*/

	MYSQL_ROW column;
	/*while (column=mysql_fetch_row(res))
	{
	cout<<column[0]<<endl;
	}*/
	column=mysql_fetch_row(res);
	string res;
	res=column[0];
	if (res.compare("1")==0)
	{
		return true;
	}else
	{
		return false;
	}
}

