#ifndef SHMEM_H
#define SHMEM_H

#include <semaphore.h>

typedef struct
{
	sem_t nsem;
	sem_t lognsem;
	int execflag;
	int hindex;
	int tindex;
	int eindex;
	int sfinal;
	int icount;
	int smints[128];	
} shmem;

#endif /* SHMEM_H */
