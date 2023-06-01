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
sem_t mutex;
int p;
int buffer[NUM_PLACES];

void *Producer(void *threadid)
{
   int i;
   p++;
   for (i = 0; i < 100; i++)
   {
      sem_wait(&mutex);
      for (int f = 0; f < NUM_PLACES; f++){
         if (buffer[0] != 0 && buffer[1] != 0 && buffer[2] != 0)
         {
            buffer[0] = i;
            buffer[1] = 0;
            buffer[2] = 0;
            break;
         }
         if (buffer[f] == 0){
            buffer[f] = i;
            break;
         }
      }

      printf("Producer %d \n", (long)threadid);
      printf("Buffer: [%d, %d, %d] \n", buffer[0], buffer[1], buffer[2]);
      sem_post(&mutex);
      usleep(10);
   }
   p--;
   pthread_exit(NULL);
}

void *Consumer(void *threadid)
{
   int i;
   while(p>0)
   {
      sem_wait(&mutex);
      for (int f = 0; f < NUM_PLACES; f++)
      {
         if (buffer[f] != 0)
         {
            buffer[f] = 0;
            break;
         }
      }
      printf("Consumer %d \n", (long)threadid);
      printf("Buffer: [%d, %d, %d] \n", buffer[0], buffer[1], buffer[2]);
      sem_post(&mutex);
      usleep(10);
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

   sem_init(&mutex, 0, 1); // critical section to deal with buffer
   for (t = 0; t < NUM_PLACES; t++)
      buffer[t] = 0;

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
   pthread_exit(NULL);
   return 0;
}
