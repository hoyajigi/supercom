#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
#include<time.h>

int main(int arg,char *args[])
{
	int *a;
	int n,i,j;
	srand(time(NULL));
	if(arg<2){
		printf("n을 넣으라고 ㅅㅂ\n");
		return;
	}
	n=atoi(args[1]);
	a=(int *)malloc(sizeof(int)*n*n);
	for(i=0;i<n;i++){
		for(j=0;j<n;j++){
			*(a+i*n+j)=rand()%10;
			printf("%d ",*(a+i*n+j));
		}
		printf("\n");
	}
}
