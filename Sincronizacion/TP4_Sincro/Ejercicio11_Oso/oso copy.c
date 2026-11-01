sem_t vacio;//ini=M porciones que faltan poner en el tarro
sem_t lleno;//ini=0, binario, el oso espera a que le avisen si esta lleno
sem_t terminaOso;//ini=0, esperan a que le avisen (que se ponga en 1)
pthread_mutex_t mutex;

void *rutina_abeja(){
    while(1){
        printf("Una abeja se fija si hay porciones para poner\n");
        //mutexlock: lo usamos para que cuando una se fija si es la ultima, para avisarle el oso y
        //esperar que termino para que ninguna ponga cosas en el tarro cuando el oso libere las porciones
        pthread_mutex_lock(&mutex);//hace que sea mas secuencial
        sem_wait(&vacio);//si hay porciones para poner, pone una, sino es porque el oso esta comiendo
        printf("Una abeja pone una porcion\n");

        if(sem_trywait(&vacio)!=0){//si no hay mas porciones que poner en el tarro
            printf("La ultima abeja avisa al oso\n");
            sem_post(&lleno);
            sem_wait(&terminaOso);
        }else{
            sem_post(&vacio);
        }
        pthread_mutex_unlock(&mutex);

        sleep(2);
    }
}

void *rutina_oso(void *arg){
    int M=*(int *)arg;
    while(1){
        printf("El oso duerme \n");
        sem_wait(&lleno);//esta dormido inicialmente y se despierta cuando las abejas le dicen que esta lleno con el signal(lleno)
        printf("El oso se despierta\n");

        //ahora si va liberando las porciones-->duda: aca no puede pasar que el mientra el oso liebra las abejas van llenando?
        for(int i=0;i<M;i++){
            sem_post(&vacio);
        }
        sem_post(&terminaOso);//post terminooso
        printf("El oso se duerme y ya vacio el tarro\n");
        sleep(5);
    }
}