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

double scalarProduct(float *vec1, float *vec2, int size) {
	double res = 0;
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
			res[i] = vec[i] * matrix[i * size + j];
		}
	}
}

void printVec(float *vec, int size) {
    	for (int i = 0; i < size; i++){
		printf("%f ", vec[i]);
    	}
    	printf("\n");
}

double vectorModule(float *vec, int size) {
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

	FILE *inA = fopen("matA.bin", "rb");
	FILE *inB = fopen("vecB.bin", "rb");
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

   	while ((vectorModule(y, size)/vectorModule(b, size)) >= E){
        	matrixVecMul(Ay, A, y, size); //Ay
        	if(scalarProduct(Ay, Ay, size) == 0) 
			break;
        	tau = scalarProduct(y, Ay, size) / scalarProduct(Ay, Ay, size);
		mulAndSumVectors(x, 1, x, (-1) * tau, y, size);
		matrixVecMul(y, A, x, size);  //y = Ax(n+1)
		minusVec(y, y, b, size);   //y = Ax(n+1) - b
	}

	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	printf("Time taken: %lf sec.\n",end.tv_sec-start.tv_sec+ 0.000000001*(end.tv_nsec-start.tv_nsec));

/*	FILE *inX = fopen("vecX.bin", "rb");
	fread(tmp, sizeof(float), size, inX);
	double difference = -1;
	for (i = 0; i < size; i++){
		double d = abs(tmp[i] - x[i]);
		if (d > difference) difference = d;
		//printf("x[%d] dif: %lf\n", i, d);
	}
	//printVec(x, size);
	printf("%lf\n", difference);
	fclose(inX);
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

