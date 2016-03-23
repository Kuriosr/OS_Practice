#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>

void *thread_function(void*);
pthread_mutex_t work_mutex;
sem_t bin_sem;

#define WORK_LENGTH	1024
char work_area[WORK_LENGTH];
int time_toexit = 0;

int main()
{
	printf("Input some text. Enter 'end' to finish\n");
	pthread_t a_thread;
	int res = pthread_mutex_init(&work_mutex, NULL);
	if (res != 0) {
		perror("Mutexx initialization failed.");
		exit(EXIT_FAILURE);
	}
	res = sem_init(&bin_sem, 0, 0);
	if (res != 0) {
		perror("Semaphore initialization failed");
		exit(EXIT_FAILURE);
	}
	res = pthread_create(&a_thread, NULL, thread_function, NULL);
	if (res != 0) {
		perror("Thread creation failed");
		exit(EXIT_FAILURE);
	}
	while(time_toexit != 1) {
		pthread_mutex_lock(&work_mutex);
		int sem_val = 0;
		sem_getvalue(&bin_sem, &sem_val);
		if (sem_val > 0) {
			//printf("sem_val = %d\n", sem_val);
			pthread_mutex_unlock(&work_mutex);
			continue;
		}
		//printf("~~~~~~~~~~~~~~~~~~~~~~~\n");
		fgets(work_area, WORK_LENGTH, stdin);
		sem_post(&bin_sem);
		pthread_mutex_unlock(&work_mutex);
	}
	pthread_mutex_unlock(&work_mutex);
	void *thread_result;
	res = pthread_join(a_thread, &thread_result);
	if (res != 0) {
		perror("Thread join failed.");
		exit(EXIT_FAILURE);
	}
	printf("Thread Joined Successful!!\n");
	pthread_mutex_destroy(&work_mutex);
	sem_destroy(&bin_sem);
	exit(EXIT_SUCCESS);
}

void *thread_function(void *arg)
{
	while(time_toexit != 1) {
		pthread_mutex_lock(&work_mutex);
		if (sem_trywait(&bin_sem) != 0) {
			pthread_mutex_unlock(&work_mutex);
			continue;
		}
		//sem_wait(&bin_sem);
		//printf("Thread : %d\n", strncmp(work_area, "end", 3));
		if (strncmp(work_area, "end", 3) == 0) {
			time_toexit = 1;
			work_area[0] = '\0';
		}
		else if (work_area[0] != '\0') {
			printf("You input %ld characters\n", strlen(work_area) -1);
			work_area[0] = '\0';
		}
		pthread_mutex_unlock(&work_mutex);
	}
	pthread_exit(0);
}
