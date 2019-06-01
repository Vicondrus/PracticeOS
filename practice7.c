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

#define N 100

sem_t parkingSem, counterSem;

int no_cars;

void *car_parking(void *arg)
{
    sem_wait(&parkingSem);
    sem_wait(&counterSem);
    no_cars++;
    sem_post(&counterSem);
    printf("No of cars in the park: %d\n", no_cars);
    usleep(2000);
    sem_wait(&counterSem);
    no_cars--;
    sem_post(&counterSem);
    sem_post(&parkingSem);
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t threads[N];

    sem_init(&parkingSem, 0, 50);
    sem_init(&counterSem, 0, 1);

    for (int i = 0; i < N; i++)
    {
        pthread_create(&threads[i], NULL, car_parking, NULL);
        usleep((((i%4)*121)%32));
    }
}