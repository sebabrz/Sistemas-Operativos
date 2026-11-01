sem_t vacio;//ini=0, no hay lugares vacios inicialmente, asique va 0
sem_t comida;//ini=M, inicialmente hay M porciones de comida
sem_t terminaPapa;//ini=0, como hay comida inicialmente andie le avisa, el ultimo le avisa cuando no hay mas comida y se pone en 1
pthread_mutex_t nido;
pthread_t papa;
pthread_t pichones[cantidadPichones];

void *rutina_pichon(){
    while(1){
        printf("Un pichon se fija si tiene para comer\n");

        pthread_mutex_lock(&nido);
        sem_wait(&comida);
        printf("Pichon come porcion\n");

        if(sem_trywait(&comida)==0){
            sem_post(&comida);
        }else{
            printf("El ultimo pichon que comio avisa que no hay mas comida\n");
            sem_post(&vacio);
            sem_wait(&terminaPapa);
        }
        pthread_mutex_unlock(&nido);

        sleep(2);
    }
}

void *rutina_papa(void *arg){
    int M=*(int *)arg;
    while(1){
        printf("El papa espera que le avisen que no hay comida\n");
        sem_wait(&vacio);
        printf("El papa recibe el aviso de que no hay mas comida\n");

        for(int i=0;i<M;i++){
            sem_post(&comida);
        }
        printf("El papa avisa que ya repuso la comida\n");
        sem_post(&terminaPapa);//post terminooso
        sleep(2);
    }
}
int main() {
    int M=0;
    int cantidadPichones=0;
    while(M<=0){
        printf("Indique cantidad de porciones\n");
        scanf("%d", &M);
    }
    while(cantidadPichones<=0){
        printf("Indique cantidad de pichones\n");
        scanf("%d", &cantidadPichones);
    }
}