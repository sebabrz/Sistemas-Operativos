#define ASIENTOS 5     //cantidad de asientos del bar = tamanio del grupo
#define N_CLIENTES 12  //cantidad de clientes (tiene que ser >= ASIENTOS)
//------------------------- DECLARACION DE LOS SEMAFOROS --------------------------//
/* Idea general:
 *  - Los asientos se devuelven TODOS JUNTOS, y solo cuando se va el ultimo del grupo.
 *    Asi "esperar a que se vaya todo el grupo" sale solo del contador de asientos.
 *  - Para saber quien es el 5to que se sienta y quien es el ultimo que se va usamos
 *    el mismo truco: un contador en 4 y sem_trywait. Los primeros 4 lo consumen, y
 *    al 5to el trywait le falla (ahi sabe que es el ultimo de esa fase).
 *  - El mutex es la "puerta" del bar: se entra (se toma asiento) y se sale de A UNO.
 *    Se usa solo en esos momentos; NUNCA se duerme con el mutex tomado
 *    (comer y esperar al grupo pasan con la puerta libre). */
sem_t libres;   //contador: asientos libres, arranca en 5. Si vale 0, el que llega se bloquea aca (hay un grupo en el bar)
sem_t faltan;   //contador: cuantos del grupo todavia pueden pasar SIN ser el ultimo, arranca en 4.
                //Se usa en dos fases que nunca se pisan: al sentarse (para detectar al 5to)
                //y al irse (para detectar al ultimo). Cada fase lo deja otra vez en 4.
sem_t comenzar; //aca esperan los que ya se sentaron hasta que se siente el 5to, arranca en 0
pthread_mutex_t mutex; //la puerta: entrar y salir de a uno
pthread_t hilosClientes[N_CLIENTES]; 
//----------------------------------- RUTINAS -------------------------------------//
void *cliente(void *arg){
    int id = (int)(intptr_t)arg; //recupero el numero de cliente que me paso el main

    while(1){ //el cliente vuelve al bar una y otra vez
        usleep(rand() % 1000000); //tarda un tiempo al azar en llegar (hasta 1 segundo)

        //----- LLEGADA -----//
        pthread_mutex_lock(&mutex);        //paso por la puerta (de a uno)
        if(sem_trywait(&libres) != 0){     //intento tomar un asiento SIN esperar. Si falla, libres vale 0: hay un grupo en el bar
            pthread_mutex_unlock(&mutex);  //suelto la puerta ANTES de dormirme (si no, el grupo no podria salir)
            printf("Cliente %d espera a que se vaya el grupo.\n", id);
            sem_wait(&libres);             //me duermo hasta que el ultimo del grupo libere los asientos
            pthread_mutex_lock(&mutex);    //ya tengo asiento: vuelvo a pasar por la puerta, de a uno
        }                                  //(si el trywait anduvo, ya tengo mi asiento y sigo con la puerta tomada)
        printf("Cliente %d se sienta.\n", id);

        //----- ESPERO A QUE SE SIENTEN LOS 5 -----//
        if(sem_trywait(&faltan) == 0){     //el trywait anduvo: todavia hay lugar antes del 5to, asi que NO soy el ultimo en sentarme
            pthread_mutex_unlock(&mutex);  //suelto la puerta ANTES de dormirme (los que faltan tienen que poder entrar)
            sem_wait(&comenzar);           //me duermo hasta que se siente el que falta (el 5to me va a despertar)
        } else{                            //el trywait fallo: ya se sentaron 4 antes que yo, soy el 5to y el grupo esta completo
            for(int i = 0; i < ASIENTOS - 1; i++){
                sem_post(&faltan);         //repongo faltan en 4 para usarlo en la SALIDA (la fase de sentarse lo dejo en 0)
            }                              //(lo repongo ANTES de despertar a nadie, asi nadie lo usa mientras esta en 0)
            for(int i = 0; i < ASIENTOS - 1; i++){
                sem_post(&comenzar);       //despierto a los otros 4 que estaban esperando (uno por cada post)
            }
            pthread_mutex_unlock(&mutex);  //ya termine de entrar: suelto la puerta
        }

        //----- COMER -----//
        printf("Cliente %d come sushi.\n", id); //llegamos aca recien cuando estan los 5 sentados (y sin la puerta tomada)
        sleep(1);                               //tarda 1 segundo en comer

        //----- SALIDA -----//
        pthread_mutex_lock(&mutex);        //paso por la puerta para irme (de a uno)
        printf("Cliente %d termina de comer y se va.\n", id);
        if(sem_trywait(&faltan) != 0){     //el trywait fallo: ya se fueron 4 antes que yo, soy el ultimo del grupo
            for(int i = 0; i < ASIENTOS - 1; i++){
                sem_post(&faltan);         //repongo faltan en 4 para que el PROXIMO grupo pueda usarlo al sentarse
            }                              //(otra vez ANTES de liberar los asientos, asi esta listo cuando entre alguien)
            for(int i = 0; i < ASIENTOS; i++){
                sem_post(&libres);         //libero los 5 asientos juntos: ahora si pueden entrar los que esperaban (u otros que lleguen)
            }
        }                                  //si el trywait anduvo, no soy el ultimo: me voy SIN liberar mi asiento (el bar sigue ocupado)
        pthread_mutex_unlock(&mutex);      //ya sali: suelto la puerta
    }
    return NULL;
}