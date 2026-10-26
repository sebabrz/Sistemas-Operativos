#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <semaphore.h>
#include <pthread.h>

#define REPES 3

sem_t a,b,c,d,e;

void* printA(void *arg){
  for(int i = 0; i<REPES; i++){
    sem_wait(&a);
    printf("A");
    sem_post(&b);
  }
  return NULL;
}

void* printB(void *arg){
  for(int i = 0; i<REPES; i++){
    sem_wait(&b);
    printf("B");
    sem_post(&c);
  }
  return NULL;
}

void* printC(void *arg){
  for(int i = 0; i<REPES; i++){
    sem_wait(&c);
    printf("C");
    sem_post(&d);
  }
  return NULL;
}

void* printD(void *arg){
  for(int i = 0; i<REPES; i++){
    sem_wait(&d);
    printf("D");
    sem_post(&e);
  }
  return NULL;
}

void* printE(void *arg){
  for(int i = 0; i<REPES; i++){
    sem_wait(&e);
    printf("E");
    sem_post(&a);
  }
  return NULL;
}


int main(int argc, char *argv[]){
  pthread_t at,bt,ct,dt,et;
  
  sem_init(&a,0,1);
  sem_init(&b,0,0);
  sem_init(&c,0,0);
  sem_init(&d,0,0);
  sem_init(&e,0,0);


  pthread_create(&at,NULL,printA,NULL);
  pthread_create(&bt,NULL,printB,NULL);
  pthread_create(&ct,NULL,printC,NULL);
  pthread_create(&dt,NULL,printD,NULL);
  pthread_create(&et,NULL,printE,NULL);

  pthread_join(at,NULL);
  pthread_join(bt,NULL);
  pthread_join(ct,NULL);
  pthread_join(dt,NULL);
  pthread_join(et,NULL);

  sem_destroy(&a);
  sem_destroy(&b);
  sem_destroy(&c);
  sem_destroy(&d);
  sem_destroy(&e);

  printf("\n");
  return 1;
}
