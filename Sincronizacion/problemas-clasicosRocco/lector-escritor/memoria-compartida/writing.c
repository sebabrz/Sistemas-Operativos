#include "estructura.h"

int main(){
	key_t clave = generateKey();
	int id = shmget(clave, 1024, 0);
	if(id < 0){
		printf("error fallo el shmget\n");
		exit(1);
	}
	
	struct aCompartir* memCompartida = (struct aCompartir*) shmat(id,0,0);
	if(memCompartida <= (struct aCompartir*) (0)){
		printf("Error fallo shmat \n");
		exit(2);
	}
	
	 while (1){
        sem_wait(&(memCompartida->bsem_rw));

        printf(COLOR_WRITER "Escritor se encuentra escribiendo.\n" COLOR_RESET); 
        
        sem_post(&(memCompartida->bsem_rw));
        printf(COLOR_WRITER "Escritor se va.\n" COLOR_RESET);
        sleep(3);
    }
}
