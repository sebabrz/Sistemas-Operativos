#define IMPRESORAS 2   // cantidad de impresoras (generico: se puede cambiar a cualquier numero)
#define USUARIOS 15
sem_t impresoraLibre[IMPRESORAS]; // un binario por impresora: 1 = la impresora i+1 esta libre, 0 = en uso
sem_t impresorasLibres;           // contador: cuantas impresoras estan libres, arranca en IMPRESORAS (el usuario espera aca si no hay ninguna)
pthread_mutex_t mutex;            // hace atomica la eleccion de la impresora
// "usuario" es la identidad del USUARIO (solo se usa para los mensajes).
// La impresora NO se pasa: la elige requerir y la devuelve (1..IMPRESORAS).
int requerirImpresora(int usuario){
    int impresora = 0; // inicializada: si no, se podria devolver basura

    printf("Usuario %d: esperando alguna impresora\n", usuario);
    sem_wait(&impresorasLibres);            // espero a que haya alguna libre
    pthread_mutex_lock(&mutex);

    for(int i = 0; i < IMPRESORAS && impresora == 0; i++){ // busco cual es la libre
        if(sem_trywait(&impresoraLibre[i])==0){
            impresora = i + 1;              // las impresoras se numeran desde 1
        }
    }

    pthread_mutex_unlock(&mutex);
    return impresora;
}
void liberarImpresora(int usuario, int impresora){
    pthread_mutex_lock(&mutex);

    printf("Usuario %d: se libero impresora %d\n", usuario, impresora);
    sem_post(&impresoraLibre[impresora - 1]); // primero marco ESA impresora como libre...
    sem_post(&impresorasLibres);              // ...y despues aviso que hay una libre (en este orden!)

    pthread_mutex_unlock(&mutex);
}
void imprimir(int usuario, int impresora){
    printf("[Impresora %d] Usuario %d imprimiendo.\n", impresora, usuario);
    sleep(rand()%4);
    printf("[Impresora %d] Usuario %d termino de imprimir.\n", impresora, usuario);
}
void *user(void *arg){
    int usuario = (int)(intptr_t) arg; // id del usuario (1..USUARIOS)

    while(1){
        int impresora = requerirImpresora(usuario);
        printf("Usuario %d: la impresora requerida es la impresora %d\n", usuario, impresora);
        imprimir(usuario, impresora);
        liberarImpresora(usuario, impresora);
    }
    return NULL;
}
int main(){
    pthread_t usuarios[USUARIOS];
    srand(time(NULL));
    for(int i = 0; i < IMPRESORAS; i++){
        sem_init(&impresoraLibre[i], 0, 1);   // todas arrancan libres
    }
    sem_init(&impresorasLibres, 0, IMPRESORAS);
    pthread_mutex_init(&mutex, NULL);

    for(int i = 0; i < USUARIOS; i++){
        pthread_create(&usuarios[i], NULL, *user, (void *)(intptr_t)(i+1));
    }

    for(int i = 0; i < USUARIOS; i++){
        pthread_join(usuarios[i], NULL);
    }

    for(int i = 0; i < IMPRESORAS; i++){
        sem_destroy(&impresoraLibre[i]);
    }
    sem_destroy(&impresorasLibres);
    pthread_mutex_destroy(&mutex);

    return 0;
}