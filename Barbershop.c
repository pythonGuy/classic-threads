/*
 * Barbershop Problem
 *
 * This program demostrates a solution to this classic problem.
 * It is briefly described in Allen Downey's,
 * "The Little Book of Semaphores", Version 2.1.5
 *
 */
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define THREAD_COUNT 26
#define WAIT_CHAIRS  5

pthread_mutex_t print_mutex;

pthread_mutex_t wait_chairs_mutex;
int             wait_chairs_open = WAIT_CHAIRS;
pthread_cond_t  wait_chairs_cv;

sem_t customer;
sem_t barber;

void getHairCut (void *t) {
    long my_id = *(long *)t;

    pthread_mutex_lock (&print_mutex);
    printf ("Customer: %c is getting a hair cut\n", my_id);
    pthread_mutex_unlock (&print_mutex);
}

void balk (void *t) {
    long my_id = *(long *)t;

    pthread_mutex_lock (&print_mutex);
    printf ("Customer: %c is balking, walking away!\n", my_id);
    pthread_mutex_unlock (&print_mutex);
    pthread_exit(NULL);
}

void cutHair (void *t) {
    long my_id = (long)t;
    int busyTime;

    pthread_mutex_lock (&print_mutex);
    printf ("Barber is doing a hair cut\n");
    pthread_mutex_unlock (&print_mutex);

    // Simulate "hair" cut time
    srand(time(0));
    busyTime = (rand() % 2);
    sleep(busyTime);
}

void *barberTask (void *t) {
    long my_id = (long)t;

    while (1) {
        sem_wait ( &customer ); // Wait for a customer

        pthread_mutex_lock ( &wait_chairs_mutex );
        // Barber is taking a customer so a chair for waiting opens
        wait_chairs_open++;
        pthread_mutex_unlock ( &wait_chairs_mutex );

        cutHair(t);

        sem_post ( &barber ); // Barber is ready again
    }
        
}

void *customerTask (void *id) {
    long my_id = *(long *)id;

    pthread_mutex_lock ( &wait_chairs_mutex );
    if (wait_chairs_open > 0) { // If a chair in waiting area is available
        wait_chairs_open--;
        pthread_mutex_unlock ( &wait_chairs_mutex );

        sem_post ( &customer ); // Let the barber know a customer is ready
        sem_wait ( &barber );   // Wait for the barber
        getHairCut(id);

    } else { // If no chairs in waiting area, walk away
        pthread_mutex_unlock ( &wait_chairs_mutex );
        balk (id);
    }
}

int main (int argc, char *argv[])
{
    int  i, rc;
    int  tn[THREAD_COUNT];
    long barber_id = 7;
    int  customerArrivalDelay;

    pthread_t threads[THREAD_COUNT];
    pthread_attr_t attr;

    srand(time(0));

    for (i=0; i<THREAD_COUNT; i++)
        tn[i] = (long)('A'+i);

    sem_init ( &customer, 1, 0 );
    sem_init ( &barber, 1, 0 );

    /* Initialize mutex */
    pthread_mutex_init( &wait_chairs_mutex, NULL );

    /* The barber opens the shop, start the barber's task */
    pthread_create(&threads[i], NULL, barberTask, (void *)&barber_id);

    /* Customers arrive at random intervals */
    for (i=0; i<THREAD_COUNT; i++) {
        customerArrivalDelay = (rand() % 3);
        sleep(customerArrivalDelay);
        pthread_create(&threads[i], NULL, customerTask, (void *)&tn[i]);
    }

    /* Wait for all threads to complete */
    for (i=0; i<THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }
    printf ("Main(): Waited on %d  threads. Done.\n", THREAD_COUNT);

    return 0;
}
