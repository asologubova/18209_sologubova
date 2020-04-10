#define _CRT_SECURE_NO_WARNINGS
#include <omp.h>
#include <stdio.h> //printf
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define N 2500
#define Eps  0.000001

//metod minimalnih nevyazok

void mulScalar(float *res, float scalar, float *vec, int size) {
	int i = 0;
	#pragma omp parallel for 
	for (i = 0; i < size; i++) {
		res[i] = vec[i] * scalar;
	}
}

void minusVec(float *res, float *vec1, float *vec2, int size) {
	int i = 0;
	#pragma omp parallel for
	for (i = 0; i < size; i++) {
		res[i] = vec1[i] - vec2[i];
	}
}

void sumVec(float *res, float *vec1, float *vec2, int size) {
	int i = 0;
	#pragma omp parallel for
	for (i = 0; i < size; i++) {
		res[i] = vec1[i] + vec2[i];
	}
}

float scalarProduct(float *vec1, float *vec2, int size) {
	float res = 0;
	int i = 0;
	#pragma omp parallel for reduction(+: res)
	for (i = 0; i < size; i++) {
		res += (vec1[i] * vec2[i]);
	}
	return res;
}

void matrixVecMul(float *res, float *matrix, float *vec, int size) {
	int i = 0, j = 0;
	#pragma omp parallel for
	for (i = 0; i < size; i++){
		res[i] = 0;
		for (j = 0; j < size; j++){
			//res[i] += (*(vec + j)) * (*(matrix + i * size + j));
			res[i] += vec[j] * matrix[i * size + j];
		}
	}
}

void printVec(float *vec, int size) {
    	for (int i = 0; i < size; i++){
		printf("%f ", vec[i]);
    	}
    	printf("\n");
}

float vectorModule(float *vec, int size) {
	return sqrt(scalarProduct(vec, vec, size));
}

void mulAndSumVectors(float *res, float scal1, float *vec1, float scal2, float *vec2, int size){
	int i = 0;
	#pragma omp parallel for
	for (i = 0; i < size; i++){
		res[i] = scal1 * vec1[i] + scal2 * vec2[i];
	}
}

int main() {

    	int size = N;
	float E = Eps;
	float tau = 0.01;
	float alpha = 0, betta = 0;
	float *A = (float*)malloc(size * size * sizeof(float));
	float x[size], b[size], y[size], Ay[size], tmp[size];

	int i = 0, j = 0;

	FILE *inA = fopen("inData/matA.bin", "rb");
	FILE *inB = fopen("inData/vecB.bin", "rb");
	fread(A, sizeof(float), size * size, inA);
	fread(b, sizeof(float), size, inB);	
	fclose(inA);
	fclose(inB);
	for (i = 0; i < size; i++){
		x[i] = 0;
	}

	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    	matrixVecMul(y, A, x, size);  //y = Ax
	minusVec(y, y, b, size);   //y = Ax - b	
	float lenY = vectorModule(y, size);
	float lenB = vectorModule(b, size);

   	while (lenY/lenB >= E){
        	matrixVecMul(Ay, A, y, size); //Ay
		float scalAy = scalarProduct(Ay, Ay, size);
        	if(scalAy  == 0) 
			break;
        	tau = scalarProduct(y, Ay, size) / scalAy;
		mulAndSumVectors(x, 1, x, (-1) * tau, y, size);  //x(n+1) = x - tau *  y;
		matrixVecMul(y, A, x, size);  //y = Ax(n+1)
		minusVec(y, y, b, size);   //y = Ax(n+1) - b
		lenY = vectorModule(y, size);
	}

	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	printf("Time taken: %lf sec.\n",end.tv_sec-start.tv_sec+ 0.000000001*(end.tv_nsec-start.tv_nsec));

	FILE *inX = fopen("inData/vecX.bin", "rb");
	fread(tmp, sizeof(float), size, inX);
	fclose(inX);
	float difference = -1;
	for (i = 0; i < size; i++){
		float d = abs(tmp[i] - x[i]);
		if (d > difference) difference = d;
		//printf("x[%d] dif: %lf\n", i, d);
	}
	//printVec(x, size);
	printf("difference = %lf\n", difference);

	free(A);

    	return 0;
}


//���������� ������� � �������� ���������� ����������
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

