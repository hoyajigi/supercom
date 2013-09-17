#include<stdio.h>
#include"timers.h"
#define N 512
#define PRINT 1

int C[N][N];

int main()
{
	int A[N][N];
	int B[N][N];
	int i,j,k;
	int right=1;
	FILE *fA,*fB,*fC;
	fA=fopen("A","r");
	fB=fopen("B","r");
	for(i=0;i<N;i++)
		for(j=0;j<N;j++){
			fscanf(fA,"%d",&A[i][j]);
			fscanf(fB,"%d",&B[i][j]);
			C[i][j]=0;
		}
	timer_init();
	timer_start(1);
	for(i=0;i<N;i++)
		for(j=0;j<N;j++)
			for(k=0;k<N;k++)
				C[i][j]+=A[i][k]*B[k][j];
	timer_stop(1);
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			printf("%d ",C[i][j]);
		}
		printf("\n");
	}
	/*
	if(!PRINT){
		if(right)
			printf("%lf\n",timer_read(1));
		else
			printf("error");
	}*/
}
