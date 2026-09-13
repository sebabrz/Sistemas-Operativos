#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/types.h>

#define cant 9
#define KEY ((key_t) (1243)) /* número de llave */
#define SEGSIZE sizeof (struct info)

struct info{
    int valido;
};

int sudoku[cant][cant];
void generarMatriz(){
    for(int i=0;i<cant;i++){
        for(int j=0;j<cant;j++){
            sudoku[i][j]=random()%9+1;
            printf("%d ", sudoku[i][j]);
        }
        printf("\n ");
    }
}
    
int validarFilas(){
    int valido=1;//creamos una variable local de la funcion asumiendo que es verdadera y el sudoku es valido
    for(int i=0; i<cant; i++){
        int aux[cant+1] = {0};
        for(int j=0; j<cant; j++){
            if(aux[sudoku[i][j]] == sudoku[i][j]){
                valido=0;
                return valido;
            }
            aux[sudoku[i][j]] = sudoku[i][j];
        }
    }
    return valido;
}

int validarColumnas(){
    int valido=1;
    for(int i=0; i<cant; i++){
        int aux[cant+1] = {0};
        for(int j=0; j<cant; j++){
            if(aux[sudoku[j][i]] == sudoku[j][i]){
               valido=0;
                return valido;
            }
            aux[sudoku[j][i]] = sudoku[j][i];
        }
    }
    return valido;
}

int validarBloques(){
 int valido=1;
for(int i=0; i<cant; i+=3){
        for(int j=0; j<cant; j+=3){
            int aux[cant+1] = {0};
            for(int k=i;k<i+3;k++){
                for(int l=j;l<j+3;l++){
                    if(aux[sudoku[k][l]] == sudoku[k][l]){
                        valido=0;
                        return valido;
                    }
                    aux[sudoku[k][l]] = sudoku[k][l];
                }
            }
        }
}
 return valido;
}


int main(){
    srandom(time(NULL));
    printf("sudoku \n");
   // generarMatriz();
   // printf("\n");
   // si descomentas esto y comentas lo de generar matriz, esta matriz es correcta y no genera errores, 
   // proba cambiando algun nro si queres probar
   //si falla por filas va a fallar por columnas asique para probar el de columnas hay q comentar validarFilas y cuando haces el pipe de filas y todo eso
   /*
  int matrizTest[cant][cant] = { //esta usala apra validar bloques
    {1,2,3,4,5,6,7,8,9},
    {2,3,4,5,6,7,8,9,1},
    {3,4,5,6,7,8,9,1,2},
    {4,5,6,7,8,9,1,2,3},
    {5,6,7,8,9,1,2,3,4},
    {6,7,8,9,1,2,3,4,5},
    {7,8,9,1,2,3,4,5,6},
    {8,9,1,2,3,4,5,6,7},
    {9,1,2,3,4,5,6,7,8}
};
int matrizTest[cant][cant] = { //esta usala para validar filas y columnas
    {5,3,4,6,7,8,9,1,2},
    {6,7,2,1,9,5,3,4,8},
    {1,9,8,3,4,2,5,6,7},
    {8,5,9,7,6,1,4,2,3},
    {4,2,6,8,5,3,7,9,1},
    {7,1,3,9,2,4,8,5,6},
    {9,6,1,5,3,7,2,8,4},
    {2,8,7,4,1,9,6,3,5},
    {3,4,5,2,8,6,1,7,9}
};
    for(int i=0;i<cant;i++)
        for(int j=0;j<cant;j++)
            sudoku[i][j] = matrizTest[i][j];

    // print de control: lo que ves acá es lo que se valida de verdad
    for(int i=0;i<cant;i++){
        for(int j=0;j<cant;j++) printf("%d ", sudoku[i][j]);
        printf("\n");
    }
    printf("\n");
   */
    struct info *ctrl;//creo mi puntero de control el que me apunta al segmento
    int id;
    id=shmget(KEY, SEGSIZE, IPC_CREAT | 0666);
    if(id < 0){
        printf("Error en shmget\n");
        exit(1);
    }
    ctrl = (struct info*) shmat(id, 0, 0); //lo vinculo al segmento que tiene ese id que lo defini arriba, hace el mapeo de ese segmento a mi espacio de direcciones
                                            // el cast (struct info*) convierte a ctrl en puntero apra hacer ctrl->valido
    if(ctrl <= (struct info *) 0){
        printf("Error en shmat\n");
        exit(2);
    }

    pid_t procesoFila;
    procesoFila=fork();
    if(procesoFila==0){
        ctrl->valido=validarFilas();//valida filas y escribe directo en la memoria compartida
        shmdt(ctrl);
        exit(0);
    }else{
        wait(NULL); //esperamos a que el hijo labure
        if(ctrl->valido == 0){//si valido es 0, es porque falló
            printf("El sudoku no es valido porque falla en una fila\n");
            shmdt(ctrl);//hago el detached
            return 0;
        }
    }

    pid_t procesoColumnas;
    procesoColumnas=fork();
    if(procesoColumnas==0){
        ctrl->valido=validarColumnas();//valida columnas y escribe directo en la memoria compartida
        shmdt(ctrl);
        exit(0);
    }else{
        wait(NULL); //esperamos a que el hijo labure
        if(ctrl->valido == 0){//si valido es 0, es porque falló
            printf("El sudoku no es valido porque falla en una columnas\n");
            shmdt(ctrl);//hago el detached
            return 0;
        }
    }
    
    pid_t procesoBloques;
    procesoBloques=fork();
    if(procesoBloques==0){
        ctrl->valido=validarBloques();//valida bloques y escribe directo en la memoria compartida
        shmdt(ctrl);
        exit(0);
    }else{
        wait(NULL); //esperamos a que el hijo labure
        if(ctrl->valido == 0){//si valido es 0, es porque falló
            printf("El sudoku no es valido porque falla en un bloque\n");
            shmdt(ctrl);//hago el detached
            return 0;
        }
    }

    printf("el sudoku es VALIDO\n");

    return 0;
}
