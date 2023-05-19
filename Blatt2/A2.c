#define _DEFAULT_SOURCE //removes warning with usleep
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

#include <unistd.h> //enthält sleep()

#define NUM_THREADS 2
int level[NUM_THREADS];
int last[NUM_THREADS];
// int favoured;

char _lock[2];

FILE *logfile; // File handle for log file

// counter for bridge activity
long counter = 0;
long max_counter = 1000;
// signifies the busyness of the bridge: 0 = empty; 1 = busy; 2 = there will be a crash
bool busy = false;

bool _locks[NUM_THREADS] = {false}; // Array of locks, one for each thread

pthread_t threads[NUM_THREADS];


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
int unlock(long tid)
{
    _lock[tid] = false;
    return 0;
}

bool enter_bridge(long tid)
{
    
    bool success = false;

    lock(tid);
    
    counter += 1;

    if (busy == false) {
        printf("Brücke: frei; Auto Nr: %ld: Erfolgreich überquert als Thread %ld\n", counter, tid);
        success = true;
    } else {
        printf("Brücke: besetzt; Auto Nr: %ld; Kollison als Thread %ld\n", counter, tid);
        success = false;
    }
    
    unlock(tid);

    return success;
}

void *bridge_function(void *threadid)
{
    
    long tid;
    int pause;
    tid = (long)threadid;
    while (1) {
        // exit after a certain time
        if (counter > max_counter) {
            pthread_exit(NULL);
        }

        // random interval before entering the bridge; interval 1-4
        pause = (500 + (int)(((double)rand() / (RAND_MAX - 1)) * 1500));

        usleep(pause);

        // entering bridge (collision check)
        bool canEnter = enter_bridge(tid);
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


    // thread creation
    for (i = 0; i < NUM_THREADS; i++) {
        printf("In main: creating thread %ld\n", i);
        tc = pthread_create(&threads[i], NULL, bridge_function, (void *)i);
        if (tc) {
            printf("ERROR; return code from pthread_create() is %d\n", tc);
            exit(-1);
        }
    }

    // end
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_exit(NULL);
}