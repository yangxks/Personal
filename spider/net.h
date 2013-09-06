#include <netdb.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <sys/time.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#define TIME_OUT_TIME 8
#define MAX_TYPE 9
#define MAX_SIZE 4096
#define MAX_HOST 40
#define MAX_PATHNAME 256
#define MAX_FILENAME 256
#define MAX_URLS 1024
#define Max_Hrlink 1024
#define LOG_FILE "/var/log/spider.log"

static unsigned int FileLeng ;
static char Allow[MAX_TYPE][5]={"html", "htm","jsp","asp","php","xml","pdf","doc", "xls"};

typedef struct UrlInfo{
     char Host[MAX_HOST];
     char PathName[MAX_PATHNAME];
     char FileName[MAX_FILENAME];
     char GetInfo[MAX_PATHNAME];
     char SaveName[MAX_FILENAME];
}UrlInfo;
typedef struct HyperTable{
     struct HyperTable *next;
     UrlInfo *data;
}HyperTable;

char *gethost(char *p, char *h)
{
	struct hostent *addr;
	if((addr=gethostbyname(h)) == NULL)
	{
		printf("Get host ip failed!\n");
		perror("Error:");
          printf("URL is:%s\n", h);
          return NULL;
	}
	strcpy(p, inet_ntoa(*((struct in_addr*)addr->h_addr)));
	return p;
}
void cut(const char *u,char *h,char *p)
{
	int n=0,t=0,i=7;
	while(u[i]!='\0')
	{
		if(u[i]!='/') 
		{
			h[n]=u[i];
			n++;
			i++;
		}else break;
	}
	while(u[i]!='\0')
	{
		p[t]=u[i];
		t++;
		i++;
	}
     if(t==0)
     {
          p[t]='/';
          t++;
     }
	p[t]=h[n]='\0';
}
void getdata(char *d,char *h,char *p)
{
	char u[66];
	sprintf(u,"Host: %s\r\nConnection: Close\r\n\r\n",h);
	sprintf(d,"GET %s HTTP/1.1\nAccept: */*\r\n", p);
	strcat(d,"Accept-Language: zh-cn\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)\r\n");
	strcat(d,u);
}
char *getname(char *pn)
{
      int n, i;
      n = strlen(pn);
       n--;
      while(pn[n] !='/' && n>=0) n--;
      //if(n <=0) pn;
      i=0;
      n++;
      while(pn[n] != '\0')
      {
            pn[i] = pn[n];
            i++;
            n++;
      }
      pn[i] = '\0';
     
      char *str=strchr(pn, '.');
      i=0;
      if(str!=NULL)
      {
            str++;
            for(i=0;i<MAX_TYPE;i++)
               if(strcmp(str, Allow[i])==0) break;
      }
      if(i>=MAX_TYPE) { pn[0]='\0'; return pn; }
      if(pn[0]=='\0')  strcpy(pn,"index.html");          
      return pn;
}
unsigned int getlength(char *head)
{
     unsigned int sum = 0;
      char *n= strstr(head, "Length:");
      if(n!=NULL)
      {
            n =strchr(n, ':');
            n++;
            sum = atol(n);
      }
      return sum;;
}
void ToLowerCase(char * str)
{
     int i=0;
     while(str[i]!='\0')
     {
          if(str[i] > 64&&str[i] < 123) 
          str[i] = tolower(str[i]);
          i++;
     }
}
int decide(char *header)
{
      int sum=0;
      char *n= strstr(header, "HTTP/1.");

      if(n!=NULL)
      {
            n =strchr(n, ' ');
            n++;
            sum = atol(n);
      }
      return sum;
}

void itoa(unsigned int n, char *str)
{
     int i=0,t,sum=10;
     while(1)
     {
          t = n % sum;
          n = n / sum;
          if(t==0&&n==0) break;
          str[i]=48+t;
          i++;
     }
     str[i] = '\0';
     t=0;
     i--;
     char ch;
     while(t != i)
     {
          ch = str[t];
          str[t] = str[i];
          str[i] = ch;
          t++;
          i--;
          if(t>i) break;
     }
}
int checktype(UrlInfo *u)
{
     char *str = strstr(u->FileName, "doc");
     char *str1 = strstr(u->FileName, "xls");
     char *str2 = strstr(u->FileName, "pdf");
     if(str!=NULL || str1!=NULL ||str2!=NULL) return 0;
     return 1;
}
