#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/mman.h>

#define SHM_NAME "shm_fl"
#define SHM_SIZE 64

typedef struct data{
	char str[SHM_SIZE];
} data;

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	struct tm* curr = localtime(&ts.tv_sec);
	double sec_ns = (double)curr->tm_sec + ((double)ts.tv_nsec / 1000000000.);
	printf("[second] time: %2d:%2d:%.3lf pid: %d\n", curr->tm_hour, curr->tm_min, sec_ns, getpid());

	key_t shm_key = ftok(SHM_NAME, 1);
	int shmid = shmget(shm_key, SHM_SIZE, IPC_CREAT | 0666); 
	if (shmid == -1) {
		printf("[second] Memory get error\n");
		exit(-1);
	}
	
	struct data* shm_ptr = shmat(shmid, NULL, 0);
	if (!shm_ptr) {
		printf("[second] Memory attach error\n");
		exit(-1);
	}
   
	printf("%s", shm_ptr->str);

	shmdt(shm_ptr);	

	return 0;
}
