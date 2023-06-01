#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h> 
#include <semaphore.h> 
#include <unistd.h> 

#define NUM_THREADS     5
#define NUM_WRITERS     2

sem_t writer;
sem_t reader;
int buffer; 

void* Writer(void *threadid) 
{
  int i, j;
  for (i = 0; i < 1000; i++) {
    sem_wait(&writer); // Zugriff auf den Puffer für Schreibvorgänge blockieren
    for (j = 0; j < NUM_THREADS - NUM_WRITERS; j++) 
      sem_wait(&reader); // Zugriff auf den Puffer für Leser blockieren
    buffer++; // Wert in den Puffer schreiben
    printf("Writer %ld writes %d into buffer\n", (long)threadid, buffer);
    for (j = 0; j < NUM_THREADS - NUM_WRITERS; j++)
      sem_post(&reader); // Zugriff auf den Puffer für Leser freigeben
    sem_post(&writer); // Zugriff auf den Puffer für Schreibvorgänge freigeben
    sleep(1);
  }
  pthread_exit(NULL);
} 


void* Reader(void *threadid)   
{
  int i;

  while (1) {
    sem_wait(&writer); // Zugriff auf den Puffer für Schreibvorgänge blockieren
    sem_wait(&reader); // Zugriff auf den Puffer für Leser blockieren
    sem_post(&writer); // Zugriff auf den Puffer für Schreibvorgänge freigeben
    printf("Reader %ld reads %d\n", (long)threadid, buffer); fflush(stdout);
    sleep(1);
    printf("Reader %ld leaves cs\n", (long)threadid); fflush(stdout);
    sem_post(&reader); // Zugriff auf den Puffer für Leser freigeben
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) 
{ 
  pthread_t threads[NUM_THREADS];
  int rc;
  long t;
 
  sem_init(&writer, 0, 1); // Schreibzugriff auf den Puffer initialisieren
  sem_init(&reader, 0, NUM_THREADS - NUM_WRITERS); // Lesezugriff auf den Puffer initialisieren
  buffer = 0;

  for (t = 0; t < NUM_THREADS; t++) {
    printf("In main: creating thread %ld\n", t);
    if (t < NUM_WRITERS)
      rc = pthread_create(&threads[t], NULL, Writer, (void *)t);
    else
      rc = pthread_create(&threads[t], NULL, Reader, (void *)t);
   if (rc) {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
   }
  }

  for (t = 0; t < NUM_THREADS; t++) {
     pthread_join(threads[t], NULL);
  }

  sem_destroy(&reader); 
  sem_destroy(&writer);
  pthread_exit(NULL);

  return 0; 
} 
