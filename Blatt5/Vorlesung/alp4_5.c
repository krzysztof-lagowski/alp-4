#define _DEFAULT_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>

#define PRO_NUM 8   //producer
#define CON_NUM 2   //consumer
#define BUFFER_SPACE 3  //storage
#define SECONDS_TO_EXIT 4
#define THREAD_NUM (PRO_NUM + CON_NUM)
#define TIME_VAR 500

sem_t empty;    //amount of product in buffer
sem_t full;     //free space in buffer
sem_t mutex;    //for critical section
int last;       //tracking buffer index
int buffer[BUFFER_SPACE];
pthread_t threads[THREAD_NUM+1];    //+1 for exit thread
int skip_counter = 0;   // counting wasted product for version 2
int g = 0;

// variables for version 3
#define MEM_MAX_POT 2   //limits the amount of memory that can be reallocated
int *buff_ptr;          // buffer for version 3
int current_size = BUFFER_SPACE;    // tracking current size of buffer
int current_pot = 0;    // tracking how large the buffer is: BUFFER_SPACE * 2^current_pot = current_size
int done = 0;           //how many processes finish, usually all or none of the Producers

//universal perror to exit all threads
void error(long tid, char message[]){
    perror(message);
    for(int i = 0; i<THREAD_NUM; i++){
        if(i ==tid){continue;}
        pthread_cancel(threads[i]);
    }
    pthread_exit(NULL);
}

//thread to exit program based on SECONDS_TO_EXIT
void * exit_counter(void *threadid){
    (void) threadid;
    sleep(SECONDS_TO_EXIT);

    //producers exit by themselves
    for(int t = 0; t < THREAD_NUM; t++){
        if(t > PRO_NUM-1 || done == 0){
            if(t < PRO_NUM){printf("unknonw error\n");}
            pthread_cancel(threads[t]);
        }
    }
    pthread_exit(NULL);
}

void* Producer (void *threadid){
    long tid = (long) threadid;
    for(long i = 1000*tid; i <= 1000*tid+1000; i++){
	    if(sem_wait(&full) || sem_wait(&mutex)){ //check for free space, waiting, then reduce by 1 free space and entering critical section
            error(tid, "Houston, we have a problem with Produceer sem_wait");
        }

        buffer[last] = i;
        printf("Producer %ld puts %d into buffer at place %d \n", tid, buffer[last], last);
        last++;

	    if(sem_post(&mutex) || sem_post(&empty) ){ // leaving critical section and adding one more resource
            error(tid, "Houston, we have a problem with Producer sem_post");
        }
        usleep(TIME_VAR*4);
    }
    printf("Skips: %d; Producer %ld: DONE\n", skip_counter, tid);
    done ++;
    pthread_exit(NULL);
}


void* Producer2 (void *threadid){
    long tid = (long) threadid;
    int err;
    for(long i = 1000*tid; i <= 1000*tid+1000; i++){
        err = sem_trywait(&full);    //check for free space, waiting, then reduce by 1 free space
	    if(err == 1 && errno != EBUSY){
            error(tid, "error: buffer is full, no need to put something on it");
        }else if(err == 0){
            if(sem_wait(&mutex)){   //entering critical
                error(tid, "Houston, we have a problem with Producer sem_wait");
            }

            buffer[last] = i;
            printf("Skips: %d; Producer %ld puts %d into buffer at place %d \n", skip_counter, tid, buffer[last], last);
            last++;

            if(sem_post(&mutex) || sem_post(&empty) ){ //leaving critical and adding resource
                error(tid, "Houston, we have a problem with Producer sem_post");
            }
        }else{
            skip_counter++;
        }
        usleep(TIME_VAR*4);
    }
    printf("Skips: %d; Producer %ld: DONE\n", skip_counter, tid);
    done ++;
    pthread_exit(NULL);
}

void* Producer3 (void *threadid){
    long tid = (long) threadid;
    for(long i = 1000*tid; i <= 1000*tid+1000; i++){
        if(sem_wait(&mutex)){ //check for free space, waiting, then reduce by 1 free space and entering critical section
            error(tid, "Houston, we have a problem with Produceer sem_wait");
        }
	    if(last == current_size - 1 && current_pot != MEM_MAX_POT){
            //double buffer size if it's not at max allowed by MEM_MAX_POT
	        current_pot++;
	        current_size = current_size * 2;
	        buff_ptr = realloc(buff_ptr,sizeof(&buff_ptr) * 2);
            if(buff_ptr == NULL){
                error(tid, "error on memory reallocation");
            }
	    }else if(last == current_size -1 && current_pot == MEM_MAX_POT){
            //incase we need to resize past the limit the product will be thrown out
	        skip_counter++;
            if(sem_post(&mutex)){ // leaving critical section
                error(tid, "Houston, we have a problem with Producer sem_post");
            }
	        continue;
        }
        buff_ptr[last] = i;
        printf("Skips: %d; Producer %ld puts %ld into buffer at place %d; Done: %d\n",skip_counter,  tid, i, last, done);
        last++;
        
        if(sem_post(&mutex)){ // leaving critical section
            error(tid, "Houston, we have a problem with Producer sem_post");
        }
        usleep(TIME_VAR*4);
    }
    printf("Skips: %d; Producer %ld: DONE\n", skip_counter, tid);
    fflush(stdout);
    done ++;
    pthread_exit(NULL);
}

void* Consumer (void *threadid){
    long tid = (long) threadid;
    while(1){
	    if(sem_wait(&empty) || sem_wait(&mutex)){ //checking for resource and entering critical section
            error(tid, "Houston, we have a problem with Consumer sem_wait");
        }

        printf("Consumer %ld takes %d \n", tid, buffer[last-1]);
        fflush(stdout);
        last--;

	    if(sem_post(&mutex) || sem_post(&full)){ // leaving critical and removing one resource
            error(tid, "Houston, we have a problem with Consumer sem_post");
        }
        usleep(TIME_VAR);
    }
    pthread_exit(NULL);
}

void* Consumer2 (void *threadid){
    long tid = (long) threadid;
    while(1){
        if(sem_wait(&empty) || sem_wait(&mutex)){ //checking for resource and entering critical section
            error(tid, "Houston, we have a problem with Consumer sem_wait");
        }
        printf("Skips: %d; Consumer %ld takes %d \n",skip_counter, tid, buffer[last-1]);
        fflush(stdout);
        last--;

        if(sem_post(&mutex) || sem_post(&full)){ // leaving critical and removing one resource
            error(tid, "Houston, we have a problem with Consumer sem_post");
        }
        usleep(TIME_VAR);
    }
    pthread_exit(NULL);
}

void* Consumer3 (void *threadid){
    long tid = (long) threadid;
    while(1){
        usleep(TIME_VAR);
        if(last < 0){error(tid, "Error on last index out of range");}
        if(sem_wait(&mutex)){ //checking for resource and entering critical section
            error(tid, "Houston, we have a problem with Consumer sem_wait");
        }
        //checking available resources
        if(last == 0){
            if(sem_post(&mutex)){ // leaving critical and removing one resource
                error(tid, "Houston, we have a problem with Consumer sem_post");
            }
            continue;
        }
        // taking resource
        printf("Skips: %d; Consumer %ld takes %d \n",skip_counter, tid, buff_ptr[last-1]);
        fflush(stdout);
        buff_ptr[last-1] = 0;
        last--;

        //resize the buffer in case of too much overhead // auskommentiert weil Fehler
        if(last < current_size/4 && current_pot > 3){  
            current_pot--;
	        current_size = current_size/2;
	        buff_ptr = realloc(buff_ptr, sizeof(&buff_ptr)/2);
        }

        if(sem_post(&mutex)){ // leaving critical and removing one resource
            error(tid, "Houston, we have a problem with Consumer sem_post");
        }
        
        
    }
    pthread_exit(NULL);
}

int main (int argc, char *argv[]){
    int err;
    long t;
    //feature toggle
    if(argc == 2){
        g = atoi(argv[1]);
        switch(g){
            case 1: printf("Starting Version 1 for Task 1\n"); break;
            case 2: printf("Starting Version 2 for Task 2\n"); break;
	        case 3: printf("Starting Version 3 for Task 2\n"); break;
            default: printf("unexpected argument, please enter either \"1\" or \"2\" or \"3\"\n"); return 1;
        }
    }else{
        printf("unexpected amount of arguments, please enter either \"1\" or \"2\" or \"3\"\n");
        return 1;
    }

    //thread for exiting program
    err= pthread_create(&threads[THREAD_NUM+1], NULL, exit_counter, NULL);
    if(err){
        printf("ERROR on exit thread create\n");
        exit(-1);
    }

    //init sem
    if(g == 1 || g == 2){
    	if(sem_init(&empty, 0, 0) || sem_init(&full, 0, BUFFER_SPACE)){
               perror("Error on sem_init");
        }
	    for(t=0; t<BUFFER_SPACE; t++){
	    buffer[t] = 0;
	}
    }else if(g == 3){
	    buff_ptr = (int *)malloc(BUFFER_SPACE * sizeof(long));
        if(buff_ptr == NULL){
            printf("error on init memory\n");
            exit(-1);
        }
    }
    

    if(sem_init(&mutex, 0, 1)){
	    perror("Error on sem_init of lock");
    }
    last = 0;

    //feature toggle
    if(g == 1){
        for(t = 0; t< THREAD_NUM; t++){
            if(t < PRO_NUM){
                err = pthread_create(&threads[t], NULL, Producer, (void *)t);
            }else{
                err = pthread_create(&threads[t], NULL, Consumer, (void *)t);
            }
            if(err){
                exit(-1);
            }
        }
    }else if(g == 2){
	    for(t = 0; t< THREAD_NUM; t++){
            if(t < PRO_NUM){
                err = pthread_create(&threads[t], NULL, Producer2, (void *)t);
            }else{
                err = pthread_create(&threads[t], NULL, Consumer2, (void *)t);
            }
            if(err){
                exit(-1);
            }
        }
    }else if(g == 3){
        for(t = 0; t<THREAD_NUM; t++){
            if(t<PRO_NUM){
                err = pthread_create(&threads[t], NULL, Producer3, (void *)t);
            }else{
                err = pthread_create(&threads[t], NULL, Consumer3, (void *)t);
            }
            if(err){
            exit(-1);
            }
        }
    }else{
        printf("Input out of range\n");
        return(1);
    }
    //join
    for(t = 0; t < THREAD_NUM+1; t++){
        pthread_join(threads[t], NULL);
    }
    if(g==3){
        free(buff_ptr);
    }
    pthread_exit(NULL);
}
