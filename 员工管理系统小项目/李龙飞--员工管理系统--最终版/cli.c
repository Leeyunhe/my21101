#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#define PORT_s 	8888 				//范围：1024~49151
//#define IP_s 	"192.168.0.113" 	//填IP，终端输入 ifconfig查找,通过外部传参

#define ERR_MSG(msg) do{\
	fprintf(stderr,"__%d__",__LINE__);\
	perror(msg);\
}while(0)

typedef struct information{
	char operation;//操作码,A注册，B登录，C增，D删，E改，F查
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

int num = 0;//记录登录成功的用户，用于注销登录状态
int do_register(int sfd);
int do_login(int sfd);
int do_recv(int sfd);
int do_logout(int sfd);
int do_add(int sfd);
int do_delete(int sfd);
int do_change(int sfd,int flag);
int do_search(int sfd,int flag);

int temp_sfd;
typedef void (*sighandler_t)(int);
void handler(int sig)
{
	//注销该客户端的登录信息
	//即给服务器发送 退出包
	do_logout(temp_sfd);	
	//关闭主进程，退出
	exit(10);
	return;
}

int main(int argc, const char *argv[])
{
	//外部传参输入IP 地址
	if(argc < 2)
	{
		printf("请输入服务器IP 地址>>>>\n");
		return -1;
	}
	//创建流式套接字
	int sfd = socket(AF_INET,SOCK_STREAM,0);
	if(sfd < 0)
	{
		ERR_MSG("sfd");
		return -1;
	}
	//填充服务器的地址信息结构体，main 7 IP_s
	struct sockaddr_in sin;
	sin.sin_family 		= AF_INET; 					//地址族，固定填 AF_INET
	sin.sin_port 		= htons(PORT_s); 			//端口号的网络字节序
	sin.sin_addr.s_addr = inet_addr(argv[1]); 		//IP的网络字节序
	//连接服务器
	if(connect(sfd,(struct sockaddr*)&sin,sizeof(sin)) < 0)
	{
		ERR_MSG("connect");
		return -1;
	}
	printf("connet soccess\n");
	
	temp_sfd = sfd;
	//捕获2号信号SIGINT "ctrl + c"
	sighandler_t s = signal(SIGINT, handler);
	if(SIG_ERR == s)
	{
		perror("signal");
		return -1; 
	}
	printf("SIGINT 捕获成功\n");

	int n = 0;//菜单选项
	int flag;//登录判断
	while(1)
	{
		system("clear");
		printf("*****************************\n");
		printf("************1,注册***********\n");
		printf("************2,登录***********\n");
		printf("************3,退出***********\n");
		printf("*****************************\n");
		printf("请输入选项>>>>");
		scanf("%d",&n);
		while(getchar()!=10);
		switch(n)
		{
		case 1:
			do_register(sfd);
			break;
		case 2:
			flag = do_login(sfd);//1管理员登录成功，0普通用户登录，-1登录失败
			if(flag >= 0)
			{
				if(flag == 1)
				{
					printf("**********管理员登录成功***********\n");
					while(1)
					{	
						system("clear");
						printf("*******管*****理*****员******\n");
						printf("************1,增加***********\n");
						printf("************2,删除***********\n");
						printf("************3,修改***********\n");
						printf("************4,查询***********\n");
						printf("************5,退出***********\n");
						printf("*****************************\n");
						printf("请输入选项>>>>");
						scanf("%d",&n);
						while(getchar()!=10);
						if(1 == n)
						{
							printf("**************正在增加中*************\n");
							do_add(sfd);
						}
						else if(2 == n)
						{
							printf("**************正在删除中*************\n");
							do_delete(sfd);
						}
						else if(3 == n)
						{
							printf("**************正在修改中*************\n");
							do_change(sfd,flag);
						}
						else if(4 == n)
						{
							printf("**************正在查询中*************\n");
							do_search(sfd,flag);
						}
						else if(5 == n)
						{
							printf("**************正在退出中*************\n");
							do_logout(sfd);
							break;
						}
						else
						{
							printf("输入有误，请重新输入\n");
						}
						printf("请按下任意按键继续>>>>>>>>>>\n");
						while(getchar()!=10);
					}
				}
				else
				{
					printf("**********普通用户登录成功***********\n");
					while(1)
					{							
						system("clear");
						printf("***普****通*****用*****户****\n");
						printf("************1,修改***********\n");
						printf("************2,查询***********\n");
						printf("************3,退出***********\n");
						printf("*****************************\n");
						printf("请输入选项>>>>");
						scanf("%d",&n);
						while(getchar()!=10);
						if(1 == n)
						{
							printf("**************正在修改中*************\n");
							do_change(sfd,flag);
						}
						else if(2 == n)
						{
							printf("**************正在查询中*************\n");
							do_search(sfd,flag);
						}
						else if(3 == n)
						{
							printf("**************正在退出中*************\n");
							do_logout(sfd);
							break;
						}
						else
						{
							printf("输入有误，请重新输入\n");
						}
						printf("请按下任意按键继续>>>>>>>>>>\n");
						while(getchar()!=10);
					}
				}
			}
			break;
		case 3:
			printf("即将关闭客户端......\n");
			close(sfd);
			return 0;
		default:
			printf("输入有误，请重新输入\n");
			break;
		}
		printf("请按下任意按键继续>>>>>>>>>>\n");
		while(getchar()!=10);
	}
	//关闭文件描述符
	close(sfd);
	return 0;
}

int do_register(int sfd)
{
	//封数据包
	info *acc1;
	ssize_t res = 0;

	acc1->operation = 'A';
	acc1->flag = 0;
	printf("请输入工号>>>\n");
	scanf("%d",&acc1->num);
	getchar();
	printf("请输入姓名>>>\n");
	fgets(acc1->name,20,stdin);
	acc1->name[strlen(acc1->name)-1] = 0;
	printf("请输入密码>>>\n");
	fgets(acc1->passworld,20,stdin);
	acc1->passworld[strlen(acc1->passworld)-1] = 0;

	//	printf("[%c] %d %s %s\n",acc1->operation,acc1->num,acc1->name,acc1->passworld);

	//发送数据
	res = send(sfd,acc1,sizeof(*acc1),0);
	if(res < 0)
	{
		ERR_MSG("send");
		return -1;
	}
	printf("成功发送注册请求......\n");

	do_recv(sfd);
	return 0;
}

int do_recv(int sfd)
{
	//封数据包
	info *acc0 = (info *)malloc(sizeof(info));//结构体变量.用于接收数据
	ssize_t res = 0;

	//等待接收服务器发送注册反馈信息
	res = recv(sfd,acc0,sizeof(*acc0),0);
	if(res < 0)
	{
		ERR_MSG("recv");
		return -1;
	}
	else if(0 == res)
	{
		printf("服务器关闭\n");
		return 0;
	}
	//printf("%c\n",acc0->operation);
	printf("%s\n",acc0->ack+1);

	//跟据不同的操作码，执行不同的显示
	int ret = -1;//返回值
	switch(acc0->operation)
	{
	case 'A':
		break;
	case 'B':
		if(acc0->ack[0] == 'Y')
		{
			num = acc0->num;//记录登录成功的用户
			ret = acc0->flag;//管理员1，普通用户0
		}
		else
		{
			ret = -1;//登录失败
		}
		break;
	case 'C':
		break;
	case 'D':
		break;
	case 'E':
		break;
	case 'F':
		if(acc0->ack[0] == 'Y')
		{
			if(acc0->flag == 1)
			{
				printf("工号:%d  姓名:%s  密码:%s\n",acc0->num,acc0->name,acc0->passworld);
				printf("性别:%s  年龄:%d  地址:%s\n",acc0->sex,acc0->age,acc0->location);
				printf("电话:%s  薪资:%d  部门:%s\n",acc0->phonenumber,acc0->salary,acc0->section);
			}
			else{
				printf("工号:%d  姓名:%s 性别:%s\
						年龄:%d  电话:%s 部门:%s\n",acc0->num,acc0->name,acc0->sex,acc0->age,acc0->phonenumber,acc0->section);
			}
		}
		break;
	case 'G':
		break;
	default:
		printf("反馈包有误.......\n");
		break;
	}
	free(acc0);
	//printf("ret = %d\n",ret);
	return ret;
}

//登录函数，返回值1管理员登录，返回值0普通用户登录，返回值-1登录失败
int do_login(int sfd)
{
	//封数据包
	info * acc1 = (info *)malloc(sizeof(info));
	ssize_t res = 0;

	acc1->operation = 'B';
	printf("请输入工号>>>\n");
	scanf("%d",&acc1->num);
	getchar();
	printf("请输入姓名>>>\n");
	fgets(acc1->name,20,stdin);
	acc1->name[strlen(acc1->name)-1] = 0;
	printf("请输入密码>>>\n");
	fgets(acc1->passworld,20,stdin);
	acc1->passworld[strlen(acc1->passworld)-1] = 0;

	//printf("[%c] %d %s %s\n",acc1->operation,acc1->num,acc1->name,acc1->passworld);

	//发送数据
	res = send(sfd,acc1,sizeof(*acc1),0);
	if(res < 0)
	{
		ERR_MSG("send");
		return -1;
	}
	printf("成功发送登录请求......\n");

	//等待接收服务器发送登录反馈信息
	free(acc1);
	return do_recv(sfd);
}
//退出登录，清除登录状态
int do_logout(int sfd)
{
	//封数据包
	info * acc1 = (info *)malloc(sizeof(info));
	ssize_t res = 0;

	acc1->operation = 'G';//退出
	acc1->num = num;
	num = 0;

	//发送数据
	res = send(sfd,acc1,sizeof(*acc1),0);
	if(res < 0)
	{
		ERR_MSG("send");
		return -1;
	}

	do_recv(sfd);
	free(acc1);
	return 0;
}
//管理员增加新用户信息
int do_add(int sfd)
{
	//封数据包
	info * acc1 = (info *)malloc(sizeof(info));
	ssize_t res = 0;

	acc1->operation = 'C';//增加，调用服务器的注册函数
	acc1->flag = 0;
	printf("请输入工号>>>\n");
	scanf("%d",&acc1->num);
	getchar();
	printf("请输入密码>>>\n");
	fgets(acc1->passworld,20,stdin);
	acc1->passworld[strlen(acc1->passworld)-1] = 0;
	printf("请输入姓名>>>\n");
	fgets(acc1->name,20,stdin);
	acc1->name[strlen(acc1->name)-1] = 0;
	printf("请输入性别(m/f)>>>\n");
	fgets(acc1->sex,5,stdin);
	acc1->sex[strlen(acc1->sex)-1] = 0;
	printf("请输入年龄>>>\n");
	scanf("%d",&acc1->age);
	getchar();
	printf("请输入部门>>>\n");
	fgets(acc1->section,20,stdin);
	acc1->section[strlen(acc1->section)-1] = 0;
	printf("请输入薪资>>>\n");
	scanf("%d",&acc1->salary);
	getchar();
	printf("请输入地址>>>\n");
	fgets(acc1->location,20,stdin);
	acc1->location[strlen(acc1->location)-1] = 0;
	printf("请输入电话>>>\n");
	fgets(acc1->phonenumber,20,stdin);
	acc1->phonenumber[strlen(acc1->phonenumber)-1] = 0;

	//	printf("[%c] %d %s %s\n",acc1->operation,acc1->num,acc1->name,acc1->passworld);
	//发送数据
	res = send(sfd,acc1,sizeof(*acc1),0);
	if(res < 0)
	{
		ERR_MSG("send");
		return -1;
	}
	printf("成功发送增加注册请求......\n");
	do_recv(sfd);
	free(acc1);
	return 0;
}
//删除
int do_delete(int sfd)
{
	//封数据包
	info * acc1 = (info *)malloc(sizeof(info));
	ssize_t res = 0;

	acc1->operation = 'D';//删除
	printf("请输入待删除信息的员工工号>>>>>");
	scanf("%d",&acc1->num);
	getchar();
	printf("请输入待删除信息的员工姓名>>>>>");
	fgets(acc1->name,20,stdin);
	acc1->name[strlen(acc1->name)-1] = 0;

	//发送数据
	res = send(sfd,acc1,sizeof(*acc1),0);
	if(res < 0)
	{
		ERR_MSG("send");
		return -1;
	}
	printf("成功发送删除信息请求......\n");
	do_recv(sfd);
	free(acc1);
	return 0;
}
//修改
int do_change(int sfd,int flag)
{
	//如果是管理员，需要输入要修改的员工的工号和姓名进行验证，避免修改错误，权限赋值１
	//如果是普通用户，只能修改自己的信息，只需要自己的工号，不需要验证，权限赋值０
	//不同权限，可修改内容不同

	//封数据包
	info * acc1 = (info *)malloc(sizeof(info));
	ssize_t res = 0;
	acc1->operation = 'E';//修改

	for(int i = 0;i<8;i++)
	{
		acc1->mark[i] = 0;
	}

	int n = 0;//菜单选项
	if(flag == 1)
	{
		//当前登录为管理员
		acc1->flag = 1;//权限赋值，用于服务器区别
		printf("请输入待修改信息的员工工号>>>>>");
		scanf("%d",&acc1->num);
		getchar();
		printf("请输入待修改信息的员工姓名>>>>>");
		fgets(acc1->name,20,stdin);
		acc1->name[strlen(acc1->name)-1] = 0;
	}
	else
	{
		//当前登录为普通用户
		acc1->flag = 0;//权限赋值，用于服务器区别
		acc1->num = num;//记录当前登录普通用户的工号
	}
	if(flag == 1)
	{
		while(1)
		{
			system("clear");
			printf("**********[管理员]************\n");
			printf("---1.密码---2.性别---3.年龄---\n");
			printf("---4.部门---5.电话---6.地址---\n");
			printf("---7.薪资---8.修改---9.取消---\n");
			printf("******************************\n");

			printf("请输入信息修改选项>>>>>>\n");
			scanf("%d",&n);
			while(getchar()!=10);
			printf("%d\n",n);


			if(1 == n){
				printf("请输入密码>>>\n");
				fgets(acc1->passworld,20,stdin);
				acc1->passworld[strlen(acc1->passworld)-1] = 0;
			}
			else if(2 == n){
				printf("请输入性别(m/f)>>>\n");
				fgets(acc1->sex,5,stdin);
				acc1->sex[strlen(acc1->sex)-1] = 0;
			}
			else if(3 == n){
				printf("请输入年龄>>>\n");
				scanf("%d",&acc1->age);
				getchar();
			}
			else if(4 == n){
				printf("请输入部门>>>\n");
				fgets(acc1->section,20,stdin);
				acc1->section[strlen(acc1->section)-1] = 0;
			}
			else if(5 == n){
				printf("请输入电话>>>\n");
				fgets(acc1->phonenumber,20,stdin);
				acc1->phonenumber[strlen(acc1->phonenumber)-1] = 0;
			}
			else if(6 == n){
				printf("请输入地址>>>\n");
				fgets(acc1->location,20,stdin);
				acc1->location[strlen(acc1->location)-1] = 0;
			}
			else if(7 == n){
				printf("请输入薪资>>>\n");
				scanf("%d",&acc1->salary);
				getchar();
			}
			else if(8 == n){
				printf("正在修改信息......\n");
				break;
			}
			else if(9 == n){
				printf("取消修改信息......\n");
				return 0;
			}
			else{
				printf("输入有误，请重新输入\n");
			}
			if(n > 0 && n < 8)
			{
				acc1->mark[n-1] = 1;
			}
			printf("请按下任意键继续>>>>>>\n");
			while(getchar() != 10);
		}
	}
	else
	{
		while(1)
		{
			system("clear");
			printf("********[普通用户]************\n");
			printf("---1.密码---2.性别---3.年龄---\n");
			printf("---4.部门---5.电话---6.地址---\n");
			printf("---7.修改---8.取消------------\n");
			printf("******************************\n");

			printf("请输入选项>>>>>>\n");
			scanf("%d",&n);
			while(getchar()!=10);
			if(1 == n){
				printf("请输入密码>>>\n");
				fgets(acc1->passworld,20,stdin);
				acc1->passworld[strlen(acc1->passworld)-1] = 0;
			}
			else if(2 == n){
				printf("请输入性别(m/f)>>>\n");
				fgets(acc1->sex,5,stdin);
				acc1->sex[strlen(acc1->sex)-1] = 0;
			}
			else if(3 == n){
				printf("请输入年龄>>>\n");
				scanf("%d",&acc1->age);
				getchar();
			}
			else if(4 == n){
				printf("请输入部门>>>\n");
				fgets(acc1->section,20,stdin);
				acc1->section[strlen(acc1->section)-1] = 0;
			}
			else if(5 == n){
				printf("请输入电话>>>\n");
				fgets(acc1->phonenumber,20,stdin);
				acc1->phonenumber[strlen(acc1->phonenumber)-1] = 0;
			}
			else if(6 == n){
				printf("请输入地址>>>\n");
				fgets(acc1->location,20,stdin);
				acc1->location[strlen(acc1->location)-1] = 0;
			}
			else if(7 == n){
				printf("正在修改信息......\n");
				break;
			}
			else if(8 == n){
				printf("取消修改信息......\n");
				return 0;
			}
			else{
				printf("输入有误，请重新输入\n");
			}
			if(n > 0 && n < 7)
			{
				acc1->mark[n-1] = 1;
			}
			printf("请按下任意键继续>>>>>>\n");
			while(getchar() != 10);
		}
	}
	//发送数据
	res = send(sfd,acc1,sizeof(*acc1),0);
	if(res < 0)
	{
		ERR_MSG("send");
		return -1;
	}
	printf("成功发送修改信息请求......\n");
	do_recv(sfd);
	free(acc1);
	return 0;
}
//查询
int do_search(int sfd,int flag)
{
	//封数据包
	info * acc1 = (info *)malloc(sizeof(info));
	ssize_t res = 0;
	acc1->operation = 'F';//查询
	acc1->flag = flag;

	printf("请输入要查询员工的工号>>>\n");
	scanf("%d",&acc1->num);
	getchar();

	//发送数据
	res = send(sfd,acc1,sizeof(*acc1),0);
	if(res < 0)
	{
		ERR_MSG("send");
		return -1;
	}
	printf("成功发送查询信息请求......\n");
	do_recv(sfd);
	free(acc1);
	return 0;
}
