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

typedef struct a
{
    int start;
    int result;
}ARGT;

int sum, N;
sem_t sem;

void *compute(void *arg)
{
    ARGT *x = (ARGT *)arg;
    x->result=0;
    for(int i=x->start;i<(x->start)+N;i++)
        x->result+=i;
    printf("Thread %d has %d\n",x->start/N,x->result);
    sem_wait(&sem);
    sum+=x->result;
    sem_post(&sem);
    return 0;
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Program use: ./practice4 <N>");
        exit(1);
    }

    N=atoi(argv[1]);

    sem_init(&sem,0,1);

    pthread_t threads[N+1];
    ARGT starts[N+1];
    printf("N is %d\n",N);
    for(int i=0;i<N;i++)
    {
        starts[i].start=i*N+1;
        if(pthread_create(&threads[i],NULL,compute,&starts[i])<0)
            printf("Could not create threads\n");
    }

    for(int i=0;i<N;i++)
        pthread_join(threads[i],NULL);
    
    //for(int i=0;i<N;i++)
    //    sum
    printf("%d\n",sum);
}