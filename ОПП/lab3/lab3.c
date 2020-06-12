#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<time.h>
#include<sys/time.h>

//A = M * N, B = N * K, C = M * K
#define M 2500
#define N 2500
#define K 2500
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

void fullData(float *At, float *A, int size){
	FILE *inAt = fopen("inData/matAt.bin", "rb");
	FILE *inA = fopen("inData/matA.bin", "rb");

	fread(A, sizeof(float), size * size, inA);
	fread(At, sizeof(float), size, inAt);	

	fclose(inA);
	fclose(inAt);
}

int main(){

    int commSize, rank_cart;
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

    double time_start, time_end;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);
    //MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm comm;   //коммуникатор для новой тополии - решетки
    MPI_Comm commX;
    MPI_Comm commY;  
    MPI_Dims_create(commSize, NUM_DIMS, dims); //разбиение всех процессов (размер группы MPI_COMM_WORLD) в N-мерную топологию
    sizex = dims[0];
    sizey = dims[1];
    A_parts_size = M / sizex;
    B_parts_size = K / sizey;
    int *sizematA = (int*)malloc(sizex * sizeof(int));
    int *sizematB = (int*)malloc(sizey * sizeof(int));
    MPI_Cart_create(MPI_COMM_WORLD, NUM_DIMS, dims, periods, reorder, &comm);  //создание нового коммуникатора
    MPI_Comm_rank(comm, &rank_cart);
    MPI_Cart_get(comm, NUM_DIMS, dims, periods, coords);
    int rankx = coords[0];
    int ranky = coords[1];

    float *A_part;
    float *B_part;
    float *C_part;

    int i, j, k, r;

    if(rank_cart == 0){
        //printf("sizex = %d, sizey = %d\n", sizex, sizey);
        for(i = 0; i < sizex; i++){
			sizematA[i] = A_parts_size;
			if(i < (M % sizex)){
				sizematA[i]++;
			}
            //printf("sizematA[%d] = %d ", i, sizematA[i]);
		}
        //printf("\n");
        for(i = 0; i < sizey; i++){
			sizematB[i] = B_parts_size;
			if(i < (K % sizey)){
				sizematB[i]++;
			}
            //printf("sizematB[%d] = %d ", i, sizematB[i]);
		}	
        //printf("\n");		

		time_start = MPI_Wtime(); 
		//printf("A_parts_size %d sizex %d\n", A_parts_size, sizex);
        //printf("B_parts_size  %d sizey %d\n", B_parts_size, sizey);
        //printf("sizex = %d, sizey = %d\n", sizex, sizey);
        //printf("C_size  %d  %d  %d\n", M, K, M * K);
	}

    MPI_Bcast(sizematA, sizex, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(sizematB, sizey, MPI_INT, 0, MPI_COMM_WORLD);

    /*if (rank_cart != 0){
        printf("rank_cart = %d \nrank_x = %d \nrank_y = %d\n\n", rank_cart, rankx, ranky);
    }*/
    A_part = (float*)malloc(N * sizematA[rankx] * sizeof(float));  //sizematA[rankx] вместо A_parts_size
    B_part = (float*)malloc(N * sizematB[ranky] * sizeof(float));  //sizematB[ranky] вместо B_parts_size
    C_part = (float*)malloc(sizematA[rankx] * sizematB[ranky] * sizeof(float));  //тоже

    //для MPI_Scatter:
    coords[0] = 0;  //по x
    coords[1] = 1;  //по y (сохраняется измерение по y)
    MPI_Cart_sub(comm, coords, &commY);  //разбиваем коммуникатор на подгруппы по y
    coords[0] = 1;
    coords[1] = 0;
    MPI_Cart_sub(comm, coords, &commX);  //разбиваем коммуникатор на подгруппы по x

	if(rank_cart == 0){
		A = (float*)malloc(M * N * sizeof(float));
		B = (float*)malloc(N * K * sizeof(float));
		C = (float*)malloc(M * K * sizeof(float));
		transposeB = (float*)malloc(K * N * sizeof(float));
		      
		/*fill_Matrix(A, M, N);
		fill_Matrix(B, N, K);*/
        fullData(A, B, N);
        
		transpose(B, transposeB, N, K);

		//PrintM(A, M, N);
		//PrintM(transposeB, K, N);
	}

    //int rank_cart;
    MPI_Comm_rank(comm, &rank_cart);
    MPI_Cart_coords(comm, rank_cart, 2, coords);

    if (coords[1] == 0) {  //распределение A по x
        MPI_Scatter(A, sizematA[rankx] * N, MPI_FLOAT, A_part, sizematA[rankx] * N, MPI_FLOAT, 0, commX);
    }
    if (coords[0] == 0) {  //распределение B по y
        MPI_Scatter(transposeB, sizematB[ranky] * N, MPI_FLOAT, B_part, sizematB[ranky] * N, MPI_FLOAT, 0, commY);
    }
   
    MPI_Bcast(A_part, sizematA[rankx] * N, MPI_FLOAT, 0, commY);  //кусочки A - по всем y
    MPI_Bcast(B_part, sizematB[ranky] * N, MPI_FLOAT, 0, commX);  //кусочки B - по всем x

    //произведение подматриц
    for (i = 0; i < sizematA[rankx]; i++) {
        for (j = 0; j < sizematB[ranky]; j++) {
            if (rank_cart == 0){
                C[i * K + j] = 0;
                for (k = 0; k < N; k++) {
                    C[i * K + j] += A_part[i * N + k] * B_part[j * N + k];
                }
            }
            else {
                C_part[i * sizematB[ranky] + j] = 0;
                for (k = 0; k < N; k++) {
                    C_part[i * sizematB[ranky] + j] += A_part[i * N + k] * B_part[j * N + k];
                }
            }
        }
    }
    //MPI_Barrier(comm);
    //if (rank_cart == 0) 
       // PrintM(C, M, K);
 	MPI_Barrier(comm);

    //int rank_x, rank_y;
    
	if(rank_cart == 0){
        float *tmp = (float*)malloc(M * K * sizeof(float));
		for(i = 0; i < sizex; i++){
			for(j = 0; j < sizey; j++){
				if(i != 0 || j != 0){
					int tmp_rank;
					coords[0] = i;
					coords[1] = j;
					MPI_Cart_rank(comm, coords, &tmp_rank);

                    //MPI_Recv(&rank_x, 1, MPI_INT, tmp_rank, tmp_rank, comm, MPI_STATUS_IGNORE);
                    //MPI_Barrier(comm);
                    //MPI_Recv(&rank_y, 1, MPI_INT, tmp_rank, tmp_rank, comm, MPI_STATUS_IGNORE);
 	                //MPI_Barrier(comm);
                     //tmp = (float*)malloc(sizematA[i] * sizematB[j] * sizeof(float));
                    
                    //printf("tmp_rank = %d x = %d y = %d\n", tmp_rank, coords[0], coords[1]);
					MPI_Recv(tmp, sizematA[i] * sizematB[j], MPI_FLOAT, tmp_rank, 123, comm, MPI_STATUS_IGNORE);
                    //printf("Hello 4 ;-)\n");

                    int off_x = 0, off_y = 0;
                    for (k = 0; k < i; k++) {
                        off_x += sizematA[k];
                    }
                    for (k = 0; k < j; k++) {
                        off_y += sizematB[k];
                    }
                    //int offset = off_x * K + off_y;

                    for(k = 0; k < sizematA[i]; k++){  // k - смещение относительно блока по x
                        for(r = 0; r < sizematB[j]; r++){ // r - смещение относительно блока по y
                            C[(off_x + k) * K + (off_y + r) ] = tmp[k * sizematB[j] + r];  //НЕТ (левая часть)!
                        }
                    } 
                    //PrintM(tmp, sizematA[i], sizematB[j]);
                    //printf("Hello 5 ;-)\n");
                }
			}
		}
        free(tmp);
        time_end = MPI_Wtime();
        //PrintM(C, M, K);
        free(A);
        free(B);
        free(transposeB);
        printf("\nTime is %f\n", time_end - time_start);  
	} 
    else{
        //MPI_Send(&rankx, 1, MPI_INT, 0, i, comm);
        //MPI_Barrier(comm);
        //MPI_Send(&ranky, 1, MPI_INT, 0, i, comm);
        //MPI_Barrier(comm);
        //printf("Hello 1 ;-)\n");
		MPI_Send(C_part, sizematA[rankx] * sizematB[ranky], MPI_FLOAT, 0, 123, comm);  //MPI_COMM_WORLD или comm?
        //printf("Hello 2 ;-)\n");
	}

    if (rank_cart == 0){
		float *matAtA = (float*)malloc(sizeof(float) * N);
		FILE *inAtA = fopen("inData/matAtA.bin", "rb");
		fread(matAtA, sizeof(float), N, inAtA);
		fclose(inAtA);

		float difference = -1;
		for (i = 0; i < N; i++){
			float d = abs(matAtA[i] - C[i]);
			if (d > difference) difference = d;
			//printf("x[%d] dif: %lf\n", i, d);
		}
		//printVec(x, N);
		printf("difference = %lf\n", difference);

		free(matAtA);
        free(C);
	}

    free(A_part); 
    free(B_part);
    free(C_part);
    free(sizematA);
    free(sizematB);
    MPI_Finalize();
	return 0;
}