/*
 * Dining philosophers problem
 *
 * This program demostrates a solution to this classic problem.
 * The solution is based on pseudo code in Allen Downey's,
 * "The Little Book of Semaphores", Version 2.1.5
 *
 */

#include <stdio.h>

#include <pthread.h>
#include <semaphore.h>

#define NUM_PHILOSOPHERS 5

#define left(i) i
#define right(i) (i+1)%5

sem_t footman;

void *philosopher (void*);

double delta, pi = 0.0;
int intervals = 100000;

static pthread_mutex_t forks[NUM_PHILOSOPHERS];

int main() {

    int i, id[NUM_PHILOSOPHERS];
    pthread_t tid[NUM_PHILOSOPHERS];

    // Initialize the  locks for the forks
    for (i=0; i<NUM_PHILOSOPHERS; i++)
        pthread_mutex_init(&(forks[i]), NULL);

    // Initialize the footman semaphore
    sem_init(&footman, 0, NUM_PHILOSOPHERS-1);

    for (i=0; i<NUM_PHILOSOPHERS; i++) {
        id[i] = i;
        pthread_create (&tid[i], NULL, philosopher,
                        (void *)&id[i]);
        }

    for (i=0; i<NUM_PHILOSOPHERS; i++)
        pthread_join(tid[i], NULL);
}

void *philosopher (void *id) {
    int philosopher_id = *(int *)id;

    while ( 1 ) {
        // Wait for the footman to let us eat
        sem_wait (&footman);

        pthread_mutex_lock ( &(forks[left(philosopher_id)]) );
        pthread_mutex_lock ( &(forks[right(philosopher_id)]) );

        printf ("Philosopher %d is eating\n", philosopher_id);
        printf ("Philosopher %d is done\n", philosopher_id);

        pthread_mutex_unlock ( &(forks[right(philosopher_id)]) );
        pthread_mutex_unlock ( &(forks[left(philosopher_id)]) );

        // Let the footman know we are finished
        sem_post (&footman);
    }

    return NULL;
}
