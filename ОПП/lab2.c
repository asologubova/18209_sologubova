#include <omp.h>
#include <stdio.h>


#define N 31400
#define Eps  0.00001

// Используем метод <...>

void mulScalar(double *res, double *vec, double scalar, int size) {
	#pragma omp parallel for
	for (int i = 0; i < size; i++) {
		result[i] = vec[i] * scalar;
	}
}

void minusVec(double *vec1, double *vec2, double *res, int size) {
	#pragma omp parallel for
	for (int i = 0; i < size; i++) {
		res[i] = vec1[i] - vec2[i];
	}
}

double scalarProduct(double *vec1, double *vec2, int size) {
	double res = 0;
    #pragma omp parallel for
	for (int i = 0; i < size; i++) {
		res = res + vec1[i] * vec2[i];
	}
	return res;
}

void matrixVecMul(double *A, double *vec, double *res, int size) {
	#pragma omp parallel for
    for (int i = 0; i < size; i++) {
        res[i] = scalarProduct(&(A[size * i]), vec, size);
	}
}

void printVec(double *vec, int size) {
    #pragma omp parallel for
    for (int i = 0; i < size; i++){
		printf("%f ", vec[i]);
    }
    printf("\n");
}

double vectorModule(double *vec, int size) {
	return sqrt(scalarProduct(vec, vec, size));
}

int main() {

}