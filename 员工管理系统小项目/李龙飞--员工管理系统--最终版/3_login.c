#include "main.h"

int do_login(info *acc,int newfd)
{
	info *acc0 = acc;
	//打开数据库，如果数据库不存在则创建数据库
	sqlite3* db = NULL;
	if(sqlite3_open("./information.db",&db) != SQLITE_OK)
	{
		fprintf(stderr,"__%d__sqlite3_open:[%d] %s\n",__LINE__,sqlite3_errcode(db),sqlite3_errmsg(db));
		return -1;
	}
//	printf("sqlite3_open success\n");
	//判断用户是否注册，并查询密码是否正确，并查询是否是管理员
	
	//通过sqlite3_get_table函数查询数据库
	char sql[128] = "";
	sprintf(sql,"select * from user_info where \"工号\" = %d",acc0->num);
	char **pres = NULL;
	int row,column;
	char *errmsg = NULL;
	if(sqlite3_get_table(db,sql,&pres,&row,&column,&errmsg) != SQLITE_OK)
	{
		fprintf(stderr,"__%d__ sqlite3_get_table:%s\n",__LINE__,errmsg);
		return -1;
	}
//	printf("get table success\n");
	//跟据查询结果的行列，判断是否存在当前用户信息，即是否注册
	//row == 0,不存在,row == 1，存在并判断密码是否正确
	if(row == 0)
	{
		acc0->ack[0] = 'N';
		sprintf(acc0->ack+1,"当前用户未注册，请注册后登录......");
		printf("用户[%s]未注册，请注册后登录......\n",acc0->name);
		if(send(newfd,acc0,sizeof(*acc0),0) < 0)
		{
			perror("send");
			return -1;
		}
		//关闭数据库
		sqlite3_free_table(pres);
		sqlite3_close(db);
		return -1;//登录失败返回　-1
	}
	//用户信息存在
//	printf("row=%d column=%d\n", row, column);
//	printf("[%s]\n",pres[11]);
//	printf("[%s]\n",pres[12]);
//	printf("[%s]\n",pres[13]);

	if(strcmp(acc0->name,pres[11]) != 0 || strcmp(acc0->passworld,pres[12]) != 0)
	{
		acc0->ack[0] = 'N';
		sprintf(acc0->ack+1,"用户名密码有误，登录失败......");
		printf("用户名[%s]或密码有误，登录失败......\n",acc0->name);
		if(send(newfd,acc0,sizeof(*acc0),0) < 0)
		{
			perror("send");
			return -1;
		}
		//关闭数据库
		sqlite3_free_table(pres);
		sqlite3_close(db);
		return -1;//登录失败返回　-1
	}
//	printf("密码正确\n");
	//************************密码正确**************************
	
	//密码正确，在【数据库】创建一张 [online] 表，如果存在则直接打开
	bzero(sql,sizeof(sql));
	sprintf(sql,"create table if not exists online(num int primary key)");
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != 0)
	{
		fprintf(stderr,"__%d__sqlite3_exec:%s\n",__LINE__,errmsg);
		return -1;
	}
//	printf("create online success\n");
	//将当前登录成功　用户名　添加其中
	bzero(sql,sizeof(sql));
	sprintf(sql,"insert into online values(%d)",acc0->num);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != 0)
	{
		//fprintf(stderr,"__%d__sqlite3_exec:%s\n",__LINE__,errmsg);
		acc0->ack[0] = 'N';
		sprintf(acc0->ack+1,"当前用户已登录，登录失败......");
		printf("用户名[%s]已登录，登录失败......\n",acc0->name);
		if(send(newfd,acc0,sizeof(*acc0),0) < 0)
		{
			perror("send");
			return -1;
		}
		//关闭数据库
		sqlite3_free_table(pres);
		sqlite3_close(db);
		return -1;//登录失败返回　-1
	}
//	printf("insert success\n");
//	printf("用户名[%s]登录成功......\n",acc0->name);

	//************************无重复登录，登录成功***********************
	
	//给客户端发送反馈包
	acc0->ack[0] = 'Y';
	if(strcmp("1",pres[13]) == 0)
	{
		acc0->flag = 1;//是否是管理员
	}
	else
	{
		acc0->flag = 0;//不是管理员
	}
//	printf("flag = %d\n",acc0->flag);
	sprintf(acc0->ack+1,"用户名密码正确，允许登录......");
	if(1 == acc0->flag)
	{
		printf("管理员[%s]登录成功......\n",acc0->name);
	}
	else
	{
		printf("普通用户[%s]登录成功......\n",acc0->name);
	}

	if(send(newfd,acc0,sizeof(*acc0),0) < 0)
	{
		perror("send");
		return -1;
	}
	//printf("发送数据成功\n");

	//关闭数据库
	sqlite3_free_table(pres);
	sqlite3_close(db);
	
	return 1;//登录成功返回 1
}
