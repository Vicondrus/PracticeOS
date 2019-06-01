#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>

int main(int argc, char *argv[])
{
    int pid;
    int stat;
    pid = fork();
    if (pid == -1)
    {
        printf("Could not create process\n");
        exit(1);
    }
    if (pid == 0)
    {
        printf("Child says: pid=%d, ppid=%d, cpid=%d\n",getpid(),getppid(),0);
        execv("/bin/ls",(char *[]){"/bin/ls", argv[1], NULL});
    }
    else
    {
        printf("Parent says: pid=%d, ppid=%d, cpid=%d\n",getpid(),getppid(),pid);
        waitpid(pid,&stat,0);
        printf("Stat: %d\n",stat);
    }

    return 0;
}