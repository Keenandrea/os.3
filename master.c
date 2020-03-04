/* PREPROCESSOR DIRECTIVES ============================================= */
/* ===================================================================== */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h> 
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/ipc.h> 
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>

#include "shmem.h"
/* END ================================================================= */


/* GLOBAL VARIABLES ==================================================== */
/* ===================================================================== */
shmem* smseg; 

int saflag = 0;
int sipcid = 0;
char fin[] = "input.dat";
/*
struct sigaction sactrl;
struct sigaction satime;

static int satimer()
{
	struct itimerval

}

void killctrl(int, siginfo_t *, void *);
void killtime(int, siginfo_t *, void *);
*/

/* END ================================================================= */


/* FUNCTION PROTOTYPES ================================================= */
/* ===================================================================== */
void sminit(); 
void smexit();
void detach();
int fpinit();
void readtosm(FILE *, int);
/* END ================================================================= */


/* HANDLES SIGNALS ===================================================== */
/* ===================================================================== */
void sahandler(int sig)
{
	char msgctrl[] = "\nmaster: exit: received ctrl-c interrupt signal\n";
	char msgtime[] = "\nmaster: exit: computation exceeded 100 seconds\n";
	int msglenctrl = sizeof(msgctrl);
	int msglentime = sizeof(msgtime);

	switch(sig)
	{
		case SIGALRM:
			write(STDERR_FILENO, msgtime, msglentime);
			break;		
		case SIGINT:
			write(STDERR_FILENO, msgctrl, msglenctrl);
			break;
	}
	saflag = 1;	
}
/* END ================================================================= */


/* MAIN ================================================================ */
/* ===================================================================== */
int main(int argc, char *argv[])
{
	int lcount;
	lcount = fpinit();		

	sminit();
	
	FILE *fptr = fopen(fin, "r");
	if(fptr == NULL)
	{
		perror("\nmaster: error: failed to read input file");
		smexit();
		exit(EXIT_FAILURE);
	}

	readtosm(fptr, lcount);

	signal(SIGALRM, sahandler);
	alarm(100);
	signal(SIGINT,  sahandler);	

	if(saflag == 1)
	{
		detach();
	}

	smexit();
	fclose(fptr);
return 0;
}
/* END ================================================================= */


/* READS FILE TO SHARED MEMORY  ======================================== */
/* ===================================================================== */
void readtosm(FILE* in, int lcount)
{
	char line[lcount][20];
	int i = 0;
	int j = 0;

	while(fgets(line[i], lcount, in))
	{
		line[i][strlen(line[i]) - 1] = '\0';
		i++;
	}
	j = i;
	for(i = 0; i < j; ++i)
	{
		int fnum = atoi(line[i]);
		smseg->smints[i] = fnum;
		printf("smseg index %i: %i\n", i, smseg->smints[i]);
	}
	printf("\n");
}
/* END ================================================================= */


/* CREATES INPUT DATA FILE  ============================================ */
/* ===================================================================== */
int fpinit()
{
	int lower = 1;
	int upper = 256;
	int range = 64;
	/*
	srand(time(0));
	int range = (rand() % (100 - 64 + 1)) + 64;	
	*/

	FILE *fp = fopen(fin, "w");
	if(fp == NULL)
	{
		perror("\nmaster: error: failed to open input file");
		exit(EXIT_FAILURE);
	}

	srand(time(0));
	
	int i;
	for(i = 0; i < range; i++)
	{
		int n = (rand() % (upper - lower + 1)) + lower;
		fprintf(fp, "%i\n", n);
	}

	fclose(fp);
	return range;
}
/* END ================================================================= */


/* INITIATES SHARED MEMORY & SEMAPHORES ================================ */
/* ===================================================================== */
void sminit()
{
	key_t smkey = ftok("shmfile", 'a');
	if(smkey == -1)
	{
		perror("\nmaster: error: ftok failed");
		exit(EXIT_FAILURE);
	}

	sipcid = shmget(smkey, sizeof(shmem), 0600 | IPC_CREAT);
	if(sipcid == -1)
	{
		perror("\nmaster: error: failed to create shared memory");
		exit(EXIT_FAILURE);
	}

	smseg = (shmem*)shmat(sipcid,(void*)0, 0);

	if(smseg == (void*)-1)
	{
		perror("\nmaster: error: failed to attach shared memory");
		exit(EXIT_FAILURE);
	}

	if(sem_init(&(smseg->nsem), 1, 1) == -1)
	{
		perror("\nmaster: error: sem_init failed");
		smexit();
		exit(EXIT_FAILURE);
	}
	
	if(sem_init(&(smseg->lognsem), 1, 1) == -1)
	{
		perror("\nmaster: error: sem_init failed");
		smexit();
		exit(EXIT_FAILURE);
	}
}
/* END ================================================================= */


void smexit()
{
	shmdt(smseg);
	shmctl(sipcid, IPC_RMID, NULL);
}

void detach()
{
	smexit();
}
