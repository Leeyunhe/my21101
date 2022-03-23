#include "main.h"

int do_logout(info * acc,int newfd)
{

	info *acc0 = acc;
	//打开数据库，如果数据库不存在则创建数据库
	sqlite3* db = NULL;
	if(sqlite3_open("./information.db",&db) != SQLITE_OK)
	{
		fprintf(stderr,"__%d__sqlite3_open:[%d] %s\n",__LINE__,sqlite3_errcode(db),sqlite3_errmsg(db));
		return -1;
	}
	//printf("sqlite3_open success\n");
	//将当前用户从online表中删除
	char sql[128] = "";
	char *errmsg = NULL;
	sprintf(sql,"delete from online where num = %d",acc0->num);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != 0)
	{
		fprintf(stderr,"__%d__sqlite3_exec:%s\n",__LINE__,errmsg);
		acc0->ack[0] = 'N';
		sprintf(acc0->ack+1,"退出失败......");
	}
	else
	{
		//退出成功
		acc0->ack[0] = 'Y';
		printf("退出成功......\n");
		sprintf(acc0->ack+1,"退出成功......");
	}
	//发送数据包
	if(send(newfd,acc0,sizeof(*acc0),0) < 0)
	{
		perror("send");
		return -1;
	}

	//关闭数据库
	sqlite3_close(db);
	return 1;
}
