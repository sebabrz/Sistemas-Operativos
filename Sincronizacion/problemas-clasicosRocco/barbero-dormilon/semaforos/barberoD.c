#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 25 // Cantidad de clientes


/* Semaforos */
sem_t sem_Sillas;
sem_t sem_Clientes;
sem_t sem_Barbero;
sem_t cant_Clientes;
pthread_mutex_t t_barbero;

/* Procedimientos */
void* accionBarbero(void* numero);
void* accionClientes(void* numero);
void delay(void);

/* Metodos Auxiliares */
void dormirSiesta();
void despertarBarbero();
void cortarPelo();
void cerrarBarberia();

int main()
{

	//Semilla del random
	int i;
    srandom(60);

    //Threads
    pthread_t barbero_tid;
    pthread_t clientes_tid [NUM_THREADS];

	//Cantidad de Clientes
    int cant=NUM_THREADS-1;

    //Inicializacion de los semaforos
    sem_init(&sem_Barbero,0,0);
    sem_init(&sem_Sillas ,0,10);
    sem_init(&cant_Clientes,0,cant);
    sem_init(&sem_Clientes,0,0);
    pthread_mutex_init(&t_barbero, NULL);

    //Inicializacion de los threads
    pthread_create(&barbero_tid,NULL,accionBarbero,NULL);

    //Threads de Empleados
    for(int i=0;i<NUM_THREADS;i++){
       	 if(pthread_create(&clientes_tid[i],NULL,accionClientes,i))
       	 {
            printf("\n Inicializacion de los Threads Clientes Falló \n");
           	exit(EXIT_FAILURE);
          }
    }

    for(int i=0;i<NUM_THREADS;i++){
      pthread_join(clientes_tid[i],NULL);
    }

    pthread_join(barbero_tid,NULL);

    return 0;
}

void* accionBarbero(void* numero) {

    while (1) {

    	if(sem_trywait(&sem_Clientes)!=0){ // Si no hay clientes
    		dormirSiesta();
        	sem_wait(&sem_Clientes); //Esperar por clientes y dormir
        	despertarBarbero();
    	}
    	else // Hay clientes 
    		printf(" 	* Un cliente pasa a la silla \n");

        pthread_mutex_lock(&t_barbero);  //Proteger el numero de asientos disponibles
        sem_post(&sem_Barbero); //Traer un cliente para cortarle el pelo
        cortarPelo();
        pthread_mutex_unlock(&t_barbero); //Liberar el lock de las sillas

        if(sem_trywait(&cant_Clientes)!=0){ // Si es el ultimo cliente
        	cerrarBarberia();
            return 0;
        }

    }

}

void* accionClientes(void* numero) {

	int num = (int)numero+1;

    	delay();

        if (sem_trywait(&sem_Sillas)==0) {  //Si hay asientos libres se disminuye en uno el numero de asientos

        	printf(" + Cliente %i se sienta en la sala de espera..\n",num);
            
            sem_post(&sem_Clientes); // Se intenta despertar al barbero , si esta despierto luego lo atiende
            sem_wait(&sem_Barbero); //Esperar si el barbero esta ocupado
            printf(" 				- Cliente %i se retira satisfecho...\n",num); 
            sem_post(&sem_Sillas); // Libero mi silla 

        } else { //No hay asientos libres

        	printf(" # Espero a que se desocupe una silla ,  soy cliente %i \n",num); 
            sem_wait(&sem_Sillas); //Esperar por una silla
            printf(" + Despues de esperar me sente en la silla ,  soy cliente %i \n",num);

            sem_post(&sem_Clientes); // Se intenta despertar al barbero , si esta despierto luego lo atiende
            sem_wait(&sem_Barbero); //Esperar si el barbero esta ocupado
            printf(" 				- Cliente %i se retira satisfecho...\n",num); 
            sem_post(&sem_Sillas); // Libero mi silla

        }

}

void dormirSiesta(){
	printf("-------------------------------------------------------\n");
    printf("****************** BARBERO DURMIENDO ******************\n");
    printf("-------------------------------------------------------\n");
}

void despertarBarbero(){
    printf("***************** BARBERO SE DESPIERTA **************** \n");
}

void cortarPelo(){
    printf("************** BARBERO CORTANDO EL PELO ***************\n");
    sleep(7);
}

void cerrarBarberia(){
	printf("-------------------------------------------------------\n");
    printf("************* BARBERIA CERRADA POR EL DIA *************\n");
    printf("-------------------------------------------------------\n");
}

void delay(void) {
    int i;
    int delaytime;
    delaytime = random();
    for (i = 0; i<delaytime; i++);
}