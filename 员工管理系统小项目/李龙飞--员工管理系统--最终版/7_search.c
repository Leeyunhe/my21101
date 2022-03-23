#include "main.h"

int do_search(info *acc,int newfd)
{
	info *acc0 = (info*)malloc(sizeof(info));
	
	acc0->operation = acc->operation;
	acc0->num = acc->num;
	acc0->flag = acc->flag;
	
	//打开数据库，如果数据库不存在则创建数据库
	sqlite3* db = NULL;
	if(sqlite3_open("./information.db",&db) != SQLITE_OK)
	{
		fprintf(stderr,"__%d__sqlite3_open:[%d] %s\n",__LINE__,sqlite3_errcode(db),sqlite3_errmsg(db));
		return -1;
	}

	//根据输入的工号进行查询
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
		sprintf(acc0->ack+1,"未注册，查询失败......");
		printf("未注册，查询失败......\n");
		if(send(newfd,acc0,sizeof(*acc0),0) < 0)
		{
			perror("send");
			return -1;
		}
		//关闭数据库
		free(acc0);
		sqlite3_free_table(pres);
		sqlite3_close(db);
		return -1;//
	}
	acc0->ack[0] = 'Y';
	sprintf(acc0->ack+1,"**********查询成功*********");

	//如果注册，查询到员工信息，则根据是否为管理员查询，返回不同的结果
/*
	printf("row=%d column=%d\n", row, column);
	printf("工号:%s  姓名:%s  密码:%s\n",pres[10],pres[11],pres[12]);
	printf("性别:%s  年龄:%s  地址:%s\n",pres[14],pres[15],pres[16]);
	printf("电话:%s  薪资:%s  部门:%s\n",pres[18],pres[17],pres[19]);

	printf("工号:%d  姓名:%s  密码:%s\n",acc0->num,acc0->name,acc0->passworld);
	printf("性别:%s  年龄:%d  地址:%s\n",acc0->sex,acc0->age,acc0->location);
	printf("电话:%s  薪资:%d  部门:%s\n",acc0->phonenumber,acc0->salary,acc0->section);
*/

	//查询成功封数据包
	if(1 == acc0->flag)//管理员有额外的查询结果
	{
			bzero(acc0->location,sizeof(acc0->location));
		sprintf(acc0->location,"%s",pres[16]);//地址
		acc0->salary = atoi(pres[17]);//薪资
			bzero(acc0->passworld,sizeof(acc0->passworld));
		sprintf(acc0->passworld,"%s",pres[12]);//密码
	}
	//将查询出来的pres[]中的数据（字符）通过atoi函数转换成整形，赋值给int变量
	acc0->num = atoi(pres[10]);//工号
		bzero(acc0->name,sizeof(acc0->name));
	sprintf(acc0->name,"%s",pres[11]);//名字
		bzero(acc0->sex,sizeof(acc0->sex));
	sprintf(acc0->sex,"%s",pres[14]);//性别
	acc0->age = atoi(pres[15]);//年龄
		bzero(acc0->phonenumber,sizeof(acc0->phonenumber));
	sprintf(acc0->phonenumber,"%s",pres[18]);//电话
		bzero(acc0->section,sizeof(acc0->section));
	sprintf(acc0->section,"%s",pres[19]);//部门

	if(send(newfd,acc0,sizeof(*acc0),0) < 0)
	{
		perror("send");
		return -1;
	}

	printf("%s\n",acc0->ack+1);
	//关闭数据库
	free(acc0);
	sqlite3_free_table(pres);
	sqlite3_close(db);
	return 0;
}
