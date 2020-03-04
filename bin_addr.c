#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h> 
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/ipc.h> 
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>

#include <pthread.h>
#include <semaphore.h>

#include "shmem.h"

int main(int argc, char *argv[])
{
    printf("\nworld");
}
