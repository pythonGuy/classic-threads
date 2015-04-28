/*
 * Cigarette smokers problem
 *
 * This program demostrates a solution to this classic problem.
 * The solution is based on pseudo code in Allen Downey's,
 * "The Little Book of Semaphores", Version 2.1.5
 *
 */

#include <stdio.h>

#include <pthread.h>
#include <semaphore.h>

#define False 0
#define True  1

sem_t agentASem;
sem_t agentBSem;
sem_t agentCSem;

sem_t tobacco;
sem_t paper;
sem_t match;

pthread_mutex_t pusherMtx = PTHREAD_MUTEX_INITIALIZER;

int numTobacco, numPaper, numMatch;

void *AgentA (void *);
void *AgentB (void *);
void *AgentC (void *);

void *PusherA (void *);
void *PusherB (void *);
void *PusherC (void *);

void *SmokerA (void *);
void *SmokerB (void *);
void *SmokerC (void *);

void *AgentA (void *arg) {
    while ( 1 ) {
        printf ("AgentA is waiting\n");
        sem_wait ( &agentASem );
        printf ("AgentA is running\n");
        sem_post ( &tobacco );
        sem_post ( &paper );
    }
}

void *AgentB (void *arg) {
    while ( 1 ) {
        printf ("AgentB is waiting\n");
        sem_wait ( &agentBSem );
        printf ("AgentB is running\n");
        sem_post ( &paper );
        sem_post ( &match );
    }
}

void *AgentC (void *arg) {
    while ( 1 ) {
        printf ("AgentC is waiting\n");
        sem_wait ( &agentCSem );
        printf ("AgentC is running\n");
        sem_post ( &tobacco );
        sem_post ( &match );
    }
}

void *PusherA (void *arg) {
    while ( 1 ) {
        sem_wait ( &tobacco );

        pthread_mutex_lock ( &pusherMtx );
        if (numPaper > 0) {
            numPaper -= 1;
            sem_post ( &match );
        } else if (numMatch > 0) {
            numMatch -= 1;
            sem_post ( &paper );
        } else {
            numTobacco +1;
        }
        pthread_mutex_unlock ( &pusherMtx );
    }
}

void *PusherB (void *arg) {
    while ( 1 ) {
        sem_wait ( &paper );

        pthread_mutex_lock ( &pusherMtx );
        if (numTobacco > 0) {
            numTobacco -= 1;
            sem_post ( &match );
        } else if (numMatch > 0) {
            numMatch -= 1;
            sem_post ( &tobacco );
        } else {
            numPaper += 1;
        }
        pthread_mutex_unlock ( &pusherMtx );
    }
}

void *PusherC (void *arg) {
    while ( 1 ) {
        sem_wait ( &match );

        pthread_mutex_lock ( &pusherMtx );
        if (numTobacco > 0) {
            numTobacco -= 1;
            sem_post ( &paper );
        } else if (numPaper > 0) {
            numPaper -= 1;
            sem_post ( &tobacco );
        } else {
            numMatch += 1;
        }
        pthread_mutex_unlock ( &pusherMtx );
    }
}

void *SmokerA (void *arg) {
    // SmokerA always has paper, PusherA waits for
    // tobacco and a match to become available, then
    // signals/wakes SmokerA with a match.

    while ( 1 ) {
        sem_wait ( &match );
        printf ("Smoker A is making a cigarette, then waking all Agents\n");
        sem_post ( &agentASem );
        sem_post ( &agentBSem );
        sem_post ( &agentCSem );
        printf ("Smoker A is smoking\n");
    }
}

void *SmokerB (void *arg) {
    // SmokerB always has tobacco, PusherB waits for
    // a match and paper to become available, then
    // signals/wakes SmokerB with paper.

    while ( 1 ) {
        sem_wait ( &paper );
        printf ("Smoker B is making a cigarette, then waking all Agents\n");
        sem_post ( &agentBSem );
        sem_post ( &agentCSem );
        sem_post ( &agentASem );
        printf ("Smoker B is smoking\n");
    }
}

void *SmokerC (void *arg) {
    // SmokerC always has a match, PusherC waits for
    // a paper and tobacco to become available, then
    // signals/wakes SmokerC with tobacco.

    while ( 1 ) {
        sem_wait ( &tobacco );
        printf ("Smoker C is making a cigarette, then waking all Agents\n");
        sem_post ( &agentCSem );
        sem_post ( &agentASem );
        sem_post ( &agentBSem );
        printf ("Smoker C is smoking\n");
    }
}

int main() {

    pthread_t a1, a2, a3;
    pthread_t p1, p2, p3;
    pthread_t s1, s2, s3;

    int AgentA_ID = 'A';
    int AgentB_ID = 'B';
    int AgentC_ID = 'C';

    numTobacco = numPaper = numMatch = 0;

    sem_init (&agentASem, 1, 1);
    sem_init (&agentBSem, 1, 1);
    sem_init (&agentCSem, 1, 1);

    sem_init (&tobacco, 1, 0);
    sem_init (&paper, 1, 0);
    sem_init (&match, 1, 0);

    pthread_create ( &a1, NULL, AgentA, NULL );
    pthread_create ( &a2, NULL, AgentB, NULL );
    pthread_create ( &a3, NULL, AgentC, NULL );

    pthread_create ( &p1, NULL, PusherA, NULL );
    pthread_create ( &p2, NULL, PusherB, NULL );
    pthread_create ( &p3, NULL, PusherC, NULL );

    pthread_create ( &s1, NULL, SmokerA, NULL );
    pthread_create ( &s2, NULL, SmokerB, NULL );
    pthread_create ( &s3, NULL, SmokerC, NULL );

    sem_post ( &agentASem );  // Let Agent A start
    sem_post ( &agentBSem );  // Let Agent B start
    sem_post ( &agentCSem );  // Let Agent C start

    pthread_join (s1, NULL); // This will wait forever because the smokers run forever
}
