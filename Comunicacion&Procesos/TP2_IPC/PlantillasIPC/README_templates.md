# Plantillas de laboratorio — IPC

Tres puntos de partida, uno por mecanismo, sacados de los patrones que ya usás en el repo (no de un tutorial genérico).

## Qué plantilla abrir según el ejercicio

- **`pipe_template.c`** → comunicación entre padre e hijo (con o sin ida y vuelta).
- **`message_queue_template.c`** → procesos que no tienen por qué ser padre-hijo, o que necesitan clasificar mensajes por tipo/prioridad.
- **`shared_memory_template.c`** → varios procesos leyendo/escribiendo directamente sobre la misma estructura de datos (matrices, contadores, etc.), sin necesidad de "enviar" nada.

## Qué tomé como base y por qué

- **Pipes** → basada en `Pipes/Ejercicio1.c`: es la versión más general (dos pipes, uno por sentido), de la que después salen tus otros ejercicios (la conversación de `Ejercicio2.c`, o mandar structs/enteros como en `Ejercicio3.c`/`Ejercicio3.1_Struct.c`). Si el ejercicio es unidireccional, simplemente borrá el segundo pipe.
- **Colas de mensajes** → basada en `ColaDeMensajes/Ejercicio1.c` (la más simple: un emisor, un receptor, `struct` con `tipo`), pero dejé comentado el patrón de `IPC_NOWAIT` de `Ejercicio2.c` y el de prioridad con tipo negativo de `Ejercicio3.c`, por si el ejercicio los necesita.
- **Memoria compartida** → basada en `Memoria Compartida/Ejercicio2.c` (suma de matrices): usa un `for` con `fork()` adentro, que es más general que encadenar forks a mano como en `Ejercicio1.c` (conteo de vocales), porque sirve tanto si necesitás 1 hijo como 9.

## Qué vas a modificar normalmente

- El `#define KEY` (elegir un número que no se pise con otro ejercicio que tengas corriendo).
- El contenido del `struct` (qué datos manda el mensaje / qué guarda el segmento compartido).
- Las secciones marcadas `===== LÓGICA ... =====` y `TODO`: ahí va lo específico de cada ejercicio.
- En memoria compartida, `CANT_PROCESOS` según cuántos hijos necesites.
- En colas de mensajes, el `tipo` de mensaje y si el `msgrcv` va bloqueante (sin `IPC_NOWAIT`) o no bloqueante.

## Qué conviene dejar intacto

- Todo lo marcado `NO TOCAR`: creación del recurso (`pipe`/`msgget`/`shmget`), el cierre de extremos del pipe, el cálculo de `longitud` en colas (`sizeof(struct) - sizeof(long)`), el `shmdt`/`shmctl` y `msgctl` de limpieza.
- El chequeo de errores de `fork()` (`pid < 0`) y de la creación del recurso — son una línea, pero evitan que te tapes de resultados sin sentido si algo falló.
