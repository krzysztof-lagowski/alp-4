#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

//#define numberofthreads 5
int numberofthreads; //wird in der main durch numberofprods + numberofcons berechnet
int numberofprods = 3;
int numberofcons = 1;
int numberofproducts = 100; //countervariable wie viele Produkte erstellt wwerden sollen (Anzahl der Durchlaufe)
#define NUM_PLACES 3

int last = 0;
int buffer[NUM_PLACES];

pthread_mutex_t monitorlock;

pthread_cond_t emptymonitor;
pthread_cond_t fullmonitor;
pthread_mutex_t mutexmonitor;

int counter;


void take(void *threadid){ // Entnehmen von Produkten
    pthread_mutex_lock(&mutexmonitor);
    while (counter == 0) 
    {
        pthread_cond_wait(&emptymonitor, &mutexmonitor);
    }
    //printf ("Consumer %d takes %d \n", (long) threadid, buffer[last - 1]); 
    fflush (stdout);


    /*Erhält die Reihenfolge durch leeren und aufrutschen, jeder platz im array wird um eins nach vorne geschoben wenne etwas entnommen wird*/
    //prüft ob im ersten Platz des Arrays ein "Produkt" liegt
    if (buffer[0] != 0) {

        //falls ja, überschreibe buffer[0] mit buffer[1] und rücke auf, buffer [2] wird 0 gesetzt da ein Item vorne entnommen wurde
        printf ("Consumer %d takes %d \n", (long) threadid, buffer[0]);
        buffer[0] = buffer[1];
        buffer[1] = buffer[2];
        buffer[2] = 0;

    //falls in buffer[0] kein Produkt, prüfe buffer[1] und rücke ggf das array auf 
    } else {
        if (buffer[1] != 0) {
            printf ("Consumer %d takes %d \n", (long) threadid, buffer[1]);
            buffer[0] = buffer[2];
            buffer[1] = 0;
            buffer[2] = 0;

        //falls buffer[0] und buffer [1] == 0 -> entnimm Produkt aus buffer 2
        } else {
            printf ("Consumer %d takes %d \n", (long) threadid, buffer[2]);
            buffer[2] = 0;
        }
    }
    counter --;
    printf("Buffer: [%d, %d, %d] \n", buffer[0],buffer[1],buffer[2]);
    fflush (stdout);
    last --;
    pthread_cond_signal(&fullmonitor);
    pthread_mutex_unlock(&mutexmonitor);
}

void store(void *threadid, int value){
    pthread_mutex_lock(&mutexmonitor);
    while (counter == NUM_PLACES){
        pthread_cond_wait(&fullmonitor, &mutexmonitor);
    }
    buffer[last] = value;
    counter ++;
    printf("Producer %ld puts %d into buffer at place %d \n", (long)threadid, buffer[last], last); 
    printf("Buffer: [%d, %d, %d] \n", buffer[0],buffer[1],buffer[2]);
    fflush (stdout);
    last++;
    pthread_cond_signal(&emptymonitor);
    pthread_mutex_unlock(&mutexmonitor);
}

void *Producer(void *threadid)
{
   int i;
   for (i = 1; i < numberofproducts; i++)
   {
      store(threadid, i);
   }
   pthread_exit(NULL);
}

void *Consumer(void *threadid)
{
    int i;
    int ;

   while (1)
   {
      take(threadid);
   }
   pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    int rc;
    long t;
    counter = 0;

    numberofthreads = numberofprods + numberofcons;
    pthread_t threads[numberofthreads];

    pthread_cond_init(&emptymonitor, NULL);
    pthread_cond_init(&fullmonitor, NULL);
    pthread_mutex_init(&mutexmonitor, NULL);

    for (t=0; t < NUM_PLACES; t++)
    buffer[t] = 0;
    last = 0; 

    for(t=0; t < numberofthreads; t++) 
    {
        printf ("In main: creating thread %ld\n", t);
        fflush (stdout);
        if (t == 0)
            rc = pthread_create (&threads[t], NULL, Producer, (void *)t);
        
        else
            rc = pthread_create (&threads[t], NULL, Consumer, (void *)t);

        if (rc) 
        {
            printf ("ERROR; return code from pthread_create () is %d\n", rc);
            fflush (stdout);
            exit (-1);
        }
     }

  for(t=0; t < numberofthreads; t++) {
     pthread_join (threads[t], NULL);
  }


    pthread_cond_destroy(&fullmonitor);
    pthread_cond_destroy(&emptymonitor);
    pthread_mutex_destroy(&mutexmonitor);
    pthread_exit(NULL);

    return 0;
}


