#pragma once
#include"stdio.h"
#include"conio.h"//调用控制台输入输出函数头文件
#include"string.h"
#include"time.h"
#include"stdlib.h"
#include <string>

#define STR_LEN  50//字符串长度

typedef struct train//定义列车信息结构体
{
	char stationId[7]; //车站号
	char from[STR_LEN]; //车票起点
	char to[STR_LEN]; //车票终点
	char stime[16];//出发时间
	char rtime[16];//到达时间
	int money; //票价
	int ticket;//票数
}Train;
typedef struct appuser
{
	char appname[STR_LEN];//用户名
	char appkey[STR_LEN];//密码
	int usernum;//用户编号
}Userapp;
typedef struct userticket
{
	char name[STR_LEN];//姓名 
	char id[STR_LEN];//身份证号 
	char tele[STR_LEN];//用户电话 
	char trainId[6];//用户所购的列车号 
	int order;//订单编号
	int usernum;//用户编号
	
}Userticket;
typedef struct TrainNode  //定义列车节点 
{
	Train data;
	struct TrainNode* next;
}TN;
typedef struct UserNode	//定义购票信息链表 
{
	Userticket data;
	struct UserNode* next;
}UN;
typedef struct AppUserNode  //定义用户信息链表
{
	Userapp data;
	struct AppUserNode* next;
}AU;

void usermenu();//用户菜单显示
void adminmenu();//管理员菜单显示
void loginmenu();//用户登录菜单显示
int loginin(AU* e);//登录
void orderTicket(TN* s, UN* u,int log);	//订票服务 
void refundTicket(TN* s, UN* u);//退票服务
void searchticket(UN* u,int log);//已购票信息查询
void search(TN* s);//列车信息查找 
void search_t_id(TN* s);//按列车号查询 
void search_t_from(TN* s);//按发车站查询 
void search_t_to(TN* s);//按终点站查询 
void updateInfo(TN* s);//列车信息修改  
void except_id(TN* s, TN* l);//列车号录入查重 
void except_update_id(TN* s);//修改列车号查重 
void Judge_ticket(TN* s);//票数非负判断 
void Judge_money(TN* s);//票价非负判断 
void all_show(TN* s);//查询全部列车
void allp_show(TN* s, UN* u);//查询用户购票信息
void Info(TN* s);//列车信息显示 
void luru(TN* s);//列车信息录入 
void deletetrain(TN* s);//列车信息删除
void loadtrain(TN* s);//列车数据载入程序
void loaduser(AU* e);//用户数据载入程序
void loadticket(UN* u);//购票数据载入程序
void savetrain(TN* s);//保存列车数据
void saveuser(AU* e);//保存用户数据
void saveticket(UN* u);//保存购票数据