#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#define SIZE 5
int nums[SIZE] = {0,1,2,3,4};
void *mihilo(void *data);
int main()
{
int i;
pid_t pid;
pthread_t hilo[SIZE];

for (i = 0; i < SIZE; i++)
pthread_create(&hilo[i], NULL, mihilo, (void *) (intptr_t)i);

for (i = 0; i < SIZE; i++) {
pthread_join(hilo[i], NULL);
printf("Num[i]: %d \n",nums[i]);
}
exit(0);
}

void *mihilo(void *data){
int j = (intptr_t) data;
nums[j] = 2*(j);
printf("Num[j]: %d \n",nums[j]);
}