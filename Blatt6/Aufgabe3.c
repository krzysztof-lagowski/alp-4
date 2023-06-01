#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int numberofthreads; //wird in der main durch numberofprods + numberofcons berechnet
int numberofprods = 3;
int numberofcons = 1;
int numberofproducts = 100; //countervariable wie viele Produkte erstellt werden sollen (Anzahl der Durchläufe)
#define NUM_PLACES 3

int last = 0;
int buffer[NUM_PLACES];

pthread_mutex_t monitorlock;

pthread_cond_t emptymonitor;
pthread_cond_t fullmonitor;
pthread_mutex_t mutexmonitor;

int counter;

void take(void* threadid, int productsToTake)
{
    pthread_mutex_lock(&mutexmonitor);

    while (counter < productsToTake)
    {
        pthread_cond_wait(&emptymonitor, &mutexmonitor);
    }

    for (int i = 0; i < productsToTake; i++)
    {
        if (buffer[i] != 0)
        {
            printf("Consumer %ld takes %d\n", (long)threadid, buffer[i]);
            buffer[i] = 0;
            counter--;
        }
    }

    for (int i = productsToTake; i < NUM_PLACES; i++)
    {
        buffer[i - productsToTake] = buffer[i];
        buffer[i] = 0;
    }

    printf("Buffer: [%d, %d, %d]\n", buffer[0], buffer[1], buffer[2]);
    fflush(stdout);

    last -= productsToTake;

    pthread_cond_signal(&fullmonitor);
    pthread_mutex_unlock(&mutexmonitor);
}

void store(void* threadid, int value)
{
    pthread_mutex_lock(&mutexmonitor);

    while (counter == NUM_PLACES)
    {
        pthread_cond_wait(&fullmonitor, &mutexmonitor);
    }

    buffer[last] = value;
    counter++;
    printf("Producer %ld puts %d into buffer at place %d\n", (long)threadid, buffer[last], last);
    printf("Buffer: [%d, %d, %d]\n", buffer[0], buffer[1], buffer[2]);
    fflush(stdout);

    last++;
    pthread_cond_signal(&emptymonitor);
    pthread_mutex_unlock(&mutexmonitor);
}

void* Producer(void* threadid)
{
    int i;
    for (i = 1; i < numberofproducts; i++)
    {
        store(threadid, i);
    }
    pthread_exit(NULL);
}

void* Consumer(void* threadid)
{
    int i;

    while (1)
    {
        take(threadid, 2); // Wie viele Produkte pro Iteration sollen genommen werden (hier 2)
    }
    pthread_exit(NULL);
}

int main(int argc, char const* argv[])
{
    int rc;
    long t;
    counter = 0;
    int numberofthreads = numberofprods + numberofcons;

    pthread_t threads[numberofthreads];

    pthread_cond_init(&emptymonitor, NULL);
    pthread_cond_init(&fullmonitor, NULL);
    pthread_mutex_init(&mutexmonitor, NULL);

    for (t = 0; t < NUM_PLACES; t++)
        buffer[t] = 0;
    last = 0;

    for (t = 0; t < numberofthreads; t++)
    {
        printf("In main: creating thread %ld\n", t);
        if (t == 0)
            rc = pthread_create(&threads[t],NULL, Producer, (void*)t);
        else
            rc = pthread_create(&threads[t], NULL, Consumer, (void*)t);
        if (rc)
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    for (t = 0; t < numberofthreads; t++)
    {
        pthread_join(threads[t], NULL);
    }

    pthread_cond_destroy(&fullmonitor);
    pthread_cond_destroy(&emptymonitor);
    pthread_mutex_destroy(&mutexmonitor);
    pthread_exit(NULL);

    return 0;
}
