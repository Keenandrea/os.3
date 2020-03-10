/* PREPROCESSOR DIRECTIVES ============================================= */
/* ===================================================================== */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h> 
#include <sys/shm.h>
#include <sys/ipc.h> 
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>

#include "shmem.h"
/* END ================================================================= */


/* GLOBAL VARIABLES ==================================================== */
/* ===================================================================== */
shmem* smseg;
int sipcid = 0;
char fn[] = "log.out";
/* END ================================================================= */


/* FUNCTION PROTOTYPES ================================================= */
/* ===================================================================== */
void sminit();
void csectionone(int, int);
/* END ================================================================= */


/* RETURNS EXIT STATUS OF CHILD ======================================== */
/* ===================================================================== */
void wexitstatusval()
{
	shmdt(smseg);
	exit(42);
}
/* END ================================================================= */


/* BIN_ADDR PROCESS MAIN =============================================== */
/* ===================================================================== */
int main(int argc, char *argv[])
{
	char xx[64];
	char yy[20];
	int index;
	int count;

	sminit();

	/* place arguments i
 * 	   n their correct v
 * 	   ariable places */
	strcpy(xx, argv[1]);
	strcpy(yy, argv[2]);
	index = atoi(xx);
	count = atoi(yy);

	csectionone(index, count);
	
	wexitstatusval(); 
return 0;
}
/* END ================================================================= */


/* CRITICAL SECTION FOR N/2 ============================================ */
/* ===================================================================== */
void csectionone(int index, int count)
{
	int hrquest;
	int trquest;
	int result;	
	
	/* employ shared memo
 * 	   ory to update shar
 * 	   ed memory int arra
 * 	   y indices       */
	smseg->hindex = index;
	smseg->tindex = index + (count / 2);
	smseg->eindex = count;	

	for(hrquest = smseg->hindex, trquest = smseg->tindex; hrquest < (smseg->hindex + 5), trquest < (smseg->tindex + 5); hrquest++, trquest++)
	{
		/* employ the active process to add together each pair of integers */
		result = smseg->smints[hrquest] + smseg->smints[trquest];
		
		if((smseg->hindex + 5) > (count / 2))
		{
			fprintf(stderr, "[wait]  -> [pid: %i] [time: %i]\n", getpid(), time(NULL));
			sem_wait(&(smseg->nsem));
			fprintf(stderr, "[enter] -> [pid: %i] [time: %i]\n", getpid(), time(NULL));
			FILE * fout = fopen(fn,"a");
			if(fout == NULL)
			{	
				perror("\nbin_addr: error: failed to open output file");
				exit(EXIT_FAILURE);
			}
			sleep(1);
			fprintf(fout, "\n\t%i\t\t%i\t\t%i", getpid(), index, count);
			fprintf(fout, "\t\t[%i and %i]\t\t[%i + %i]\t\t%i\t\t\t\t%i\n", hrquest, trquest, smseg->smints[hrquest], smseg->smints[trquest], hrquest, result);
			fprintf(fout, "---------------------------------------------------------------------------------------------------------------------------------");
			fprintf(fout, "-----------------------\n");
			fclose(fout);
			sleep(1);
			fprintf(stderr, "[exit]  -> [pid: %i] [time: %i]\n", getpid(), time(NULL));
			sem_post(&(smseg->nsem));
			smseg->smints[hrquest] = result;
			wexitstatusval();
		}
		
		fprintf(stderr, "[wait]  -> [pid: %i] [time: %i]\n", getpid(), time(NULL));
		sem_wait(&(smseg->nsem));
		fprintf(stderr, "[enter] -> [pid: %i] [time: %i]\n", getpid(), time(NULL));
		FILE * fout = fopen(fn,"a");
		if(fout == NULL)
		{	
			perror("\nbin_addr: error: failed to open output file");
			exit(EXIT_FAILURE);
		}
		sleep(1);
		fprintf(fout, "\n\t%i\t\t%i\t\t%i", getpid(), index, count);
		fprintf(fout, "\t\t[%i and %i]\t\t[%i + %i]\t\t%i\t\t\t\t%i\n", hrquest, trquest, smseg->smints[hrquest], smseg->smints[trquest], hrquest, result);
		fprintf(fout, "---------------------------------------------------------------------------------------------------------------------------------");
		fprintf(fout, "-----------------------\n");
		fclose(fout);
		sleep(1);
		fprintf(stderr, "[exit]  -> [pid: %i] [time: %i]\n", getpid(), time(NULL));
		sem_post(&(smseg->nsem));
		smseg->smints[hrquest] = result;
	}
}
/* END ================================================================= */


/* INITIATES SHARED MEMORY ============================================= */
/* ===================================================================== */
void sminit()
{
	key_t smkey = ftok("shmfile", 'a');
	if(smkey == -1)
	{
		perror("\nbin_addr: error: ftok failed");
		exit(EXIT_FAILURE);
	}

	sipcid = shmget(smkey, sizeof(shmem), 0600 | IPC_CREAT);
	if(sipcid == -1)
	{
		perror("\nbin_addr: error: failed to create shared memory");
		exit(EXIT_FAILURE);
	}

	smseg = (shmem*)shmat(sipcid,(void*)0, 0);

	if(smseg == (void*)-1)
	{
		perror("\nbin_addr: error: failed to attach shared memory");
		exit(EXIT_FAILURE);
	}
}
/* END ================================================================ */

