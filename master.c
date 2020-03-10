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
#include <math.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>

#include "shmem.h"
/* END ================================================================= */


/* GLOBAL VARIABLES ==================================================== */
/* ===================================================================== */
shmem* smseg; 
int sipcid = 0;
int pcap = 16;
int *pids;
char fin[] = "input.dat";
char fon[] = "log.out";
struct sigaction satime;
struct sigaction sactrl;
/* END ================================================================= */


/* FUNCTION PROTOTYPES ================================================= */
/* ===================================================================== */
static int satimer();
void killctrl(int, siginfo_t *, void *);
void killtime(int, siginfo_t *, void *);
void moppingup();
void sminit(); 
int fpinit();
void readsm(int);
int squarert(int);
int power(int,int);
void overlay(int, int);
//void forktwo();
/* END ================================================================= */


/* MAIN ================================================================ */
/* ===================================================================== */
int main(int argc, char *argv[])
{
	int parts = 0;
	int count = 0;
	int pairs = 0;
	int total = 0;

	satime.sa_sigaction = killtime;
	sigemptyset(&satime.sa_mask);
	satime.sa_flags = 0;
	sigaction(SIGALRM, &satime, NULL);

	sactrl.sa_sigaction = killctrl;
	sigemptyset(&sactrl.sa_mask);
	sactrl.sa_flags = 0;
	sigaction(SIGINT, &sactrl, NULL);

	count = fpinit();
	total = count;		
	sminit();
	readsm(count);
	parts = squarert(count);

	int n;
	int pass;
	int status;
	int index = 0;
	int acount = 0; 
	int ecount = 0; 
	int pidpos = 0;	
	int execap = 0;
	int remain = 0;


	int p, k, p1;
	p = 16;
	int r;
	int ii = 0;
	while(1)
	{
		ii++;
		r = (p / log(p));
		k = log(p);
		printf("%i groups of %i numbers each\n", r, k);
		p = r;
	
		if(k == 0)
		{
			break;
		}	   
	}
	/* allocate array of pids dynamically*/
	pids = (int*)calloc(pcap, sizeof(int));

	execap = parts;
	pairs = count / 2;
	
	/* write file header rows */
	FILE* fop = fopen(fon, "a");
	if(fop == NULL)
	{
		perror("\nmaster: error: failed open output file");
		exit(EXIT_FAILURE);
	}
	fprintf(fop, "\n\tLog of n/2 Processes");
	fprintf(fop, "\n\t--- -- --- ---------");
	fprintf(fop, "\n\n\tPID\t\tIndex\t\tSize\t\tPair Positions\t\tPair Values\t\tResult Positions\t\tResult Values\n");
	fclose(fop);

	/* time */
	satimer();
	/* loop and execute sqrt of count */
	for(pass = 1; pass <= parts; pass++)
	{ 
		index = 0;
		while(1)
		{
			pid_t cpid;		
			if(acount < pcap)
			{
				cpid = fork();
				/* a child */
				if(cpid == 0)
				{
					/* replace the curren
 * 					   t process image wi
 * 					   th a new process i
 * 					   mage. the new proc
 * 					   ess image overlays
 * 					   or runs another pr
 * 					   ogram from the cur
 * 					   rent program    */
					overlay(index, count);
				}	
				printf("child %i start\n", cpid);
				pids[pidpos] = cpid;
				pidpos++;
				acount++;
				
				if((index + 5) > pairs)
				{
					index = index + 1;						
				} else {
					index = index + 5;
				}
			}			
			
			/* waitpid() system call suspends execution of the ca
 * 			   lling process until a child specified by pid argum
 * 			   ent has changed state. greater than 0 means wait f
 * 			   or the child whose process ID is equal to he value
 * 			   of pid. my arguments make waitpid() like wait() */
			if((cpid = waitpid((pid_t)-1, &status, 0)) > 0)
			{
				/* evaluates to a non
 * 				   -zero value if sta
 * 				   tus was returned f
 * 				   or a child process
 * 				   that terminated no
 * 				   rmally          */
				if(WIFEXITED(status))
				{
					/* if the value of WIFEXITED
 * 					   is non-zero, this macro e
 * 					   valuates to the low-order
 * 					   8 bits of the status argu
 * 					   ment that the child proce
 * 					   ss pased to exti()     */		
					if(WEXITSTATUS(status) == 42)
					{	
						ecount++;
						acount--;
						printf("child %i dead\n", cpid);
					}
				}
			}
			/* fork fail
 * 			   ed out */		
			if(cpid < 0)
			{
				perror("\nmaster: error: failed to fork");
				switch(errno)
				{
					case EAGAIN:
						perror("\nprocess limit");
					case ENOMEM:
						perror("\nout of memory");
	
				}
				exit(EXIT_FAILURE);;
			}
		
			/* get out of dodge if exited chi
 * 			   ldren equals max processes all
 * 			   owed on machine or if execv ha
 * 			   s overlaid desired processes*/
			if(ecount == pcap && execap == 0)
			{
				break; 
			}
			/* if our index plus our num
 * 			   ber of pairs has either e
 * 			   qualed to or exceeded abo
 * 			   ve our count, our next mo
 * 			   ve is get out of dodge */
			if((index + pairs) >= count)
			{
				break;
			}
		}
		pairs = pairs / 2;
		count = count / 2;
		execap--;
		/* gets out of
 * 		   dodge if we
 * 		   fininshed c
 * 		   omputes  */
		if(count == 1)
		{
			break;
		}
	}

	shmdt(smseg);
	shmctl(sipcid, IPC_RMID, NULL); 
	moppingup();
return 0;
}
/* END ================================================================= */


/* OVERLAYS PROGRAM IMAGE WITH EXECV =================================== */
/* ===================================================================== */
void overlay(int index, int count)
{
	char xx[64]; 
	char yy[20];
					
	snprintf(xx, sizeof(xx), "%i", index);	
	snprintf(yy, sizeof(yy), "%i", count);
					
	char* fargs[] = {"./bin_addr", xx, yy, NULL};
	execv(fargs[0], fargs);

	/* master will not reach here unerred */	
	perror("\nmaster: error: exec failure");
	exit(EXIT_FAILURE);	
}
/* END ================================================================= */


/* SQUARES THE ROOT ==================================================== */
/* ===================================================================== */
int squarert(int n)
{
	float temp;
	float sqrt;
	int rvalue;	

	sqrt = n / 2;
	temp = 0;

	while(sqrt != temp)
	{	
		temp = sqrt;
		sqrt = (n / temp + temp) / 2;
	}
	rvalue = (int) sqrt;	
	
return rvalue;
}
/* END ================================================================= */


/* READS FILE TO SHARED MEMORY  ======================================== */
/* ===================================================================== */
void readsm(int count)
{
	char line[count][20];
	int i = 0;
	int j = 0;
	
	FILE *in = fopen(fin, "r");
	if(in == NULL)
	{
		perror("\nmaster: error: failed to read input file");
		exit(EXIT_FAILURE);
	}

	while(fgets(line[i], count, in))
	{
		line[i][strlen(line[i]) - 1] = '\0';
		i++;
	}
	
	j = i;
	for(i = 0; i < j; ++i)
	{
		int fnum = atoi(line[i]);
		smseg->smints[i] = fnum;
	}
	fclose(in);
}
/* END ================================================================= */


/* CREATES INPUT DATA FILE  ============================================ */
/* ===================================================================== */
int fpinit()
{
	int lower = 1;
	int upper = 256;
	int range = 16;

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
		exit(EXIT_FAILURE);
	}
	
	if(sem_init(&(smseg->lognsem), 1, 1) == -1)
	{
		perror("\nmaster: error: sem_init failed");
		exit(EXIT_FAILURE);
	}
}
/* END ================================================================= */


/* HANDLES SIGNALS ===================================================== */
/* ===================================================================== */
void killtime(int sig, siginfo_t *sainfo, void *ptr)
{
	char msgtime[] = "\nmaster: exit: computation exceeded 100 seconds\n";
	int msglentime = sizeof(msgtime);

	write(STDERR_FILENO, msgtime, msglentime);
	
	shmdt(smseg);
	shmctl(sipcid, IPC_RMID, NULL);

	int i;
	for(i = 0; i < pcap; i++)
	{
		if(pids[i] != 0)
		{
			if(kill(pids[i], SIGTERM) == -1)
			{
				perror("\nmaster: error: ");			
			}
		}
	}
	free(pids); 
	exit(EXIT_SUCCESS);			
}
/* END ================================================================= */


/* HANDLES SIGNALS ===================================================== */
/* ===================================================================== */
void killctrl(int sig, siginfo_t *sainfo, void *ptr)
{
	char msgctrl[] = "\nmaster: exit: received ctrl-c interrupt signal\n";
	int msglenctrl = sizeof(msgctrl);

	write(STDERR_FILENO, msgctrl, msglenctrl);
	
	shmdt(smseg);
	shmctl(sipcid, IPC_RMID, NULL);

	int i;
	for(i = 0; i < pcap; i++)
	{
		if(pids[i] != 0)
		{
			if(kill(pids[i], SIGTERM) == -1)
			{
				perror("\nmaster: error: ");
			}
		}
	}
	free(pids);
	exit(EXIT_SUCCESS);			
}
/* END ================================================================= */


/* KILLS ANY REMAINING PROCESSES ======================================= */
/* ===================================================================== */
void moppingup()
{
	int i;
	for(i = 0; i < pcap; i++)
	{
		if(pids[i] > 0)
		{
			kill(pids[i], SIGTERM);
		}
	}
	
	free(pids);
}

/* SETS TIMER ==== ===================================================== */
/* ===================================================================== */
static int satimer()
{
	struct itimerval t;
	t.it_value.tv_sec = 100;
	t.it_value.tv_usec = 0;
	t.it_interval.tv_sec = 0;
	t.it_interval.tv_usec = 0;
	
	return(setitimer(ITIMER_REAL, &t, NULL));
}
/* END ================================================================= */
