#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include"train.h"
#include"stdlib.h"

void usermenu()//用户菜单函数 
{
	system("cls");
	system("date/t");//给定日期
	printf("\n\t-----------------------青岛流亭国际机场订票系统-----------------------\n");
	printf("\n\t----------TsingTao LiuTing International Airport Order System-----------\n");
	printf("\n\t\t\t*********************************\n");
	printf("\n\t\t\t\t1.查询航班情况\n\t\t\t\t2.订票服务\n\t\t\t\t3.退票服务\n\t\t\t\t4.已购票信息查询\n\t\t\t\t5.退出系统\n\t\t\t\t");
	printf("\n\t\t\t*********************************\n\t\t\t");
	printf("输入您所要执行的功能(1-5)：");
}
void adminmenu()//管理员菜单函数
{
	system("cls");
	system("date/t");//给定日期
	printf("\n\t----------------青岛流亭国际机场管理系统-----------------\n");
	printf("\n\t---TsingTao LiuTing International Airport Sdmin System---\n");
	printf("\n\t\t\t*********************************\n");
	printf("\t\t\t\t1.录入航班信息\n\t\t\t\t2.查询航班情况\n\t\t\t\t3.查询用户购票信息\n\t\t\t\t4.修改航班信息\n\t\t\t\t5.删除航班信息\n\t\t\t\t6.退出系统\n\t\t\t\t");
	printf("\n\t\t\t*********************************\n\t\t\t");
	printf("输入您所要执行的功能(1-6)：");
}
void loginmenu()//用户登录菜单显示
{
	system("cls");
	printf("\n\t-------------------登录Login-----------------------\n");
	printf("\n\t\t*********************************\n");
	printf("\n\t  （未注册的用户将自动以输入的用户名和密码注册）\n\n");
}
int loginin(AU* e)//登录
{
	AU* a,* n,* m;
	n = e;
	m = e;
	while (n->next != NULL)
	{
		n= n->next;
	}//利用循环去找到尾结点
	a = (AU*)malloc(sizeof(AU));//开辟一个新结点
	if (a == NULL)
	{
		printf("\t\t登录出错！\n");
		exit(0);
	}
	printf("\t\t请输入用户名：");
	scanf_s("%s", a->data.appname,50);
	while (m != NULL&&m->data.usernum<=n->data.usernum)
	{
		if (strcmp(a->data.appname,m->data.appname)==0)
		{
			printf("\t\t请输入密码：");
			scanf_s("%s", a->data.appkey,50);
			if (strcmp(a->data.appkey,m->data.appkey)==0)
			{
				printf("\n\t\t登陆成功！\n\n");
				system("pause");
				n->next = NULL;
				free(a);
				return m->data.usernum;
			}
			else
			{
				printf("\n\t\t密码错误，请重新登陆！\n\n");
				system("pause");
				return -1;
			}
		}
		else
		{
			m = m->next;
		}
	}
	printf("\t\t请输入密码：");
	scanf_s("%s", a->data.appkey,50);
	printf("\n\t\t注册成功，请重新登录！\n\n");
	system("pause");
	a->data.usernum = n->data.usernum + 1;
	a->next = NULL;
	n->next = a;
	n = a;
	saveuser(e);
	return - 1;
}
void except_id(TN* p, TN* u)//录入航班号查重 
{
	TN* q;
	q = u;//用于存放服务端头结点
	while (q != NULL)
	{
		if(!strcmp(p->data.stationId,q->data.stationId))//找到航班号相等的则提示服务端人员重新输入
		{
			printf("\t当前航班已存在,请重新输入：");
			scanf_s("%s", p->data.stationId,7);
			except_id(p, u);
		}
		else
			q = q->next;//不断往后迭代
	}
}
void Judge_ticket(TN* u)//票数非负判断 
{
	if (u->data.ticket < 0)
	{
		printf("\t票数输入有误，请重新输入： ");
		scanf_s("%d", &u->data.ticket);
		Judge_ticket(u);//输入错误再次调用提示
	}
}
void Judge_money(TN* u)	//票价非负判断 
{
	if (u->data.money < 0)
	{
		printf("\t票价输入有误，请重新输入： ");
		scanf_s("%d", &u->data.money);
		Judge_money(u);//输入错误再次调用提示
	}
}
void luru(TN* u)//文件信息录入 
{
	TN* p, * q;
	q = u;//用于存放服务端头结点
	while (q->next != NULL)
	{
		q = q->next;
	}//利用循环去找到尾结点
	p = (TN*)malloc(sizeof(TN));//开辟一个新结点
	system("cls");
	printf("\n\t---------------------------------航班信息录入---------------------------------\n");
	if (p == NULL)
	{
		printf("录入出错！");
		exit(0);
	}//p等于NULL则开辟空间失败
	printf("\n\t1.请输入航班号（六位）：");//首先输入航班号，因为航班号是每个飞机的专有属性
	scanf_s("%s", p->data.stationId,7);
	except_id(p, u);//录入的航班号查重，防止航班号相同
	printf("\t2.请输入起飞机场名称：");
	scanf_s("%s", p->data.from,50);
	printf("\t3.请输入终点机场名称：");
	scanf_s("%s", p->data.to,50);
	printf("\t4.请输入航班起飞时间(时分格式为00:00)：");
	scanf_s("%s", p->data.stime,16);
	printf("\t5.请输入航班降落时间(时分格式为00:00)：");
	scanf_s("%s", p->data.rtime,16);
	printf("\t6.请输入票价：");
	scanf_s("%d", &p->data.money);
	Judge_money(p);//输入的票价要判断，防止输入操作出现错误
	printf("\t7.请输入票数：");
	scanf_s("%d", &p->data.ticket);
	Judge_ticket(p);//输入的票数要判断，防止输入操作出现错误
	p->next = NULL;//最后把next置为空
	q->next = p;//把新开辟的结点链接到尾结点上
	q = p;//最后更新一下尾节点位置
	printf("\t录入成功！\t\n");
	savetrain(u);
	system("pause");
}
void orderTicket(TN* s, UN* u,int log)		//订票服务 
{
	TN* p;//用来存放服务端头结点
	UN* q;//定义用户端结点
	int n;
	p = s;
	char t_id[7];//保存需要输入的航班号
	system("cls");
	printf("\n\t---------------------------------航班订单填写---------------------------------\n");
	printf("\t请输入您需要预订的航班号：");
	scanf_s("%s",t_id,7);//首先输入航班号
	while (p != NULL && strcmp(p->data.stationId,t_id)  )//循环终止条件为找到航班号相同的航班或者走到尾节点
	{
		p = p->next;//不断循环迭代
	}
	if (p == NULL)//当为空结束上面循环时，说明在录入的文件中没有该航班
	{
		printf("\t**暂无相关票可预订!**\n");
	}
	else//没有为空结束时，说明找到了符合预定的航班号
	{
		if (p->data.ticket <= 0)//如果该航班票数不足，那么提示用户已售空
		{
			printf("\t**该航班票已经售空**\n");
		}
		else
		{
			q = u;
			while (q->next != NULL)
			{
				q = q->next;
			}//把录入的用户链接到尾结点上
			UN* r = (UN*)malloc(sizeof(UN));
			printf("\n\t#####请按提示输入您的个人信息#####\n");
			printf("\t请输入您的姓名：");
			scanf_s("%s", r->data.name,50);
			printf("\t请输入您的身份证号码：");
			scanf_s("%s", r->data.id,50);
			printf("\t请输入您的手机号码：");
			scanf_s("%s", r->data.tele,50);
			r->data.usernum = log;
			 strcpy(r->data.trainId, t_id);

			p->data.ticket = p->data.ticket - 1;//用户订票成功后需要将该航班总票数-1
			srand((int)time(NULL));//利用时间戳的方式来指定随机数
			n = rand() % 8999 + 1000;//用随机数的方式设计一个四位数的订单编号
			r->data.order = n;
			printf("\t订票成功，您已成功预订一张航班号为%s的车票,", r->data.trainId);
			printf("订单编号为%d\n", r->data.order);
			r->next = NULL;
			q->next = r;
			q = r;
		}
	}
	savetrain(s);
	saveticket(u);
	system("pause");
}
void refundTicket(TN* s, UN* u)//退票服务
{
	TN* p;
	UN* r,* t;//定义用户端结点
	char id[STR_LEN] = {0};//定义身份证号
	int ord;//定义订单编号
	p = s;//定义服务端头结点
	r = u;//将r置为用户端的头结点
	t = u;
	char input_t_id[7];//定义购票时的航班号
	system("cls");
	printf("\n\t---------------------------------航班退票界面---------------------------------\n");
	printf("\t请输入您需要退票的航班号：");
	scanf_s("%s",input_t_id,7);
	printf("\t请输入您的身份证号：");
	scanf_s("%s", id,50);
	printf("\t请输入您的订单编号：");
	scanf_s("%d", &ord);
	//以上输入的身份证号订单编号，都是用户所独有专有的信息，不可能会重复
	while (r != NULL)
	{
		if (strcmp(r->data.id, id) == 0 && !strcmp(input_t_id, r->data.trainId) && ord == r->data.order)//注意身份证号要用strcmp函数去比较
		{
			printf("\t您当前的订票信息如下：\n");
			printf("\t名字：%s\n", r->data.name);
			printf("\t电话号码为：%s\n", r->data.tele);
			printf("\t航班号码为:%s\n", r->data.trainId);
			break;//找到完全匹配的用户时，把用户其余信息打印，然后跳出该循环
		}
		else
		{
			t = r;
			r = r->next;//不断往后迭代
		}
	}
	if (r == NULL)//当上述循环以NULL结束时，说明没有找到该用户的信息
	{
		printf("\t**查询不到您的订票信息**\n");
	}
	else//不是以空结束的循环，说明找到了用户的购票信息
	{
		while (p != NULL && !strcmp(p->data.stationId,input_t_id))
		{
			p = p->next;//利用该循环去迭代找出用户订购的航班
		}
		if (p == NULL)//如果以空结束循环说明没有该航班
		{
			printf("查询不到该票信息\n");
		}
		else//找到对应的航班号后
		{
			p->data.ticket += 1;//将航班票票数+1
			if (r->next != NULL)
			{
				t->next = r->next;
			}
			else if (t == r)
			{
				t = t->next;
				free(t);
			}
			else
			{
				t->next = NULL;
			}
			free(r);//将此退票用户结点free掉
			printf("\t**退票成功，感谢使用!**\n");
			savetrain(s);
			saveticket(u);
		}
	}
	system("pause");
}
void Info(TN* s)//航班信息显示 
{
	printf("\t%7s", s->data.stationId);
	printf("%8s", s->data.from);
	printf("%8s", s->data.to);
	printf("%10s", s->data.stime);
	printf("%15s", s->data.rtime);
	printf("%15d", s->data.money);
	printf("%15d\n", s->data.ticket);
}
void search_t_id(TN* s)//按航班号查询 
{
	char num[7];//定义航班号
	TN* p;
	p = s;//定义航班头结点
	printf("请输入您需要查找的航班信息的航班号：");
	scanf_s("%s",num,7);
	printf("\n\t\t\t\t******-----查询结果-----******\n");
	printf("\t航班号\t始发站\t到达站\t起飞时间\t降落时间\t票价(元)\t票数（张）\n");
	while (p)
	{
		if (!strcmp(p->data.stationId,num))//航班号一样时跳出循环
		{
			Info(p);
			break;
		}
		p = p->next;
	}
	if (p == NULL)
	{
		printf("\t\t未找到该航班号的信息!\n");
	}
}
void search_t_from(TN* s)//按始发站查询 
{
	TN* p;
	char pstart[STR_LEN] = { 0 };//定义始发站
	p = s->next;
	int flag = 0;
	printf("请输入您需要查找的航班信息的始发站：");
	scanf_s("%s", &pstart, 50);
	printf("\n\t\t\t\t******-----查询结果-----******\n");
	printf("\t航班号\t始发站\t到达站\t起飞时间\t降落时间\t票价(元)\t票数（张）\n");
	while (p)//p不为空时进入循环
	{
		if (strcmp(p->data.from, pstart) == 0)//始发站相等时进入该if语句
		{
			Info(p);
			flag = 1;//找到发车站相等的航班将flag置为1
		}
		if (p->next == NULL)//如果p->next等于NULL则跳出循环
			break;
		else
			p = p->next;//next不为空则继续迭代

	}
	if (p == NULL || flag == 0)//为空时或者flag等于0时
	{
		printf("\t\t\t.......未找到该航班信息!\n");
	}
}
void search_t_to(TN* s)//按终点站查询 
{
	TN* p;
	char preach[STR_LEN] = {0};//定义终点站
	p = s->next;
	int flag = 0;
	printf("请输入您需要查找的航班信息的终点站：");
	scanf_s("%s", &preach,50);
	printf("\n\t\t\t\t******-----查询结果-----******\n");
	printf("\t航班号\t始发站\t到达站\t起飞时间\t降落时间\t票价(元)\t票数（张）\n");
	while (p)
	{
		if (strcmp(p->data.to, preach) == 0)//终点站相等时进入if循环
		{
			Info(p);
			flag = 1;//修改flag的值
		}
		if (p->next == NULL)
			break;
		else
			p = p->next;
	}
	if (p == NULL || flag == 0)
	{
		printf("\t\t\t.......未找到该航班信息!\n");
	}
}
void all_show(TN* s)//查询全部航班
{
	TN* p;
	p = s->next;
	printf("\n\t\t\t\t******-----查询结果-----******\n");
	if (p == NULL)
	{
		printf("\t\t\t.......未查询到任何航班信息！\n");
	}
	printf("\t航班号\t发车站\t到达站\t起飞时间\t降落时间\t票价(元)\t票数（张）\n");
	while (p != NULL)
	{
		Info(p);
		p = p->next;
	}
}
void searchticket(UN* u, int log)//已购票信息查询
{
	UN* q;
	int i = 0;
	q = u;
	system("cls");
	printf("\n\t\t\t******-----查询结果-----******\n");
	if (q == NULL)
	{
		printf("\t\t\t未查询到任何用户购票信息！\n");
	}
	else
	{
		printf("您的购票信息如下：\n\n");
		printf("航班号\t姓名\t\t  身份证号\t   电话号码\t     订单编号\t\t\t\t\t ");
		while (q != NULL)
		{
			if (q->data.usernum == log)
			{
				printf("%7s", q->data.trainId);
				printf("%8s", q->data.name);
				printf("%21s", q->data.id);
				printf("%17s", q->data.tele);
				printf("%18d", q->data.order);
				i++;
			}
			q = q->next;
		}
	}
	if (i == 0) {
		printf("\n\t\t\t未查询到任何用户购票信息！\n");
	}
	printf("\n");
	system("pause");
}
void allp_show(TN* s, UN* u)//查询用户购票信息
{
	UN* r;
	r = u->next;
	system("cls");
	printf("\n\t\t\t******-----查询结果-----******\n");
	if (r == NULL)
	{
		printf("\t\t\t未查询到任何用户购票信息！");
	}
	else
	{
		printf("航班号\t姓名\t\t  身份证号\t   电话号码\t     订单编号\t\t\t\t\t ");
		while (r != NULL)
		{
			printf("%7s", r->data.trainId);
			printf("%8s", r->data.name);
			printf("%21s", r->data.id);
			printf("%17s", r->data.tele);
			printf("%18d", r->data.order);
			r = r->next;
		}
	}
	system("pause");
}
void search(TN* s)//航班信息查找 
{
	int num;//定义查询方式
	system("cls");
	printf("\n\t---------------------------------航班信息查询---------------------------------\n");
	printf("\t1.按航班号查询\n\t2.按始发站查询\n\t3.按终点站查询\n\t4.查询所有航班信息\n\n");
	printf("\t请输入您的查询方式：");
	scanf_s("%d", &num);
	printf("\n\t---------------------------------————————-----------------------------\n");
	if (num == 1)
		search_t_id(s);
	else if (num == 2)
		search_t_from(s);
	else if (num == 3)
		search_t_to(s);
	else if (num == 4)
		all_show(s);
	else
		printf("\t\t输入错误！！\n");
	system("pause");
}
void except_update_id(TN* s)//修改航班号查重 
{
	TN* q;
	q = s->next;
	while (q)
	{
		if(!strcmp(s->data.stationId,q->data.stationId))
		{
			printf("\t当前航班班次已存在,请重新输入：");
			scanf_s("%s", s->data.stationId,7);
			except_update_id(s);
		}
		else
			q = q->next;
	}
}
void updateInfo(TN* s)//航班信息修改 
{
	system("cls");
	char num[7];//定义需要修改的航班号
	int n;//定义需要修改的信息
	TN* p;
	p = s->next;
	if (p == NULL)
	{
		printf("\t**暂时没有可以修改的航班信息**\n");
	}
	else
	{
		printf("\t请输入需要修改航班信息的航班号：");
		scanf_s("%s", num,7);
		while (strcmp(p->data.stationId,num))
		{
			p = p->next;
			if (p == NULL)
			{
				printf("\t找不到该航班信息\n");
				break;
			}
		}
		if (p!=NULL)
		{
			printf("\n\t---------------------------------航班信息修改---------------------------------\n");
			printf("\t1.航班号\n\t2.始发站\n\t3.终点站\n\t4.起飞时间\n\t5.降落时间\n\t6.票价\n\t7.票数\n");
			printf("\t---------------------------------——————---------------------------------\n");
			printf("\t请输入您要修改的信息编号：");
			scanf_s("%d", &n);
			switch (n)
			{
			case 1:
				printf("\t请输入变更后的航班号:");
				scanf_s("%s", p->data.stationId,7);
				except_update_id(p);//航班号查重
				break;
			case 2:
				printf("\t请输入变更后的起始站:");
				scanf_s("%s", &p->data.from,50);
				break;
			case 3:
				printf("\t请输入变更后的终点站:");
				scanf_s("%s", &p->data.to,50);
				break;
			case 4:
				printf("\t请输入变更后的出发时间(时分格式为00:00)：");
				scanf_s("%s", &p->data.stime,16);
				break;
			case 5:
				printf("\t请输入变更后的到达时间(时分格式为00:00)：");
				scanf_s("%s", &p->data.rtime,16);
				break;
			case 6:
				printf("\t请输入变更后的票价：");
				scanf_s("%d", &p->data.money);
				Judge_money(p);//票价判负
				break;
			case 7:
				printf("\t请输入变更后的票数：");
				scanf_s("%d", &p->data.ticket);
				Judge_ticket(p);//票数判负
				break;
			default:
				printf("\t输入错误！\n");
			}
			if (n >= 1 && n <= 7)//如果n输入合法则修改成功
			{
				printf("\t**修改成功**\n");
				savetrain(s);
			}
		}
	}
	system("pause");
}
void deletetrain(TN* s)//航班信息删除
{
	char num[7];
	int i;
	TN* p,* q;
	p = s;
	q = s;
	if (p == NULL)
	{
		printf("\t**暂时没有可以删除的航班信息**\n");
	}
	else
	{
		system("cls");
		printf("\n\t---------------------------------航班信息删除---------------------------------\n");
		printf("\t请输入需要删除航班信息的航班号：");
		scanf_s("%s",num,7);
		while (1)
		{
			if (!strcmp(num,p->data.stationId))
			{
				break;
			}
			else if (p ->next== NULL)
			{
				printf("\t未找到想要删除的航班信息！\n");
				system("pause");
				return;
			}
			q = p;
			p = p->next;
		}
			printf("\n\t\t\t******-----航班信息-----******\n");
			printf("\t航班号\t发车站\t到达站\t发车时间\t到达时间\t票价(元)\t票数（张）\n");
			Info(p);
			printf("\t是否确定删除？\n");
			printf("\t1.删除\t2.保留\n");
			printf("\t请确定对应选项：");
			scanf_s("%d", &i);
			if (i == 1)
			{
				if (p->next != NULL)
				{
					q->next = p->next;
				}
				else if (q == p)
				{
					q = q->next;
					free(q);
				}
				else
				{
					q->next = NULL;
				}
				free(p);
				printf("\t删除成功！\n");
			}
			else
			{
				printf("\t航班信息保留成功！\n");
			}
	}
	system("pause");
}
void loadtrain(TN* s)//航班数据载入程序
{
	TN* r = (TN*)malloc(sizeof(TN));
	if (!r)
	{
		printf("\n\t\t\t********航班数据导入失败！*********\n");
	}
	FILE* fp;
	s->next = NULL;
	r = s;
	if ((fp = fopen("train.txt", "rb+")) == NULL)
	{
		printf("\n\t\t\t********航班数据导入失败！*********\n");
	}
	else
	{
		while (!feof(fp))
		{
			TN* p = (TN*)malloc(sizeof(TN));
			fscanf(fp, "%s %s %s %s %s %d %d\n",p->data.stationId, p->data.from, p->data.to, p->data.stime, p->data.rtime, &p->data.money, &p->data.ticket);
			p->next = NULL;
			r->next = p;
			r = p;
		}
		printf("\n\t\t\t========航班数据导入成功！=========\n");
	}
	fclose(fp);
}
void loaduser(AU* e)//用户数据载入程序
{
	AU* r = (AU*)malloc(sizeof(AU));
	if (!r)
	{
		printf("\n\t\t\t********用户数据导入失败！*********\n");
	}
	FILE* fp;
	e->next = NULL;
	r = e;
	if ((fp = fopen("userapp.txt", "rb+")) == NULL)
	{
		printf("\n\t\t\t********用户数据导入失败！*********\n");
	}
	else
	{
		printf("\n\t\t\t========用户数据导入成功！=========\n");
		while (!feof(fp))
		{
			AU* p = (AU*)malloc(sizeof(AU));
			fscanf(fp, "%s %s %d\n", p->data.appname, p->data.appkey, &p->data.usernum);
			p->next = NULL;
			r->next = p;
			r = p;
		}
	}
	fclose(fp);
}
void loadticket(UN* u)//购票数据载入程序
{
	UN* r;
	FILE* fp;
	u->next = NULL;
	r = u;
	if ((fp = fopen("userticket.txt", "rb+")) == NULL)
	{
		printf("\n\t\t\t********购票数据导入失败！*********\n");
	}
	else
	{
		printf("\n\t\t\t========购票数据导入成功！=========\n");
		while (!feof(fp))
		{
			UN* p = (UN*)malloc(sizeof(UN));
			fscanf(fp, "%s %s %s %s %d %d\n",  p->data.name, p->data.id, p->data.tele, p->data.trainId, &p->data.order, &p->data.usernum);
			p->next = NULL;
			r->next = p;
			r = p;
		}
	}
	fclose(fp);
}
void savetrain(TN* s)//保存航班数据
{
	TN* p;
	FILE* fp;
	p = s->next;
	if ((fp = fopen("train.txt", "wb")) == NULL)
	{
		printf("\t********航班数据保存失败！*********\n");
		exit(0);
	}
	else
		printf("\t========航班数据保存成功！=========\n");
	while (p)//p不为空时继续写入到文件
	{
		char arr[1024] = { 0 };
		sprintf(arr, "%s %s %s %s %s %d %d\n", p->data.stationId, p->data.from, p->data.to, p->data.stime, p->data.rtime, p->data.money, p->data.ticket);
		fwrite(arr, 1, strlen(arr), fp);//二进制的形式写入到文件中
		p = p->next;
	}
	fclose(fp);
}
void saveuser(AU* e)//保存用户数据
{
	AU* p;
	FILE* fp;
	p = e->next;
	if ((fp = fopen("userapp.txt", "wb")) == NULL)
	{
		printf("\t********用户数据保存失败！*********\n");
		exit(0);
	}
	else
		printf("\t========用户数据保存成功！=========\n");
	while (p)//p不为空时继续写入到文件
	{
		char arr[1024] = { 0 };
		sprintf(arr, "%s %s %d\n", p->data.appname, p->data.appkey, p->data.usernum);
		fwrite(arr, 1, strlen(arr), fp);//二进制的形式写入到文件中
		p = p->next;
	}
	fclose(fp);
}
void saveticket(UN* u)//保存购票数据
{
	UN* p;
	FILE* fp;
	p = u->next;
	if ((fp = fopen("userticket.txt", "wb")) == NULL)
	{
		printf("\t********购票数据保存失败！*********\n");
		exit(0);
	}
	else
		printf("\t========购票数据保存成功！=========\n");
	while (p)//p不为空时继续写入到文件
	{
		char arr[1024] = { 0 };
		sprintf(arr, "%s %s %s %s %d %d\n", p->data.name, p->data.id, p->data.tele, p->data.trainId, p->data.order, p->data.usernum);
		fwrite(arr, 1, strlen(arr), fp);//二进制的形式写入到文件中
		p = p->next;
	}
	fclose(fp);
}