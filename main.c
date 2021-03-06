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
    int return1 = sem_wait(&s1);
    int return2 = sem_wait(&s2);
    sleep(2);
    if (return1 >= 0)
        sem_post(&s1);
    else if (return1 == -2)
        printf("Deadlock detected. Request to use resource s1 was killed for p1.\n");
    if (return2 >= 0)
        sem_post(&s2);
    else if (return2 == -2)
        printf("Deadlock detected. Request to use resource s2 was killed for p1.\n");
    //int p=5;
    //while (1)
    /* {
        if(sem_wait(&s1)==-2){
            int ra=(((rand() % 5) + 1) * 10000);
            printf("\ndormindo por: %d ms", ra);
            usleep(ra);
            continue;
        }         
        //printf("\nBloqueio do s1 P1 ender: %p",&s1);
        sem_wait(&s1);
        sem_wait(&s2);
        //printf("\nBloqueio do s2 P1 ender: %p",&s2);
        printf("\nP1: em recurso r1: %ld",++r1);
        printf("\nP1: em recurso r2: %ld",++r2);
        sem_post(&s1);
        printf("\nLiberação do s1 P1\n");
        sem_post(&s2);
        printf("\nLiberação do s2 P1\n");
        //p--;
    }  */
}
void *p2(void *args){
    //int q=5;
    int return2 = sem_wait(&s2);
    int return1 = sem_wait(&s1);
    sleep(2);
    if (return2 >= 0)
        sem_post(&s2);
    else if (return2 == -2)
        printf("Deadlock detected. Request to use resource s2 was killed for p2.\n");
    if (return1 >= 0)
        sem_post(&s1);
    else if (return1 == -2)
        printf("Deadlock detected. Request to use resource s1 was killed for p2.\n");
    /* while (1)
    {
        sem_wait(&s1);
        //printf("\nBloqueio do s1 P2 ender: %p\n",&s1);
        sem_wait(&s2);
        //printf("\nBloqueio do s2 P2 ender: %p\n",&s2);
        printf("\nP2: em recurso r1: %ld",++r1);
        printf("\nP2: em recurso r2: %ld",++r2);
        sem_post(&s1);
        printf("\nLiberação do s1 P2\n");
        sem_post(&s2);
        printf("\nLiberação do s2 P2\n");
        //q--;
    } */
}

int main(int argc, char const *argv[])
{
    //comentario
    pthread_t t1,t2;
    sem_init(&s1,0,1);
    sem_init(&s2,0,1);
    pthread_create(&t1,NULL,p1,NULL);
    pthread_create(&t2,NULL,p2,NULL);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    return 0;
}