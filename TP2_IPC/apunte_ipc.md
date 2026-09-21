# Apunte corto — Procesos e IPC (TP2)

Machete rápido para repasar antes de programar o de rendir. No es documentación del proyecto, es un resumen de los conceptos que aparecen en él.

---

## Procesos y `fork()`

### En criollo
Un proceso es un programa corriendo. `fork()` es "clonate a vos mismo": de golpe hay dos copias corriendo el mismo código, cada una siguiendo desde el mismo punto.

### Concepto técnico
`fork()` crea un proceso hijo copiando el espacio de memoria del padre en ese instante. A partir de ahí son independientes: cada uno tiene su propia copia de las variables (cambiar una en el hijo no afecta al padre).

### En mi código
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

---

## Pipes

### En criollo
Un caño con dos puntas: lo que uno escribe de un lado, el otro lo lee del otro lado. Es unidireccional — para ida y vuelta hacen falta **dos** caños.

### Concepto técnico
`pipe()` crea un buffer en el kernel con dos file descriptors: uno de lectura (`[0]`) y uno de escritura (`[1]`). Solo lo pueden usar procesos con relación de parentesco (porque los fd se heredan por `fork()`).

### En mi código
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

### Funciones importantes
- **`pipe(int fd[2])`** → crea el pipe. Devuelve `0` u `-1`.
- **`write(fd, buffer, n)`** → manda `n` bytes. Si mando un string, uso `strlen(str)+1` (el `+1` es el `\0`).
- **`read(fd, buffer, n)`** → lee hasta `n` bytes, se **bloquea** si no hay nada escrito todavía.
- **`close(fd)`** → cierra un extremo. Fundamental: si no cerrás el que no usás, `read` nunca detecta el fin de la escritura.

### Para recordar
- Emisor y receptor tienen que acordar el tamaño del mensaje (no hay "delimitador" automático).
- `sizeof(puntero)` ≠ `strlen(string)+1` → error clásico si declarás el mensaje como `char*` en vez de array.
- Cerrar los extremos que no usás no es opcional, es parte del protocolo.
- Comunicación bidireccional = dos pipes, no uno solo.

---

## Memoria compartida (System V)

### En criollo
Un pizarrón que varios procesos pueden ver y tocar directamente. No se "manda" nada: todos leen/escriben la misma memoria física.

### Concepto técnico
`shmget` crea (o se asocia a) un segmento de memoria identificado por una `key`. `shmat` lo "engancha" al espacio de direcciones del proceso y devuelve un puntero normal de C. A partir de ahí, leer/escribir ahí es como cualquier variable — no hay llamada al sistema de por medio en cada acceso (por eso es más rápida que pipes/colas).

### En mi código
Uso una `struct` compartida (contadores de vocales, o matrices A/B/C). El padre crea el segmento con `shmget(KEY, tamaño, IPC_CREAT|0666)` y lo mapea con `shmat` **antes** de los `fork()`, así todos los hijos heredan el mismo puntero. Cada hijo escribe en una parte distinta de la estructura (una vocal, una fila), así que no compiten por el mismo dato.

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
- Si te olvidás de destruirlo, la próxima corrida con la misma `key` reusa el segmento viejo (con basura de la vez pasada) → bug fantasma.

---

## Colas de mensajes (System V)

### En criollo
Un buzón administrado por el sistema operativo: un proceso mete mensajes con una etiqueta ("tipo"), y otro los saca pidiendo esa etiqueta. Nadie necesita conocer al otro proceso, solo el número del buzón.

### Concepto técnico
`msgget` crea/asocia la cola. Cada mensaje es un `struct` que **empieza** con un campo `long tipo` (obligatorio) seguido de los datos. `msgsnd` deposita, `msgrcv` retira. Es comunicación **indirecta** (a diferencia del pipe, que es directa entre padre e hijo): la cola es del kernel, no de un proceso puntual.

### En mi código
Cada `struct mensaje` tiene `tipo` + datos. Calculo la longitud a enviar/recibir como `sizeof(struct) - sizeof(long)` (el tipo no cuenta como payload). Uso el `tipo` para clasificar: mensajes normales (1 o 2, leídos cada uno por un hijo distinto), o prioridades de pedidos (alta/media/baja) en el ejercicio de comida.

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
- Sin `IPC_NOWAIT`, `msgrcv` **bloquea** si no hay mensaje de ese tipo (usado en el ejercicio 1). Con `IPC_NOWAIT`, devuelve `-1` al instante si no hay nada (usado para el "cuarto proceso" y el receptor de pedidos, así puedo cortar el bucle o probar otra prioridad).
- El orden solo está garantizado **dentro del mismo tipo**, no en general.
- Cada ejercicio usa una `KEY` fija distinta a propósito, para no pisarse entre ejecuciones de ejercicios distintos.

---

## Bloqueante vs no bloqueante (sincronización)

### En criollo
Bloqueante = "me quedo esperando acá hasta que haya algo". No bloqueante = "me fijo, y si no hay nada, sigo mi vida".

### En mi código
- `read()` de un pipe: bloqueante siempre (si no escribieron nada, espera).
- `msgrcv()` sin `IPC_NOWAIT`: bloqueante (ejercicio 1 de colas).
- `msgrcv()` con `IPC_NOWAIT`: no bloqueante (ejercicios 2 y 3 de colas — lo uso para saber "ya no hay más" y cortar el `for`, o para probar la siguiente prioridad sin trabarme).
- `wait(NULL)`: también es bloqueante — el padre se detiene hasta que un hijo termine.

### Para recordar
- `IPC_NOWAIT` es el flag que cambia el comportamiento default (bloqueante) a no bloqueante.
- Si necesitás "esperar hasta que haya algo", no pongas `IPC_NOWAIT`. Si necesitás "revisar y seguir", sí.

---

## Flags/parámetros que suelen confundir

- **`IPC_CREAT`** → "creá el recurso si no existe todavía". Si ya existe, simplemente te asociás. No es "recrear siempre".
- **`0666`** (en `shmget`/`msgget`) → permisos en octal, en criollo: quién puede leer y escribir el recurso (como los permisos de un archivo). Técnicamente: 3 dígitos = dueño/grupo/otros, 6 = lectura+escritura.
- **`IPC_NOWAIT`** → hace que `msgrcv` no se quede esperando; devuelve error si no hay mensaje del tipo pedido.
- **`IPC_RMID`** → el "comando" que le paso a `msgctl`/`shmctl` para decir "destruí este recurso ya".
- **`KEY`** (`key_t`) → el número que hace que dos llamadas a `msgget`/`shmget` (en procesos distintos, o el mismo proceso en otra corrida) se refieran al mismo recurso del kernel.
- **`sizeof(struct) - sizeof(long)`** → no es un capricho: se resta el campo `tipo`, porque ese campo no viaja como "dato", es metadata que usa el propio mecanismo.

---

## C vs Sistemas Operativos

| Función | ¿De qué es? |
|---|---|
| `printf()`, `scanf()`, `strcpy()`, `sizeof()` | C puro (estándar, no toca el kernel) |
| `malloc()` | C (gestión de memoria del proceso, no IPC) |
| `fork()`, `wait()`, `getpid()` | Sistema Operativo — control de procesos |
| `pipe()`, `read()`, `write()`, `close()` | Sistema Operativo — IPC (aunque `read`/`write` también se usan para archivos) |
| `msgget()`, `msgsnd()`, `msgrcv()`, `msgctl()` | Sistema Operativo — IPC (colas de mensajes) |
| `shmget()`, `shmat()`, `shmdt()`, `shmctl()` | Sistema Operativo — IPC (memoria compartida) |

Regla rápida: si la función te devuelve un file descriptor, un PID, o gestiona un recurso del kernel (cola, segmento, pipe) → es SO. Si solo trabaja con la memoria de tu propio proceso sin pedirle nada al kernel → es C.

---

## Comparación rápida

| Mecanismo | Idea | ¿Comparten memoria? | ¿Envían datos? | Punto importante |
|---|---|---|---|---|
| Pipe | Caño entre padre e hijo | No | Sí, como bytes crudos | Unidireccional, requiere parentesco |
| Memoria compartida | Pizarrón común | Sí | No (no hay "envío", se lee/escribe directo) | Más rápida, pero hay que cuidar quién escribe qué |
| Cola de mensajes | Buzón con etiquetas | No | Sí, como mensajes con tipo | No requiere parentesco, clasifica por tipo |

---

## Para acordarme antes del parcial

- **Proceso**: un programa en ejecución, con su propio espacio de memoria.
- **`fork()`**: clona el proceso actual; a partir de ahí, padre e hijo corren por separado.
- **IPC** (Inter-Process Communication): cualquier mecanismo para que procesos (que tienen memoria separada) se pasen información.
- **Pipe**: caño unidireccional, solo entre procesos emparentados, sin clasificación de mensajes.
- **Memoria compartida**: mismo bloque de memoria física accesible por varios procesos; la más rápida, pero sin coordinación automática.
- **Cola de mensajes**: buzón del kernel con mensajes etiquetados por tipo; no hace falta parentesco.
- **Rol del kernel/SO**: es el dueño y administrador de todos estos recursos (pipes, colas, segmentos); los procesos solo piden acceso mediante llamadas al sistema.
- **Compartir memoria vs enviar mensajes**: compartir memoria es "todos ven lo mismo, directo"; enviar mensajes es "te copio una copia de los datos a través del kernel". Memoria compartida es más rápida pero más peligrosa (hay que cuidar el acceso concurrente); mensajes son más lentos pero más seguros/aislados.
- **Que una operación "bloquee"**: el proceso que la llama se queda esperando (dormido) hasta que se cumpla una condición (hay dato para leer, hay espacio para escribir, terminó un hijo). Lo opuesto es no bloqueante (`IPC_NOWAIT`): revisa y sigue.
