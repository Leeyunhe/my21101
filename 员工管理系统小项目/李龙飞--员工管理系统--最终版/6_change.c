#include "main.h"

int do_change(info * acc,int newfd)
{

	info *acc0 = acc;
	//打开数据库，如果数据库不存在则创建数据库
	sqlite3* db = NULL;
	if(sqlite3_open("./information.db",&db) != SQLITE_OK)
	{
		fprintf(stderr,"__%d__sqlite3_open:[%d] %s\n",__LINE__,sqlite3_errcode(db),sqlite3_errmsg(db));
		return -1;
	}

	//通过sqlite3_get_table函数查询数据库
	char sql[300] = "";
	sprintf(sql,"select * from user_info where \"工号\" = %d",acc0->num);
	char **pres = NULL;
	int row,column;
	char *errmsg = NULL;
	if(sqlite3_get_table(db,sql,&pres,&row,&column,&errmsg) != SQLITE_OK)
	{
		fprintf(stderr,"__%d__ sqlite3_get_table:%s\n",__LINE__,errmsg);
		return -1;
	}

	//如果是管理员查询，则判断员工是否注册，以及姓名工号是否一致
	if(1 == acc0->flag)
	{
		//跟据查询结果的行列，判断用户是否注册
		if(row == 0)
		{
			acc0->ack[0] = 'N';
			sprintf(acc0->ack+1,"该员工未注册，修改失败......");
			printf("该员工未注册，修改失败......\n");
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
			sprintf(acc0->ack+1,"工号与姓名不符，修改失败......");
			printf("工号与姓名不符，修改失败......\n");
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
	}

	//员工信息存在，根据工号进行修改，1,修改,0,不修改
	//[0]~[6]密码，性别，年龄，部门，电话，地址，薪资

	//printf("[0] = [%d]\n",acc0->mark[0]);

	if(1 == acc0->mark[0])
	{
		printf("正在修改密码......\n");
		//在user_info表中修改用户信息
		bzero(sql,sizeof(sql));
		sprintf(sql,"update user_info set \"密码\" =\"%s\" where \"工号\" = %d",acc0->passworld,acc0->num);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != 0)
		{
			fprintf(stderr,"__%d__do_update:%s\n",__LINE__,errmsg);
			//给客户端发送失败提示
			acc0->ack[0] = 'N';
			sprintf(acc0->ack+1,"******修改失败******");
		}
	}
	if(1 == acc0->mark[1])
	{
		printf("正在修改性别......\n");
		//在user_info表中修改用户信息
		bzero(sql,sizeof(sql));
		sprintf(sql,"update user_info set \"性别\" = \"%s\" where \"工号\" = %d",acc0->sex,acc0->num);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != 0)
		{
			fprintf(stderr,"__%d__do_insert:%s\n",__LINE__,errmsg);
			//给客户端发送失败提示
			acc0->ack[0] = 'N';
			sprintf(acc0->ack+1,"******修改失败******");
		}
	}
	if(1 == acc0->mark[2])
	{
		printf("正在修改年龄......\n");
		//在user_info表中修改用户信息
		bzero(sql,sizeof(sql));
		sprintf(sql,"update user_info set \"年龄\" = %d where \"工号\" = %d",acc0->age,acc0->num);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != 0)
		{
			fprintf(stderr,"__%d__do_insert:%s\n",__LINE__,errmsg);
			//给客户端发送失败提示
			acc0->ack[0] = 'N';
			sprintf(acc0->ack+1,"******修改失败******");
		}
	}
	if(1 == acc0->mark[3])
	{
		printf("正在修改部门......\n");
		//在user_info表中修改用户信息
		bzero(sql,sizeof(sql));
		sprintf(sql,"update user_info set \"部门\" = \"%s\" where \"工号\" = %d",acc0->section,acc0->num);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != 0)
		{
			fprintf(stderr,"__%d__do_insert:%s\n",__LINE__,errmsg);
			//给客户端发送失败提示
			acc0->ack[0] = 'N';
			sprintf(acc0->ack+1,"******修改失败******");
		}
	}
	if(1 == acc0->mark[4])
	{
		printf("正在修改电话......\n");
		//在user_info表中修改用户信息
		bzero(sql,sizeof(sql));
		sprintf(sql,"update user_info set \"电话\" = \"%s\" where \"工号\" = %d",acc0->phonenumber,acc0->num);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != 0)
		{
			fprintf(stderr,"__%d__do_insert:%s\n",__LINE__,errmsg);
			//给客户端发送失败提示
			acc0->ack[0] = 'N';
			sprintf(acc0->ack+1,"******修改失败******");
		}
	}
	if(1 == acc0->mark[5])
	{
		printf("正在修改地址......\n");
		//在user_info表中修改用户信息
		bzero(sql,sizeof(sql));
		sprintf(sql,"update user_info set \"地址\" = \"%s\" where \"工号\" = %d",acc0->location,acc0->num);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != 0)
		{
			fprintf(stderr,"__%d__do_insert:%s\n",__LINE__,errmsg);
			//给客户端发送失败提示
			acc0->ack[0] = 'N';
			sprintf(acc0->ack+1,"******修改失败******");
		}
	}
	if(1 == acc0->mark[6])
	{
		printf("正在修改薪资......\n");
		//在user_info表中修改用户信息
		bzero(sql,sizeof(sql));
		sprintf(sql,"update user_info set \"薪资\" = %d where \"工号\" = %d",acc0->salary,acc0->num);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != 0)
		{
			fprintf(stderr,"__%d__do_insert:%s\n",__LINE__,errmsg);
			//给客户端发送失败提示
			acc0->ack[0] = 'N';
			sprintf(acc0->ack+1,"******修改失败******");
		}
	}

	if(acc0->ack[0] != 'N')
	{
		//给客户端发送删除成功提示
		acc0->ack[0] = 'Y';
		sprintf(acc0->ack+1,"**********修改成功*********");
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
