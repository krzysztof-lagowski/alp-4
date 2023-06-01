// simple producer consumer example with semaphores 

#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h> 
#include <semaphore.h> 
#include <unistd.h> 

#define NUM_THREADS     5
#define NUM_PLACES	3
sem_t empty;
sem_t full;
sem_t mutex;
int last;
int buffer[NUM_PLACES]; 

void* Producer (void *threadid) 
{
  int i;
  for (i= 0; i < 1000; i++) {
    sem_wait(&full);
    sem_wait(&mutex);
    buffer[last] = i; 
    printf("Producer %d puts %d into buffer at place %d \n", (long) threadid, buffer[last], last);
    last++;
    sem_post(&mutex); 
    sem_post(&empty); 
  }
  pthread_exit (NULL);
} 


void* Consumer (void *threadid)   
{
  int i;

  while (1) {
    sem_wait(&empty);
    sem_wait(&mutex);
    printf ("Consumer %d takes %d \n", (long) threadid, buffer[last - 1]); fflush (stdout);
    last--;
    printf("Buffer: [%d, %d, %d] \n", buffer[0],buffer[1],buffer[2]);
    sem_post(&mutex);
    sem_post(&full);
  }
  pthread_exit (NULL);
}

int main (int argc, char *argv[]) 
{ 
  pthread_t threads[NUM_THREADS];
  int rc;
  long t;
 
  sem_init(&empty, 0, 0); // amount data in buffer
  sem_init(&full, 0, 1000); // amount of free places in buffer
  sem_init(&mutex, 0, 1); // critical section to deal with buffer
  for (t=0; t < NUM_PLACES; t++)
    buffer[t] = 0;
  last = 0; 

  for(t=0; t < NUM_THREADS; t++) {
     printf ("In main: creating thread %ld\n", t);
    if (t == 0)
      rc = pthread_create (&threads[t], NULL, Producer, (void *)t);
    else
      rc = pthread_create (&threads[t], NULL, Consumer, (void *)t);

     if (rc) {
        printf ("ERROR; return code from pthread_create () is %d\n", rc);
        exit (-1);
     }
  }

  for(t=0; t < NUM_THREADS; t++) {
     pthread_join (threads[t], NULL);
  }

  sem_destroy(&mutex);
  sem_destroy(&full); 
  sem_destroy(&empty);
  pthread_exit(NULL);
  return 0; 
} 

