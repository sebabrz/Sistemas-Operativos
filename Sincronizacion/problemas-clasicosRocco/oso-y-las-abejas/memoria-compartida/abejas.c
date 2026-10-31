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
	
	while(1) {
		sleep(1);  // Simula el tiempo de producción de miel
        
        // Espera a que haya espacio en el tarro
        sem_wait(&(memCompartida->vacio));
		sem_wait(&(memCompartida->turno));
		sem_post(&(memCompartida->turno));

        // Produce miel
        printf("Abeja GUARDA miel\n");
        sem_post(&(memCompartida->lleno));

        sem_wait(&(memCompartida->mutex)); // tomo el mutex para realizar una operacion que debe estar protegida

        sem_wait(&(memCompartida->turno)); //Para chequear si debo despertar o no al oso tambien necesito que haya algun turno habilitado.
        
        sem_post(&(memCompartida->turno)); // devuelvo el turno inmediatamente.

        // Verifica si el tarro está lleno
        if (sem_trywait(&(memCompartida->vacio)) != 0) {
            // Si no hay más mensajes VACIO, el tarro está lleno
            printf("Abeja despierta al oso\n");
            sem_post(&(memCompartida->despertar_oso));
        } else {
            // Si aún hay espacio, devuelve el mensaje VACIO
            sem_post(&(memCompartida->vacio));
        }
        sem_post(&(memCompartida->mutex)); // devuelvo el mutex
    }
	
    return 0;
}
