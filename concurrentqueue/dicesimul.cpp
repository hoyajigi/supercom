#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<queue>
#include<time.h>

#define MAX_SIZE 2400

using namespace std;

queue<int> q;
pthread_mutex_t mutex,screenlock;

void *dice(void *arg)
{
	int i,tmp;
	for(i=0;i<1000;i++){
		tmp=rand()%6+1;
		pthread_mutex_lock(&mutex);
		q.push(tmp);
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

void *probabillity(void *arg)
{
	int dice[6];
	int i,tmp;
	for(i=0;i<6;i++)
		dice[i]=0;
	for(i=0;i<600;i++){
		pthread_mutex_lock(&mutex);
		tmp=q.front();
		q.pop();
		pthread_mutex_unlock(&mutex);
		dice[tmp-1]++;
	}
	pthread_mutex_lock(&screenlock);
	for(i=0;i<6;i++)
		printf("%d : %.3lf\n",i+1,(double)dice[i]/600);
	pthread_mutex_unlock(&screenlock);
	return NULL;
}

int main()
{
	pthread_t thr[5];
	int i;
	srand(time(NULL));
	pthread_mutex_init(&mutex,NULL);
	pthread_mutex_init(&screenlock,NULL);

	for(i=0;i<2;i++)
		pthread_create(&thr[i],NULL,dice,NULL);
	for(i=0;i<2;i++)
		pthread_join(thr[i],NULL);
	for(i=2;i<5;i++)
		pthread_create(&thr[i],NULL,probabillity,NULL);
	for(i=2;i<5;i++)
		pthread_join(thr[i],NULL);

	return 0;
}
