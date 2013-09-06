#include "find.h"
#include "queue.h"

int DownloadPage(UrlInfo *U1)
{
    char log_str[1024];
    FILE *fp;
    unsigned long ul = 1;
    int sockfd, n, i, sum;
    char header[1024],buffer[MAX_SIZE],data[460];
    struct sockaddr_in link;

    getdata(data, U1->Host, U1->GetInfo);
    if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        sprintf(log_str, "Socket error: %s.", strerror(errno));
        fprintf(stderr, "Socket error: %s\n", strerror(errno));
        return err_info(sockfd, log_str);
    }

    link.sin_family = AF_INET;
    link.sin_port = htons(80);
    if(gethost(header, U1->Host) == NULL)
        return err_info(sockfd, "DNS is error!");
      
    inet_aton(header, (struct in_addr *)&(link.sin_addr.s_addr));
    fprintf(stderr, "Waiting for connect %s.............\n", header);

    ioctl(sockfd, FIONBIO, &ul);
    if(connect(sockfd, (struct sockaddr*)&link, sizeof(struct sockaddr_in)) == -1) {          
        if(time_out(sockfd) == 0) {                                   
            sprintf(log_str, "Connect time out! URLs:http://%s%s%s",
                    U1->Host, U1->PathName, U1->FileName);
            return err_info(sockfd, log_str);
        }
    }
    
    printf("connect %s OK!\n", U1->Host);
    while((n=write(sockfd, data, strlen(data))) < 0) {

        if(errno == EINTR) {
            fprintf(stderr, "write() errno==EINTR!\n");
            sleep(1);
            continue;
        }
        else if(errno == EAGAIN) {
            fprintf(stderr, "write() errno==EAGAIN!\n");
            sleep(1);
            continue;
        } else {
            sprintf(header ,"write() error: %s\n", strerror(errno));            
            return err_info(sockfd ,header);
        }        
    }
    
    fprintf(stdout, "Send data OK,send size:%d.\n", n);
    fprintf(stdout, "Waiting for data from %s...........\n", U1->Host);

    ul = 1;
    if(time_out(sockfd) == 0)
        return err_info(sockfd, "read data time out!");

    while((n=read(sockfd,buffer,1)) < 0) {
        if(errno == EINTR) {
            fprintf(stderr, "read() errno==EINTR!\n");
            sleep(1);
            continue;
        }
        else if(errno == EAGAIN) {
            fprintf(stderr, "read() errno==EAGAIN!\n");
            sleep(1);
            continue;
        } else {
            sprintf(header ,"read() error: %s.", strerror(errno));            
            return err_info(sockfd ,header);
        }        
    }

    sum=0,i=0;
    while((n=read(sockfd,buffer,1)) == 1)
    {                              
        if(i < 3)
        {              
            if(buffer[0] == '\r' || buffer[0] == '\n')  i++;
            else i = 0;                
            header[sum]=buffer[0];
            sum++;
        }
        else if( i>=3 ) break;
    }
    
    header[sum-2]='\0';
    if(strlen(header) < 6) return 0;
    if(decide(header) > 1023)
    {
        fprintf(stdout, "header:####################\nH%s######################\n", header);
        sprintf(header, "error request: %d", decide(header));
        return err_info(sockfd, header);
    }
    
    if((fp=fopen(U1->SaveName, "w")) == NULL) {
        sprintf(header, "Create file error,%s\n", strerror(errno));          
        return err_info(sockfd, header);
    }

    FileLeng = getlength(header);
    printf("The file length:%u (%uk)\nSaving to '%s'\n", FileLeng,FileLeng/1024, U1->SaveName);
    memset(buffer,0,strlen(buffer));
    sprintf(buffer, "URLs:http://%s%s%s.\n",
            U1->Host, U1->PathName, U1->FileName);
    fwrite(buffer, strlen(buffer), 1, fp);
        
    while((n=read(sockfd, buffer, MAX_SIZE)) > 0) {                        
        fwrite(buffer, n, 1, fp);      
        fflush(fp);            
    }
    
    if(n==-1) {
        sprintf(header, "read error:%s\n", strerror(errno));
        err_info(sockfd, header);
    }
    fclose(fp);
    close(sockfd);
    
    return 0;
}

void Download(char *Host)
{
    qnode *qn;
    FILE *fp;
    unsigned int i;  
 
    UrlInfo url;
    HyperTable *Ht=(HyperTable *)malloc(sizeof(HyperTable));
    Ht->data=NULL;
        
    if((fp=fopen("dns", "w")) == NULL) {
        fprintf(stderr, "Create file error:%s\n", strerror(errno));
        return;
    }
    
    fprintf(fp, "http://%s/", Host);
    fputs("\n",fp);
    fclose(fp);
    strcpy(url.Host, Host);
    strcpy(url.GetInfo, "/");
    itoa(1, url.SaveName);
    strcpy(url.FileName, url.SaveName);
    DownloadPage(&url);    
    Queue *Q=CreateQueue();
    Ht->next = NULL;
    Parse_href(&url, Ht);
    Queue_addurl(Q, Ht, "dns");

    i=1;
    while(1)
    {
        qn = OutQueue(Q);
        if(qn == NULL) break;
        i++;
        itoa(i, qn->data->SaveName); 
        if(DownloadPage(qn->data) != 0)
        {
            i--;
            free(qn->data);
            continue;
        }
        if(checktype(qn->data)==0)
        {
            free(qn->data);
            continue;
        }
        printf("Download: http://%s%s%s\n", qn->data->Host,qn->data->PathName,qn->data->FileName);        
        Ht->data =NULL;
        Ht->next= NULL;
        printf("parseing url.........\n");
        Parse_href(qn->data,Ht);
        if(Ht->next!= NULL)   Queue_addurl(Q,Ht,"dns");
        free(qn->data);                   
    }
}

int main(int argc, char **argv)
{
    char host[128];
    
    strcpy(host, argv[1]);
	if(argc!=2)	{
		printf("please input a DNS'name!\n");
		exit(1);
	}
    
    if(mkdir(host, S_IRWXU) == -1) {
        fprintf(stderr, "mkdir error:%s\n",strerror(errno));
        exit(0);
    }
    
    Chdir(host);     
    Download(host);
    return 0;
}
