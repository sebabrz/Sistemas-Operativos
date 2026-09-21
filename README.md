# Sistemas Operativos

Repositorio con los trabajos prácticos, actividades y ejercicios de la materia **Sistemas Operativos** (Ingeniería en Computación / Ingeniería en Sistemas de Información, Universidad Nacional del Sur), cursada en C sobre Linux.

Es material de estudio propio: código escrito para aprender y resolver los ejercicios de la cursada, no una librería ni un proyecto de producción. Puede haber cosas mejorables o en progreso.

## Estructura del repo

La lista de abajo es un vistazo a nivel de carpeta, no un índice de archivos: dentro de `ProgramasRandoms/`, `maru/` y de cada TP/Actividad van a ir apareciendo (y a veces desapareciendo o renombrándose) ejercicios sueltos a medida que avanza la cursada, así que el árbol se mantiene a ese nivel para no quedar desactualizado.

```
Sistemas-Operativos/
├── Diapositivas/
├── TP1_ProcesosHilos/
│   ├── Ejercicio1/
│   ├── Ejercicio2/
│   ├── Ejercicio3/
│   ├── Ejercicio4/
│   └── Ejercicio5/
├── TP2_IPC/
│   ├── Pipes/
│   ├── ColaDeMensajes/
│   ├── Memoria Compartida/
│   ├── PlantillasIPC/
│   └── apunte_ipc.md
├── Actividades/
│   ├── Actividad3-Procesos-2026-Multiplicacion/
│   └── Actividad3-Procesos-2026-Sudoku/
├── ProgramasRandoms/
├── maru/
└── plantillaMain.c
```

- **`Diapositivas/`** — Diapositivas de la cátedra (Clases 1 a 12), usadas como referencia teórica para todo el resto del repo.
- **`TP1_ProcesosHilos/`** — Trabajo práctico de procesos e hilos (`fork()`, `pthread`, identificación de procesos, suma de matrices con hilos y con procesos, etc.), organizado por ejercicio.
- **`TP2_IPC/`** — Trabajo práctico de comunicación entre procesos (IPC): pipes, colas de mensajes y memoria compartida, con la API System V (`msgget`/`msgsnd`/`msgrcv`, `shmget`/`shmat`/`shmdt`). Incluye `PlantillasIPC/` con plantillas base para cada mecanismo y `apunte_ipc.md` con apuntes teóricos del tema.
- **`Actividades/`** — Actividades sobre procesos, con una subcarpeta por actividad (por ahora, multiplicación de matrices y resolución de Sudoku en paralelo).
- **`ProgramasRandoms/`** — Programas sueltos de práctica sin atarse a un TP puntual (recursividad, procesos zombie, medición de tiempos, matrices, etc.). Es la carpeta más variable del repo: crece con ejercicios nuevos a medida que aparecen.
- **`maru/`** — Otra tanda de ejercicios sobre los mismos temas (procesos, hilos, pipes, colas, memoria compartida), resueltos con distintos enfoques a modo de comparación/práctica extra. También va sumando contenido con el tiempo.
- **`plantillaMain.c`** — Esqueleto base (includes típicos + `main()` vacío) usado como punto de partida al arrancar un ejercicio nuevo.

## Cómo compilar

Todo el código es C estándar sobre llamadas al sistema de Linux (POSIX / System V IPC), sin dependencias externas más allá de `pthread` en los ejercicios de hilos. Se compila directo con `gcc`:

```bash
gcc archivo.c -o ejecutable
./ejecutable
```

Para los ejercicios con hilos, agregar `-lpthread`:

```bash
gcc archivo.c -o ejecutable -lpthread
```

Algunos ejercicios de IPC crean colas de mensajes o segmentos de memoria compartida que persisten en el sistema si el programa se corta antes de liberarlos. Para inspeccionar o limpiar a mano:

```bash
ipcs -q            # colas de mensajes activas
ipcs -m            # segmentos de memoria compartida activos
ipcrm -Q <key>     # elimina una cola por su key
ipcrm -M <key>     # elimina un segmento por su key
```
