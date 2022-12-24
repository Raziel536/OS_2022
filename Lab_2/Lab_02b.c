#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

void exitFunc();

int main(int argc, char** argv) {
	(void) argc;
	(void) argv;
    int check = mkfifo("./fifo_file", 0777);
	if (check == -1) {
		int error = errno;
		fprintf(stderr, "Error %d: %s", error, strerror(error));
		exit(1);
	}
	int fd;
	struct timespec ts;
	struct tm* curr;
	pid_t pid = fork();
	atexit(exitFunc);
    if(pid > 0) {
             clock_gettime(CLOCK_REALTIME, &ts);
		struct tm* curr = localtime(&ts.tv_sec);
		double sec_ns = (double)curr->tm_sec + ((double)ts.tv_nsec / 1000000000.);
		char str[256];
		sprintf(str, "[PARENT] {%2d:%2d:%.5lf} pid = %d", curr->tm_hour, curr->tm_min, sec_ns, getpid());
		
		fd = open("./fifo_file", O_WRONLY);
		write(fd, (void*) str, sizeof(str));
		close(fd);
		unlink("./fifo_file");
        
    } else {
        sleep(10);
		clock_gettime(CLOCK_REALTIME, &ts);
		struct tm* curr = localtime(&ts.tv_sec);
		double sec_ns = (double)curr->tm_sec + ((double)ts.tv_nsec / 1000000000.);
		char buf[256];
		int len;
		printf("CHILD {%2d:%2d:%.5lf}: ", curr->tm_hour, curr->tm_min, sec_ns);
		
		fd = open("./fifo_file", O_RDONLY);
		if ((len = read(fd, buf, 256)) != 0) 
			printf("%s\n", buf);
        close(fd);
    }
    return 0;
}


void exitFunc() {
	printf("Exit from %d\n", getpid());
}