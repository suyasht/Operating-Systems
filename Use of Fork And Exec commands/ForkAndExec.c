#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc,char* argv[])
{
	if(argc!=3) // Checks for the correct number of command line argument passed
	{
		printf("Pass 2 arguments ( Height and Number of Children) \n");
		exit(1);
	}
	int t= atoi(argv[1]);   //Converts the Height H received from command line to INT
	int s= atoi(argv[2]);	//Converts the Children C received from command line to INT
	int h=t,c=s;
	pid_t pid;
	printf("(%d): Process starting \n", getpid() );
	printf("(%d): Parents id = (%d)\n", getpid(), getppid());
	printf("(%d): Height in the tree = %d \n", getpid(), h);

	if(h>1) //If height is greater than 1, child process are created
	{
		printf("(%d): Creating %d children at height (%d)\n", getpid(), c , (h-1) );
		for(int y=1;y<=c;y++)
		{
			pid = fork();

			if(pid < 0)
			{
				perror("Fork failed \n");    // IF fork fails, and error is displayed
			}

			if (pid == 0) //Runs the exec command for each child process
			{
				char xy[10];
				sprintf(xy,"%d",(h-1));
				char* decrementH=xy;
				char ab[10];
				sprintf(ab,"%d",c);
				char* child=ab;
				//char *args[]= {"./test.c",cd,ch,NULL};
				execlp(argv[0],argv[0],decrementH, child,(char *)NULL);
				perror("EXEC Failed");   // IF EXECLP fails, the eroor is displayed

			}

		}
	}
	for(int z=1;z<=c;z++) //Waits for all child process to finish, so that there are no zombie processes
	{
		wait(NULL);
	}
	printf("(%d): Terminating at height (%d) \n",getpid() ,h);

	return 0;
}



