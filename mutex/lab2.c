#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

typedef struct{
	pthread_mutex_t *mutex;
}
thread_args;

void *worker1(void *);
void *worker2(void *);

int main()
{
	int i;
	pthread_t thr[2];
	thread_args arg;
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex,NULL);
	arg.mutex=&mutex;

	pthread_create(&thr[0],NULL,worker1,&arg);
	pthread_create(&thr[1],NULL,worker2,&arg);
	for(i=0;i<2;i++)
		pthread_join(thr[i],NULL);
	return 0;
}

void *worker1(void *arg)
{
	thread_args *args=(thread_args *) arg;
	pthread_mutex_t *mutex=args->mutex;
	printf("thread 0 aquire lock\n");
	pthread_mutex_lock(mutex);
	printf("thread 0 sleep 5 sec..\n");
	sleep(5);
	printf("thread 0 wake up..\nthread 0 unlock\n");
	pthread_mutex_unlock(mutex);
	printf("thread 0 finish..\n");
	return NULL;
}

void *worker2(void *arg)
{
	thread_args *args=(thread_args *) arg;
	pthread_mutex_t *mutex=args->mutex;
	printf("thread 1 sleep 2 sec..\n");
	sleep(2);
	printf("thread 1 wake up\nthread 1 unlock\n");
	pthread_mutex_unlock(mutex);
	printf("thread 1 finish..\n");
	return NULL;
}
