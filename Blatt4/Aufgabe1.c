#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5

pthread_mutex_t chopsticks[NUM_PHILOSOPHERS];

void* Philosopher(void* threadid)
{
    long philosopherId = (long) threadid;
    int leftChopstick = philosopherId;
    int rightChopstick = (philosopherId + 1) % NUM_PHILOSOPHERS;
    int numMeals = 0;

    while (numMeals < 100) {
        // Denken
        printf("Philosopher %ld is thinking.\n", philosopherId);
        usleep(rand() % 2000 + 1000);

        // Versuche den linken chopstick zu nehmen und zu locken
        if (pthread_mutex_trylock(&chopsticks[leftChopstick]) == 0) {
            // Erfolgreich linken chopstick aufgeoben
            printf("Philosopher %ld picked up left chopstick .\n", philosopherId);

            // Versuche den rechten chopstick zu nehmen und zu locken
            if (pthread_mutex_trylock(&chopsticks[rightChopstick]) == 0) {
                // erfolgreich rechten chopstick in der hand
                printf("Philosopher %ld picked up right chopstick .\n", philosopherId);

                // essen
                printf("Philosopher %ld is eating.\n", philosopherId);
                usleep(rand() % 2000 + 1000);

                // Release rechten ch   opstick
                pthread_mutex_unlock(&chopsticks[rightChopstick]);
                printf("Philosopher %ld released right chopstick.\n", philosopherId);
            }

            // Release linken chopstick
            pthread_mutex_unlock(&chopsticks[leftChopstick]);
            printf("Philosopher %ld released left chopstick.\n", philosopherId);

            numMeals++;
        }
    }

    pthread_exit(NULL);
}

int main()
{
    pthread_t philosophers[NUM_PHILOSOPHERS];
    int rc;

    //Mutex init
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&chopsticks[i], NULL);
    }

    // starte Philsophen Threads
    for (long t = 0; t < NUM_PHILOSOPHERS; t++) {
        printf("Creating philosopher %ld\n", t);
        rc = pthread_create(&philosophers[t], NULL, Philosopher, (void*) t);
        if (rc) {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    // Joine Threads, also warte bis Thread vom Philosophen zu ende geht
    for (long t = 0; t < NUM_PHILOSOPHERS; t++) {
        pthread_join(philosophers[t], NULL);
    }

    // Mutex releasen
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&chopsticks[i]);
    }

    pthread_exit(NULL);
    return 0;
}
