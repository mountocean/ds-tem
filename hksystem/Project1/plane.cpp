#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include"plane.h"
#include"stdlib.h"

void usermenu()//�û��˵����� 
{
	system("cls");
	system("date/t");//��������
	printf("\n\t-----------------------�ൺ��ͤ���ʻ�����Ʊϵͳ-----------------------\n");
	printf("\n\t----------TsingTao LiuTing International Airport Order System-----------\n");
	printf("\n\t\t\t*********************************\n");
	printf("\n\t\t\t\t1.��ѯ�������\n\t\t\t\t2.��Ʊ����\n\t\t\t\t3.��Ʊ����\n\t\t\t\t4.�ѹ�Ʊ��Ϣ��ѯ\n\t\t\t\t5.�˳�ϵͳ\n\t\t\t\t");
	printf("\n\t\t\t*********************************\n\t\t\t");
	printf("��������Ҫִ�еĹ���(1-5)��");
}
void adminmenu()//����Ա�˵�����
{
	system("cls");
	system("date/t");//��������
	printf("\n\t----------------�ൺ��ͤ���ʻ�������ϵͳ-----------------\n");
	printf("\n\t---TsingTao LiuTing International Airport Sdmin System---\n");
	printf("\n\t\t\t*********************************\n");
	printf("\t\t\t\t1.¼�뺽����Ϣ\n\t\t\t\t2.��ѯ�������\n\t\t\t\t3.��ѯ�û���Ʊ��Ϣ\n\t\t\t\t4.�޸ĺ�����Ϣ\n\t\t\t\t5.ɾ��������Ϣ\n\t\t\t\t6.�˳�ϵͳ\n\t\t\t\t");
	printf("\n\t\t\t*********************************\n\t\t\t");
	printf("��������Ҫִ�еĹ���(1-6)��");
}
void loginmenu()//�û���¼�˵���ʾ
{
	system("cls");
	printf("\n\t-------------------��¼Login-----------------------\n");
	printf("\n\t\t*********************************\n");
	printf("\n\t  ��δע����û����Զ���������û���������ע�ᣩ\n\n");
}
int loginin(AU* e)//��¼
{
	AU* a,* n,* m;
	n = e;
	m = e;
	while (n->next != NULL)
	{
		n= n->next;
	}//����ѭ��ȥ�ҵ�β���
	a = (AU*)malloc(sizeof(AU));//����һ���½��
	if (a == NULL)
	{
		printf("\t\t��¼����\n");
		exit(0);
	}
	printf("\t\t�������û�����");
	scanf_s("%s", a->data.appname,50);
	while (m != NULL&&m->data.usernum<=n->data.usernum)
	{
		if (strcmp(a->data.appname,m->data.appname)==0)
		{
			printf("\t\t���������룺");
			scanf_s("%s", a->data.appkey,50);
			if (strcmp(a->data.appkey,m->data.appkey)==0)
			{
				printf("\n\t\t��½�ɹ���\n\n");
				system("pause");
				n->next = NULL;
				free(a);
				return m->data.usernum;
			}
			else
			{
				printf("\n\t\t������������µ�½��\n\n");
				system("pause");
				return -1;
			}
		}
		else
		{
			m = m->next;
		}
	}
	printf("\t\t���������룺");
	scanf_s("%s", a->data.appkey,50);
	printf("\n\t\tע��ɹ��������µ�¼��\n\n");
	system("pause");
	a->data.usernum = n->data.usernum + 1;
	a->next = NULL;
	n->next = a;
	n = a;
	saveuser(e);
	return - 1;
}
void except_id(TN* p, TN* u)//¼�뺽��Ų��� 
{
	TN* q;
	q = u;//���ڴ�ŷ����ͷ���
	while (q != NULL)
	{
		if(!strcmp(p->data.stationId,q->data.stationId))//�ҵ��������ȵ�����ʾ�������Ա��������
		{
			printf("\t��ǰ�����Ѵ���,���������룺");
			scanf_s("%s", p->data.stationId,6);
			except_id(p, u);
		}
		else
			q = q->next;//�����������
	}
}
void Judge_ticket(TN* u)//Ʊ���Ǹ��ж� 
{
	if (u->data.ticket < 0)
	{
		printf("\tƱ�������������������룺 ");
		scanf_s("%d", &u->data.ticket);
		Judge_ticket(u);//��������ٴε�����ʾ
	}
}
void Judge_money(TN* u)	//Ʊ�۷Ǹ��ж� 
{
	if (u->data.money < 0)
	{
		printf("\tƱ�������������������룺 ");
		scanf_s("%d", &u->data.money);
		Judge_money(u);//��������ٴε�����ʾ
	}
}
void luru(TN* u)//�ļ���Ϣ¼�� 
{
	TN* p, * q;
	q = u;//���ڴ�ŷ����ͷ���
	while (q->next != NULL)
	{
		q = q->next;
	}//����ѭ��ȥ�ҵ�β���
	p = (TN*)malloc(sizeof(TN));//����һ���½��
	system("cls");
	printf("\n\t---------------------------------������Ϣ¼��---------------------------------\n");
	if (p == NULL)
	{
		printf("¼�����");
		exit(0);
	}//p����NULL�򿪱ٿռ�ʧ��
	printf("\n\t1.�����뺽��ţ���λ����");//�������뺽��ţ���Ϊ�������ÿ���ɻ���ר������
	scanf_s("%s", p->data.stationId,7);
	except_id(p, u);//¼��ĺ���Ų��أ���ֹ�������ͬ
	printf("\t2.��������ɻ������ƣ�");
	scanf_s("%s", p->data.from,50);
	printf("\t3.�������յ�������ƣ�");
	scanf_s("%s", p->data.to,50);
	printf("\t4.�����뺽�����ʱ��(ʱ�ָ�ʽΪ00:00)��");
	scanf_s("%s", p->data.stime,16);
	printf("\t5.�����뺽�ཱུ��ʱ��(ʱ�ָ�ʽΪ00:00,�ڶ���+1)��");
	scanf_s("%s", p->data.rtime,16);
	printf("\t6.������Ʊ�ۣ�");
	scanf_s("%d", &p->data.money);
	Judge_money(p);//�����Ʊ��Ҫ�жϣ���ֹ����������ִ���
	printf("\t7.������Ʊ����");
	scanf_s("%d", &p->data.ticket);
	Judge_ticket(p);//�����Ʊ��Ҫ�жϣ���ֹ����������ִ���
	p->next = NULL;//����next��Ϊ��
	q->next = p;//���¿��ٵĽ�����ӵ�β�����
	q = p;//������һ��β�ڵ�λ��
	printf("\t¼��ɹ���\t\n");
	saveplane(u);
	system("pause");
}
void orderTicket(TN* s, UN* u,int log)		//��Ʊ���� 
{
	TN* p;//������ŷ����ͷ���
	UN* q;//�����û��˽��
	int n;
	p = s;
	char t_id[7];//������Ҫ����ĺ����
	system("cls");
	printf("\n\t---------------------------------���ඩ����д---------------------------------\n");
	printf("\t����������ҪԤ���ĺ���ţ�");
	scanf_s("%s",t_id,7);//�������뺽���
	while (p != NULL && strcmp(p->data.stationId,t_id)!=0  )//ѭ����ֹ����Ϊ�ҵ��������ͬ�ĺ�������ߵ�β�ڵ�
	{
		p = p->next;//����ѭ������
	}
	if (p == NULL)//��Ϊ�ս�������ѭ��ʱ��˵����¼����ļ���û�иú���
	{
		printf("\t**�������Ʊ��Ԥ��!**\n");
	}
	else//û��Ϊ�ս���ʱ��˵���ҵ��˷���Ԥ���ĺ����
	{
		if (p->data.ticket <= 0)//����ú���Ʊ�����㣬��ô��ʾ�û����ۿ�
		{
			printf("\t**�ú���Ʊ�Ѿ��ۿ�**\n");
		}
		else
		{
			q = u;
			while (q->next != NULL)
			{
				q = q->next;
			}//��¼����û����ӵ�β�����
			UN* r = (UN*)malloc(sizeof(UN));
			printf("\n\t#####�밴��ʾ�������ĸ�����Ϣ#####\n");
			printf("\t����������������");
			scanf_s("%s", r->data.name,50);
			printf("\t�������������֤���룺");
			scanf_s("%s", r->data.id,50);
			printf("\t�����������ֻ����룺");
			scanf_s("%s", r->data.tele,50);
			r->data.usernum = log;
			strcpy(r->data.planeId,t_id);

			p->data.ticket = p->data.ticket - 1;//�û���Ʊ�ɹ�����Ҫ���ú�����Ʊ��-1
			srand((int)time(NULL));//����ʱ����ķ�ʽ��ָ�������
			n = rand() % 8999 + 1000;//��������ķ�ʽ���һ����λ����Ч����
			r->data.order = n;
			printf("\t��Ʊ�ɹ������ѳɹ�Ԥ��һ�ź����Ϊ%s�Ļ�Ʊ,", r->data.planeId);
			printf("����Ч����Ϊ%d��ƾ������Ʊ�����μ�\n", r->data.order);
			r->next = NULL;
			q->next = r;
			q = r;
		}
	}
	saveplane(s);
	saveticket(u);
	system("pause");
}
void refundTicket(TN* s, UN* u)//��Ʊ����
{
	TN* p;
	UN* r,* t;//�����û��˽��
	char id[STR_LEN] = {0};//�������֤��
	int ord;//���嶩�����
	p = s;//��������ͷ���
	r = u;//��r��Ϊ�û��˵�ͷ���
	t = u;
	char input_t_id[7];//���幺Ʊʱ�ĺ����
	system("cls");
	printf("\n\t---------------------------------������Ʊ����---------------------------------\n");
	printf("\t����������Ҫ��Ʊ�ĺ���ţ�");
	scanf_s("%s",input_t_id,7);
	printf("\t�������������֤�ţ�");
	scanf_s("%s",id,50);
	printf("\t����������Ч���룺");
	scanf_s("%d",&ord);
	//������������֤��Ч���룬�����û�������ר�е���Ϣ�������ܻ��ظ�
	while (r != NULL)
	{
		if ( strcmp(r->data.id,id) == 0 && strcmp(input_t_id, r->data.planeId) == 0 && ord == r->data.order)//ע�����֤��Ҫ��strcmp����ȥ�Ƚ�
		{
			printf("\t����ǰ�Ķ�Ʊ��Ϣ���£�\n");
			printf("\t���֣�%s\n", r->data.name);
			printf("\t�绰����Ϊ��%s\n", r->data.tele);
			printf("\t�������Ϊ:%s\n", r->data.planeId);
			break;//�ҵ���ȫƥ����û�ʱ�����û�������Ϣ��ӡ��Ȼ��������ѭ��
		}
		else
		{
			t = r;
			r = r->next;//�����������
		}
	}
	if (r == NULL)//������ѭ����NULL����ʱ��˵��û���ҵ����û�����Ϣ
	{
		printf("\t**��ѯ�������Ķ�Ʊ��Ϣ**\n");
	}
	else//�����Կս�����ѭ����˵���ҵ����û��Ĺ�Ʊ��Ϣ
	{
		while (p != NULL && !(strcmp(p->data.stationId,input_t_id)==0 && strcmp(input_t_id, r->data.planeId) == 0 && ord == r->data.order))
		{
			p = p->next;//���ø�ѭ��ȥ�����ҳ��û������ĺ���
		}
		if (p == NULL)//����Կս���ѭ��˵��û�иú���
		{
			printf("��ѯ������Ʊ��Ϣ\n");
		}
		else//�ҵ���Ӧ�ĺ���ź�
		{
			p->data.ticket += 1;//������ƱƱ��+1
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
			free(r);//������Ʊ�û����free��
			printf("\t**��Ʊ�ɹ�����лʹ��!**\n");
			saveplane(s);
			saveticket(u);
		}
	}
	system("pause");
}
void Info(TN* s)//������Ϣ��ʾ 
{
	printf("\t%6s", s->data.stationId);
	printf("\t%8s", s->data.from);
	printf("\t%8s", s->data.to);
	printf("%10s", s->data.stime);
	printf("%15s", s->data.rtime);
	printf("%15d", s->data.money);
	printf("%15d\n", s->data.ticket);
}
void search_t_id(TN* s)//������Ų�ѯ 
{
	char num[7];//���庽���
	TN* p;
	p = s;//���庽��ͷ���
	printf("����������Ҫ���ҵĺ�����Ϣ�ĺ���ţ�");
	scanf_s("%s",num,7);
	printf("\n\t\t\t\t******-----��ѯ���-----******\n");
	printf("\t�����\tʼ������\t�������\t���ʱ��\t����ʱ��\tƱ��(Ԫ)\tƱ�����ţ�\n");
	while (p)
	{
		if (!strcmp(p->data.stationId,num))//�����һ��ʱ����ѭ��
		{
			Info(p);
			break;
		}
		p = p->next;
	}
	if (p == NULL)
	{
		printf("\t\tδ�ҵ��ú���ŵ���Ϣ!\n");
	}
}
void search_t_from(TN* s)//��ʼ��������ѯ 
{
	TN* p;
	char pstart[STR_LEN] = { 0 };//����ʼ������
	p = s->next;
	int flag = 0;
	printf("����������Ҫ���ҵĺ�����Ϣ��ʼ��������");
	scanf_s("%s", &pstart, 50);
	printf("\n\t\t\t\t******-----��ѯ���-----******\n");
	printf("\t�����\tʼ������\t�������\t���ʱ��\t����ʱ��\tƱ��(Ԫ)\tƱ�����ţ�\n");
	while (p)//p��Ϊ��ʱ����ѭ��
	{
		if (strcmp(p->data.from, pstart) == 0)//ʼ���������ʱ�����if���
		{
			Info(p);
			flag = 1;//�ҵ���ɻ�����ȵĺ��ཫflag��Ϊ1
		}
		if (p->next == NULL)//���p->next����NULL������ѭ��
			break;
		else
			p = p->next;//next��Ϊ�����������

	}
	if (p == NULL || flag == 0)//Ϊ��ʱ����flag����0ʱ
	{
		printf("\t\t\t.......δ�ҵ��ú�����Ϣ!\n");
	}
}
void search_t_to(TN* s)//���յ������ѯ 
{
	TN* p;
	char preach[STR_LEN] = {0};//�����յ����
	p = s->next;
	int flag = 0;
	printf("����������Ҫ���ҵĺ�����Ϣ���յ������");
	scanf_s("%s", &preach,50);
	printf("\n\t\t\t\t******-----��ѯ���-----******\n");
	printf("\t�����\tʼ������\t�������\t���ʱ��\t����ʱ��\tƱ��(Ԫ)\tƱ�����ţ�\n");
	while (p)
	{
		if (strcmp(p->data.to, preach) == 0)//�յ�������ʱ����ifѭ��
		{
			Info(p);
			flag = 1;//�޸�flag��ֵ
		}
		if (p->next == NULL)
			break;
		else
			p = p->next;
	}
	if (p == NULL || flag == 0)
	{
		printf("\t\t\t.......δ�ҵ��ú�����Ϣ!\n");
	}
}
void all_show(TN* s)//��ѯȫ������
{
	TN* p;
	p = s->next;
	printf("\n\t\t\t\t******-----��ѯ���-----******\n");
	if (p == NULL)
	{
		printf("\t\t\t.......δ��ѯ���κκ�����Ϣ��\n");
	}
	printf("\t�����\t  ʼ������\t�������   ���ʱ��\t����ʱ��\tƱ��(Ԫ)\tƱ�����ţ�\n");
	while (p != NULL)
	{
		Info(p);
		p = p->next;
	}
}
void searchticket(UN* u, int log)//�ѹ�Ʊ��Ϣ��ѯ
{
	UN* q;
	int i = 0;
	q = u;
	system("cls");
	printf("\n\t\t\t******-----��ѯ���-----******\n");
	if (q == NULL)
	{
		printf("\t\t\tδ��ѯ���κ��û���Ʊ��Ϣ��\n");
	}
	else
	{
		printf("���Ĺ�Ʊ��Ϣ���£�\n\n");
		printf("�����\t  ����\t\t\t  ���֤��\t   �绰����\t     �������\t\t\t\t\t\n ");
		while (q != NULL)
		{
			if (q->data.usernum == log)
			{
				printf("%7s", q->data.planeId);
				printf("%8s", q->data.name);
				printf("%21s", q->data.id);
				printf("%17s", q->data.tele);
				printf("%18d\n", q->data.order);
				i++;
			}
			q = q->next;
		}
	}
	if (i == 0) {
		printf("\n\t\t\tδ��ѯ���κ��û���Ʊ��Ϣ��\n");
	}
	printf("\n");
	system("pause");
}
void allp_show(TN* s, UN* u)//��ѯ�û���Ʊ��Ϣ
{
	UN* r;
	r = u->next;
	system("cls");
	printf("\n\t\t\t******-----��ѯ���-----******\n");
	if (r == NULL)
	{
		printf("\t\t\tδ��ѯ���κ��û���Ʊ��Ϣ��");
	}
	else
	{
		printf("�����\t  ����\t\t  ���֤��\t   �绰����\t     �������\t\n ");
		while (r != NULL)
		{
			printf("%7s", r->data.planeId);
			printf("%8s", r->data.name);
			printf("%21s", r->data.id);
			printf("%17s", r->data.tele);
			printf("%18d\n", r->data.order);
			r = r->next;
		}
	}
	system("pause");
}
void search(TN* s)//������Ϣ���� 
{
	int num;//�����ѯ��ʽ
	system("cls");
	printf("\n\t---------------------------------������Ϣ��ѯ---------------------------------\n");
	printf("\t1.������Ų�ѯ\n\t2.��ʼ��������ѯ\n\t3.���յ������ѯ\n\t4.��ѯ���к�����Ϣ\n\n");
	printf("\t���������Ĳ�ѯ��ʽ��");
	scanf_s("%d", &num);
	printf("\n\t---------------------------------����������������-----------------------------\n");
	if (num == 1)
		search_t_id(s);
	else if (num == 2)
		search_t_from(s);
	else if (num == 3)
		search_t_to(s);
	else if (num == 4)
		all_show(s);
	else
		printf("\t\t������󣡣�\n");
	system("pause");
}
void except_update_id(TN* s)//�޸ĺ���Ų��� 
{
	TN* q;
	q = s->next;
	while (q)
	{
		if(!strcmp(s->data.stationId,q->data.stationId))
		{
			printf("\t��ǰ�������Ѵ���,���������룺");
			scanf_s("%s", s->data.stationId,6);
			except_update_id(s);
		}
		else
			q = q->next;
	}
}
void updateInfo(TN* s)//������Ϣ�޸� 
{
	system("cls");
	char num[7];//������Ҫ�޸ĵĺ����
	int n;//������Ҫ�޸ĵ���Ϣ
	TN* p;
	p = s->next;
	if (p == NULL)
	{
		printf("\t**��ʱû�п����޸ĵĺ�����Ϣ**\n");
	}
	else
	{
		printf("\t��������Ҫ�޸ĺ�����Ϣ�ĺ���ţ�");
		scanf_s("%s", num,7);
		while (strcmp(p->data.stationId,num)!=0)
		{
			p = p->next;
			if (p == NULL)
			{
				printf("\t�Ҳ����ú�����Ϣ\n");
				break;
			}
		}
		if (p!=NULL)
		{
			printf("\n\t---------------------------------������Ϣ�޸�---------------------------------\n");
			printf("\t1.�����\n\t2.ʼ������\n\t3.�յ����\n\t4.���ʱ��\n\t5.����ʱ��\n\t6.Ʊ��\n\t7.Ʊ��\n");
			printf("\t---------------------------------������������---------------------------------\n");
			printf("\t��������Ҫ�޸ĵ���Ϣ��ţ�");
			scanf_s("%d", &n);
			switch (n)
			{
			case 1:
				printf("\t����������ĺ���ţ���λ��:");
				scanf_s("%s", p->data.stationId,7);
				except_update_id(p);//����Ų���
				break;
			case 2:
				printf("\t�������������ʼ����:");
				scanf_s("%s", &p->data.from,50);
				break;
			case 3:
				printf("\t������������յ����:");
				scanf_s("%s", &p->data.to,50);
				break;
			case 4:
				printf("\t����������ĳ���ʱ��(ʱ�ָ�ʽΪ00:00)��");
				scanf_s("%s", &p->data.stime,16);
				break;
			case 5:
				printf("\t����������ĵ���ʱ��(ʱ�ָ�ʽΪ00:00,�ڶ���+1)��");
				scanf_s("%s", &p->data.rtime,16);
				break;
			case 6:
				printf("\t�����������Ʊ�ۣ�");
				scanf_s("%d", &p->data.money);
				Judge_money(p);//Ʊ���и�
				break;
			case 7:
				printf("\t�����������Ʊ����");
				scanf_s("%d", &p->data.ticket);
				Judge_ticket(p);//Ʊ���и�
				break;
			default:
				printf("\t�������\n");
			}
			if (n >= 1 && n <= 7)//���n����Ϸ����޸ĳɹ�
			{
				printf("\t**�޸ĳɹ�**\n");
				saveplane(s);
			}
		}
	}
	system("pause");
}
void deleteplane(TN* s)//������Ϣɾ��
{
	char num[7];
	int i;
	TN* p,* q;
	p = s;
	q = s;
	if (p == NULL)
	{
		printf("\t**��ʱû�п���ɾ���ĺ�����Ϣ**\n");
	}
	else
	{
		system("cls");
		printf("\n\t---------------------------------������Ϣɾ��---------------------------------\n");
		printf("\t��������Ҫɾ��������Ϣ�ĺ���ţ�");
		scanf_s("%s",num,7);
		while (1)
		{
			if (strcmp(num,p->data.stationId)==0)
			{
				break;
			}
			else if (p ->next== NULL)
			{
				printf("\tδ�ҵ���Ҫɾ���ĺ�����Ϣ��\n");
				system("pause");
				return;
			}
			q = p;
			p = p->next;
		}
			printf("\n\t\t\t******-----������Ϣ-----******\n");
			printf("\t�����\tʼ������\t�������\t���ʱ��\t����ʱ��\tƱ��(Ԫ)\tƱ�����ţ�\n");
			Info(p);
			printf("\t�Ƿ�ȷ��ɾ����\n");
			printf("\t1.ɾ��\t2.����\n");
			printf("\t��ȷ����Ӧѡ�");
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
				printf("\tɾ���ɹ���\n");
			}
			else
			{
				printf("\t������Ϣ�����ɹ���\n");
			}
	}
	system("pause");
}
void loadplane(TN* s)//���������������
{
	TN* r = (TN*)malloc(sizeof(TN));
	if (!r)
	{
		printf("\n\t\t\t********�������ݵ���ʧ�ܣ�*********\n");
	}
	FILE* fp;
	s->next = NULL;
	r = s;
	if ((fp = fopen("plane.txt", "rb+")) == NULL)
	{
		printf("\n\t\t\t********�������ݵ���ʧ�ܣ�*********\n");
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
		printf("\n\t\t\t========�������ݵ���ɹ���=========\n");
	}
	fclose(fp);
}
void loaduser(AU* e)//�û������������
{
	AU* r = (AU*)malloc(sizeof(AU));
	if (!r)
	{
		printf("\n\t\t\t********�û����ݵ���ʧ�ܣ�*********\n");
	}
	FILE* fp;
	e->next = NULL;
	r = e;
	if ((fp = fopen("userapp.txt", "rb+")) == NULL)
	{
		printf("\n\t\t\t********�û����ݵ���ʧ�ܣ�*********\n");
	}
	else
	{
		printf("\n\t\t\t========�û����ݵ���ɹ���=========\n");
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
void loadticket(UN* u)//��Ʊ�����������
{
	UN* r;
	FILE* fp;
	u->next = NULL;
	r = u;
	if ((fp = fopen("userticket.txt", "rb+")) == NULL)
	{
		printf("\n\t\t\t********��Ʊ���ݵ���ʧ�ܣ�*********\n");
	}
	else
	{
		printf("\n\t\t\t========��Ʊ���ݵ���ɹ���=========\n");
		while (!feof(fp))
		{
			UN* p = (UN*)malloc(sizeof(UN));
			fscanf(fp, "%s %s %s %s %d %d\n",  p->data.name, p->data.id, p->data.tele, p->data.planeId, &p->data.order, &p->data.usernum);
			p->next = NULL;
			r->next = p;
			r = p;
		}
	}
	fclose(fp);
}
void saveplane(TN* s)//���溽������
{
	TN* p;
	FILE* fp;
	p = s->next;
	if ((fp = fopen("plane.txt", "wb")) == NULL)
	{
		printf("\t********�������ݱ���ʧ�ܣ�*********\n");
		exit(0);
	}
	else
		printf("\t========�������ݱ���ɹ���=========\n");
	while (p)//p��Ϊ��ʱ����д�뵽�ļ�
	{
		char arr[1024] = { 0 };
		sprintf(arr, "%s %s %s %s %s %d %d\n", p->data.stationId, p->data.from, p->data.to, p->data.stime, p->data.rtime, p->data.money, p->data.ticket);
		fwrite(arr, 1, strlen(arr), fp);//�����Ƶ���ʽд�뵽�ļ���
		p = p->next;
	}
	fclose(fp);
}
void saveuser(AU* e)//�����û�����
{
	AU* p;
	FILE* fp;
	p = e->next;
	if ((fp = fopen("userapp.txt", "wb")) == NULL)
	{
		printf("\t********�û����ݱ���ʧ�ܣ�*********\n");
		exit(0);
	}
	else
		printf("\t========�û����ݱ���ɹ���=========\n");
	while (p)//p��Ϊ��ʱ����д�뵽�ļ�
	{
		char arr[1024] = { 0 };
		sprintf(arr, "%s %s %d\n", p->data.appname, p->data.appkey, p->data.usernum);
		fwrite(arr, 1, strlen(arr), fp);//�����Ƶ���ʽд�뵽�ļ���
		p = p->next;
	}
	fclose(fp);
}
void saveticket(UN* u)//���湺Ʊ����
{
	UN* p;
	FILE* fp;
	p = u->next;
	if ((fp = fopen("userticket.txt", "wb")) == NULL)
	{
		printf("\t********��Ʊ���ݱ���ʧ�ܣ�*********\n");
		exit(0);
	}
	else
		printf("\t========��Ʊ���ݱ���ɹ���=========\n");
	while (p)//p��Ϊ��ʱ����д�뵽�ļ�
	{
		char arr[1024] = { 0 };
		sprintf(arr, "%s %s %s %s %d %d\n", p->data.name, p->data.id, p->data.tele, p->data.planeId, p->data.order, p->data.usernum);
		fwrite(arr, 1, strlen(arr), fp);//�����Ƶ���ʽд�뵽�ļ���
		p = p->next;
	}
	fclose(fp);
}