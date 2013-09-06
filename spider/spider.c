#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

void main()
{
     FILE *fp = fopen("dns", "r");
     if(fp == NULL)
     {
          fprintf(stderr, "Open file failed:%s\n", strerror(errno));
          printf("Cant't find dns file !\n");
          exit(1);
     }
     char str[68], PathName[86];
     if(getcwd(PathName, sizeof(PathName)) == NULL)
     {
          fprintf(stderr, "getcwd failed:%s\n", strerror(errno));
          exit(1);
     }
     int i=0;
     strcat(PathName, "/getweb");
     pid_t    pid = fork();
     int r;
     if(pid < 0)
     {
          printf("Create fork failed!\n");
          fprintf(stderr, "fork:%s\n", strerror(errno));         
     }
     else if(pid == 0)
     {
          while(1)
          {
               i++;
               fgets(str, 67 , fp);
               int sum = strlen(str);
               if(str[sum-1] == '\n')   str[sum-1] = '\0';            
               printf("pn is: %s; dns is: %s\n", PathName,str);
               r = execl(PathName , "getweb", str, NULL);
               if(r < 0)
                    fprintf(stderr, "execl:%s\n", strerror(errno));
               
               if(feof(fp)) break;
          }
          fclose(fp);
     }
}
