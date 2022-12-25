#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <pthread.h> 
#include <fcntl.h>
#include <stdlib.h>

pthread_rwlock_t rwl;
int count = 0;

void* writer(void* args) {
    (void)args;
    for(int i=0; i < 10; ++i) {
        pthread_rwlock_wrlock(&rwl);
	    ++count;
	    sleep(2);
	    pthread_rwlock_unlock(&rwl);
	    sleep(2);
    }
    pthread_exit(0);
}

void* reader(void* args) {
	(void)args;
	for(int i=0; i < 10; ++i) {
		pthread_rwlock_rdlock(&rwl);
		printf("counter: %d tid: %ld\n", count, pthread_self());
		sleep(2);
		pthread_rwlock_unlock(&rwl);
		sleep(2); 
	}
	pthread_exit(0);
}

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;
	int arrs = 10;
	pthread_t w_tid;
	pthread_t r_tid[arrs];
	pthread_rwlock_init(&rwl, NULL);

	pthread_create(&w_tid, NULL, writer, NULL);
	for(int i = 0; i < arrs; ++i) {
		pthread_create(&r_tid[i], NULL, reader, NULL);
	}

	pthread_join(w_tid, NULL);
	for(int i = 0; i < arrs; ++i) {
		pthread_join(r_tid[i], NULL);
	}
    pthread_rwlock_destroy(&rwl);
	return 0;
}
