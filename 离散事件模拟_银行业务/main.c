#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define Maxsize 1000
#define CloseTime 500
struct Event
{
    int OccurTime;
    int NType;
    struct Event* next;//方便进行链表的插入；
};                      //定义事件类型，包括发生时间与事件种类
struct Qelem
{
    int ArrivalTime;
    int Duration;
};                       //定义队列元素类型
struct Queue
{
    struct Qelem* base;
    int front;
    int rear;
};                      //定义队列，利用循环队列结构
void EnQueue(struct Queue* Q,struct Qelem e)
{
    Q->base[Q->rear]=e;
    Q->rear=(Q->rear+1)%Maxsize;
}                       //插入元素到队尾
void Dequeue(struct Queue* Q)
{
    Q->front=(Q->front+1)%Maxsize;
}                        //删除队头元素
int TotalTime=0;           //定义全局变量计数总时间
int CustomerNum=0;         //定义全局变量计数客户总数
int QEmpty(struct Queue *q)
{
    if(q->front==q->rear) return 1;
    else return 0;
}
struct Qelem GetHead(struct Queue *q)
{
    struct Qelem temp;
    temp=q->base[q->front];
    return temp;
}
void Insert(struct Event **head,struct Event *en)    //按事件发生的先后顺序插入
{
    struct Event *p=*head;
    if(p==NULL)
    {
        *head=en;
        en->next=NULL;
        return ;
    }
    if(p->OccurTime>en->OccurTime)
    {
        en->next=p;
        *head=en;
        return ;
    }
    while(p->next&&p->next->OccurTime<=en->OccurTime)
    {
        p=p->next;
    }
    en->next=p->next;
    p->next=en;
}
int QueueLength(struct Queue q)
{
    return (q.rear-q.front+Maxsize)%Maxsize;
}
int Minimum(struct Queue* q)
{
    int i,res,temp;
    int num[4];
    for(i=0;i<4;i++)
    {
        num[i]=QueueLength(*(q+i+1));
    }
    for(res=0,temp=1;temp<4;temp++)
    {
        if(num[res]>num[temp]) res=temp;
    }
    return res+1;
}
struct Event* OpenForDay()
{
    TotalTime=0;
    CustomerNum=0;
    struct Event* en;
    en=(struct Event*)malloc(sizeof(struct Event));
    en->OccurTime=0;
    en->NType=0;
    return en;     //初始化链表，将第一个事件插入
}
int DurTimeCreate()
{
    int a;
    srand(time(NULL));
    a=rand()%(50-10+1)+10;
    return a;
}                               //生成业务时间，范围为10到50之间
int InterTimeCreate()
{
    int a;
    srand(time(NULL));
    a=rand()%(50-10+1)+10;
    return a;
}                               //生成下一个客户到达的时间，范围为10到50之间
void CustomerArrived(struct Event** head,struct Event* en,struct Queue q[])      //处理顾客到达，前提为en->NType==0
{
    int durtime;
    int intertime;
    int t;
    int i;
    struct Event* en1;              //产生插入的到达事件
    struct Qelem* qel;              //产生插入的队列元素
    struct Event* en2;              //产生一个插入的离开事件
    en1=(struct Event*)malloc(sizeof(struct Event));
    CustomerNum++;
    printf("第%d个客人到来了\n",CustomerNum);
    durtime=DurTimeCreate();
    intertime=InterTimeCreate();
    t=en->OccurTime+intertime;     //下一个客户到达的时间
    if(t<CloseTime)
    {
        en1->OccurTime=t;
        en1->NType=0;
        Insert(head,en1);
    }
    i=Minimum(q);
    qel=(struct Qelem*)malloc(sizeof(struct Qelem));
    qel->ArrivalTime=en->OccurTime;
    qel->Duration=durtime;
    EnQueue(q+i,*qel);
    if(QueueLength(q[i])==1)
    {
        en2=(struct Event*)malloc(sizeof(struct Event));
        en2->OccurTime=en->OccurTime+durtime;
        en2->NType=i;
        Insert(head,en2);
    }
}

void CustomerDeparture(struct Event** head,struct Event* en,struct Queue* q)     //处理离开事件，前提为en->NType>0
{
    int i;      //删去哪一个队列
    struct Event* en1;    //产生离开事件
    struct Qelem temp;
    i=en->NType;
    TotalTime+=en->OccurTime-q[i].base[q[i].front].ArrivalTime;
    printf("有客人离开了,累计目前耗时:%d\n",TotalTime);
    Dequeue(q+i);
    if(!QEmpty(q+i))
    {
        temp=GetHead(q+i);
        en1=(struct Event*)malloc(sizeof(struct Event));
        en1->OccurTime=en->OccurTime+temp.Duration;
        en1->NType=i;
        Insert(head,en1);
    }
}
int main()
{
    struct Event* head=NULL;     //定义事件表,无头节点
    struct Queue q[5];
    struct Event* p=head;
    for(int i=1;i<5;i++)
    {
        q[i].base=(struct Qelem*)malloc(sizeof(struct Qelem)*Maxsize);
        q[i].front=0;
        q[i].rear=0;
    }
    head=OpenForDay();
    head->next=NULL;
    p=head;
    while(p)
    {
        struct Event* en;
        en=p;
        head=head->next;
        if(en->NType==0)
        {
            CustomerArrived(&head,en,q);
        }
        else CustomerDeparture(&head,en,q);
        p=head;
    }
    printf("the average time is %f\n",(float)TotalTime/CustomerNum);
    return 0;
}
