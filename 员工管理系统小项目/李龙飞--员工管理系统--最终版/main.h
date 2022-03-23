#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <time.h>
#include <sqlite3.h>

#define PORT 8888 				//范围：1024~49151
//#define IP "192.168.0.113" 		//填IP，终端输入ifconfig查找,采用外部输入的方式

#define ERR_MSG(msg) do{\
    fprintf(stderr,"__%d__",__LINE__);\
    perror(msg);\
}while(0)

typedef struct information{
	char operation;//操作码,A注册，B登录，C增，D删，E改，F查 G退出
	char ack[128];//反馈信息，0位占用，Y成功，N失败
	char name[20];//姓名
	char passworld[20];//密码
	int flag;//是否为管理员，１是，０否
	char sex[5];//性别，m男,f女,
	int age;//年龄
	int num;//工号
	char location[128];//地址
	int salary;//薪资
	char phonenumber[20];//电话
	char section[20];//部门
	int mark[8];//标记要修改信息的内容，１，修改，０不修改
	//[0]~[6]密码，性别，年龄，部门，电话，地址，薪资
}info;

typedef void (*sighandler_t)(int);

int do_register(info *acc,int newfd);
int do_login(info *acc,int newfd);
int do_logout(info *acc,int newfd);
int do_delete(info *acc,int newfd);
int do_change(info *acc,int newfd);
int do_search(info *acc,int newfd);

#endif
