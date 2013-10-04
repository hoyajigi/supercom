#include<stdio.h>
#include<sys/time.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<math.h>

#define N 10000000
int NUM_THREADS=1;

int a[N];

int print_data=0;
int validation=1;

double get_time(struct timeval tv_start,struct timeval tv_end)
{
	return (double)(tv_end.tv_sec-tv_start.tv_sec)+((double)tv_end.tv_usec-tv_start.tv_usec)/1000000.0;
}

void initialize()
{
	int i;
	for(i=0;i<N;i++)
		a[i]=rand();
}

double thread_func(void *arg)
{
	int i=0;
	double sum=0.0;
	
	for(i=0;i<N;i++){
		sum+=sqrt((double)(a[i]));
	}
	return sum;
}

void validate()
{
}

void print()
{
}

int main(int argc,char **argv)
{
//	int i;
	struct timeval tv_start;
	struct timeval tv_end;
	pthread_t *tids=NULL;
//	void *status;
//	int result;
	double sum;

	if(argc>1){
		NUM_THREADS=atoi(argv[1]);
	}
	printf("Using %d threads...\n",NUM_THREADS);

	tids=(pthread_t *)malloc(sizeof(pthread_t) * NUM_THREADS);
	if(tids==NULL){
		fprintf(stderr,"ERROR[%s:%d]\n",__FILE__,__LINE__);
		exit(EXIT_FAILURE);
	}

	initialize();

	gettimeofday(&tv_start,NULL);
/*	
	for(i=1;i<NUM_THREADS;i++){
		result=pthread_create(&tids[i],NULL,thread_func,(void *)i);
		if(result!=0){
			printf("Error(%d) occurred in pthread_create.\n",result);
			exit(EXIT_FAILURE);
		}
	}
*/	sum=thread_func(0);
/*	for(i=1;i<NUM_THREADS;i++){
		pthread_join(tids[i],&status);
	}
*/	gettimeofday(&tv_end,NULL);
	printf("-- Sequential Version ==\nsum: %.4lf\n\n",sum);
	printf("Execution with %d threads: %lf sec.\n",NUM_THREADS,get_time(tv_start,tv_end));
	if(validation){
		validate();
	}
	if(print_data){
	}
	return 0;
}
