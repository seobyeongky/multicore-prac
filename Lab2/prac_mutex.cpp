#include <stdio.h>
#include <pthread.h>

#define NUM_THREAD      10
#define NUM_INCREASE	1000000

int g_cnt_global = 0;
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

void *ThreadFunc(void *arg) {
	long cnt_local = 0;

	for (int i = 0; i < NUM_INCREASE; i++) {
        pthread_mutex_lock(&g_mutex);
		g_cnt_global++;
        pthread_mutex_unlock(&g_mutex);
		cnt_local++;
	}

	return (void*)cnt_local;
}

int main(void) {
	pthread_t threads[NUM_THREAD];

	// create threads
	for (int i = 0; i < NUM_THREAD; i++) {
		if (pthread_create(&threads[i], 0, ThreadFunc, NULL) < 0) {
			printf("pthread_create error!\n");
			return 0;
		}
	}

	// wait threads end
	long ret;
	for (int i = 0; i < NUM_THREAD; i++) {
		pthread_join(threads[i], (void**)&ret);
		printf("thread %lu, local count: %lu\n", threads[i], ret);
	}

	printf("global count: %d\n", g_cnt_global);
	
	return 0;
}
