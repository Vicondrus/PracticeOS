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

int remainingPersons, onBoard, MAX;
sem_t embarkPerson, personsSem, takeOffSem, disembarkSem;

void *rescued_person(void *arg)
{
    int id = *((int *)arg);
    sem_wait(&personsSem);
    sem_wait(&embarkPerson);
    printf("Person %d embarking\n", id);
    onBoard++;
    remainingPersons--;
    if (onBoard == MAX || remainingPersons == 0)
    {
        printf("Max cap reached with person %d\n", id);
        sem_post(&takeOffSem);
    }
    sem_post(&embarkPerson);
    sem_wait(&disembarkSem);
    printf("Disembarking person %d\n", id);
    onBoard--;
    usleep(1000);
    if (onBoard == 0)
    {
        printf("All have disembarked\n");
        sem_post(&takeOffSem);
    }
    sem_post(&disembarkSem);
    return 0;
}

void embark()
{
    printf("Embarking\n");
    sem_trywait(&disembarkSem);
    for (int i = 0; i < MAX; i++)
        sem_post(&personsSem);
}

void fly()
{
    sem_wait(&takeOffSem);
    printf("Flying\n");
    usleep(500);
}

void flyBack(int MAX)
{
    sem_wait(&takeOffSem);
    printf("Flying back\n");
    usleep(500);
}

void disembark()
{
    printf("Disembarking\n");
    sem_post(&disembarkSem);
}

void *airplane(void *arg)
{
    int size = *(int *)arg;
    while (remainingPersons > size)
    {
        printf("Plane waiting\n");
        embark();
        fly();
        disembark();
        flyBack(size);
    }
    printf("Plane waiting\n");
    embark();
    fly();
    disembark();
    printf("Finished\n");
    return 0;
}

int main(int argc, char *argv[])
{
    int N;
    if (argc != 3)
    {
        printf("Parameters not good. Use program <N> <MAX>\n");
        exit(1);
    }
    N = atoi(argv[1]);
    MAX = atoi(argv[2]);
    printf("%d %d\n", N, MAX);
    pthread_t threads[N + 2];
    int args[N + 2];

    remainingPersons = N;

    printf("init sems\n");
    if (sem_init(&embarkPerson, 0, 1) == -1)
    {
        printf("could not initialize embark person\n");
    }

    if (sem_init(&personsSem, 0, 0) == -1)
    {
        printf("could not initialize persons sem\n");
    }

    if (sem_init(&disembarkSem, 0, 0) == -1)
    {
        printf("could not initialize disembark sem\n");
    }

    if (sem_init(&takeOffSem, 0, 0) == -1)
    {
        printf("could not initialize persons sem\n");
    }
    pthread_t plane;
    if (pthread_create(&plane, NULL, airplane, &MAX) != 0)
    {
        printf("Error creating plane thread\n");
    }

    for (int i = 0; i < N; i++)
    {
        args[i] = i;
        if (pthread_create(&threads[i], NULL, rescued_person, &args[i]) != 0)
        {
            printf("Error creating threads\n");
        }
    }
    for (int i = 0; i < N; i++)
    {
        pthread_join(threads[i], NULL);
    }
    pthread_join(plane, NULL);
    return 0;
}