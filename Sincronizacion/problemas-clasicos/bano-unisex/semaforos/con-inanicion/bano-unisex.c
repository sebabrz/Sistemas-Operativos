#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define threadsMujeres 6
#define threadsHombres 6
#define cortePelo 300000
#define cantMujer 3
#define cantHombres 3
#define tiempoBano 0

#define PINK "\033[38;5;206m" 
#define LIGHT_BLUE "\033[38;5;117m" 
/*
entran varios hombres y mujeres
2 condiciones
hombres y mujeres no puede estar juntos
no mas de 3 personas
*/
sem_t HombresDentro, HombresMax;
sem_t MujeresDentro, MujeresMax;
sem_t genero;
pthread_mutex_t  mutexHombres, mutexMujeres;

void* mujer(){
	while(1){
		sem_wait(&MujeresMax);
		pthread_mutex_lock(&mutexMujeres);
		if(sem_trywait(&MujeresDentro) == 0){
			//No soy la primer mujer
			sem_post(&MujeresDentro);
		}else{
			//Soy la primer mujer
			//Bloque el recurso
			sem_wait(&genero);
		}
		//Cuento que una mujer entro
		sem_post(&MujeresDentro);
		printf("%s La mujer %i entro\n",PINK, pthread_self());
		pthread_mutex_unlock(&mutexMujeres);


		printf("%sMujer %i: voy al baño\n",PINK , pthread_self());
		//Simulo el tiempo
		sleep(tiempoBano);
		pthread_mutex_lock(&mutexMujeres);

		printf("%s La mujer %i se fue\n",PINK, pthread_self());
		sem_wait(&MujeresDentro);

		if(sem_trywait(&MujeresDentro) == 0){
			//No soy la ultima mujer
			sem_post(&MujeresDentro);
		}else{
			//Soy la ultima mujer
			//Libero el recurso
			sem_post(&genero);
		}
		pthread_mutex_unlock(&mutexMujeres);
		sem_post(&MujeresMax);
	}
}
void* hombre(){
	while(1){
		sem_wait(&HombresMax);
		pthread_mutex_lock(&mutexHombres);
		if(sem_trywait(&HombresDentro) == 0){
			//No soy la primer mujer
			sem_post(&HombresDentro);
		}else{
			//Soy la primer mujer
			//Bloque el recurso
			sem_wait(&genero);
		}
		//Cuento que un hombre entro
		printf("%s El hombre %i entro\n",LIGHT_BLUE, pthread_self());
		sem_post(&HombresDentro);
		pthread_mutex_unlock(&mutexHombres);

		printf("%sHombre %i: voy al baño\n", LIGHT_BLUE ,pthread_self());
		//simulo el tiempo
		sleep(tiempoBano);

		pthread_mutex_lock(&mutexHombres);

		printf("%s El hombre %i se fue\n",LIGHT_BLUE, pthread_self());
		sem_wait(&HombresDentro);
		
		if(sem_trywait(&HombresDentro) == 0){
			//No soy la ultima mujer
			sem_post(&HombresDentro);
		}else{
			//Soy la ultima mujer
			//Libero el recurso
			sem_post(&genero);
		}
		pthread_mutex_unlock(&mutexHombres);
		sem_post(&HombresMax);
	}
}


int main(int argc, char **argv){
	pthread_t mujeres[threadsMujeres];
	pthread_t hombres[threadsHombres];

	//Contadores de lugares disponibles
	sem_init(&genero,0,1);
	sem_init(&MujeresDentro,0,0);
	sem_init(&HombresDentro,0,0);
	sem_init(&MujeresMax,0,cantMujer);
	sem_init(&HombresMax,0,cantHombres);
	//Mutexs 
	pthread_mutex_init(&mutexMujeres, NULL);
	pthread_mutex_init(&mutexHombres, NULL);

	for (int i = 0; i < threadsMujeres; i++){
		pthread_create(&mujeres[i],NULL, &mujer, NULL);
	}
	for (int i = 0; i < threadsHombres; i++){
		pthread_create(&hombres[i],NULL, &hombre, NULL);
	}
	
	for (int i = 0; i < threadsMujeres; i++){
		pthread_join(mujeres[i],NULL);
	}
	for (int i = 0; i < threadsHombres; i++){
		pthread_join(hombres[i],NULL);
	}

	sem_close(&genero);
	sem_close(&MujeresDentro);
	sem_close(&HombresDentro);
	sem_close(&MujeresMax);
	sem_close(&HombresMax);
    pthread_mutex_destroy(&mutexMujeres);
    pthread_mutex_destroy(&mutexHombres);
	return 0;
}

