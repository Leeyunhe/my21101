#include "main.h"

//回收僵尸进程
void handler(int sig)
{
	while(waitpid(-1,NULL,WNOHANG) > 0);
	return;
}

int main(int argc, const char *argv[])
{
	//外部输入IP地址
	if(argc < 2)
	{
		printf("请输入服务器IP地址>>>>\n");
		return -1;
	}
	//捕获17号信号，注册信号处理函数
	//17号信号为当子进程状态变化时，父进程接受到的信号,用于监控和回收僵尸子进程
	sighandler_t s = signal(17,handler);
	if(SIG_ERR == s)
	{
		ERR_MSG("signal");
		return -1;
	}
	//创建流式套接字
	int sfd = socket(AF_INET,SOCK_STREAM,0);
	if(sfd < 0)
	{
		ERR_MSG("sfd");
		return -1;
	}
	//填充地址信息结构体
	struct sockaddr_in sin;
	sin.sin_family 		= AF_INET; 					//地址族，固定填 AF_INET
	sin.sin_port 		= htons(PORT); 				//端口号的网络字节序	
	sin.sin_addr.s_addr = inet_addr(argv[1]); 		//IP的网络字节序

	//绑定服务器的IP 和 端口
	if(bind(sfd,(struct sockaddr*)&sin,sizeof(sin)) < 0)
	{
		ERR_MSG("bind");
		return -1;
	}
	printf("bind success\n");

	//数据库初始化
	//将套接字设置为被动监听状态
	if(listen(sfd,10) < 0)
	{
		ERR_MSG("listen");
		return -1;
	}
	printf("listen success\n");

	struct sockaddr_in cin; 			//存储客户端的地址信息
	socklen_t addrlen = sizeof(cin);
	int newfd;	

	while(1)
	{
		//父进程只负责链接，获取连接成功后的套接子
		newfd = accept(sfd,(struct sockaddr *)&cin,&addrlen);
		if(newfd < 0)
		{
			ERR_MSG("accept");
			return -1;
		}
		//将cin 中的网络字节序port准换成本机字节序，IP转换成点分十进制
		printf("[%s | %d] newfd = %d　已连接\n",inet_ntoa(cin.sin_addr),ntohs(cin.sin_port),newfd);

		//客户端连接成功，创建子进程专门用来交互
		if(fork() == 0)
		{
			//子进程只用来交互
			close(sfd);
			//功能区代码
			//************************************************************************************

			ssize_t res = 0;
			while(1)
			{				
				info *acc2 = (info*)malloc(sizeof(info));
				//接受数据
				res = recv(newfd,acc2,sizeof(*acc2),0);
				if(res < 0)
				{
					ERR_MSG("recv");
					break;
				}
				else if(0 == res)
				{
					printf("[%s | %d] newfd=%d 客户端断开连接\n", \
							inet_ntoa(cin.sin_addr), ntohs(cin.sin_port), newfd);
					break;
				}
				//printf("[%s | %d] newfd=%d：%c\n", \
						inet_ntoa(cin.sin_addr), ntohs(cin.sin_port), newfd,acc2->operation);
		
				//printf("%c\n",acc2->operation);
				//printf("%s\n",acc2->name);
				//printf("%s\n",acc2->passworld);
				//printf("%d\n",acc2->num);
				
				switch(acc2->operation)
				{
				//操作码,A注册，B登录，C增，D删，E改，F查
				case 'A':
					printf("**************正在注册中*************\n");
					do_register(acc2,newfd);
					break;
				case 'B':
					printf("**************正在登录中*************\n");
					do_login(acc2,newfd);
					break;
				case 'C':
					printf("**************正在增加中*************\n");
					do_register(acc2,newfd);
					break;
				case 'D':
					printf("**************正在删除中*************\n");
					do_delete(acc2,newfd);
					break;
				case 'E':
					printf("**************正在修改中*************\n");
					do_change(acc2,newfd);
					break;
				case 'F':
					printf("**************正在查询中*************\n");
					do_search(acc2,newfd);
					break;
				case 'G':
					printf("**************正在退出中*************\n");
					do_logout(acc2,newfd);
					break;
				default:
					break;
				}
			free(acc2);
			}			
			//************************************************************************************
			exit(0);//子进程不允许运行accept，所以子进程recv_cli_info接收，需要退出
		}
		//父进程中的newfd没有用处，所以fork后，关闭
		close(newfd);
	}
	//关闭文件描述符
	close(sfd);
	return 0;
}
