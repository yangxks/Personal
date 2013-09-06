typedef struct queue{
    struct qnode *front;
    struct qnode *rear;
}Queue;

typedef struct qnode{
    UrlInfo *data;
    struct qnode *next;
}qnode;

Queue *CreateQueue() {
    Queue *Q =(Queue *)malloc(sizeof(Queue));
    qnode *u =(qnode *)malloc(sizeof(qnode));
    
    u->data = NULL;
    u->next = NULL;
    Q->front = u;
    Q->rear = u;

    return Q;
}

int isempty(Queue *Q) {
    if(Q->front == Q->rear)
        return 0;
    else return 1;
}

void InQueue(Queue *Q,UrlInfo *u) {
    qnode *n=(qnode *)malloc(sizeof(qnode));
     
    n->data = u;
    n->next = NULL;
    qnode *q = Q->front;
    while(q->next != NULL) q=q->next;
    q->next = n;
    Q->rear = n;
}

qnode *OutQueue(Queue *Q) {
    qnode *r,*q = Q->front;
     
    if(q->next != NULL) {
        r=q->next;
        if(q->next->next == NULL) {
            q->next = NULL;
            Q->rear=q;
        }else
            q->next = q->next->next;
        return r;
    }
    else
        return NULL;
}

void print(Queue *q)
{
    qnode *n=OutQueue(q);
     
    while(n != NULL) {
        printf("Url is:http://%s%s\n", n->data->Host, n->data->GetInfo);
        free(n->data);
        free(n);
        n=OutQueue(q);         
    }
}

void input(Queue *q)
{
    char str[29];
    int i=0;
     
    while(i<2) {
        printf("Please input a string:");
        scanf("%s",str);          
        UrlInfo *u=(UrlInfo *)malloc(sizeof(UrlInfo));
        strncpy(u->Host, str, strlen(str));
        InQueue(q, u);
        i++;
    }     
}

void delete_node(HyperTable *t, HyperTable *p)
{
    if(p->next == NULL)
        t->next=NULL;
    else
        t->next = p->next;
    free(p);
    
    return;
}
void Queue_addurl(Queue *Q, HyperTable *h, char *name)
{
    HyperTable *p,*t;
    int i;
    char ch,str[MAX_URLS],str1[MAX_URLS];
    FILE *fp = fopen(name, "r+");
     
    if(fp == NULL) {
        fprintf(stderr, "Create file error:%s\n", strerror(errno));
        fclose(fp);
        return;
    }
     
    while(1)
    {
        i=0;
        ch =fgetc(fp);
        if(feof(fp)) break;
        while(ch!= '\n')
        {
            str[i]=ch;
            ch=fgetc(fp);
            i++;
        }
        str[i]='\0';
        p=h->next;
        t=h;
        while(p!=NULL)
        {               
            sprintf(str1, "http://%s%s",p->data->Host, p->data->GetInfo);
            if(strncmp(str, str1, MAX_URLS) == 0)
            {                                                                        
                delete_node(t, p);
                p=t->next;
            }
            t=p;
            if(p!=NULL)   p =p->next;
        }          
    }
    i=0;
    p=h->next;
    while(p != NULL)
    {
        InQueue(Q, p->data);    
        sprintf(str, "http://%s%s", p->data->Host, p->data->GetInfo);
        fwrite(str, strlen(str), 1, fp);
        fputs("\n", fp);
        p =p->next;
    }
    fclose(fp);
     
}

int time_out(int sock)
{
    int error, len;
    len = sizeof(int);
    struct timeval tm;
    fd_set set;
    unsigned long ul = 1;
    int ret;
     
    tm.tv_sec = TIME_OUT_TIME;
    tm.tv_usec = 0;
    FD_ZERO(&set);
    FD_SET(sock, &set);

    ret = 0;
    error = -1;
    if(select(sock+1, NULL, &set, NULL, &tm) > 0) {
        getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
        if(error == 0)
            ret = 1;
    }
    ul = 0;
    ioctl(sock, FIONBIO, &ul);
     
    return ret;
}

int err_info(int sock, char *info)
{
	FILE *fp = fopen(LOG_FILE, "a+");
    if (fp == NULL)
		return 1;
    
    int num;
    time_t now;
    struct tm  *timenow;
    char str[1024], date[128];

    time(&now);
    timenow = localtime(&now);
    sprintf(date, "%s", asctime(timenow));
    num = strlen(date);
    date[num-1] = '\0';
    
    sprintf(str, "%s  -------%s.\n", date, info);
	fwrite(str, strlen(str), 1, fp);
	fclose(fp);
    close(sock);

    return 0;
}
