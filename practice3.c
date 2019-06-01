#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int fd1[2], fd2[2];

int main()
{
    pipe(fd1);
    pipe(fd2);

    int pid;
    pid = fork();
    if (pid == 0)
    {
        char buf[30];
        char aux[30] = "Sol-Do-La-Ti-Do-Re-Do";
        strncpy(buf + 1, aux, strlen(aux));
        buf[0] = strlen(aux);
        write(fd2[1], buf, 1);
        write(fd2[1], buf + 1, buf[0]);
        read(fd1[0], buf, 1);
        read(fd1[0], buf + 1, buf[0]);
        buf[buf[0] + 1] = 0;
        printf("Child process says: %s\n", buf + 1);
    }
    else
    {
        char buf[30];
        char aux[30] = "Sol-Do-La-Fa-Me-Do-Re";
        strncpy(buf + 1, aux, strlen(aux));
        buf[0] = strlen(aux);
        write(fd1[1], buf, 1);
        write(fd1[1], buf + 1, buf[0]);
        read(fd2[0], buf, 1);
        read(fd2[0], buf + 1, buf[0]);
        buf[buf[0] + 1] = 0;
        printf("Parent process says: %s\n", buf + 1);
    }
}