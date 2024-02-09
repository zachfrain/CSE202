#include <stdio.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <time.h>

volatile int avg = 0; /* Global variables to hold the final average */

/* function to initialize an array of integers with random values */
void initialize(int*);

/* Wrapper function prototypes for the system calls */
void unix_error(const char *msg);
pid_t Fork();
pid_t Wait(int *status);
pid_t Waitpid(pid_t pid, int *status, int options);
int Sigqueue(pid_t pid, int signum, union sigval value);
int Sigemptyset(sigset_t *set);
int Sigfillset(sigset_t *set);
int Sigaction(int signum, const struct sigaction *new_act, struct sigaction *old_act);
int Sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
ssize_t Write(int d, const void *buffer, size_t nbytes);
typedef void handler_t;
handler_t *Signal(int signum, handler_t *handler);

/* Prototype of the handler */
void sigusr1_handler(int sig, siginfo_t *value, void *ucontext);

/* main function */
int main(){
  int A[N];
  pid_t children[P];
  initialize(A);
  /* install a portable handler for SIGUSR1 using the wrapper function Signal */
  Signal(SIGUSR1, (handler_t*)sigusr1_handler);
  /* print the message for installing SIGUSR1 handler */
  char buffer[256];
  sprintf(buffer, "Parent process %d installing SIGUSR1 handler\n",getppid());
  Write(1,buffer,strlen(buffer));
  /* create (P child processes) to calculate a partial average and send the signal SIGUSR1 to the parent*/
	pid_t pid;
	for(int i = 0; i < P; i++){
		if((pid = Fork()) == 0){
			int tempAvg = 0;
			int start = (i * N) / P;
                	int end = start + (N/P);
			char buffer1[256];
			sprintf(buffer1, "Child process %d finding the average from %d to %d\n", getpid(),start,end);
			Write(1,buffer1,strlen(buffer1));
			for(int j = start; j < end; ++j){ tempAvg += A[j]; }
			char buffer2[256];
			sprintf(buffer2, "Child process %d sending SIGUSR1 to parent process with the partial average %d\n",getpid(),tempAvg/(N/P));
			Write(1,buffer2,strlen(buffer2));
			
			union sigval value;
                        value.sival_int = tempAvg / (N/P);
			sleep(i);
			Sigqueue(getppid(), SIGUSR1, value);
			exit(0);
		}
		
	}
  /* reap the (P) children */
	int status;
 	for(int i = 0; i < P; i++){
		pid_t a = Wait(&status);
		if(WIFEXITED(status)){
			char buffer3[256];
			sprintf(buffer3, "Child process %d terminated normally with exit status %d\n",a,status);
			Write(1,buffer3,strlen(buffer3));
		} else {
			char buffer3[256];
                        sprintf(buffer3, "Child process %d terminated abnormally with exit status %d\n",a,status);
                        Write(1,buffer3,strlen(buffer3));
		}
	}
  /* print the array A if the macro TEST is defined */
#ifdef TEST
  printf("A = {");
  for(int i=0; i<N-1; i++){
    printf("%d, ", A[i]);
  }
  printf("%d}\n", A[N-1]);
#endif

  /* print the final average */
  char buffer4[256];
  sprintf(buffer4,"Average = %d\n",(avg/P));
  Write(1,buffer4,strlen(buffer4));
  
  return 0;
}
/* Definition of the function initialize */
void initialize(int M[N]){
    int i;
    srand(time(NULL));
    for(i=0; i<N; i++){
        M[i] = rand() % N;
    }
}
/* Define the Handler for SIGUSR1 here */
void sigusr1_handler(int sig, siginfo_t *value, void *ucontext){
    /* Follow the guidelines to write safe signal handlers*/
	char out[128];
	int temp = value->si_value.sival_ptr;
	avg = avg + temp;
	sprintf(out, "Parent process caught SIGUSR1 with partial average: %d\n", temp);
	Write(1,out,strlen(out));
}

/* Define the Wrapper functions for the system calls */
void unix_error(const char *msg){
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(0);
}

pid_t Fork(){
	pid_t pid;
	if((pid=fork()) < 0){ unix_error("fork error:"); }
	return pid;
}

pid_t Wait(int *status){
	pid_t child = wait(status);
	if(child == -1){ unix_error("wait error:"); }
	return child;
}

pid_t Waitpid(pid_t pid, int *status, int options){
	pid_t wpid;
	if((wpid = waitpid(pid,status,options)) < 0){ unix_error("waitpid error:"); }
	return wpid;
}

int Sigqueue(pid_t pid, int signum, const union sigval value){
	if(sigqueue(pid,signum,value) == -1){ 
		unix_error("sigqueue error:"); 
	}
	return 0;
}

int Sigemptyset(sigset_t *set){
	if(sigemptyset(set) == -1){ unix_error("sigemptyset error:"); }
	return 0;
}

int Sigfillset(sigset_t *set){
	if(sigfillset(set) == -1){ unix_error("sigfillset error:"); }
	return 0;
}

int Sigaction(int signum, const struct sigaction *new_act, struct sigaction *old_act){
	if(sigaction(signum, new_act, old_act) == -1){ unix_error("sigaction error:"); }
	return 0;
}

int Sigprocmask(int how, const sigset_t *set, sigset_t *oldset){
	if(sigprocmask(how,set,oldset) < 0){ unix_error("sigprocmask error:"); }
	return 0;
}

ssize_t Write(int d, const void *buffer, size_t nbytes){
	ssize_t cc;
	if((cc = write(d,buffer, nbytes)) < 0){ unix_error("write error:"); }
	return cc;
}

handler_t *Signal(int signum, handler_t *handler){
	struct sigaction action, old_action;
	action.sa_handler = handler;
	Sigemptyset(&action.sa_mask);
	action.sa_flags = SA_RESTART|SA_SIGINFO;
	if(Sigaction(signum, &action, &old_action) < 0){ unix_error("signal error:"); }
	return (old_action.sa_handler);
}
