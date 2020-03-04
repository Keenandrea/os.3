#ifndef SHMEM_H
#define SHMEM_H

#include <semaphore.h>

typedef struct
{
	sem_t nsem;
	sem_t lognsem;
	int smints[];	
} shmem;

#endif /* SHMEM_H */
