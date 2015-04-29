/*
 * Producer Consumer problem
 *
 * This program demostrates a solution to this classic problem.
 * The solution is based on pseudo code in Allen Downey's,
 * "The Little Book of Semaphores", Version 2.1.5
 *
 */

#include <stdio.h>

#include <pthread.h>
#include <semaphore.h>

void *producer (void*);
void *consumer (void*);

static pthread_mutex_t buf_mutex;

sem_t item_ready;
sem_t consumer_ready;

static int buffer;

int main() {

    int producer_id = 111; // Unique ID to identify the thread
    int consumer_id = 222; // Unique ID to identify the thread
    pthread_t producer_t;
    pthread_t consumer_t;

    // Initialize the  lock for the buffer
    pthread_mutex_init(&buf_mutex, NULL);

    // Indicate nothing in the buffer yet, and consumer not ready
    sem_init (&item_ready, 1, 0);
    sem_init (&consumer_ready, 1, 0);

    // Start the consumer and producer
    pthread_create (&consumer_t, NULL, consumer,
                    (void *)&consumer_id);

    pthread_create (&producer_t, NULL, producer,
                    (void *)&producer_id);

    pthread_join(producer_t, NULL);
}

void *producer (void *id) {
    int producer_id = *(int *)id;
    int i = 10000; // Run for 10,000 iterations
    int event_wait_time;

    printf ("Producer started, ID: %d\n", producer_id);
    srand(time(NULL));

    while ( i > 0 ) {
        i -= 1;

        // Simulate waiting for an event
        event_wait_time = ( rand() % 3) + 1;
        usleep (event_wait_time*1000);

        // Wait for the consumer to be ready
        sem_wait ( &consumer_ready );

        // Get exclusive use of the buffer
        pthread_mutex_lock ( &buf_mutex );
        buffer = i;
        pthread_mutex_unlock ( &buf_mutex );

        // Signal item in the buffer
        sem_post ( &item_ready );
    }
}

void *consumer (void *id) {
    int consumer_id = *(int *)id;

    printf ("Consumer started, ID: %d\n", consumer_id);

    // Let producer know we are ready
    sem_post ( &consumer_ready );

    while ( 1 ) {

        // Wait for the consumer to start/be ready
        sem_wait ( &item_ready );

        // Get exclusive use of the buffer
        pthread_mutex_lock ( &buf_mutex );
        printf ("Buffer value: %i\n", buffer);
        pthread_mutex_unlock ( &buf_mutex );

        // Let producer know we are ready for another value
        sem_post ( &consumer_ready );
    }
}
