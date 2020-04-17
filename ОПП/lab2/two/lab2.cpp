#define _CRT_SECURE_NO_WARNINGS
#include <omp.h>
#include <stdio.h> //printf
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define N 2500
#define Eps  0.000001

//metod minimalnih nevyazok

void printVec(float *vec, int size) {
    	for (int i = 0; i < size; i++){
		printf("%f ", vec[i]);
    	}
    	printf("\n");
}

int main() {
	
    	int size = N;
	float E = Eps;
	float tau = 0.01;
	float *A = (float*)malloc(size * size * sizeof(float));
	float x[size], b[size], y[size], Ay[size], tmp[size];

	FILE *inA = fopen("inData/matA.bin", "rb");
	FILE *inB = fopen("inData/vecB.bin", "rb");
	fread(A, sizeof(float), size * size, inA);
	fread(b, sizeof(float), size, inB);	
	fclose(inA);
	fclose(inB);

	int i = 0, j = 0;
	float lenY = 0, lenB = 0, res = 1;
	float scal1 = 0, scal2 = 0;

	for (i = 0; i < size; i++){
		x[i] = 0;
		lenB += (b[i] * b[i]);
	}
	lenB = sqrt(lenB);
 
	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);

	while (res >= E){
		#pragma omp parallel
		{
			#pragma omp single
			{
				scal1 = 0;
				scal2 = 0;
				tau = 0;
				lenY = 0;
			}
			#pragma omp for
			for (i = 0; i < size; i++){
				y[i] = 0;
				for (j = 0; j < size; j++){
					y[i] += (*(A + i * size + j)) * (*(x + j));
					//y[i] += A[i * size + j] * x[j];
				}
			}
			#pragma omp for
			for (i = 0; i < size; i++) {
				y[i] = y[i] - b[i];
			}
			#pragma omp for reduction(+: lenY)
			for (i = 0; i < size; i++) {
				lenY += (y[i] * y[i]);
			}
			#pragma omp for
			for (i = 0; i < size; i++){
				Ay[i] = 0;
				for (j = 0; j < size; j++){
					Ay[i] += (*(A + i * size + j)) * (*(y + j));
					//Ay[i] += A[i * size + j] * y[j];
				}
			}
			#pragma omp for reduction(+: scal1)
			for (i = 0; i < size; i++) {
				scal1 += (y[i] * Ay[i]);
			}
			#pragma omp single
			{
				scal1 = scal1;			
			}
			#pragma omp for reduction(+: scal2)
			for (i = 0; i < size; i++) {
				scal2 += (Ay[i] * Ay[i]);
			}
			#pragma omp single
			{
				tau = scal1 / scal2;			
			}
			#pragma omp for
			for (i = 0; i < size; i++) {
				x[i] = x[i] - tau * y[i];
			}
			#pragma omp single
			{
				lenY = sqrt(lenY);
				res = lenY/lenB;
			}
		}
	}

	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	printf("Time taken: %lf sec.\n",end.tv_sec-start.tv_sec+ 0.000000001*(end.tv_nsec-start.tv_nsec));

	FILE *inX = fopen("inData/vecX.bin", "rb");
	fread(tmp, sizeof(float), size, inX);
	fclose(inX);

/*	float difference = -1;
	for (i = 0; i < size; i++){
		double d = abs(tmp[i] - x[i]);
		if (d > difference) difference = d;
		//printf("x[%d] dif: %lf\n", i, d);
	}
	//printVec(x, size);
	printf("%lf\n", difference);
*/
	free(A);

    	return 0;
}


//заполнение мартицы и векторов начальными значениями
/*	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			if(i == j) A[i * size + j] = 2;
			else A[i * size + j] = 1;
		}
	}
	for (i = 0; i < size; i++) {
		x[i] = 0;
		//tmp[i] = 0;
	}
	for (i = 0; i < size; i++) {
		b[i] = size + 1;
	}
*/



