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
#include <sys/sem.h>

#define SHM_NAME "shm_fl"
#define SHM_SIZE 64

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	struct sembuf lock = {0, -1, 0};
	

	struct timespec ts;

	key_t key = ftok(SHM_NAME, 1);

	int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666); 
	if (shmid == -1) {
		fprintf(stderr, "[first] shmget: %s(%d)\n", strerror(errno), errno);
		exit(-1);
	}

	int semid = semget(key, 1, IPC_CREAT | 0666);
	if (semid == -1) {
		fprintf(stderr, "[first] semget: %s(%d)\n", strerror(errno), errno);
		exit(-1);
	}
	
	char* shm_ptr = shmat(shmid, NULL, 0);
	if (!shm_ptr) {
		printf("[second] Memory attach error\n");
		exit(-1);
	}

	
	clock_gettime(CLOCK_REALTIME, &ts);
	struct tm* curr = localtime(&ts.tv_sec);
	double sec_ns = (double)curr->tm_sec + ((double)ts.tv_nsec / 1000000000.);
	printf("[second] time: %2d:%2d:%.3lf pid: %d\n", curr->tm_hour, curr->tm_min, sec_ns, getpid());
	printf("%s", shm_ptr);
	
	int op = semop(semid, &lock, 1);
	if (op == -1) {
		fprintf(stderr, "[second] semop: %s(%d)\n", strerror(errno), errno);
	}
	
	shmdt(shm_ptr);	

	return 0;
}
