#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#define SIZE 5
int nums[SIZE] = {0,1,2,3,4};
int main(){
int i;
pid_t pid;
pid = fork();
if (pid == 0) {
for (i = 0; i < SIZE; i++) {
nums[i] = 2*i;
printf("Num[i]: %d \n",nums[i]);}
} else if (pid > 0) {
for (i = 0; i < SIZE; i++)
printf("Padre: %d ",nums[i]);
}
exit(0);
}