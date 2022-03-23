#include "main.h"

int do_delete(info *acc,int newfd)
{

	info *acc0 = acc;
	//打开数据库，如果数据库不存在则创建数据库
	sqlite3* db = NULL;
	if(sqlite3_open("./information.db",&db) != SQLITE_OK)
	{
		fprintf(stderr,"__%d__sqlite3_open:[%d] %s\n",__LINE__,sqlite3_errcode(db),sqlite3_errmsg(db));
		return -1;
	}

	//在表中比较输入的工号和姓名是否一致，避免删除错误

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
	//跟据查询结果的行列，判断用户是否注册
	if(row == 0)
	{
		acc0->ack[0] = 'N';
		sprintf(acc0->ack+1,"未注册，删除失败......");
		printf("未注册，删除失败......\n");
		if(send(newfd,acc0,sizeof(*acc0),0) < 0)
		{
			perror("send");
			return -1;
		}
		//关闭数据库
		sqlite3_free_table(pres);
		sqlite3_close(db);
		return -1;//
	}
	//如果注册，判断工号姓名是否一致
	if(strcmp(acc0->name,pres[11]) != 0 )
	{
		acc0->ack[0] = 'N';
		sprintf(acc0->ack+1,"姓名有误，删除失败......");
		printf("姓名有误，删除失败......\n");
		if(send(newfd,acc0,sizeof(*acc0),0) < 0)
		{
			perror("send");
			return -1;
		}
		//关闭数据库
		sqlite3_free_table(pres);
		sqlite3_close(db);
		return -1;//失败返回　-1
	}

	//在user_info表中删除用户信息
	sprintf(sql,"delete from user_info where \"工号\" = %d and \"姓名\" = \"%s\"",acc0->num,acc0->name);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != 0)
	{
		fprintf(stderr,"__%d__do_insert:%s\n",__LINE__,errmsg);
		//给客户端发送删除失败提示
		acc0->ack[0] = 'N';
		sprintf(acc0->ack+1,"******删除失败******");
	}
	else
	{
		//给客户端发送删除成功提示
		acc0->ack[0] = 'Y';
		sprintf(acc0->ack+1,"**********删除成功*********");
	}
	if(send(newfd,acc0,sizeof(*acc0),0) < 0)
	{
		perror("send");
		return -1;
	}
	printf("%s\n",acc0->ack+1);
	//关闭数据库
	sqlite3_free_table(pres);
	sqlite3_close(db);
	return 0;
}
