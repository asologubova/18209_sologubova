#define _CRT_SECURE_NO_WARNINGS
#include <omp.h>
#include <stdio.h> //printf
#include <stdlib.h>
#include <math.h>
#include <sys/times.h> //times
#include <unistd.h>  //sysconf

#define N 2500
#define Eps  0.00001

//metod minimalnih nevyazok

void mulScalar(double scalar, double *vec, double *res, int size) {
	for (int i = 0; i < size; i++) {
		res[i] = vec[i] * scalar;
	}
}

void minusVec(double *vec1, double *vec2, double *res, int size) {
	for (int i = 0; i < size; i++) {
		res[i] = vec1[i] - vec2[i];
	}
}


double scalarProduct(double *vec1, double *vec2, int size) {
	double res = 0;
	for (int i = 0; i < size; i++) {
		res += (vec1[i] * vec2[i]);
	}
	return res;
}
void matrixVecMul(double *matrix, double *vec, double *res, int size) {

//    	for (int i = 0; i < size; i++) {
//        	res[i] = scalarProduct1(&(matrix[size * i]), vec, size);
//	}

	for (int i = 0; i < size; i++){
		res[i] = 0;
		for (int j = 0; j < size; j++){
			res[i] += vec[j] * matrix[i * size + j];
		}
	}
}

void printVec(double *vec, int size) {
    	for (int i = 0; i < size; i++){
		printf("%f ", vec[i]);
    	}
    	printf("\n\n");
}

void printMatrix(double *mat, int size){
	for (int i = 0; i < size; i++){
		for (int j = 0; j < size; j++){
			printf("%lf ", mat[i* size + j]);
		}
		printf("\n\n");
	}
}

double vectorModule(double *vec, int size) {
	return sqrt(scalarProduct(vec, vec, size));
}

int main() {
    	//omp_set_num_threads(1);
//	FILE *inA = fopen("matA.bin", "rb");
//	FILE *inB = fopen("vecB.bin", "rb");
//	FILE *inX = fopen("vecX.bin", "rb");
	
    	struct tms start, end;
    	unsigned long long clocks_per_sec = sysconf(_SC_CLK_TCK);
    	unsigned long long clocks;

    	int size = 4;
	double E = Eps;
	double tau = 0.01;
	double *A = (double*)malloc(size * size * sizeof(double));
	double *x = (double*)malloc(size * sizeof(double));
	double *b = (double*)malloc(size * sizeof(double));
	double *y = (double*)malloc(size * sizeof(double));
	double *tmp = (double*)malloc(size * sizeof(double));

	int i = 0, j = 0;

	
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			if(i == j) A[i * size + j] = 2;
			else A[i * size + j] = 1;
		}
	}
	for (i = 0; i < size; i++) {
		x[i] = 0;
		tmp[i] = 0;
	}
	for (i = 0; i < size; i++) {
		b[i] = size + 1;
	}
/*
	fread(A, sizeof(double), size * size, inA);
	fread(b, sizeof(double), size, inB);	

	for (int i = 0; i < size; i++){
		x[i] = 0;
		tmp[i] = 0;
	}
*/
    	times(&start);	

    	matrixVecMul(A, x, y, size);  //y = Ax
    	printVec(y,size);
	minusVec(y, b, y, size);   //y = Ax - b
	printVec(y,size);
	
	int k = 0;
	double moduleB = vectorModule(b,size);
	double div = vectorModule(y, size)/moduleB;
    	printf("div = %lf\n\n", div);
	while (div >= E){
        	k++;
		printf("iteration %d:\n", k);
		matrixVecMul(A, y, tmp, size); //tmp = Ay
        	//printVec(tmp, size);
		
		int scal = scalarProduct(tmp, tmp, size);
		printf("scal = %d:\n", scal);
		if(scal == 0) 
			break;
        	tau = scalarProduct(tmp, y, size) / scalarProduct(tmp, tmp, size);
		printf("tau = %lf\n", tau);
		mulScalar(tau, y, tmp, size); //tmp = y * tau
		//printVec(tmp, size);	
		minusVec(x, tmp, x, size);  //x(n+1) = x(n) - y * tau
		//printVec(x, size);
		matrixVecMul(A, x, y, size);  //y = Ax(n+1)
		//printVec(y, size);
		minusVec(y, b, y, size);   //y = Ax(n+1) - b
		printf("y : ");
		printVec(y, size);

		double div = vectorModule(y, size)/moduleB;
    		printf("div = %lf\n\n", div);

	}

	times(&end);
	clocks = end.tms_utime - start.tms_utime;
    	printf("Time taken: %lf sec.\n", (double)clocks / clocks_per_sec);

//	fread(tmp, sizeof(double), size, inX);

/*	double difference = 0;
	for (int i = 0; i < size; i++){
		double d = abs(tmp[i] - x[i]);
		if (d > difference) difference = d;
		printf("x[%d] dif: %lf\n", i, d);
	}
*/
//	printVec(x, size);
//	printf("%lf\n", difference);

	free(A);
	free(x);
	free(b);
    	free(y);
	free(tmp);

//	fclose(inA);
//	fclose(inB);
//	fclose(inX);

    	return 0;
}
