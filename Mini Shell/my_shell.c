#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <fcntl.h>
#include <signal.h>

void handler(int num)
{
  printf("\n");
  //write(STDOUT_FILENO, "\n", 13);
}
void execute_command_pipe(char* argv[], char* argv2[])
{
    int ppe[2];
    int s;
    int read=0;
    int write=1;
    pipe(ppe);
    pid_t pid = fork();

    if(pid==0)
    {
        dup2(ppe[write], STDOUT_FILENO);
        close(ppe[read]);
        close(ppe[write]);
        execvp(argv[0],argv);
        fprintf(stderr, "Child process could not do execvp \n");
        exit(1);
    }
    else
    {
        pid=fork();
        if(pid==0)
        {
            dup2(ppe[read], STDIN_FILENO);
            close(ppe[write]);
            close(ppe[read]);
            execvp(argv2[0],argv2);
            fprintf(stderr, "Child process could not do execvp \n");
            exit(1);
        }
        else
        {
            close(ppe[read]);
            close(ppe[write]);
            waitpid(pid, &s, 0);
        }
    }
}
void execute_command_background(char* argv[])
{
  pid_t pid= fork();

	if(pid<0)
	{
		perror("Fork Failed \n");
		exit(0);
	}
  if(pid==0)                  //child
  {
    printf("data1[x] : %s\n", argv[0] );
    //printf("data1[x] : %s\n", argv[1] );
    //printf("data1[x] : %s\n", argv[2] );
      execvp(argv[0],argv);
      fprintf(stderr, "Child process could not do execvp\n");
			//exit(WEXIT_STATUS);
  }
   if(pid >1) // parent
  {
    printf("PID is %d\n", pid);

  }
}
int execute_command(char progpath[], char* argv[])
{

  pid_t pid= fork();

		if(pid<0)
		{
			perror("Fork Failed \n");
			exit(0);
		}
  if(pid==0)                  //child
  {
		 // printf("PID is %d \n",getpid() );

      execvp(argv[0],argv);
      fprintf(stderr, "Child process could not do execvp\n");
			exit(0);
  }
   if(pid >1) // parent
  {
      //printf("Child is %d\n", pid);
      wait(NULL);
  }
	return 0;
}

void write_to_file(char* fname, char a[], char* b[])
{
  int pid, status;
	int newfd;
  //printf("%s is file name\n",fname );
	if ((newfd = open(fname, O_APPEND|O_CREAT|O_RDWR|O_TRUNC, 0666)) < 0)
  {
		perror("Open Failed");
		exit(1);
	}
  int savestdout= dup(1);
	dup2(newfd,1);
  execute_command(a,b);
  dup2(savestdout,1);
  close(newfd);

}
void read_from_file(char arg[],char argum[], char* argv[])
{
   FILE *fp;
   char buff[255];
   char* data1[255];

   fp = fopen(arg, "r");
	 int x=0;
   while (!feof(fp))
   {
     fscanf(fp, "%s", buff);
		 data1[x]=buff;
     x++;
   }
	 char* data[x-1];
   int i;
	 for (  i = 0 ; i< x; i++)
	 {
			if(i==0)
			data[i]=argum;
			if(i > 0 && !strcmp(data1[i], data1[i-1] ))
			data[i]=data1[i-1];

	}
  data[i]=NULL;
fclose(fp);
	execute_command(argum,data);

}


void read_from_terminal(char line[])
{
  printf("minish> ");
  char *x= fgets(line, 1024, stdin);
  if(!x)
  {
      exit(1);
  }
  size_t length = strlen(line);
  if (line[length - 1] == '\n')
  {
      line[length - 1] = '\0';
  }
  if(strcmp(line, "exit")==0)
  {
        exit(0);
  }
}
int main()
{
    char line[1024];
    char* argv[100];
    char path[20];
    int argc;
    char *token;
    char line2[1024];
    signal(SIGINT, handler);
    while(1)
    {
        read_from_terminal(line);
        token = strtok(line," ");
        int i=0;
        while(token!=NULL)
        {
            argv[i]=token;
            token = strtok(NULL," ");
            i++;
        }
        argv[i]=NULL;
        argc=i;
        for(i=0; i<strlen(path); i++)
        {
            if(path[i]=='\n'){
                path[i]='\0';
            }
        }
        char* argv1[100];
        for(int y=0; y<argc;y++)
        {
          if(!strcmp(argv[y], ">") || !strcmp(argv[y], "<") || !strcmp(argv[y], "&") || !strcmp(argv[y], "|"))
          {
             break;
          }
          argv1[y]=argv[y];

        }
        int z=0;
        char* argv2[10];
        for(int y=0; y<argc;y++)
        {
          if(!strcmp(argv[y], "|"))
          {
            for(int x=y+1; x<argc;x++)
            {
              argv2[z]=argv[x];
              z++;
            }
            break;
          }

        }
        //printf("argv2[0] is %s\n",argv2[0] );
        //printf("argv2[1] is %s\n",argv2[1] );
        int count=0;
				int co=0;
        if(argc>1)
        {
            for(int x=0; x<argc;x++)
            {
              if(!strcmp(argv[x], ">"))
              {
                argv1[x]=NULL;
                char *name=argv[x+1];
                write_to_file(name, argv1[0],argv1);
                count=1;
              }
              if(!strcmp(argv[x], "<"))
              {
                argv1[x]=NULL;
                char *name=argv[x+1];
                count=1;
                read_from_file(name, argv1[0], argv1);
              }

              if(!strcmp(argv[x], "&"))
              {
								co=1;
                argv1[x]=NULL;
								execute_command_background(argv1);
                count=1;
              }
              if(!strcmp(argv[x], "|"))
              {
                //printf("argv2[0] is %s\n",argv2[0] );
                argv1[x]=NULL;
                argv2[x]=NULL;
								execute_command_pipe(argv1,argv2);
                count=1;
              }
          }
        }
				co=0;
        if(count==0)
        execute_command(argv[0],argv);
      }
}
