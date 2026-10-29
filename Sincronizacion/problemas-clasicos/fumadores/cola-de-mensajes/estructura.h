
#ifndef ESTRUCTURA_H
#define ESTRUCTURA_H

#include <sys/types.h>  
#include <sys/ipc.h>    
#include <unistd.h>     
#include <stdio.h>      
#include <stdlib.h> 
#include <sys/msg.h>
#include <sys/wait.h>
#include <errno.h>


#define P 1
#define T 2
#define F 3
#define ARMANDO 4

#define RESET "\033[0m"
#define GREEN "\033[32m"
#define CYAN "\033[36m"

// Tamaño de la estructura de mensaje
#define MSG_SIZE sizeof(struct mensaje) - sizeof(long)

// Estructura del mensaje
struct mensaje {
    long tipo;
};

// Declarar la función generateKey
key_t generateKey();

// Implementación de la función generateKey
key_t generateKey() {
    int projectId = 14;
    char path[200];
    getcwd(path, sizeof(path));  // Obtener el directorio de trabajo actual
    key_t key = ftok(path, projectId);  // Generar clave única
    return key;
}

#endif // ESTRUCTURA_H
