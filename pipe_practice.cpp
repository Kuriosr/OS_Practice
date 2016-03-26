#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LEN 1024

int main()
{
	char some_data[MAX_LEN] = "kfdjakfjkdf";
	int file_pipes[2];				//file descriptor
	if (pipe(file_pipes) == 0) {
		pid_t fork_result = fork();
		printf("file descriptor = %d, %d\n", file_pipes[0], file_pipes[1]);
		if (fork_result == 0) {
			close(0);
			int child_newdescriptor = dup(file_pipes[0]); // stdin is point to the file_pipes[0]
			printf("child new descriptor = %d\n", child_newdescriptor);
			close(file_pipes[0]);
			close(file_pipes[1]);
			while(read(child_newdescriptor, some_data, MAX_LEN) > 0) {
				printf("%s\n", some_data);
			}
		}
		else {
			close(file_pipes[0]);
			while(scanf("%s", some_data)) {
				write(file_pipes[1], some_data, strlen(some_data) + 1);
			}
			close(file_pipes[1]);
		}
	}
	exit(EXIT_SUCCESS);
}

