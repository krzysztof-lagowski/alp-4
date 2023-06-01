// simple producer consumer example with semaphores

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

//#define numberofthreads 5
int numberofthreads;
int numberofprods;
int numberofcons;
#define NUM_PLACES 3
sem_t empty;
sem_t full;
sem_t mutex;
int last;
int buffer[NUM_PLACES];

void *Producer(void *threadid)
{
   int i;
   for (i = 0; i < 100; i++)
   {
      sem_wait(&full);
      sem_wait(&mutex);
      buffer[last] = i;
      printf("Producer %d puts %d into buffer at place %d \n", (long)threadid, buffer[last], last);
      last++;
      sem_post(&mutex);
      sem_post(&empty);
   }
   pthread_exit(NULL);
}

void *Consumer(void *threadid)
{
   int i;

   while (1)
   {
      sem_wait(&empty);
      sem_wait(&mutex);
      printf("Consumer %d takes %d \n", (long)threadid, buffer[last - 1]);
      buffer[last - 1] = NULL; //der Buffer wird an der jeweiligen Stelle geleert
      printf("Buffer: [%d, %d, %d] \n", buffer[0],buffer[1],buffer[2]);
      fflush(stdout);
      last--;
      sem_post(&mutex);
      sem_post(&full);
   }
   pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
   
   int rc;
   long t;

   numberofprods = 0;
   int p = printf("Please enter number of Producers:\n");
   int s = fscanf(stdin, "%d", &numberofprods);
   if (numberofprods >= 100)
   {
      printf("Please Enter Number of Producers<100: \n");
      return (1);
   }


   numberofcons = 0;
   int q = printf("Please enter number of Consumers:\n");
   int k = fscanf(stdin, "%d", &numberofcons);
   if (numberofcons >= 100)
   {
      printf("Please Enter Number of Consumers<100: \n");
      return (1);
   }
   numberofthreads = numberofprods + numberofcons;
   pthread_t threads[numberofthreads];

   sem_init(&empty, 0, 0); // amount data in buffer
   sem_init(&full, 0, 3);  // amount of free places in buffer
   sem_init(&mutex, 0, 1); // critical section to deal with buffer
   for (t = 0; t < NUM_PLACES; t++)
      buffer[t] = 0;
   last = 0;

   for (t = 0; t < numberofthreads; t++)
   {
      printf("In main: creating thread %ld\n", t);
      if (t < numberofprods)
         rc = pthread_create(&threads[t], NULL, Producer, (void *)t);
      else
         rc = pthread_create(&threads[t], NULL, Consumer, (void *)t);

      if (rc)
      {
         printf("ERROR; return code from pthread_create () is %d\n", rc);
         exit(-1);
      }
   }
   for (t = 0; t < numberofthreads; t++)
   {
      pthread_join(threads[t], NULL);
   }


   sem_destroy(&mutex);
   sem_destroy(&full);
   sem_destroy(&empty);
   pthread_exit(NULL);
   return 0;
}
