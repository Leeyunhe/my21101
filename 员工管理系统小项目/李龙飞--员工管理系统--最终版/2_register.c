#include "main.h"

int do_register(info *acc,int newfd)
{
	info *acc0 = acc;
	//打开数据库，如果数据库不存在则创建数据库
	sqlite3* db = NULL;
	if(sqlite3_open("./information.db",&db) != SQLITE_OK)
	{
		fprintf(stderr,"__%d__sqlite3_open:[%d] %s\n",__LINE__,sqlite3_errcode(db),sqlite3_errmsg(db));
		return -1;
	}
	//创建一个张表，如果存在则直接打开
	char sql[450] = "create table if not exists user_info(\"工号\" int primary key,\
					 \"姓名\" char,\"密码\" int,flag int,\"性别\" char,\"年龄\" int,\
					 \"地址\" char,\"薪资\" int,\"电话\" int,\"部门\" char)";	
	char *errmsg = NULL;
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != 0)
	{
		fprintf(stderr,"__%d__sqlite3_exec:%s\n",__LINE__,errmsg);
		return -1;
	}

	/*
	printf("%c\n",acc0->operation);
	printf("%d\n",acc0->num);
	printf("%s\n",acc0->name);
	printf("%s\n",acc0->passworld);
	printf("%d\n",acc0->flag);
	*/

	//注册，将用户信息插入到用户信息数据库中，若当前注册用户已存在，则反馈注册失败，已注册
	bzero(sql,sizeof(sql));
	if('A' == acc0->operation)
	{
		sprintf(sql,"insert into user_info(\"姓名\",\"密码\",\"工号\",flag,\"年龄\",\"薪资\",\"性别\",\"地址\",\"电话\",\"部门\") \
				values(\"%s\",\"%s\",%d,%d,'-','-',\"-\",\"-\",\"-\",\"-\")"\
				,acc0->name,acc0->passworld,acc0->num,acc0->flag);
	}
	if('C' == acc0->operation)
	{
		sprintf(sql,"insert into user_info(\"姓名\",\"密码\",\"工号\",flag,\"年龄\",\"薪资\",\"性别\",\"地址\",\"电话\",\"部门\") \
				values(\"%s\",\"%s\",%d,%d,%d,%d,\"%s\",\"%s\",\"%s\",\"%s\")"\
				,acc0->name,acc0->passworld,acc0->num,acc0->flag,acc0->age,acc0->salary,acc0->sex,acc0->location,acc0->phonenumber,acc0->section);
	}
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != 0)
	{
		fprintf(stderr,"__%d__do_insert:%s\n",__LINE__,errmsg);
		//给客户端发送注册失败提示
		acc0->ack[0] = 'N';
		sprintf(acc0->ack+1,"******注册失败，用户名已存在******");
	}
	else
	{
		//给客户端发送注册成功提示
		acc0->ack[0] = 'Y';
		sprintf(acc0->ack+1,"**********注册成功*********");
	}
	if(send(newfd,acc0,sizeof(*acc0),0) < 0)
	{
		perror("send");
		return -1;
	}
	printf("%s\n",acc0->ack+1);
	//关闭数据库
	sqlite3_close(db);
	return 0;
}
