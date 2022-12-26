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

	struct timespec ts;
	struct sembuf unlock = {0, 1, 0};

	char str[SHM_SIZE];

	key_t key = ftok(SHM_NAME, 1); //generate an IPC key

	int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666); //create a new shared memory segment
	if (shmid == -1) {
		fprintf(stderr, "[first] shmget: %s(%d)\n", strerror(errno), errno);
		exit(-1);
	}

	int semid = semget(key, 1, IPC_CREAT | 0666);
	if (semid == -1) {
		fprintf(stderr, "[first] semget: %s(%d)\n", strerror(errno), errno);
		exit(-1);
	}


	char* shm_ptr = shmat(shmid, NULL, 0); //attach a shm segment identified by shmid
	if (!shm_ptr) {
		printf("[first] Memory attach error\n");
		exit(-1);
	}
	
	//check that the sending program was run only once
	struct shmid_ds buf;
	shmctl(shmid, IPC_STAT, &buf); //Copy information from the kernel data structure associated with shmid into the shmid_ds structure pointed to by buf.
	if(buf.shm_nattch > 1){
		printf("There is already a sending process\n");
		exit(0);
	}
	
	while(1) { 
		clock_gettime(CLOCK_REALTIME, &ts);
		struct tm* curr = localtime(&ts.tv_sec);
		double sec_ns = (double)curr->tm_sec + ((double)ts.tv_nsec / 1000000000.);
		sprintf(str, "[first] time: %2d:%2d:%.3lf pid: %d", curr->tm_hour, curr->tm_min, sec_ns, getpid());
		strcpy(shm_ptr, str);
		
		int op = semop(semid, &unlock, 1);
		if (op == -1) {
			fprintf(stderr, "[first] semop: %s(%d)\n", strerror(errno), errno);
		}
		sleep(5);
	}

	semctl(semid, 0, IPC_RMID);
	shmdt(shm_ptr); //detach shm segment
	shmctl(shmid, IPC_RMID, NULL); //mark the segment to be destroyed
	
	return 0;
}
