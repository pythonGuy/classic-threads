/*
 * Dining Savages problem
 *
 * This program demostrates a solution to this less classical problem.
 * The solution is based on pseudo code in Allen Downey's,
 * "The Little Book of Semaphores", Version 2.1.5
 *
 */

#include <stdio.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>

#define NUM_SAVAGES 3

sem_t emptyPot;
sem_t fullPot;

void *savage (void*);
void *cook (void*);

static pthread_mutex_t servings_mutex;
static pthread_mutex_t print_mutex;

static int servings = 15;

int getServingsFromPot(void) {
    int retVal;

    if (servings <= 0) {
        sem_post (&emptyPot);
        retVal = 0;
    } else {
        servings--;
        retVal = 1;
    }

    pthread_mutex_unlock (&servings_mutex);

    return retVal;
}

void putServingsInPot (int num) {

    servings += num;
    sem_post (&fullPot);

}

void *cook (void *id) {
    int cook_id = *(int *)id;
    int meals = 2;
    int i;

    while ( meals ) {

        sem_wait (&emptyPot);

        putServingsInPot (15);
        meals--;

        pthread_mutex_lock (&print_mutex);
        printf ("\nCook filled pot\n\n");
        pthread_mutex_unlock (&print_mutex);

        for (i=0; i<NUM_SAVAGES; i++)
            sem_post (&fullPot);

    }

    return NULL;
}

void *savage (void *id) {
    int savage_id = *(int *)id;
    int myServing;
    int meals = 11;

    while ( meals ) {

        pthread_mutex_lock (&servings_mutex);

        myServing = getServingsFromPot();
        if (servings == 0) {
            sem_wait (&fullPot);
            myServing = getServingsFromPot();
        }

        pthread_mutex_unlock (&servings_mutex);

        meals--;

        pthread_mutex_lock (&print_mutex);
        printf ("Savage: %i is eating\n", savage_id);
        pthread_mutex_unlock (&print_mutex);

        sleep(2);

        pthread_mutex_lock (&print_mutex);
        printf ("Savage: %i is DONE eating\n", savage_id);
        pthread_mutex_unlock (&print_mutex);
    }

    return NULL;
}

int main() {

    int i, id[NUM_SAVAGES+1];
    pthread_t tid[NUM_SAVAGES+1];

    // Initialize the mutex locks
    pthread_mutex_init(&servings_mutex, NULL);
    pthread_mutex_init(&print_mutex, NULL);

    // Initialize the semaphores
    sem_init(&emptyPot, 0, 0);
    sem_init(&fullPot,  0, 0);

    for (i=0; i<NUM_SAVAGES; i++) {
        id[i] = i;
        pthread_create (&tid[i], NULL, savage, (void *)&id[i]);
        }
    pthread_create (&tid[i], NULL, cook, (void *)&id[i]);

    for (i=0; i<NUM_SAVAGES; i++)
        pthread_join(tid[i], NULL);
}
