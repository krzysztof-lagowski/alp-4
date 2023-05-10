#define _DEFAULT_SOURCE //removes warning with usleep
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>


#include <unistd.h> //enthält sleep()

#define NUM_THREADS 2

// counter for bridge activity
long counter = 0;
long max_counter = 100000;
//signifies the busieness of the bridge 0 = empty; 1 = busy; 2 = there will be a crash
bool busy = false;

pthread_t threads[NUM_THREADS];


bool enter_bridge(long tid)
{
    if (busy == false) {
            printf("Busy: false Nr: %ld: Successful Enter as Thread %ld\n", counter, tid);
        } else {
            printf("Busy: true  Nr: %ld; Crash on Enter as Thread %ld\n", counter, tid);
        }
}

void *bridge_function(void *threadid) {
    long tid;
    int pause;
    tid = (long) threadid;
    while (1) {
        //exit after certain time
        if (counter > max_counter) { pthread_exit(NULL); }

        //random interval before entering the bridge; intervall 1-4
        pause = (500 + (int) (((double) rand() / (RAND_MAX - 1)) * 1500));
        


        usleep(pause);

        //entering bridge (collision check)
        enter_bridge(tid);
        counter += 1;

        //fährt los -> brücke ist blockiert
        busy = true;

        //benötigte Zeit für überquerung der Brücke
        usleep(600); //

        //Brücke überquert -> Brücke ist frei
        busy = false;
    }
}

int main() {
    long i;
    int tc; //error handle  

    //thread creation AUS VL
    for (i = 0; i < NUM_THREADS; i++) {
        printf("In main: creating thread %ld\n", i);
        tc = pthread_create(&threads[i], NULL, bridge_function, (void *) i);
        if (tc) {
            printf("ERROR; return code from pthread_create() is %d\n", tc);
            exit(-1);
        }
    }

    //end AUS VL
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_exit(NULL);
}