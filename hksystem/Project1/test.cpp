#define _CRT_SECURE_NO_WARNINGS
#include"train.h"

int main()
{
	TN* s;//定义服务端server
	int m,n,log=-1;
	UN* u;//定义用户端user
	AU* e;
	s = (TN*)malloc(sizeof(TN));
	u = (UN*)malloc(sizeof(UN));
	e = (AU*)malloc(sizeof(AU));//各开辟好一个头结点，不存储有效数据
	s->next = NULL;
	u->next = NULL;
	e->next = NULL;//next置为NULL
	loadtrain(s);
	loadticket(u);
	loaduser(e);
	system("color F3");//设置默认控制台前景和背景颜色,可以根据自己喜欢的颜色修改
	printf("\n\t-----------------------欢迎来到火车票服务系统-----------------------\n");
	printf("\n\t\t\t*********************************\n");
	printf("\n\t\t\t\t1.我是用户\n\t\t\t\t2.我是管理员\n\t\t\t\t");
	printf("\n\t\t\t*********************************\n\t\t\t");
	printf("输入您所要执行的功能(1-2)：");
	scanf_s("%d", &m);
	if (m == 1)
	{
		while (log == -1)
		{
			loginmenu();
			log = loginin(e);
		}
		while (1)
		{
			usermenu();
			scanf_s("%d", &n);
			switch (n)
			{
			case 1:
				search(s); break;
			case 2:
				orderTicket(s, u,log); break;
			case 3:
				refundTicket(s, u); break;
			case 4:
				searchticket(u,log); break;
			case 5:
				exit(0); break;
			default:
				continue;//输入错误用continue跳出本次循环
			}
		}
	}
	else if (m==2)
	{
		system("cls");
		int i;
		char key[6] = "88888",key1[6] = "";
		printf("\t请输入管理员密钥（共5次错误机会）：");
		for (i = 1; ;)
		{
			scanf_s("%s", key1,6);
			if (strcmp(key, key1) != 0)
			{
				i++;
				if (i > 5)
				{
					printf("\t密钥错误次数过多，程序自动结束");
					exit(0);
				}
				printf("\t输入错误，请重新输入：");
			}
			else
			{
				printf("\t密钥验证成功！\n");
				system("pause");
				break;
			}
		}
		while (1)
		{
			adminmenu();
			scanf_s("%d", &n);
			switch (n)
			{
			case 1:
				luru(s); break;
			case 2:
				search(s); break;
			case 3:
				allp_show(s, u); break;
			case 4:
				updateInfo(s); break;
			case 5:
				deletetrain(s); break;
			case 6:
				exit(0); break;
			default:
				continue;//输入错误用continue跳出本次循环
			}
		}
	}
	return 0;
}