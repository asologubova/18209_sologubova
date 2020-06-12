#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h> //printf, fseek
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

#define N 2500
#define Eps  0.00001

//metod minimalnih nevyazok

void mulScalar(float *res, float scalar, float *vec, int size){
	int i = 0;
	for (i = 0; i < size; i++) {
		res[i] = vec[i] * scalar;
	}
}

void minusVec(float *res, float *vec1, float *vec2, int size) {
	int i = 0;
	for (i = 0; i < size; i++) {
		res[i] = vec1[i] - vec2[i];
	}
}

void sumVec(float *res, float *vec1, float *vec2, int size) {
	int i = 0;
	for (i = 0; i < size; i++) {
		res[i] = vec1[i] + vec2[i];
	}
}

float scalarProduct(float *vec1, float *vec2, int size) {
	float res = 0;
	int i = 0;
	for (i = 0; i < size; i++) {
		res += (vec1[i] * vec2[i]);
	}
	return res;
}

void matrixVecMul(float *res, float *matrix, float *vec, 
			int size, int rank, int count) {
	int i = 0, j = 0;
	for (i = 0; i < count; i++){
		res[i] = 0;
		for (j = 0; j < size; j++){
			res[i] += vec[j] * matrix[i * size + j];
		}
	}
}

void mulAndSumVectors(float *res, float scal1, float *vec1, float scal2, 
			float *vec2, int size){
	int i;
	for (i = 0; i < size; i++){
		res[i] = scal1 * vec1[i] + scal2 * vec2[i];
	}
}

void fullData(float *A, float *b, float *x, int size){
	FILE *inA = fopen("inData/matA.bin", "rb");
	FILE *inB = fopen("inData/vecB.bin", "rb");

	fread(A, sizeof(float), size * size, inA);
	fread(b, sizeof(float), size, inB);	

	fclose(inA);
	fclose(inB);

	int i;
	for (i = 0; i < size; i++){
		x[i] = 0;
	}
}

void printVec(float *vec, int size){
	int i;
	for (i = 0; i< size; i++){
		printf("%f ", vec[i]);
	}
	printf("\n");
}

void printMat(float *mat, int row, int col){
	int i, j;
	for (i = 0; i < row; i++){
		for (j = 0; j < col; j++){
			printf("%f ", mat[i * col + j]);
		}
		printf("\n");
	}
	printf("\n");
}

int main() {

	int size = N;
	float E = Eps;

	//MPI:
	int rank, commSize;
  	MPI_Init(NULL, NULL);
  	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  	MPI_Comm_size(MPI_COMM_WORLD, &commSize);
	int count = size / commSize;
	int remainder = size % commSize;
	MPI_Status status;

	int i = 0, r = 1, j = 0;
	double time_start, time_end;
	
	float *A;
	float *A_part;
	float *x = (float*)malloc(sizeof(float) * size);
	float *b = (float*)malloc(sizeof(float) * size);
	float *y = (float*)malloc(sizeof(float) * size);

	int *countsMat = (int*)malloc(sizeof(int) * commSize);
	int *offsetsMat = (int*)malloc(sizeof(int) * commSize);
	int *countsVec = (int*)malloc(sizeof(int) * commSize);
	int *offsetsVec = (int*)malloc(sizeof(int) * commSize);

	int offsetMat = 0, offsetVec = 0;
	for (i = 0, r = 1; i < commSize; i++, r++){
		countsMat[i] = (count + (r <= remainder ? 1 : 0)) * size;
		countsVec[i] = count + (r <= remainder ? 1 : 0);

		offsetsMat[i] = offsetMat;
		offsetMat += countsMat[i];
		offsetsVec[i] = offsetVec;
		offsetVec += countsVec[i];
	}
	A_part = (float*)malloc(sizeof(float) * countsMat[rank]);

	if (rank == 0){
		A = (float*)malloc(sizeof(float) * size * size);
		fullData(A, b, x, size);
		/*for (i = 0; i < size; i++){
			for (j = 0; j < size; j++){
				A[i * size + j] = (i == j) ? 2 : 1;
			}
			b[i] = size + 1;
			x[i] = 0;
		}*/
	}

	MPI_Bcast(b, size, MPI_FLOAT, 0, MPI_COMM_WORLD);
	MPI_Bcast(x, size, MPI_FLOAT, 0, MPI_COMM_WORLD);

	MPI_Scatterv(A, countsMat, offsetsMat, MPI_FLOAT, A_part, countsMat[rank], MPI_FLOAT, 0, MPI_COMM_WORLD);

	if (rank == 0){
		time_start = MPI_Wtime();
	}

	float *tmp = (float*)malloc(sizeof(float) * countsVec[rank]);
	int k = 0;
	while(1){
		k++;
		matrixVecMul(tmp, A_part, x, size, rank, countsVec[rank]);  //y = Ax 
		minusVec(tmp, tmp, b + offsetsVec[rank], countsVec[rank]);  //y = Ax - b

		MPI_Allgatherv(tmp, countsVec[rank], MPI_FLOAT, y, countsVec, offsetsVec, MPI_FLOAT, MPI_COMM_WORLD);
		float sumY = 0, sumB = 0;
		float scalY = scalarProduct(tmp, tmp, countsVec[rank]);
		float scalB = scalarProduct(b + offsetsVec[rank], b + offsetsVec[rank], countsVec[rank]);

		MPI_Allreduce(&scalY, &sumY, 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
		MPI_Allreduce(&scalB, &sumB, 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
		float cond = sqrt(sumY / sumB);

		if (cond < E){
			break;
		}
		matrixVecMul(tmp, A_part, y, size, rank, countsVec[rank]);  //Ay
		float scalAy = scalarProduct(tmp, tmp, countsVec[rank]);
		float sumAyAy = 0, sumYAy = 0;
		MPI_Allreduce(&scalAy, &sumAyAy, 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
		
		if(sumAyAy == 0){
			printf("Denominator is NULL");
			break;
		}
		float scalYAy = scalarProduct(tmp, y + offsetsVec[rank], countsVec[rank]);

		MPI_Allreduce(&scalYAy, &sumYAy, 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
		float tau = sumYAy / sumAyAy;

		mulAndSumVectors(tmp, 1, x + offsetsVec[rank], (-1) * tau, y + offsetsVec[rank], countsVec[rank]);  //x(n+1) = x - tau *  y;
		MPI_Allgatherv(tmp, countsVec[rank], MPI_FLOAT, x, countsVec, offsetsVec, MPI_FLOAT, MPI_COMM_WORLD);
	}

	if (rank == 0){
		time_end = MPI_Wtime();
		double worktime = time_end - time_start;
		printf("Time: %lf sec\n", worktime);

		float *fileX = (float*)malloc(sizeof(float) * size);
		FILE *inX = fopen("inData/vecX.bin", "rb");
		fread(fileX, sizeof(float), size, inX);
		fclose(inX);

		float difference = -1;
		for (i = 0; i < size; i++){
			float d = abs(fileX[i] - x[i]);
			if (d > difference) difference = d;
			//printf("x[%d] dif: %lf\n", i, d);
		}
		//printVec(x, size);
		printf("difference = %lf\n", difference);

		free(fileX);
		free(A);
	}

	free(A_part);
	free(x);
	free(b);
	free(y);
	free(tmp);
	free(countsMat);
	free(countsVec);
	free(offsetsMat);
	free(offsetsVec);
	
	MPI_Finalize();

	return 0;
}