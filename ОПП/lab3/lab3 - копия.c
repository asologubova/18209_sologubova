#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<time.h>
#include<sys/time.h>

//A = MxN, B = NxK и C = MxK
#define M 5
#define N 5
#define K 5
#define NUM_DIMS 2

void transpose(float *Matrix, float* tr, int row, int col){
    int i, j;
    for(i = 0; i < row; i++){
        for(j = 0; j < col; j++){
            tr[j * row + i] = Matrix[i * col + j];
        }
    }
}

void fill_Matrix(float *Matrix, int row, int col){
    int i, j;
    for(i = 0; i < row; i++){
        for(j = 0; j < col; j++){
            if(i == 2 * j){
                Matrix[i * col + j] = 3;
            }
            else {
                Matrix[i * col + j] = 2;
            }
        }
    }  

}

void PrintM(float *Matrix, int row, int col){
    int i, j;
    for(i = 0; i < row; i++){
        for(j = 0; j < col; j++){
            printf("%f ", Matrix[i * col + j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(){

    int commSize, rank;
    int dims[NUM_DIMS] = { 0 };    //целочисленный массив размером ndims, определяющий количество процессов в каждом измерении
    int periods[NUM_DIMS] = { 0 };  //массив размером ndims логических значений,определяющих периодичность (true) или нет (false) в каждом измерении
    int coords[NUM_DIMS] = { 0 };
    int reorder = 0;
    int sizex, sizey;
    int coord[NUM_DIMS] = { 0 };
    int prevx, nextx, prevy, nexty;
    int A_parts_size;
    int B_parts_size;
    float *A, *B, *C, *transposeB;
    /* float * B_p = (float*)malloc(N*K*sizeof(float));
    fill_Matrix(B_p,N,K);
    PrintM(B_p,N,K);
        
    PrintM( transpose(B_p,N,K),K,N);
    return 0;*/
    double time_start, time_end;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm comm;   //коммуникатор для новой тополии - решетки
    MPI_Comm commX;
    MPI_Comm commY;  
    MPI_Dims_create(commSize, NUM_DIMS, dims); //разбиение всех процессов (размер группы MPI_COMM_WORLD) в N-мерную топологию
    sizex = dims[0];
    sizey = dims[1];
    A_parts_size = M / sizex;
    B_parts_size = K / sizey;
    int i, j, k, r;

    if(rank == 0){
		time_start = MPI_Wtime(); 
		printf("A_parts_size %d sizex %d\n", A_parts_size, sizex);
        printf("B_parts_size  %d sizey %d\n", B_parts_size, sizey);
        printf("C_size  %d  %d  %d\n", M, N, M * N);
	}

    MPI_Cart_create(MPI_COMM_WORLD, NUM_DIMS, dims, periods, reorder, &comm);
    float *A_part = (float*)malloc(N * A_parts_size * sizeof(float));
    float *B_part = (float*)malloc(N * B_parts_size * sizeof(float));
    float *C_part = (float*)malloc(A_parts_size * B_parts_size * sizeof(float));
    coords[0] = 0;  //по x
    coords[1] = 1;  //по y
    MPI_Cart_sub(comm, coords, &commY); 
    coords[0] = 1;
    coords[1] = 0;
    MPI_Cart_sub(comm, coords, &commX); 

	if(rank == 0){
		A = (float*)malloc(M * N * sizeof(float));
		B = (float*)malloc(N * K * sizeof(float));
		C = (float*)malloc(M * K * sizeof(float));
		transposeB = (float*)malloc(K * N * sizeof(float));
		      
		fill_Matrix(A, M, N);
		fill_Matrix(B, N, K);
		transpose(B, transposeB, N, K);
		PrintM(A, M, N);
		PrintM(transposeB, K, N);
	}

    int cart_rank;
    MPI_Comm_rank(comm, &cart_rank);
    MPI_Cart_coords(comm, cart_rank, 2, coords);

    if (coords[1] == 0) {  //все процессы по x
        MPI_Scatter(A, A_parts_size * N, MPI_FLOAT, A_part, A_parts_size * N, MPI_FLOAT, 0, commX); // групповая операция
    }
    if (coords[0] == 0) {  //все процессы по y
        MPI_Scatter(transposeB, B_parts_size * N, MPI_FLOAT, B_part, B_parts_size * N, MPI_FLOAT, 0, commY);
    }
   
    MPI_Bcast(A_part, A_parts_size * N, MPI_FLOAT, 0, commY);
    MPI_Bcast(B_part, B_parts_size * N, MPI_FLOAT, 0, commX);

    //float matrixEl = 0;
    //произведение подматриц
    for (i = 0; i < A_parts_size; i++) {
        for (j = 0; j < B_parts_size; j++) {
            C_part[i * B_parts_size + j] = 0;
            for (k = 0; k < N; k++) {
                C_part[i * B_parts_size + j] += A_part[i * N + k] * B_part[j * N + k];
            }
        }
    }
 	MPI_Barrier(comm);

	if(rank == 0){
		for(i = 0; i < sizex; i++){
			for(j = 0; j < sizey; j++){
				if(i != 0 || j != 0){
					int tmp_rank;
					coords[0] = i;
					coords[1] = j;
					MPI_Cart_rank(comm, coords, &tmp_rank);
					MPI_Recv(C_part, A_parts_size * B_parts_size, MPI_FLOAT, tmp_rank, 123, comm, MPI_STATUS_IGNORE);
				}
				for(k = 0; k < A_parts_size; k++){  // k - смещение относительно блока по x
					for(r = 0; r < B_parts_size; r++){ // r - смещение относительно блока по y
						C[(i * A_parts_size + k) * K + (j * B_parts_size + r)] = C_part[k * B_parts_size + r];
					}
				}   
			}
		}
        time_end = MPI_Wtime();
        PrintM(C, M, K);
        free(A);
        free(B);
        free(C);
        free(transposeB);
        printf("\nElapsed time is %f\n", time_end - time_start);  
	} 
    else{
		MPI_Send(C_part, A_parts_size * B_parts_size, MPI_FLOAT, 0, 123, comm);
	}
    free(A_part); 
    free(B_part);
    free(C_part);
    MPI_Finalize();
	return 0;
}