#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAXSIZE 10000
struct Event
{
    int OccurTime;
    int NType;         //规定0是到来
    struct Event* next;//方便进行链表的插入
};                      //定义事件类型，包括发生时间与事件种类
struct Qelem            //代表客户
{
    int ArrivalTime;    //到达时间
    int Duration;       //业务持续时间
    int Amount;         //Amount表示该客户随机的存款或取款的钱数,正数为存款,负数为借款
    int tag;            //标志客户的序列
};                       //定义队列元素类型，即客户
struct Queue            //队列，即直观上的窗口队列
{
    struct Qelem* base; //
    int front;
    int rear;
};                      //定义队列，利用循环队列结构(此为取款或还款的队列)
void EnQueue(struct Queue* Q,struct Qelem e)
{
    Q->base[Q->rear] = e;
    Q->rear = (Q->rear+1)%MAXSIZE;
}                       //插入元素到队尾
void Dequeue(struct Queue* Q)
{
    Q->front=(Q->front+1)%MAXSIZE;
}                        //删除队头元素
int QueueLength(struct Queue q)
{
    return (q.rear-q.front+MAXSIZE)%MAXSIZE;
}                           //计算出队列的长度，采用循环队列的方式
void MoveItToTheLast(struct Queue* Q,int i)     //将第i个元素移向队尾
{
    int m = i;
    int rear = Q->rear;
    EnQueue(Q,Q->base[i]);   //将第i个元素插入队尾，同时rear向后移了一位
    for(;m < Q->rear-1;m++)
    {
        Q->base[i] = Q->base[i+1];
    }
    Q->rear = rear;
}
int TotalTime = 0;   //记录模拟系统的时间
int CustomerNum = 0;  //记录目前有多少客户来过了
int MoneyNums;         //记录银行的存款
int CloseTime;       //用以设置银行的关门时间
int QEmpty(struct Queue *q)
{
    if(q->front==q->rear) return 1;
    else return 0;
}                       //判断是否为空队列，1为空，0为非空
struct Qelem GetHead(struct Queue *q)       //返回一个队列元素，这是该队列的队头元素
{
    struct Qelem temp;
    temp=q->base[q->front];
    return temp;
}
void Insert(struct Event **head,struct Event *en)    //按事件发生的先后顺序插入，head是事件表的头指针，en是待插入的事件（该事件表无头结点）
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
int Minimum(struct Queue* q , int nums)      //需判断nums>=2，此函数为求最短队列的序号
{
    int i,res,temp;
    int num[nums];
    for(i=0;i<nums;i++)
    {
        num[i]=QueueLength(*(q+i+1));
    }
    for(res=0,temp=1;temp<nums;temp++)
    {
        if(num[res]>num[temp]) res=temp;
    }
    return res+1;
}
/*int DurTimeCreate()
{
    int a;
    srand((unsigned)time(NULL));
    a=rand()%(50-10+1)+10;
    return a;
}                               //生成业务时间，范围为10到50之间
int InterTimeCreate()
{
    int a;
    srand((unsigned)time(NULL));
    a=rand()%(50-10+1)+10;
    return a;
}                               //生成下一个客户到达的时间，范围为10到50之间
int MoneyCreate()
{
    int a;
    srand((unsigned)time(NULL));
    a = rand()%(1000+1000+1)-1000;
    return a;
}                   //生成客户这次要处理的金额数，范围为-1000到1000之间
 */
struct Event* OpenForDay()    //模拟系统的开始，返回一个到来事件
{
    TotalTime = 0;          //开始时间置0
    CustomerNum = 0;        //开始人数置0
    struct Event* Come;     //插入一个到来事件
    Come = (struct Event*)malloc(sizeof(struct Event));
    Come->next = NULL;
    Come->NType = 0;
    Come->OccurTime = 0;
    return Come ;           //返回一个事件表的表头
}
void CustomerArrived(struct Event** head,struct Event* en,struct Queue q[],int nums)  //处理客户到达，前提为en->NType == 0
{
    int durtime;            //需要生成客户的业务办理时间
    int intertime;          //生成下一个客户到来的时间间隔
    int Money;
    int t;
    int i;
    struct Event *en1;      //产生插入的到达事件
    struct Qelem *qel;      //产生队列元素，即客户
    struct Event *en2;
    en1 = (struct Event*)malloc(sizeof(struct Event));
    CustomerNum++;
    durtime = rand()%(50-10+1)+10;
    intertime = rand()%(50-10+1)+10;
    Money = rand()%(1000+1000+1)-1000;
    t = en->OccurTime+intertime ;       //下一个客户到达的时间
    if(t < CloseTime)
    {
        en1 -> OccurTime = t;
        en1 -> NType = 0;
        Insert(head,en1);
    }               //如果该客户到来是在关门之前，则将其插入到事件表
    i = Minimum(q,nums);                //找到最短的队列
    qel = (struct Qelem*)malloc(sizeof(struct Qelem));
    qel->ArrivalTime = en->OccurTime;
    qel->Duration = durtime;
    qel->Amount = Money;
    qel->tag = CustomerNum;     //表明了这是第几个客户
    printf("第%d个客户，到达时间:%d,前往了%d号窗口,想要处理的金额:%d,预期业务需处理的时间:%d\n",qel->tag,en->OccurTime,i,qel->Amount,qel->Duration);
    printf("\n");
    EnQueue(q+i,*qel);
    if(QueueLength(q[i]) == 1 && Money+MoneyNums>=0)          //若只有一个客户且该客户不会使银行资金为负
    {
        en2 = (struct Event*)malloc(sizeof(struct Event));
        en2->OccurTime = en->OccurTime+durtime;
        en2->NType = i;
        Insert(head,en2);
    }                                  //生成一个离开事件
    if(QueueLength(q[i]) == 1 && MoneyNums+Money<0)         //若只有一个客户且该客户会使银行资金为负
    {
        en2 = (struct Event*)malloc(sizeof(struct Event));
        en2->OccurTime = en->OccurTime+durtime;
        en2->NType = i;
        Insert(head,en2);
    }
}
void DealGetque(struct Event** head,struct Event* en,struct Queue q[],int MoneyTemp)
{
    int Length;
    int MoneyNumsTemp = MoneyNums;
    int num = 0;
    Length = QueueLength(q[0]);
    struct Event* en1[Length+1];     //生成离开事件
    int Count = 0;
    en1[Count] = (struct Event*)malloc(sizeof(struct Event));
    *en1[Count] = *en;
    for(int i = q[0].front;i < q[0].rear&&MoneyNumsTemp>=MoneyTemp&&num <= Length;)        //遍历q[0]
    {
        if(q[0].base[i].Amount+MoneyNumsTemp >= 0)
        {
            en1[Count+1] = (struct Event*)malloc(sizeof(struct Event));
            if(en1[Count]->OccurTime+q[0].base[i].Duration <= CloseTime)
            {
                en1[Count+1]->OccurTime = en1[Count]->OccurTime+q[0].base[i].Duration;
            }else
            {
                en1[Count+1]->OccurTime = CloseTime;
            }
            Count++;
            MoneyNumsTemp += q[0].base[i].Amount;
            en1[Count]->NType = 10000;
            Insert(head,en1[Count]);    //产生离开事件
            i++;
            num++;
        }else               //将该元素移动到队尾
        {
            MoveItToTheLast(q,i);
            num++;
        }
    }
}

void CustomerDeparture(struct Event** head,struct Event* en,struct Queue q[],int nums)      //处理离开事件，前提是en->NType>0
{
    int i = 1;
    int MoneyTemp;
    struct Event* en1;
    struct Qelem temp;
    struct Qelem qel;
    i = en->NType;
    if(i == 10000)
    {
        if(q[0].base[q[0].front].Amount+MoneyNums >= 0)
        {
            if(en->OccurTime < CloseTime)
            {
                TotalTime += en->OccurTime-q[0].base[q[0].front].ArrivalTime;
                MoneyNums += q[0].base[q[0].front].Amount;
                qel = q[0].base[q[0].front];
                printf("第%d位客人，在%d的系统时间离开,他/她总共的业务时长为%d个单位的时间,处理金额为%d元\n",qel.tag,en->OccurTime,en->OccurTime-q[0].base[q[0].front].ArrivalTime,qel.Amount);
                printf("\n");
                Dequeue(q);
                return ;
            }else
            {
                TotalTime += CloseTime-q[0].base[q[0].front].ArrivalTime;
                printf("第%d位客人在处理业务中,由于银行关闭,被迫离开\n",q[0].base[q[0].front].tag);
                printf("\n");
                Dequeue(q);
                return ;
            }
        }else
        {
            return;
        }
    }
    if(q[i].base[q[i].front].Amount >= 0)           //若该位离开的客户是存款
    {
        if(en->OccurTime < CloseTime)
        {
            MoneyTemp = MoneyNums;          //记录下此时银行所有的存款
            MoneyNums += q[i].base[q[i].front].Amount;
            //下面检索取款队列中是否有可走的,并产生离开事件插入到事件表中
            if(!QEmpty(q))
            {
                DealGetque(head,en,q,MoneyTemp);
            }
            TotalTime += en->OccurTime-q[i].base[q[i].front].ArrivalTime;
            qel = q[i].base[q[i].front];
            printf("第%d位客人，在%d的系统时间离开，他/她总共的业务时长为%d个单位的时间,处理金额为%d元\n",qel.tag,en->OccurTime,en->OccurTime-q[i].base[q[i].front].ArrivalTime,qel.Amount);
            printf("\n");
            Dequeue(q+i);
        }else
        {
            TotalTime += CloseTime-q[i].base[q[i].front].ArrivalTime;
            printf("第%d位客人在处理业务中,由于银行关闭,被迫离开\n",q[i].base[q[i].front].tag);
            printf("\n");
            Dequeue(q+i);
            return ;
        }
        
    }else
    {
        if(en->OccurTime < CloseTime)
        {
            if(MoneyNums+q[i].base[q[i].front].Amount >= 0)
            {
                MoneyNums += q[i].base[q[i].front].Amount;
                TotalTime += en->OccurTime-q[i].base[q[i].front].ArrivalTime;
                qel = q[i].base[q[i].front];
                printf("第%d位客人，在%d的系统时间离开，他/她总共的业务时长为%d个单位的时间,处理金额为%d元\n",qel.tag,en->OccurTime,en->OccurTime-q[i].base[q[i].front].ArrivalTime,qel.Amount);
                printf("\n");
                Dequeue(q+i);
            }else
            {
                TotalTime += en->OccurTime-q[i].base[q[i].front].ArrivalTime;
                q[i].base[q[i].front].ArrivalTime = en->OccurTime;
                printf("第%d位客人离开了,前往了0号取款队列\n",q[i].base[q[i].front].tag);
                printf("\n");
                EnQueue(q,q[i].base[q[i].front]);
                Dequeue(q+i);
            }
        }else
        {
            TotalTime += CloseTime-q[i].base[q[i].front].ArrivalTime;
            printf("第%d位客人在处理业务中,由于银行关闭,被迫离开\n",q[i].base[q[i].front].tag);
            printf("\n");
            Dequeue(q+i);
            return ;
        }
        
    }
    if(!QEmpty(q+i))
    {
        temp=GetHead(q+i);
        en1=(struct Event*)malloc(sizeof(struct Event));
        en1->OccurTime = en->OccurTime+temp.Duration;
        en1->NType=i;
        Insert(head,en1);               //产生下一个离开事件
    }
}


int main()
{
    struct Event *head = NULL;    //定义事件链表，无头结点
    int nums;                     //nums为有多少条存款队列
    struct Queue *q;
    srand((unsigned)time(NULL));
    printf("请输入银行今天的营业总时间:");
    scanf("%d",&CloseTime);
    printf("请输入银行今天的总资金:");
    scanf("%d",&MoneyNums);
    printf("请输入第一种队列数:");
    scanf("%d",&nums);
    q=(struct Queue*)malloc(sizeof(struct Queue)*(nums+1));      //申请nums+1个队列空间，其中第0个队列为取款队列
    for(int i=0;i<nums+1;i++)
    {
        q[i].base=(struct Qelem*)malloc(sizeof(struct Qelem)*MAXSIZE);
        q[i].front=0;
        q[i].rear=0;
    }
    head = OpenForDay();         //一天开始，第一个客户到达
    while(head)
    {
        struct Event* en;
        en = head;
        head = head->next;
        if(en->NType == 0)
        {
            CustomerArrived(&head,en,q,nums);
        }
        else CustomerDeparture(&head,en,q,nums);
    }
    printf("银行关闭,此时所有人员被迫离开\n");
    printf("\n");
    printf("本次营业总体的平均等待时间为:%f\n",(float)TotalTime/CustomerNum);
    printf("\n");
    return 0;
}
