#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>

int fd1[2], fd2[2], fd3[2];

int main(int argc, char *argv[])
{
    int fd;

    pipe(fd1);
    pipe(fd2);
    pipe(fd3);

    if (argc != 2)
    {
        printf("Arguments not good\n");
        exit(1);
    }

    fd = open(argv[1], O_RDONLY);

    if (fd < 0)
    {
        printf("Could not open file\n");
        exit(2);
    }

    char line[200];
    char buf[2];
    buf[0] = 0;
    int k = 0;
    while (buf[0] != '\n')
    {
        read(fd, buf, 1);
        line[k++] = buf[0];
    }
    line[k] = 0;
    char path[20];
    int nb;
    if (sscanf(line, "%s%d", path, &nb) < 2)
    {
        printf("Error reading file name and number\n");
        exit(3);
    }
    printf("%s %d\n", path, nb);
    int fdw = open(path,O_WRONLY);
    int pid2, pid3;
    pid2 = fork();
    switch (pid2)
    {
    case -1:
        printf("Error creating process 1\n");
        break;

    case 0:
        printf("Process 2 created\n");
        int aux = nb;
        while (aux > 0)
        {
            read(fd1[0], (void *)&aux, sizeof(int));
            printf("process 2 read %u\n", aux);
            aux--;
            //printf("process 2 writing %d\n",aux);
            write(fd2[1], (void *)&aux, sizeof(int));
        }
        printf("process 2 exited while\n");
        int pidq2 = getpid();
        if (aux == -1)
        {
            printf("process 2 writing pid\n");
            if (write(fdw, (void *)&pidq2, sizeof(int)) < 0)
                {
                    printf("Error writing pid\n");
                }
        }
        break;

    default:
        pid3 = fork();
        switch (pid3)
        {
        case -1:
            printf("Error creating process 1\n");
            break;

        case 0:
            printf("Process 3 created\n");
            aux = 2;
            while (aux > 0)
            {
                read(fd2[0], (void *)&aux, sizeof(int));
                printf("Process 3 read %u\n", aux);
                aux--;
                write(fd3[1], (void *)&aux, sizeof(int));
            }
            printf("process 3 exited while\n");
            int pidq3 = getpid();
            if (aux == -1)
            {
                printf("process 3 writing pid\n");
                if (write(fdw, (void *)&pidq3, sizeof(int)) < 0)
                {
                    printf("Error writing pid\n");
                }
            }
            break;

        default:
            printf("Doing process 1's job\n");
            aux = nb;
            write(fd1[1], (void *)&aux, sizeof(int));
            while (aux > 0)
            {
                read(fd3[0], (void *)&aux, sizeof(int));
                printf("process 1 read %u\n", aux);
                aux--;
                write(fd1[1], (void *)&aux, sizeof(int));
            }
            printf("process 1 exited while\n");
            int pidq1 = getpid();
            if (aux == -1)
            {
                printf("process 1 writing pid\n");
                if (write(fdw, (void *)&pidq1, sizeof(int)) < 0)
                {
                    printf("Error writing pid\n");
                }
            }
            waitpid(pid3, NULL, 0);
            break;
        }
        waitpid(pid2, NULL, 0);
        break;
    }
    close(fd);
    return 0;
}