sem_t agente;//ini=1
sem_t tabaco;//ini=0
sem_t papel;//ini=0
sem_t fosforo;//ini=0
pthread_mutex_t mesa;


void* fumt(){
    while(1){
        pthread_mutex_lock(&mesa);
        if(sem_trywait(&papel)==0){
            if(sem_trywait(&fosforo)==0){
                printf("Fumador tabaco toma papel y fosforos y fuma...\n");
                sem_post(&agente);
            }else{
                sem_post(&papel);
                printf("Fumador tabaco devuelve el papel...\n");
            }
        }
        pthread_mutex_unlock(&mesa);
        sleep(2);
    }
}
void* fump(){
    while(1){
        pthread_mutex_lock(&mesa);
        if(sem_trywait(&tabaco)==0){
            if(sem_trywait(&fosforo)==0){
                printf("Fumador papel toma tabaco y fosforos y fuma...\n");
                sem_post(&agente);
            }else{
                sem_post(&tabaco);
                printf("Fumador papel devuelve los fosforos...\n");
            }
        }
        pthread_mutex_unlock(&mesa);
        sleep(2);
    }
}
void* fumf(){
    while(1){
       pthread_mutex_lock(&mesa);
       if(sem_trywait(&tabaco)==0){
        if(sem_trywait(&papel)==0){
            printf("Fumador fosforo toma tabaco y papel y fuma...\n");
            sem_post(&agente);
        }else{     
            sem_post(&tabaco);
            printf("Fumador fosforos devuelve el tabaco...\n");
        }
       }
       pthread_mutex_unlock(&mesa);
       sleep(2);
    }
}
void* ag(){
    int r;
    while(1){
        sem_wait(&agente);
        
        r= rand() % 3;
        switch (r) {
      case 0:                    // pone papel y fósforos
        sem_post(&papel);
        sem_post(&fosforo);
        printf("Agente pone papel y fosforos en la mesa\n");
        break;
      case 1:                    // pone tabaco y fósforos
        sem_post(&tabaco);
        sem_post(&fosforo);
        printf("Agente pone tabaco y fosforos en la mesa\n");
        break;
      case 2:                    // pone tabaco y papel
        sem_post(&tabaco);
        sem_post(&papel);
        printf("Agente pone tabaco y papel en la mesa\n");
        break;
    }
    }
}
