#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX 1000
#define SHM_SIZE ((MAX + 1) * sizeof(int))  // +1 para guardar la cantidad de numeros que va a tener la secuencia

int main(){
    pid_t pid;
    int n;
    printf("Ingrese un entero positivo: \n");
    scanf("%d", &n);
    while(n<=0){
        printf("Ingrese un entero positivo: \n");
    scanf("%d", &n);
    }

    int fd = shm_open("COLLATZ", O_CREAT | O_RDWR, 0666); //COLLATZ es el nombre de mi bloque de shm, O_CREAT es la flag que dice que si no existe lo cree, 
    //O_RDWR es la flag que dice que se pueda leer y escribir en la memoria compartida, y el 0666 es el permiso del objeto para que se pueda hacer lo mismo
    
    ftruncate(fd, SHM_SIZE);//como shm_open arranca con tamaño 0, le asigno el valor de SHM_SIZE
   
   //creo el puntero con mmap hacia la memoria, 0 es porque no me improta en donde ponga este puntero
   //SHM_SIZE es cuantos bytes quiero mapear
   //PROT_READ PROT_WRITE son flags que dicen quiero poder leer Y escribir sobre esta memoria
   //MAP_SHARED es una flag que le dice al sistema que esta memoria debe ser compartida entre todos los procesos que la mapeen
   //fd es el file descriptor que me dio shm_open(), para indicar cual objeto de memoria queremos mapear
   // 0 es un offset, "desde qué byte empezar" dentro del objeto — normalmente 0, desde el principio.
   //mmap() te devuelve un puntero genérico (void *), que casteás a (int *) porque en tu caso vas a usar esa memoria como un arreglo de enteros.
    int *shm_ptr = (int *) mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    int i=1; //indice en donde vamos a comenzar a guardar los numeros generados.
    pid=fork();
    if (pid < 0) {
        printf("Error al crear el proceso hijo\n");
        exit(1);
    } else if (pid == 0) {
         shm_ptr[i]=n;
            i++;
        // Proceso hijo: genera y muestra la secuencia de Collatz
        while (n != 1) {
            if (n % 2 == 0) {
                n = n / 2;
            } else {
                n = 3 * n + 1;
            }  
            shm_ptr[i]=n;
            i++;    
        }

        shm_ptr[0] = i - 1; 
    } else {
        // Proceso padre: espera a que el hijo termine
        wait(NULL);
        printf("El proceso hijo ha finalizado.\n");
        int cantNumeros= shm_ptr[0];
        for(i=1; i<=cantNumeros;i++){
            printf("%d, ", shm_ptr[i]);
        }
        printf("\n");
        shm_unlink("COLLATZ");
    }

    return 0;
}