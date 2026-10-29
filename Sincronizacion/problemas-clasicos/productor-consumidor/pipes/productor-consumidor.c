#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


#define maxProducts 100
#define bufferSize 20

int pipeProdConsu[2];

void productor(int id){
    int lastMessage = -1;
    for(int i=0;i<maxProducts;i++){
        int  prod = rand()%100;
        printf("El productor %i: creo el producto %i\n", id, prod);
        
        //Escribo en el "buffer"
        write(pipeProdConsu[1],&prod,sizeof(int));     
    }
    write(pipeProdConsu[1],&lastMessage,sizeof(int));     
}

void consumidor(int id){
    int prod;
    do{
        read(pipeProdConsu[0], &prod, sizeof(prod));
        printf("El consumidor %i: consume el producto %i\n",id,prod);
    }while(prod != -1);
}
int main(){
    pipe(pipeProdConsu);

    pid_t productorProc = fork();
    if(productorProc==0){
        //Cierro el canal de lectura
        printf("Se creo el productor\n");
        close(pipeProdConsu[0]);
        productor(1);
        exit(0);
    }
    pid_t consumidorProd = fork();
    if(consumidorProd == 0){
        printf("Se creo el consumidor\n");
        //Cierro el canal de escritura
        close(pipeProdConsu[1]);
        consumidor(2);
        exit(0);
    }
    wait(NULL);
    wait(NULL);

    return 0;
}