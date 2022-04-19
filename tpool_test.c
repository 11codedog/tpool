#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#include "threadpool.h"

const size_t num_threads = 4;
int n, count = 0;
pthread_mutex_t lk = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;

void Tproducer() {
	while(1) {
		pthread_mutex_lock(&lk);
		while(!(count < n)) {
			pthread_cond_wait(&cv, &lk);
		}
		assert(count < n);
		printf("(");
		count++;
		pthread_cond_broadcast(&cv);
		pthread_mutex_unlock(&lk);
	}
}

void Tconsumer() {
	while(1) {
		pthread_mutex_lock(&lk);
		while(!(count > 0)) {
			pthread_cond_wait(&cv, &lk);
		}
		assert(count > 0);
		printf(")");
		count--;
		pthread_cond_broadcast(&cv);
		pthread_mutex_unlock(&lk);
	}
}

int main(int argc, char *argv[]) {
	setbuf(stdout, NULL);
	assert(argc == 2);
	n = atoi(argv[1]);
	tpool_t *tm;
	tm = tpool_create(4);
	for(int i = 0; i < 8; i++) {
		tpool_add_work(tm, Tconsumer, NULL);
		tpool_add_work(tm, Tproducer, NULL);
	}
	tpool_wait(tm);
	return 0;
}
