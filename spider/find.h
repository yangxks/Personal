#include "net.h"

int find(char *str, int posion)
{
    int i, m,sum, sign, begin, href;

    i=posion;
    sum =strlen(str);
    m=0;
    sign=1;

    while((i++)<sum)
    {
        if(str[i] !='<')
            sign =1;
        else if(str[i]== '<' && str[i+1] =='a' && str[i+2]== ' ')
            sign =2;
        else if(str[i]== '<' && str[i+1]=='a' && str[i+2] != ' ')
            sign =3;
        else sign =4;
                 
        if(sign ==2)  break;
        else if(sign ==3) i++;
        else continue;
    }

    if(sign!=2) return -1;
    begin=i;
    if(i==sum-3) return begin;
    i=i+2;

    while((i++) <sum)
    {
        if(str[i] !='h')
            sign = 1;
        else if(str[i]=='h' && str[i+1]=='r' && str[i+2] == 'e' &&str[i+3]=='f')
            sign = 2;
        else if(str[i]=='h' && str[i+1]=='r' && str[i+2] == 'e' &&str[i+3] !='f')
            sign = 3;
        else if(str[i]=='h' && str[i+1]=='r' && str[i+2] != 'e')
            sign = 4;                 
                          
        if(sign ==2) break;
        else if(sign == 3) i=i+2;
        else if(sign == 4) i++;
    }
    
    href = i;m =0;

    while(i<=sum)
    {
        if(str[i] =='"')
        {
            m =5;
            break;
        }
        i++;
    }
    
    if(sign ==2 && m ==5) return i;
    else if(sign==sum) return -2; 
    return begin;
}

int checkname(char *url, const UrlInfo *furl, UrlInfo *surl)
{
    char name[MAX_PATHNAME],*s;
    int sum, i;
   
    sum = strlen(url);         
    s = strstr(url, "http://");

    if(s != NULL)
    {
        cut(url, surl->Host, surl->GetInfo);       
        strcpy(name ,surl->GetInfo);
        strcpy(surl->FileName ,getname(name));
               
        if(strlen(surl->FileName) == 0) return 0;
        strcpy(surl->PathName, surl->GetInfo);
        if((sum = strlen(surl->GetInfo))>1)
        {
            while(surl->PathName[sum] != '/')
                sum--;
            surl->PathName[sum+1]='\0';
        }
    }else
    {          
        strcpy(surl->Host, furl->Host);
               
        sum = strlen(url);
        while(url[sum] != '/') sum--;
        i=0;
        while( i <= sum)
        {
            surl->PathName[i] = url[i];
            i++;
        }
        surl->PathName[i] = '\0';
        strcpy(name, url);
        strcpy(surl->FileName, getname(name));
        if(strlen(surl->FileName) == 0) return 0;
        strcpy(surl->GetInfo ,url);
    }
    
    return 1;          
}

int check(char *url, const char *dns)
{
    int sum;
    char DNS[30],*s,*s1;
    s = strstr(dns, ".");
    if(s!=NULL)
    {
            //s=s+4;
        strcpy(DNS,s);
    }
    s =strstr(url, DNS);
    if(s!=NULL) return 1;     
    s = strstr(url, "www.");
    s1 = strstr(url, "http://");
    if(s !=NULL || s1 != NULL) return 0;
     
    sum = strlen(url);
    if(url[0] !='/')
    {
        while(sum != -2)
        {
            url[sum+2] = url[sum +1];
            sum--;
        }
        url[0]='/';
    }
    return 2;
}

int ToHeavy(char *url, const UrlInfo *furl, const HyperTable *h)
{
    int n,sum = strlen(url);

    if(sum <=0)
        return 0;  
    while(sum >= 0)
    {
        if(url[sum] == '?' || url[sum] == '#')
        {
            url[sum]='\0';
            break;
        }
        sum--;
    }

    if(check(url, furl->Host) > 0)
    {
        char str1[MAX_URLS], str[MAX_URLS];
        sum=10;
        char *s = strstr(url, "http://");
        if(s == NULL)  sprintf(str1, "http://%s%s", furl->Host,url);
        else   strcpy(str1, url);
        HyperTable *p=h->next;
        while(p != NULL)
        {
                // printf("host is:%s ,GetInfo is: %s\n", p->data->Host, p->data->GetInfo);
            sprintf(str, "http://%s%s", p->data->Host, p->data->GetInfo);
            n = strlen(str);
            if(str[n-1] == '/' && str1[strlen(str1)-1] != '/')
            {
                n = strlen(str1);
                    //printf("Url is: %s, n = %d\n", url, n);
                str1[n] = '/';
                str1[n+1] = '\0';
            }
            n = strlen(str1);
            if(str1[n-1] == '/' && str[strlen(str)-1] != '/')
            {
                n= strlen(str);
                str[n] = '/';
                str[n+1] = '\0';
            }
            if(strcmp(str1, str) ==0)
            {
                sum=0;
                break;
            }
            p = p->next;
        }
        if(sum==0)  return 0; else return 1;
    }
    return 0;
     
}

void InsertTable(HyperTable *Hy1, UrlInfo *insert)
{
    if(Hy1 == NULL)
    {
        printf("dddd\n");
        return;
    }
    HyperTable *p =Hy1;     
    while(p->next !=NULL) p=p->next;
    HyperTable *n =(HyperTable *)malloc(sizeof(HyperTable));
    n->data =insert;
    n->next =NULL;
    p->next =n;
}
void PrintTable(HyperTable *h)
{
    HyperTable *p=h->next;
    char str[MAX_URLS];
    while(p != NULL)
    {
        strcpy(str,"http://");
        strcat(str, p->data->Host);
        strcat(str, p->data->GetInfo);
        printf("Href is:%s, Host is:%s\n", str,p->data->Host);
        printf("Get info is:%s", p->data->GetInfo);
        printf(",Path name is: %s,File name is:%s\n\n\n", p->data->PathName, p->data->FileName);
        p = p->next;
    }          
}
void Mkdir(const char *pathname)
{
    if(mkdir(pathname, S_IRWXU) == -1);
    {
        fprintf(stderr, "mkdir error:%s\n",strerror(errno));
        exit(0);
    }
}
void Chdir(const char *pathname)
{
    if(chdir(pathname) == -1)
    {
        fprintf(stderr, "chdir error:%s\n", strerror(errno));
        exit(0);
    }
}
void Parse_href(UrlInfo *furl, HyperTable *Ht1)
{
    FILE *fp;
    char  first[4096], str1[4096],str[4096];
    int i,sum,sign,n;
    
    if((fp=fopen(furl->SaveName, "r"))==NULL)
    {
        printf("file:%s.\n", furl->SaveName);
        fprintf(stderr, "Open file error,%s\n", strerror(errno));
        return;
    }
    n=0;
    Ht1->next=NULL;
    first[0]='\0';
    while(1)
    {
        memset(str,0,strlen(str));
        memset(str1,0,strlen(str1));
        strcpy(str, first);
        memset(first,0,strlen(first));
        char *ch=fgets(str1, 1800, fp);
        if(ch==NULL) break;
        if(feof(fp)) break;
        strcat(str,str1);
        sum =strlen(str);      
        if(sum<2) continue;
        str[sum-1]=' ';str[sum]='\0';
        ToLowerCase(str);
        sign=-1;          
        while((sign=find(str,sign)) !=-1)
        {
            if(str[sign] =='<')
            {                   
                i=0;
                while(str[sign]!='\0')
                {
                    first[i]=str[sign];
                    sign++;i++;
                }
                first[i]='\0';
                if(i>20) memset(first,0,strlen(first));
                break;
            } else if(sign==-2) break;               
            sign++;i=0;
            while(str[sign]!='"' && str[sign]!='\0')
            {
                str1[i]=str[sign];
                i++;
                sign++;
            }
            str1[i]='\0';
            UrlInfo *SonUrl=(UrlInfo *)malloc(sizeof(UrlInfo));
            if(ToHeavy(str1, furl, Ht1) == 0) continue;
            if(checkname(str1, furl, SonUrl) >0)
            {                    
                InsertTable(Ht1 ,SonUrl);                  
                n++;
            }
        }            
    }
        // PrintTable(Ht1);
        //printf("Search link: %d\n", n);
    fclose(fp);
}
