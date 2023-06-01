#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_THREADS 5
#define MAX_BUFFER_SIZE 10

sem_t empty;
sem_t full;
sem_t mutex;
int buffer[MAX_BUFFER_SIZE];
int buffer_size = 0;

void* Producer(void* threadid)
{
    int i;
    for (i = 0; i < 1000; i++)
    {
        sem_wait(&empty);
        sem_wait(&mutex);

        if (buffer_size < MAX_BUFFER_SIZE) // buffer_size < 10 ?
        {
            buffer[buffer_size] = i;     
            printf("Producer %ld puts %d into buffer at place %d\n", (long)threadid, buffer[buffer_size], buffer_size);
            buffer_size++; //buffer wurde befüllt also +1
        }

        sem_post(&mutex);
        sem_post(&full);
    }
    pthread_exit(NULL);
}

void* Consumer(void* threadid)
{
    int i;
    while (1)
    {
        sem_wait(&full);
        sem_wait(&mutex);

        if (buffer_size > 0)
        {
            int value = buffer[buffer_size - 1];
            buffer_size--;
            printf("Buffer: [%d, %d, %d] \n", buffer[0],buffer[1],buffer[2]);
            printf("Consumer %ld takes %d\n", (long)threadid, value);
            //Konsument hat aus buffer genommen, also sinkt Buffer um -1
        }

        sem_post(&mutex);
        sem_post(&empty);
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
    pthread_t threads[NUM_THREADS];
    int rc;
    long t;

    sem_init(&empty, 0, MAX_BUFFER_SIZE);  // initialize empty to maximum buffer size
    sem_init(&full, 0, 0);  // initialize full to 0
    sem_init(&mutex, 0, 1);  // initialize mutex to 1

    for (t = 0; t < NUM_THREADS; t++)
    {
        printf("In main: creating thread %ld\n", t);
        if (t == 0)
            rc = pthread_create(&threads[t], NULL, Producer, (void*)t);
        else
            rc = pthread_create(&threads[t], NULL, Consumer, (void*)t);

        if (rc)
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    for (t = 0; t < NUM_THREADS; t++)
    {
        pthread_join(threads[t], NULL);
    }

    sem_destroy(&mutex);
    sem_destroy(&full);
    sem_destroy(&empty);
    pthread_exit(NULL);
    return 0;
}
