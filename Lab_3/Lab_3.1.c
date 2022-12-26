#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

#define SHM_NAME "smn_fl"
#define SHM_SIZE 64

typedef struct data {
	char str[SHM_SIZE];
} data;

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	struct data data;
	char str[SHM_SIZE];
	char str1[SHM_SIZE];

	key_t shm_key = ftok(SHM_NAME, 1); 
	int shmid = shmget(shm_key, SHM_SIZE, IPC_CREAT | 0666); 
	if (shmid == -1) {
		printf("[first] Memory get error\n");
		exit(-1);
	}

	struct data* shm_ptr = shmat(shmid, NULL, 0); 
	if (!shm_ptr) {
		printf("[first] Memory attach error\n");
		exit(-1);
	}
	
	struct shmid_ds buf;
	shmctl(shmid, IPC_STAT, &buf); 
	if(buf.shm_nattch > 1){
		printf("There is already a sending process\n");
		exit(0);
	}
	
	while(1) { 
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		struct tm* curr = localtime(&ts.tv_sec);
		double sec_ns = (double)curr->tm_sec + ((double)ts.tv_nsec / 1000000000.);
		strftime(str, SHM_SIZE, "[first] time: %X.", curr);
		snprintf(str1, SHM_SIZE, "%.3lf pid: %d\n", sec_ns, getpid());
		strcat(str, str1);
		strncpy(data.str, str, SHM_SIZE);
		memcpy(shm_ptr, &data, sizeof(struct data));
		sleep(5);
	}
	shmdt(shm_ptr); 
	shmctl(shmid, IPC_RMID, NULL);
	return 0;
}