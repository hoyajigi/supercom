#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

typedef struct{
	int *p;
	pthread_mutex_t *mutex;
}thread_args;

void *worker(void *);


int main()
{
	int i;
	int shared_var=0;
	pthread_t thr[4];
	thread_args arg;
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex,NULL);

	arg.p=&shared_var;
	arg.mutex=&mutex;

	for(i=0;i<4;i++){
		pthread_create(&thr[i],NULL,worker,&arg);
	}
	for(i=0;i<4;i++){
		pthread_join(thr[i],NULL);
	}

	printf("%d\n",shared_var);

	return 0;
}

void *worker(void * arg)
{
	int i;
	thread_args *args=(thread_args *) arg;
	int *p=args->p;
	pthread_mutex_t *mutex=args->mutex;
	for(i=0;i<1000;i++){
		pthread_mutex_lock(mutex);
		*p+=1;
		pthread_mutex_unlock(mutex);
	}
	return NULL;
}

