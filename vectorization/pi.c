#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#define N 100000000

//float x[N],y[N];
int size;
/*
void initialize()
{
	int i;
	for(i=0;i<N;i++){
		x[i]=(float)rand()/RAND_MAX;
		y[i]=(float)rand()/RAND_MAX;
		//printf("%f ",x[i]);
	}
}
*/
float get_phi()
{
	int i;
	double x1,y1;
	int in_circle=0;
	for(i=0;i<size;i++){
		x1=(float)rand()/RAND_MAX;
		y1=(float)rand()/RAND_MAX;
		if(x1*x1+y1*y1<1.0f)
			in_circle++;
		/*
		if(x[i]*x[i]+y[i]*y[i]<1.0f){
			in_circle++;
		//	printf("%f\n",x[i]*x[i]+y[i]*y[i]);
		}*/
	}
//	return in_circle;
	return 4*(float)in_circle/(float)size;
}

int main()
{
	size=N;
	srand(time(NULL));
//	initialize();
	printf("%f\n",get_phi());
	return 0;
}
