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
int optr = 16;
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
int fpinit(int);
void reset();
void helpme();
void optset(int, char **);
void readsm(int);
int squarert(int);
void overlay(int, int);
/* END ================================================================= */


/* MAIN ================================================================ */
/* ===================================================================== */
int main(int argc, char *argv[])
{
	int parts = 0;
	int count = 0;
	int pairs = 0;
	int total = 0;		

	optset(argc, argv);
	if(optr < 16 || optr > 128)
	{
		printf("\nmaster: error: input file must contain between 16 and 128 integers. your specified: [%i] integers\n", optr);
		exit(EXIT_FAILURE);
	}
	if(optr % 2 != 0)
	{
		printf("\nmaster: warning: you specified an odd number of integers for your input file ([%i]). odd numbers may cause undefined behavior\n", optr);
		exit(EXIT_FAILURE);
	}

	printf("\nExecuting program computations on [%i] integers\n", optr);

	satime.sa_sigaction = killtime;
	sigemptyset(&satime.sa_mask);
	satime.sa_flags = 0;
	sigaction(SIGALRM, &satime, NULL);

	sactrl.sa_sigaction = killctrl;
	sigemptyset(&sactrl.sa_mask);
	sactrl.sa_flags = 0;
	sigaction(SIGINT, &sactrl, NULL);

	count = fpinit(optr);
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
	printf("master: time: starting %i/2 processes\n", optr);
	fprintf(fop, "\n\n\tPID\t\tIndex\t\tSize\t\tPair Positions\t\tPair Values\t\tResult Positions\t\tResult Values\n");
	fclose(fop);	

	/* set to execute on
 * 	   ly n/2 portion */
	smseg->execflag = 0;

	/* time */
	satimer();
	//start_t = clock();
	struct timeval tv1, tv2, tv3, tv4;
	gettimeofday(&tv1, NULL);

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
				printf("\n");	
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
						printf("\n");
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

	gettimeofday(&tv2, NULL);
	reset();
	
	index = 0;
	acount = 0; 
	ecount = 0; 
	pidpos = 0;	
	execap = 0;
	
	int groups = 0;
	int k;
	int g;

    	k = log2(total);
    	groups = (total / k);
	execap = groups;
	
	FILE* fpt = fopen(fon, "a");
	if(fpt == NULL)
	{
		perror("\nmaster: error: failed open output file");
		exit(EXIT_FAILURE);
	}
	printf("\nmaster: time: ending %i/2 processes\n", optr);
	fprintf(fpt, "Total time taken for n/2 processes: [%f] seconds\n", (double) (tv2.tv_usec - tv1.tv_usec) / 10000000 + (double) (tv2.tv_sec - tv1.tv_sec));
	fprintf(fpt, "----- ---- ----- --- --- ---------\n\n");
	printf("master: time: starting %i/log %i processes\n\n", optr, optr);
	fprintf(fpt, "\n\n\tLog of n/log(n) Processes");
	fprintf(fpt, "\n\t--- -- -------- ---------");
	fprintf(fpt, "\n\n\tPID\t\tIndex\t\tSize\t\tGroup Aggregate\t\tGroup Results\t\tGroup Sum\t\tFinal Sum\n");
	fclose(fpt);

	/* set to execute on
 * 	   ly n/logn set  */
	smseg->execflag = 1;
	
	gettimeofday(&tv3, NULL);

	while(k != 0)
	{ 
		count = k;
		smseg->icount = (k * groups);		

		for(g = 0; g < groups; g++)
		{
			index = (g * k);			

			pid_t c_pid;		
			if(acount < pcap)
			{
				c_pid = fork();

				if(c_pid == 0)
				{
					overlay(index, count);
				}	
				printf("\n");
				pids[pidpos] = c_pid;
				pidpos++;
				acount++;
			}			
			
			if((c_pid = waitpid((pid_t)-1, &status, 0)) > 0)
			{
				if(WIFEXITED(status))
				{		
					if(WEXITSTATUS(status) == 42)
					{	
						ecount++;
						acount--;
						printf("\n");
					}
				}
			}
		
			if(c_pid < 0)
			{
				perror("\nmaster: error: failed to fork");
				switch(errno)
				{
					case EAGAIN:
						perror("\nprocess limit");
					case ENOMEM:
						perror("\nout of memory");
	
				}
				exit(EXIT_FAILURE);
			}
		
			if(ecount == pcap && execap == 0)
			{
				break; 
			}
		}

		execap--;
		k--;

		if(k == 0)
		{
			break;
		}
		/*64,32,16,8,4...*/
		total  = total / 2;
		/* n/log n groups*/
		groups = total / k;
	}
	
	gettimeofday(&tv4, NULL);
	
	FILE* fpp = fopen(fon, "a");
	if(fpp == NULL)
	{
		perror("\nmaster: error: failed open output file");
		exit(EXIT_FAILURE);
	}
	printf("\nmaster: time: ending %i/log %i processes\n", optr, optr);
	fprintf(fpp, "Total time taken for n/log n processes: [%f] seconds\n", (double) (tv4.tv_usec - tv3.tv_usec) / 1000000 + (double) (tv4.tv_sec - tv3.tv_sec));
	fprintf(fpp, "----- ---- ----- --- ----- - ---------\n\n");
	fclose(fpp);

	shmdt(smseg);
	shmctl(sipcid, IPC_RMID, NULL); 
	moppingup();
	
	printf("Program exit success.\n\n");
	printf("Open log.out file for results\n\n");

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
int fpinit(int range)
{
	int lower = 1;
	int upper = 256;

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


/* KILLS ANY REMAINING PROCESSES & FREES MEMORY ======================== */
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
/* END ================================================================= */


/* KILLS ANY REMAINING PROCESSES ======================================= */
/* ===================================================================== */
void reset()
{
	int i;
	for(i = 0; i < pcap; i++)
	{
		if(pids[i] > 0)
		{
			kill(pids[i], SIGTERM);
		}
	}
}
/* END ================================================================= */


/* SETS TIMER ========================================================== */
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


/* SETS OPTIONS ======================================================== */
/* ===================================================================== */
void optset(int argc, char *argv[])
{
	int choice;
	while((choice = getopt(argc, argv, "hr:")) != -1)
	{
		switch(choice)
		{
			case 'h':
				helpme();
				exit(EXIT_SUCCESS);
			case 'r':
				optr = atoi(optarg);
				break;
			case '?':
				fprintf(stderr, "master: error: invalid argument\n");
				exit(EXIT_FAILURE);				
		}

	}
}
/* END ================================================================= */


/* SETS HELP ========================================================== */
/* ===================================================================== */
void helpme()
{
	printf("\n|HELP|MENU|\n\n");
    	printf("\t-h : display help menu\n");
	printf("\t-r : specify number of integers written to input file. default is 16\n\n");
}
/* END ================================================================= */
