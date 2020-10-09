#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

//recursos
long int r1=0;
long int r2=0;

//semaforos
sem_t s1;
sem_t s2;

void *p1(void *args){
    while (1) {
        int return1 = sem_wait(&s1);
        int return2 = sem_wait(&s2);

        if (return1 >= 0) {
            printf("\nP1: em recurso r1: %ld",++r1);
            sem_post(&s1);
        } else if (return1 == -2) {
            int ra = (((rand() % 5) + 1));
            printf("\nDormindo por: %d s", ra);
            sleep(ra);
            printf("\nDeadlock detected. Request to use resource r1 was killed for P1.");
            continue;
        }
            
        if (return2 >= 0) {
            printf("\nP1: em recurso r2: %ld",++r2);  
            sem_post(&s2);
        } else if (return2 == -2) {
            int ra2=(((rand() % 5) + 1));
            printf("\nDormindo por: %d s", ra2);
            sleep(ra2);
            printf("\nDeadlock detected. Request to use resource s2 was killed for P1.");
            continue;
        }

    }
}
void *p2(void *args){
    while (1) {
        int return1 = sem_wait(&s1);
        int return2 = sem_wait(&s2);

        if (return2 >= 0) {
            printf("\nP2: em recurso r2: %ld",++r2);
            sem_post(&s2);
        } else if (return2 == -2) {
            printf("\nDeadlock detected. Request to use resource r2 was killed for P2.");
            int ra2=(((rand() % 5) + 1));
            printf("\nDormindo por: %d ms", ra2);
            sleep(ra2);
            continue;
        }

        if (return1 >= 0) {
            printf("\nP2: em recurso r1: %ld",++r1);
            sem_post(&s1);
        } else if (return1 == -2) {
            int ra=(((rand() % 5) + 1));
            printf("\nDormindo por: %d s", ra);
            sleep(ra);
            printf("\nDeadlock detected. Request to use resource r1 was killed for P2.");
            continue;
        }

            
    }
}

int main(int argc, char const *argv[])
{
    pthread_t t1,t2;
    sem_init(&s1,0,1);
    sem_init(&s2,0,1);
    pthread_create(&t1,NULL,p1,NULL);
    pthread_create(&t2,NULL,p2,NULL);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    return 0;
}