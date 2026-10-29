/*
 * fumadores.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

sem_t p, t, f, armado;

void *papel(void *data)
{
	while (1)
	{
		sem_wait(&t);
		if (sem_trywait(&f) == 0)
		{
			sem_post(&armado);
			printf("papel");
		}
		else
			sem_post(&t);
	}
}
void *tabaco(void *data)
{
	while (1)
	{
		sem_wait(&p);
		if (sem_trywait(&f) == 0)
		{
			sem_post(&armado);
			printf("tabaco");
		}
		else
			sem_post(&p);
	}
}
void *fosforo(void *data)
{
	while (1)
	{
		sem_wait(&t);
		if (sem_trywait(&p) == 0)
		{
			sem_post(&armado);
			printf("fosforo");
		}
		else
			sem_post(&t);
	}
}
void *agente(void *data)
{
	while (1)
	{
		sem_wait(&armado);
		int r = rand() % 3;
		switch (r)
		{
		case 0:
		{
			sem_post(&t);
			sem_post(&p);
			break;
		}
		case 1:
		{
			sem_post(&f);
			sem_post(&p);
			break;
		}
		case 2:
		{
			sem_post(&t);
			sem_post(&f);
			break;
		}
		}
	}
}
int main(int argc, char **argv)
{
	pthread_t pp, tt, ff, a;
	sem_init(&p, 0, 0);
	sem_init(&t, 0, 0);
	sem_init(&f, 0, 0);
	sem_init(&armado, 0, 1);
	pthread_create(&pp, NULL, papel, NULL);
	pthread_create(&tt, NULL, tabaco, NULL);
	pthread_create(&ff, NULL, fosforo, NULL);
	pthread_create(&a, NULL, agente, NULL);
	pthread_join(pp, NULL);
	pthread_join(tt, NULL);
	pthread_join(ff, NULL);
	pthread_join(a, NULL);
	return 0;
}
