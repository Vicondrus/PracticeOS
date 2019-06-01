#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
    int fd, n;
    if (argc != 2)
    {
        printf("Use program <file>\n");
        exit(1);
    }
    fd = open(argv[1], O_RDWR);
    n = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    printf("%d\n", n);
    if (n % 16 != 0)
    {
        ftruncate(fd, (n / 16) * 16);
    }
    n = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    printf("%d\n", n);
    lseek(fd,(n/4)*2,SEEK_SET);
    char buf[n/4+5];
    read(fd,buf,n/4);
    for(int i=0;i<n/4;i++)
        //printf("%u ",buf[i]);
    //int fout;
    //fout=open(stdout,O_WRONLY);
        write(1,&buf[i],1);
    printf("\n");
    lseek(fd,(n/4)*2,SEEK_SET);
    unsigned int *data = (unsigned int *)mmap(NULL,n/4,PROT_READ,MAP_PRIVATE,fd,0);
    unsigned int sum=0;
    for(int i=0;i<n/4;i++)
        sum+=data[i];
    printf("%d\n",sum);
    return 0;
}