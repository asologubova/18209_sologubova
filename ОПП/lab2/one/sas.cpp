#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <time.h> 

const int Nx=50,Ny=50;
int N;
float eps=0.000001;
float rez;

void printfromfile (float *buffer,float *B) {
	FILE *f=fopen("matA.bin","r");
	int l=fread(buffer,sizeof(float),N*N,f);
	fclose(f);
	f=fopen("vecB.bin","r");
	int k=fread(B,sizeof(float),N,f);
	fclose(f);
}

void matmul (float *Mat,float *V,float *R) {
	int i=0;
#pragma omp parallel for
	for ( i=0;i<N;++i) {
		R[i]=0;
		for (int j=0;j<N;++j) {
			R[i]+=(( *(Mat+i*N+j) ) * ( *(V+j) ));
		}
	}
}

float scal (float *A,float *B) {
	rez=0;
	int i=0;
	#pragma omp parallel for reduction(+:rez)
	for (i=0;i<N;++i) {
		rez+=A[i]*B[i];
	}
	return rez;
}

float mod (float *A) {
	return sqrt(scal(A,A));
}

void vectorsLin (float *A,float a,float *B,float b,float *R) {
	int i=0;
	#pragma omp parallel for
	for (i=0;i<N;++i) {
		R[i]=a*A[i]+b*B[i];
	}
}

int main (int argc,char *argv[]) {
	N=Nx*Ny;
	float* buffer = (float*)malloc(N * N * sizeof(float));
	float X[N],Y[N],AY[N];
	float B[N]; 
	printfromfile(buffer,B);
	for (int i=0;i<N;++i) {
		X[i]=0;
	}
	float lenB=mod(B);
	int count=0;
	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
while (1) {
		matmul(buffer,X,Y);
		vectorsLin(Y,1,B,-1,Y);
		float lenY=mod(Y);
		if (lenY/lenB<eps) {
			break;
		}
		matmul(buffer,Y,AY);	
		float r=scal(Y,AY)/scal(AY,AY);
		vectorsLin(X,1,Y,-r,X);
		count++;
		
	}
	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	float M[N];
	FILE *f=fopen("vecX.bin","rb");
	int k=fread(M,sizeof(float),N,f);
	fclose(f);

	float dif=-10;
	printf("%d\n",count);
	for (int i=0;i<N;++i) {
		if (abs(M[i]-X[i])>dif) dif=abs(M[i]-X[i]);
	}
	printf("\n%f",dif);

	printf("Time taken: %lf sec.\n",end.tv_sec-start.tv_sec+ 0.000000001*(end.tv_nsec-start.tv_nsec));
	free(buffer);
	return 0;
}

