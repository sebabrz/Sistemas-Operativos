# Labo-1: Conceptos Importantes

## Procesos y fork()
Un proceso es un programa corriendo. `fork()` es "clonate a vos mismo": de golpe hay dos copias corriendo el mismo código, cada una siguiendo desde el mismo punto.

### Concepto técnico
`fork()` crea un proceso hijo copiando el espacio de memoria del padre en ese instante. A partir de ahí son independientes: cada uno tiene su propia copia de las variables (cambiar una en el hijo no afecta al padre).

Uso `fork()` encadenado (varios `if/else if` según el valor de retorno) para crear 2, 3 o hasta 9 procesos según el ejercicio (colas de mensajes, suma de matrices). Siempre chequeo `pid < 0` como error.

```c
pid = fork();
if (pid < 0) {
    printf("Error");
    exit(1);
} else if (pid == 0) {
    // soy el hijo
} else {
    // soy el padre, pid = PID del hijo
}
```

### Funciones importantes
- **`fork()`** → crea el hijo. Devuelve `0` en el hijo, el PID del hijo en el padre, `-1` si falla.
- **`wait(NULL)`** → el padre espera a que termine **cualquiera** de sus hijos (uno por cada llamada). Sin esto pueden quedar zombies.
- **`getpid()` / `getppid()`** → PID propio / PID del padre. Solo para debug/prints.

### Para recordar
- `pid == 0` → estás en el hijo. `pid > 0` → estás en el padre. `pid < 0` → error, no se creó nada.
- Hay que llamar `wait()` **una vez por cada hijo** creado.
- Todo lo que ya existía en memoria antes del `fork()` (variables, colas ya creadas, punteros de memoria compartida) se hereda en ambos procesos.
- Con cantidad variable de hijos (ej. matrices) conviene un `for` con `fork()` adentro, no escribirlos a mano.

## Pipes
Un caño con dos puntas: lo que uno escribe de un lado, el otro lo lee del otro lado. Es unidireccional, para ida y vuelta hacen falta dos caños.

### Concepto técnico
`pipe()` crea un buffer en el kernel con dos file descriptors: uno de lectura (`[0]`) y uno de escritura (`[1]`). Solo lo pueden usar procesos con relación de parentesco (porque los fd se heredan por `fork()`).

En cada ejercicio armo el pipe **antes** del `fork()`, y en cada proceso cierro el extremo que no me corresponde (si escribo, cierro `[0]`; si leo, cierro `[1]`). En el ejercicio de la "conversación" uso dos pipes, uno por sentido.

```c
int pipeFD[2];
pipe(pipeFD);          // [0] lectura, [1] escritura
pid = fork();

if (pid > 0) {                 // padre: escribe
    close(pipeFD[0]);
    write(pipeFD[1], mensaje, strlen(mensaje) + 1);
} else if (pid == 0) {         // hijo: lee
    close(pipeFD[1]);
    read(pipeFD[0], buffer, sizeof(buffer));
}
```

### Cuándo lo usamos
Cuando hay poco volumen de datos a pasar, no necesitamos clasificarlo por tipo ni que lo lea un proceso cualquiera, y la comunicación es simple. Los usás para comunicación directa y secuencial entre procesos emparentados (padre e hijo). Es ideal cuando querés mandar "bytes crudos" de un proceso a otro (por ejemplo, enviando una cadena de caracteres).

### Funciones importantes
- **`pipe(int fd[2])`** → crea el pipe. Devuelve `0` u `-1`. Se chequea esto en un `if` por si falla.
- **`write(fd, buffer, n)`** → manda `n` bytes. Si mando un string, uso `strlen(str)+1` (el `+1` es el `\0`).
- **`read(fd, buffer, n)`** → lee hasta `n` bytes, se **bloquea** si no hay nada escrito todavía.
- **`close(fd)`** → cierra un extremo. Fundamental: si no cerrás el que no usás, `read` nunca detecta el fin de la escritura.

### Para recordar
- Emisor y receptor tienen que acordar el tamaño del mensaje (no hay "delimitador" automático).
- `sizeof(puntero)` ≠ `strlen(string)+1` → error clásico si declarás el mensaje como `char*` en vez de array.
- Cerrar los extremos que no usás no es opcional, es parte del contrato.
- Comunicación bidireccional = dos pipes, no uno solo.

## Memoria compartida (System V)
Un pizarrón que varios procesos pueden ver y tocar directamente. No se "manda" nada: todos leen/escriben la misma memoria física.

### Concepto técnico
`shmget` crea (o se vincula a) un segmento de memoria identificado por una `key`. `shmat` lo "mapea/engancha" al espacio de direcciones del proceso y devuelve un puntero normal de C. A partir de ahí, leer/escribir ahí es como cualquier variable, no hay llamada al sistema de por medio en cada acceso (por eso es más rápida que pipes/colas).

### En mi código
Uso una `struct` compartida (contadores de vocales, o matrices A/B/C). El padre crea el segmento con `shmget(KEY, tamaño, IPC_CREAT|0666)` y lo mapea con `shmat` antes de los `fork()`, así todos los hijos heredan el mismo puntero. Cada hijo escribe en una parte distinta de la estructura (una vocal, una fila), así que no compiten por el mismo dato.

### Cuándo lo usamos
Cuando el volumen de datos es grande (matrices/structs grandes) y te importa que sea rápido, porque una vez mapeada (`shmat`), leer/escribir no pasa por el kernel es acceso directo a memoria, sin syscall de por medio. Lo costoso es garantizar que no haya condición de carrera por sí distintos procesos modifican los mismos datos/variables/estructuras, ya que necesitaríamos sincronización. 
Lo usamos cuando podemos dividir tareas y los procesos no pisan lo de otros y están en el mismo árbol o tienen una misma key.
No hay un "envío" de mensajes, sino que todos los procesos pueden ver y modificar la misma memoria física al instante.

```c
int id = shmget(KEY, sizeof(struct MatricesCompartida), IPC_CREAT | 0666);
struct MatricesCompartida *ptr = (struct MatricesCompartida *) shmat(id, 0, 0);

// ...fork()...
// en el hijo "i": escribe SU fila, nadie más toca esa fila
ptr->matrizC[i][j] = ptr->matrizA[i][j] + ptr->matrizB[i][j];

// al final:
shmdt(ptr);
shmctl(id, IPC_RMID, NULL);
```

### Funciones importantes
- **`shmget(key, size, flags)`** → crea/obtiene el segmento. Devuelve el `id` o `-1`.
- **`shmat(id, NULL, 0)`** → mapea el segmento, devuelve un puntero (o `(void*)-1` si falla).
- **`shmdt(ptr)`** → el proceso se desvincula del segmento (no lo borra, solo dice "yo ya no lo uso").
- **`shmctl(id, IPC_RMID, NULL)`** → recién esto **elimina** el segmento del sistema. Sin esto, queda persistiendo aunque cierres el programa.

### Para recordar
- `shmget` con `IPC_CREAT` es "creá si no existe, si existe asociate al que hay".
- Si dos procesos escriben la **misma** posición al mismo tiempo sin coordinarse, hay condición de carrera. En mi código lo evito repartiendo qué escribe cada uno (distinta vocal, distinta fila).
- `shmdt` ≠ `shmctl`: desvincularse no borra el segmento; `shmctl(IPC_RMID)` sí.
- Si te olvidás de destruirlo, la próxima corrida con la misma `KEY` reusa el segmento viejo (con basura de la vez pasada) → bug fantasma.

## Colas de mensajes (System V)
Un buzón administrado por el sistema operativo: un proceso mete mensajes con una etiqueta ("tipo"), y otro los saca pidiendo esa etiqueta. Nadie necesita conocer al otro proceso, solo el número del buzón.

### Concepto técnico
`msgget` crea/asocia la cola. Cada mensaje es un `struct` que empieza con un campo `long tipo` (obligatorio) seguido de los datos. `msgsnd` deposita, `msgrcv` retira. Es comunicación indirecta (a diferencia del pipe, que es directa entre padre e hijo): la cola es del kernel, no de un proceso puntual.

### En mi código
Cada `struct mensaje` tiene `tipo` + datos. Calculo la longitud a enviar/recibir como `sizeof(struct) - sizeof(long)` (el tipo no cuenta como payload). Uso el `tipo` para clasificar: mensajes normales (1 o 2, leídos cada uno por un hijo distinto), o prioridades de pedidos (alta/media/baja) en el ejercicio de comida.

### Cuando lo usamos
Cuando los procesos no necesariamente tienen parentesco ya que puedes mandar los mensajes a un mailbox al que todos estén conectados. Los uso cuando necesito clasificarlos por tipo/prioridad.
Los procesos no comparten memoria directa: le mandan al kernel un mensaje discreto (con `msgsnd`), y el kernel lo guarda en una cola hasta que otro proceso lo pide (`msgrcv`).
El kernel te da sincronización gratis: si no hay mensajes del tipo que pediste, `msgrcv` te bloquea solo (sin que vos programes nada extra).
Los procesos ni siquiera necesitan estar corriendo al mismo tiempo para comunicarse. 

```c
struct mensaje {
    long tipo;   // obligatorio, siempre primero
    int dato;
};

int longitud = sizeof(struct mensaje) - sizeof(long); // el "tipo" no cuenta

msgsnd(msqid, &mensaje, longitud, 0);                  // encolar
msgrcv(msqid, &mensaje, longitud, 1, 0);               // sacar tipo 1, bloqueante

// para prioridades (ejercicio de pedidos): tipo < 0 = "el de menor tipo hasta este valor"
msgrcv(msqid, &receptor, longitud, ALTA, IPC_NOWAIT);  // intenta primero prioridad alta
```

### Funciones importantes
- **`msgget(key, flags)`** → crea/asocia la cola. Devuelve `id` o `-1`.
- **`msgsnd(id, &msg, longitud, flags)`** → encola un mensaje.
- **`msgrcv(id, &msg, longitud, tipo, flags)`** → retira un mensaje. `tipo=0` trae cualquiera; `tipo>0` trae ese tipo exacto; `tipo<0` trae el de menor tipo entre los ≤ al valor absoluto (así implemento prioridades).
- **`msgctl(id, IPC_RMID, NULL)`** → destruye la cola.

### Para recordar
- Sin `IPC_NOWAIT`, `msgrcv` bloquea si no hay mensaje de ese tipo. Con `IPC_NOWAIT`, devuelve `-1` al instante si no hay nada (usado para el "cuarto proceso" y el receptor de pedidos, así puedo cortar el bucle o probar otra prioridad).
- Si comparamos el `msgrcv` con `== -1`, significa que el buzón está vacío y no hay mensajes para leer.
- Si comparamos el `msgrcv` con `!= -1`, significa que hay al menos 1 mensaje en el buzón.
- El orden solo está garantizado **dentro del mismo tipo**, no en general.
- Cada ejercicio usa una `KEY` fija distinta a propósito, para no pisarse entre ejecuciones de ejercicios distintos.

## Archivos
Abrís un archivo, le decís qué vas a hacer con él (leer, escribir, agregar), operás, y lo cerrás. `FILE*` es el "handle" que representa ese archivo abierto. 

### Concepto Técnico
`fopen()` es de la biblioteca estándar de C (`stdio.h`), no una llamada al sistema directa: es una capa con buffer arriba del `open()` real del SO. El modo con el que abrís el archivo define qué podés hacer y qué pasa según exista o no. 

```c
FILE *archivo = fopen("texto.txt", "r");
if (archivo == NULL) {
    perror("fopen");
    exit(1);
}
int c;
ptr->cant_a = 0;
while ((c = fgetc(archivo)) != EOF) {
    if ((char) c == 'a') ptr->cant_a++;
}
fclose(archivo);
```

### Modos de Apertura
- **`"r"`** → lectura. El archivo tiene que existir; si no, `fopen` devuelve `NULL`.
- **`"w"`** → escritura. Si no existe lo crea; si existe, le borra todo el contenido (trunca a 0).
- **`"a"`** → escritura al final (append). Si no existe lo crea; si existe, escribe a partir del final sin borrar nada.
- **`"r+"`** → lectura y escritura. El archivo tiene que existir.
- (variantes que te pueden cruzar) **`"w+"`** = lectura y escritura, crea/trunca igual que `"w"`. **`"a+"`** = lectura y escritura, agrega al final igual que `"a"`.

### Funciones importantes
- **`fopen(nombre, modo)`** → abre el archivo, devuelve `FILE*` o `NULL` si falla.
- **`fclose(fp)`** → cierra el archivo (y vuelca el buffer). Si te olvidás, podés perder lo escrito.
- **`fprintf(fp, ...) / fscanf(fp, ...)`** → como `printf/scanf` pero apuntando a un archivo en vez de stdout/stdin.
- **`fread(buffer, tam, cant, fp) / fwrite(buffer, tam, cant, fp)`** → lectura/escritura en bloques (binario).
- **`fgetc(FILE *stream)`** → lee un solo carácter del archivo apuntado por stream, lo devuelve como `int`, y mueve el puntero de lectura al siguiente carácter (así la próxima llamada lee el que sigue).
  - Devuelve el carácter leído convertido a `unsigned char` y después a `int` (por eso se guarda en `int` y no en `char` así se puede distinguir un byte válido de `EOF`).
  - Devuelve `EOF` si llegó al final del archivo o si hay un error de lectura. `EOF` es una constante (típicamente `-1`) definida en `stdio.h`, por eso el patrón clásico es: `int c; while ((c = fgetc(archivo)) != EOF) { /* procesar c */ }` 
  - Se puede castear a `(char)` devuelta.

### Para recordar
- Chequeá siempre que `fopen` no haya devuelto `NULL` antes de usar el `FILE*` mismo espíritu que chequear `pid < 0` en `fork()` o el retorno de `msgget`/`shmget`.
- `fopen`/`fread`/`fwrite`/`fclose` son C (stdio, con buffer): no confundir con `open()`/`read()`/`write()`/`close()`, que son las syscalls "crudas" que ya usás para pipes (esas sí son SO puro, sin buffer intermedio).
- `"w"` y `"a"` crean el archivo si no existe; `"r"` y `"r+"` no, ahí `fopen` falla si el archivo no está creado.
- Cada hijo necesita su propio `fopen()`, llamado **después** del `fork()` y dentro de su propia rama. Si el padre abre el archivo una sola vez antes de forkear y todos los hijos heredan el mismo `FILE*`, están compartiendo el mismo puntero de lectura en el kernel, el primer hijo que arranca a leer se devora el archivo entero y a los demás `fgetc` les da `EOF` de entrada.

## Bloqueante vs No-Bloqueante (sincronización)
Bloqueante = "me quedo esperando acá hasta que haya algo". No bloqueante = "me fijo, y si no hay nada, sigo mi vida".
- **`read()`** de un pipe: bloqueante siempre (si no escribieron nada, espera).
- **`msgrcv()`** sin `IPC_NOWAIT`: bloqueante (ejercicio 1 de colas).
- **`msgrcv()`** con `IPC_NOWAIT`: no bloqueante (ejercicios 2 y 3 de colas — lo uso para saber "ya no hay más" y cortar el `for`, o para probar la siguiente prioridad sin trabarme).
- **`wait(NULL)`**: también es bloqueante — el padre se detiene hasta que un hijo termine.

### Para recordar
- `IPC_NOWAIT` es el flag que cambia el comportamiento default (bloqueante) a no bloqueante.
- Si necesitás "esperar hasta que haya algo", no pongas `IPC_NOWAIT`. Si necesitás "revisar y seguir", sí.
- **`msgrcv` — bloqueante vs `IPC_NOWAIT`**:
  - **Sin `IPC_NOWAIT` (bloqueante, el 5to parámetro es 0)**: si no hay mensaje disponible del tipo pedido, el proceso se queda esperando ahí, **no devuelve -1**, no hay forma de usar el resultado como condición de corte de un loop. Solo devuelve `-1` ante un error real (cola borrada, señal, parámetros inválidos).
  - **Con `IPC_NOWAIT`**: si la cola está vacía para el tipo pedido, devuelve `-1` al instante (`errno = ENOMSG`), sin bloquear. Recién ahí tiene sentido un patrón tipo:

```c
for (int i = 0; i < N; i++) {
    if (msgrcv(msqid, &msg, longitud, tipo, IPC_NOWAIT) == -1) {
        break;  // no hay mas mensajes
    }
    // procesar msg
}
```

  - **Cuándo usar cada uno**:
    - Si el enunciado pide "esperar cuando no hay mensajes" → bloqueante (sin `IPC_NOWAIT`) + **mensaje centinela** para saber cuándo cortar (no se puede resolver el corte con el valor de retorno, porque nunca falla por "vacío").
    - Si necesitás sondear sin trabarte (polling) → `IPC_NOWAIT`, y ahí sí el `-1` te sirve como condición de corte.
  - **Ojo**: con `IPC_NOWAIT`, un `perror` disparado por cola vacía es comportamiento normal esperado, no un error real — si vas a loguear, conviene distinguir `errno == ENOMSG` (cortar tranquila) de cualquier otro `errno` (ahí sí error genuino).

## Flags/parámetros que suelen confundir
- **`IPC_CREAT`** → "creá el recurso si no existe todavía". Si ya existe, simplemente te asociás. No es "recrear siempre".
- **`0666`** (en `shmget`/`msgget`) → permisos en octal, en criollo: quién puede leer y escribir el recurso (como los permisos de un archivo). Técnicamente: 3 dígitos = dueño/grupo/otros, 6 = lectura+escritura.
- **`IPC_NOWAIT`** → hace que `msgrcv` no se quede esperando; devuelve error si no hay mensaje del tipo pedido.
- **`IPC_RMID`** → el "comando" que le paso a `msgctl`/`shmctl` para decir "destruí este recurso ya".
- **`KEY (key_t)`** → el número que hace que dos llamadas a `msgget`/`shmget` (en procesos distintos, o el mismo proceso en otra corrida) se refieran al mismo recurso del kernel.
- **`sizeof(struct) - sizeof(long)`** → no es un capricho: se resta el campo `tipo`, porque ese campo no viaja como "dato", es metadata que usa el propio mecanismo.

## C vs Sistemas Operativos

| Función | ¿De qué es? |
|---|---|
| `printf()`, `scanf()`, `strcpy()`, `sizeof()` | C puro (estándar, no toca el kernel) |
| `malloc()` | C (gestión de memoria del proceso, no IPC) |
| `fork()`, `wait()`, `getpid()` | Sistema Operativo: control de procesos |
| `pipe()`, `read()`, `write()`, `close()` | Sistema Operativo: IPC (aunque `read`/`write` también se usan para archivos) |
| `msgget()`, `msgsnd()`, `msgrcv()`, `msgctl()` | Sistema Operativo: IPC (colas de mensajes) |
| `shmget()`, `shmat()`, `shmdt()`, `shmctl()` | Sistema Operativo: IPC (memoria compartida) |

Regla rápida: si la función te devuelve un file descriptor, un PID, o gestiona un recurso del kernel (cola, segmento) → es SO. Si solo trabaja con la memoria de tu propio proceso sin pedirle nada al kernel → es C.

## Comparación rápida

| Mecanismo | Idea | ¿Comparten memoria? | ¿Envían datos? | Punto importante |
|---|---|---|---|---|
| Pipe | Caño entre padre e hijo | No | Sí, como bytes crudos | Unidireccional, requiere parentesco |
| Memoria compartida | Pizarrón común | Sí | No (no hay "envío", se lee/escribe directo) | Más rápida, pero hay que cuidar quién escribe qué |
| Cola de mensajes | Buzón con etiquetas | No | Sí, como mensajes con tipo | No requiere parentesco, clasifica por tipo |

## Para acordarme antes del parcial
- **Proceso:** un programa en ejecución, con su propio espacio de memoria.
- **`fork()`:** clona el proceso actual; a partir de ahí, padre e hijo corren por separado.
- **IPC (Inter-Process Communication):** cualquier mecanismo para que procesos (que tienen memoria separada) se pasen información.
- **Pipe:** caño unidireccional, solo entre procesos emparentados, sin clasificación de mensajes.
- **Memoria compartida:** mismo bloque de memoria física accesible por varios procesos; la más rápida, pero sin coordinación automática.
- **Cola de mensajes:** buzón del kernel con mensajes etiquetados por tipo; no hace falta parentesco.
- **Rol del kernel/SO:** es el dueño y administrador de todos estos recursos (pipes, colas, segmentos); los procesos solo piden acceso mediante llamadas al sistema.
- **Compartir memoria vs enviar mensajes:** compartir memoria es "todos ven lo mismo, directo"; enviar mensajes es "te copio una copia de los datos a través del kernel". Memoria compartida es más rápida pero más peligrosa (hay que cuidar el acceso concurrente); mensajes son más lentos pero más seguros/aislados.
- **Que una operación "bloquee":** el proceso que la llama se queda esperando (dormido) hasta que se cumpla una condición (hay dato para leer, hay espacio para escribir, terminó un hijo). Lo opuesto es no bloqueante (`IPC_NOWAIT`): revisa y sigue.
