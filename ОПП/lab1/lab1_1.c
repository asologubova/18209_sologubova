#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h> //printf, fseek
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

#define N 2500
#define Eps  0.00001

//metod minimalnih nevyazok

void mulScalar(float *res, float scalar, float *vec, int size)
{
	int i = 0;
	for (i = 0; i < size; i++) {
		res[i] = vec[i] * scalar;
	}
}

void minusVec(float *res, float *vec1, float *vec2, int size) 
{
	int i = 0;
	for (i = 0; i < size; i++) {
		res[i] = vec1[i] - vec2[i];
	}
}

void sumVec(float *res, float *vec1, float *vec2, int size) 
{
	int i = 0;
	for (i = 0; i < size; i++) {
		res[i] = vec1[i] + vec2[i];
	}
}

float scalarProduct(float *vec1, float *vec2, int size) 
{
	float res = 0;
	int i = 0;
	for (i = 0; i < size; i++) {
		res += (vec1[i] * vec2[i]);
	}
	return res;
}

void matrixVecMul(float *res, float *matrix, float *vec, 
			int size, int rank, int shift) 
{
	int i = 0, j = 0;
	for (i = 0; i < shift; i++){
		res[i] = 0;
		for (j = 0; j < size; j++){
			res[i] += vec[j] * matrix[i * size + j];
		}
	}
}

void mulAndSumVectors(float *res, float scal1, float *vec1, float scal2, 
			float *vec2, int size)
{
	int i;
	for (i = 0; i < size; i++){
		res[i] = scal1 * vec1[i] + scal2 * vec2[i];
	}
}

void fullData(float *A, float *b, float *x, int size)
{
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

int countShift(int *shifts, int rank){
	int i;
	int res = 0;
	for (i = 0; i < rank; i++){
		res += shifts[i];
	}
	return res;
}

void printVec(float *vec, int size){
	int i;
	for (i = 0; i< size; i++){
		printf("%f ", vec[i]);
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
	int shift = size / commSize;
	int remainder = size % commSize;
	MPI_Status status;

	int i = 0, r = 1, j = 0;
	double time_begin, time_end;

	int *shifts = (int*)malloc(sizeof(int) * commSize);
	
	float *A;
	float *x = (float*)malloc(sizeof(float) * size);
	float *b = (float*)malloc(sizeof(float) * size);
	float *y = (float*)malloc(sizeof(float) * size);

	for (i = 0, r = 1; i < commSize; i++, r++){
		shifts[i] = shift + (r <= remainder ? 1 : 0);
		if (rank == 0)
			printf("shifts[%d] = %d :-)\n", i, shifts[i]);
	}

	float *tmp = (float*)malloc(sizeof(float) * shifts[rank]);

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

		for (i = 1; i < commSize; i++){
			if(i >= remainder){
				MPI_Send(A + size * countShift(shifts, i), size * shifts[i], MPI_FLOAT, i, 0, MPI_COMM_WORLD);
			}
			else {
				MPI_Send(A + size * countShift(shifts, i), size * shifts[i], MPI_FLOAT, i, 0, MPI_COMM_WORLD);
			}
		}
	}			
	else {
		A = (float*)malloc(sizeof(float) * size * shifts[rank]);
		MPI_Recv(A, size * shifts[rank], MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status);
	}
	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Bcast(b, size, MPI_FLOAT, 0, MPI_COMM_WORLD);
	MPI_Bcast(x, size, MPI_FLOAT, 0, MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);

	if (rank == 0){
		time_begin = MPI_Wtime();
	}

	int k = 0;
	while(1){
		k++;
		matrixVecMul(tmp, A, x, size, rank, shifts[rank]);  //y = Ax
		minusVec(tmp, tmp, b + countShift(shifts, rank), shifts[rank]);  //y = Ax - b

		MPI_Barrier(MPI_COMM_WORLD);

		//собрать y по кусочкам
		
		if (rank == 0){
			for (i = 0; i < shifts[0]; i++){
				y[i] = tmp[i];
			}
			for (i = 1; i < commSize; i++){
				MPI_Recv(y + countShift(shifts, i), shifts[i], MPI_FLOAT, i, i, MPI_COMM_WORLD, &status); //y
			}
		}
		else {
			MPI_Send(tmp, shifts[rank], MPI_FLOAT, 0, rank, MPI_COMM_WORLD); //y
		}	

		MPI_Bcast(y, size, MPI_FLOAT, 0, MPI_COMM_WORLD);

		float sumY = 0, sumB = 0;
		float scalY = scalarProduct(tmp, tmp, shifts[rank]);
		float scalB = scalarProduct(b + countShift(shifts, rank), b + countShift(shifts, rank), shifts[rank]);

		MPI_Barrier(MPI_COMM_WORLD);

		MPI_Allreduce(&scalY, &sumY, 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
		MPI_Allreduce(&scalB, &sumB, 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
		float cond = sqrt(sumY / sumB);

		//...

		if (cond < E){
			break;
		}

		//...

		matrixVecMul(tmp, A, y, size, rank, shifts[rank]);  //Ay
		
		float scalAy = scalarProduct(tmp, tmp, shifts[rank]);
		float sumAyAy = 0, sumYAy = 0;

		MPI_Barrier(MPI_COMM_WORLD);

		MPI_Allreduce(&scalAy, &sumAyAy, 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
		if(sumAyAy == 0){
			printf("Denominator is NULL");
			break;
		}

		float scalYAy = scalarProduct(tmp, y + countShift(shifts, rank), shifts[rank]);

		MPI_Barrier(MPI_COMM_WORLD);

		MPI_Allreduce(&scalYAy, &sumYAy, 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);

		float tau = sumYAy / sumAyAy;

		mulAndSumVectors(tmp, 1, x + countShift(shifts, rank), (-1) * tau, y + countShift(shifts, rank), shifts[rank]);  //x(n+1) = x - tau *  y;

		MPI_Barrier(MPI_COMM_WORLD);

		//собрать x по кусочкам
		if (rank == 0){
			for (i = 0; i < shifts[0]; i++){
				x[i] = tmp[i];
			}
			for (i = 1; i < commSize; i++){
				MPI_Recv(x + countShift(shifts, i), shifts[i], MPI_FLOAT, i, i, MPI_COMM_WORLD, &status); //x
			}
		}
		else {
			MPI_Send(tmp, shifts[rank], MPI_FLOAT, 0, rank, MPI_COMM_WORLD); //x
		}

		MPI_Bcast(x, size, MPI_FLOAT, 0, MPI_COMM_WORLD);
	}


	if (rank == 0){
		time_end = MPI_Wtime();
		double worktime = time_end-time_begin;
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
	}

	free(A);
	free(x);
	free(b);
	free(y);
	free(tmp);
	free(shifts);
	
	MPI_Finalize();

	return 0;
}