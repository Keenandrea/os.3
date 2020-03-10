#ifndef SHMEM_H
#define SHMEM_H

#include <semaphore.h>

typedef struct
{
	sem_t nsem;
	sem_t lognsem;
	int hindex;
	int tindex;
	int eindex;
	int smints[128];
	int counts[128];
	int indices[128];	
} shmem;

#endif /* SHMEM_H */
