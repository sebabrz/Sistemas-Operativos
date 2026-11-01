#define CLIENTES 15
sem_t sillas;//ini=n
sem_t barbero;//ini=0
sem_t clientes;//ini=0
sem_t atencion;//ini=1//es la silla del abrbero, la ocupo despues de que me llame para sentarme
pthread_mutex_t mutex;
pthread_t clientes_tid[CLIENTES];
pthread_t barby;

void *barbudo(){
    while(1){
        sem_wait(&clientes);//si no hay clientes espera aca dormido
        sem_post(&barbero);
        printf("El barbero corta el pelo al cliente \n");
        sleep(1);
        sem_post(&atencion);
        printf("El barbero llama al proximo cliente si lo hay\n");
    }
}

void *cliente(void *argv){
    int id=(int)(intptr_t)argv;
    if(sem_trywait(&sillas)==0){
        printf("Entro el cliente %d a la sala\n", id);
        sem_post(&clientes);//si el barbero esta dormido lo despierta, sino solo se sienta

        printf("El cliente %d se sienta a esperar\n", id);
        sem_wait(&barbero);
        sem_post(&sillas);
        sem_wait(&atencion);
        printf("El cliente %d pasa a cortarse el pelo\n", id);
        sleep(3);
        printf("El cliente %d se cortó el pelo y se va\n", id);
    }else{
        printf("El cliente %d se va porque no habia sillas...\n", id);
        sleep(2);
    }
}
