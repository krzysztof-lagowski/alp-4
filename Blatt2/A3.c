#define _DEFAULT_SOURCE //removes warning with usleep
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

#include <unistd.h> //enthält sleep()

#define NUM_THREADS 4
int level[NUM_THREADS];
int last[NUM_THREADS];

char _lock[2];

FILE *logfile; // File handle for log file

// verschiedene Counter zu Brückenaktivität
long crashcount = 0;
long counter = 0;
long max_counter = 1000;


// Brücke besetzt -> true ; Brücke frei -> false
bool busy = false;

// Array aus Bool locks, für jeden Thread ein eigenes Lock im Array
bool _locks[NUM_THREADS] = {false};

//initiert threads
pthread_t threads[NUM_THREADS];

//locking aus VL ünernommen
int lock (long tid)
{
  int i, j, loop;

  for (i = 1; i < NUM_THREADS; i++) {
    level[tid] = i;
    last[i] = tid;
    loop = 1;
    while ((loop)&&(last[i] == tid)) {
      j = 0;
      loop = 0; 
      while ((j < NUM_THREADS)&&((level[j] < i)||(j == tid)))
        j++;
      if (j < NUM_THREADS)
        loop = 1;
    }
  } 
  return 0;
}

//unlocking aus VL übernommen
int unlock(long tid)
{
    _lock[tid] = false;
    return 0;
}

//Prüft ob Brücke belegt ist oder ob sie befahren werden kann
bool enter_bridge(long tid)
{
    
    bool success = false;

    lock(tid);
    
    counter += 1;

    if (busy == false) {
        printf("Brücke: frei; Auto Nr: %ld: Erfolgreich überquert als Thread %ld\n", counter, tid);
        success = true;
    } else {
        printf("Brücke: besetzt; Auto Nr: %ld; Kollison als Thread: %ld; Crash Nummer: %ld\n", counter, tid, crashcount);
        crashcount += 1;
        success = false;
    }
    
    unlock(tid);

    return success;
}

//Funktion zu Simulation der Brückenüberquerung
void *bridge_function(void *threadid)
{
    
    long tid;
    int pause;
    tid = (long)threadid;
    while (1) {

        //wenn counter erreicht -> beende
        if (counter > max_counter) {
            printf("Gesamtzahl Crashes: %ld,\n", crashcount);
            pthread_exit(NULL);
        }

        // random zeitintervall bevor versucht wird Brücke zu befahren
        pause = (500 + (int)(((double)rand() / (RAND_MAX - 1)) * 1500));

        usleep(pause);

        //befahren der Brücke mit Kollisionscheck
        bool canEnter = enter_bridge(tid);
        printf("Counter: %ld\n", counter);
        if (canEnter) {


            // fährt los -> brücke ist blockiert
            busy = true;

            // benötigte Zeit für überquerung der Brücke
            usleep(600); //


            // Brücke überquert -> Brücke ist frei
            busy = false;
        }
    }
}

int main()
{
    long i;
    int tc; //error handle
    /*
    // Erstelle log File
    logfile = fopen("log.txt", "w");
    if (logfile == NULL) {
        printf("Error opening log file.\n");
        exit(1);
    }
    // leite Output in log file um
    if (dup2(fileno(logfile), fileno(stdout)) == -1) {
        printf("Error redirecting stdout to log file.\n");
        exit(1);
    }
    */

    // Thread Erstellung
    for (i = 0; i < NUM_THREADS; i++) {
        printf("In main: creating thread %ld\n", i);
        tc = pthread_create(&threads[i], NULL, bridge_function, (void *)i);
        if (tc) {
            printf("ERROR; return code from pthread_create() is %d\n", tc);
            exit(-1);
        }
    }

    // Endbedingung
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    //printf("Gesamtzahl Crashes: %ld,\n", crashcount);
    pthread_exit(NULL);
}