#include<stdio.h>
#include<sys/time.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>

#define NDIM 4096
int NUM_THREADS=4;

double A[NDIM][NDIM];
double x[NDIM],y[NDIM];

int print_data=0;
int validation=1;

double get_time(struct timeval tv_start,struct timeval tv_end)
{
	return (double)(tv_end.tv_sec-tv_start.tv_sec)+((double)tv_end.tv_usec-tv_start.tv_usec)/1000000.0;
}

void initialize()
{
	int i,j;
	for(i=0;i<NDIM;i++){
		for(j=0;j<NDIM;j++){
			A[i][j]=(double)(i+j)/10.0;
		}
		x[i]=(double)(rand()%100)/10.0;
		y[i]=0.0;
	}
}

void mat_vec_mul()
{
	int i,j,n=NDIM;
	for(i=0;i<n;i++){
		double sum=0.0;
		for(j=0;j<n;j++){
			sum+=A[i][j]*x[j];
		}
		y[i]=sum;
	}
}

void *thread_func(void *arg)
{
	int i,j,n=NDIM;
	int id=(int)(size_t)arg;
	int chunk=n/NUM_THREADS;
	int start=chunk*id;
	int end=chunk*(id+1);

	if(id==NUM_THREADS-1)
		end=n;

	for(i=start;i<end;i++){
		double sum=0.0;
		for(j=0;j<n;j++){
			sum+=A[i][j]*x[j];
		}
		y[i]=sum;
	}
	return 0;
}

void validate()
{
}

void print_mat()
{
}

int main(int argc,char **argv)
{
	size_t i;
	struct timeval tv_start;
	struct timeval tv_end;
	pthread_t *tids=NULL;
	void *status;
	int result;

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
	
	for(i=1;i<NUM_THREADS;i++){
		result=pthread_create(&tids[i],NULL,thread_func,(void *)i);
		if(result!=0){
			printf("Error(%d) occurred in pthread_create.\n",result);
			exit(EXIT_FAILURE);
		}
	}
	thread_func(0);
	for(i=1;i<NUM_THREADS;i++){
		pthread_join(tids[i],&status);
	}
	gettimeofday(&tv_end,NULL);
	printf("Execution with %d threads: %lf sec.\n",NUM_THREADS,get_time(tv_start,tv_end));
	if(validation){
		validate();
	}
	if(print_data){
	}
	return 0;
}
