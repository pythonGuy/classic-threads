#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define NUM_THREADS  3
#define TCOUNT 4

#define PHILOSOPHERS 5

sem_t maitreD;
int sem_init(sem_t *maitreD, int pshared, unsigned int value);

int             rendezvous_count = 0;
pthread_mutex_t rendezvous_count_mutex;
pthread_cond_t  rendezvous_count_threshold_cv;
int             rendezvous_oncePerCycle = 0;

void *task (void *t) {
    int i;
    int busyTime;
    long my_id = (long)t;

    printf ("Starting thread-%ld\n", my_id);

    for (i=0; i < TCOUNT; i++) {

        // Simulate a random amount of work
        busyTime = rand()%10 + 1;
        printf ("Thread-%ld Busy, working for %i seconds...\n",
                my_id, busyTime);
        sleep(busyTime);

        // Our "work" is done, indicate we are at rendezvous point
        pthread_mutex_lock(&rendezvous_count_mutex);
        rendezvous_count++;

        printf ("Thread-%ld work complete, waiting on barrier\n", my_id);
        /* If the rendezvous_count has reached the limit, all threads
           have reached the rendezvous. Signal all threads.
           Note that this occurs while mutex is locked. 
        */
        while (rendezvous_count < NUM_THREADS) {
            pthread_cond_wait(&rendezvous_count_threshold_cv,
                              &rendezvous_count_mutex);
        }
        if (rendezvous_count == NUM_THREADS) {
            if (!rendezvous_oncePerCycle) {
                printf("All threads at barrier\n");
                pthread_cond_broadcast(&rendezvous_count_threshold_cv);
            }
            rendezvous_oncePerCycle++;
            if (rendezvous_oncePerCycle == NUM_THREADS) {
                // Last thread has now crossed the rendezvous/barrier
                // Recycle rendezvous for reuse
                rendezvous_oncePerCycle = 0;
                rendezvous_count        = 0;
            }
        }
        //printf("\nThread-%ld has crossed the barrier", my_id);
        pthread_mutex_unlock(&rendezvous_count_mutex);

        /* Do some work so threads can alternate on mutex lock */
        sleep(1);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int i, rc;

    long t1=1, t2=2, t3=3;
    pthread_t threads[3];
    pthread_attr_t attr;

    sem_init ( &maitreD, 0, 4 );  // Allow 4 max diners at table to be eating
    srand( time(NULL) );

    /* Initialize mutex and condition variable objects */
    pthread_mutex_init(&rendezvous_count_mutex, NULL);
    pthread_cond_init (&rendezvous_count_threshold_cv, NULL);

    /* For portability, explicitly create threads in a joinable state */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&threads[0], &attr, task, (void *)t1);
    pthread_create(&threads[1], &attr, task, (void *)t2);
    pthread_create(&threads[2], &attr, task, (void *)t3);

    /* Wait for all threads to complete */
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    /* Clean up and exit */
    printf("\n");
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&rendezvous_count_mutex);
    pthread_cond_destroy(&rendezvous_count_threshold_cv);
    pthread_exit (NULL);

}
