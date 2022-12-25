#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <pthread.h> 
#include <fcntl.h>
#include <stdlib.h>


pthread_mutex_t mutex;
pthread_cond_t cond;
int count = 0;

void* writer(void* args) {
    (void)args;
    for(int i = 0; i < 10; ++i) {
	    pthread_mutex_lock(&mutex);
	    ++count;
	    sleep(4);
	    pthread_cond_broadcast(&cond);
	    pthread_mutex_unlock(&mutex);
	    sleep(4);
    }
    pthread_exit(0);
}

void* reader(void* args) {
	(void)args;
	for(int i = 0; i < 10; ++i) {
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&cond, &mutex);
		printf("counter: %d tid: %ld\n", count, pthread_self());
		sleep(2);
		pthread_mutex_unlock(&mutex);
		sleep(5); 
	}
	pthread_exit(0);
}

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;
	int arrs = 10;
	pthread_t w_tid;
	pthread_t r_tid[arrs];
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	pthread_create(&w_tid, NULL, writer, NULL);
	for(int i = 0; i < arrs; ++i) {
		pthread_create(&r_tid[i], NULL, reader, NULL);
	}

	pthread_join(w_tid, NULL);
	for(int i = 0; i < arrs; ++i) {
		pthread_join(r_tid[i], NULL);
	}
    pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);
	return 0;
}
