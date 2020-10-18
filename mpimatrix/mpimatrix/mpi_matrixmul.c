#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>



#define MATSIZE 2
#define MAXNUMBER 2
#define NRA MATSIZE
#define NCA MATSIZE
#define NRB NCA 
#define NCB MATSIZE
#define MASTER 0
#define FROM_MASTER 1
#define FROM_WORKER 2

void printMatrix(double matrix[][MATSIZE],int nrows,int ncolumns, char* title) {
    printf("\n");
    printf("%s:\n",title);
    for (int i=0; i<nrows; i++) 
    {
        printf("\n");
        for (int j=0; j<ncolumns; j++) 
        {
			printf("%6.2f   ", matrix[i][j]);
        }
    }
    printf("\n\n");
}


int main (int argc, char *argv[]) 
{

    int nproc, id, numslaves, source, dest, mtype, rows, averow, extra, offset, i, j, k, rc;
    double a[NRA][NCA], b[NRB][NCB], c[NRA][NCB];
    MPI_Status status;


    
    srand(time(NULL));

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&id);
    MPI_Comm_size(MPI_COMM_WORLD,&nproc);

  
    numslaves = nproc-1;

   
    if (id == MASTER) 
    {
        printf("Enter the first matrix element=\n");    
        for (i=0; i<NRA; i++) 
        {
            for (j=0; j<NCA; j++) 
            {
                //a[i][j]= rand()%MAXNUMBER;
                scanf("%lf",&a[i][j]);   
            }
        }
        printf("Enter the second matrix element=\n");    
        for (i=0; i<NRA; i++)
         {
            for (j=0; j<NCB; j++)
             {
                //b[i][j]= rand()%MAXNUMBER;
                scanf("%lf",&b[i][j]);    
            }
        }

        printMatrix(a,NRA,NCA,"Matrix A");
        printMatrix(b,NRB,NCB,"Matrix B");


        double start = MPI_Wtime();

       
        averow = NRA/numslaves;
        extra = NRA%numslaves;
        offset = 0; 
        mtype = FROM_MASTER;
        for (dest=1; dest<=numslaves; dest++)
         {
            rows = (dest <= extra) ? averow+1 : averow;
            

            MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
 
            MPI_Send(&rows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
         
            MPI_Send(&a[offset][0], rows*NCA, MPI_DOUBLE, dest, mtype,
                     MPI_COMM_WORLD);
           
            MPI_Send(&b, NRA*NCB, MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);
            offset = offset + rows;
        }
        mtype = FROM_WORKER;
        for (i=1; i<=numslaves; i++) 
        {
            source = i;

            MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
            MPI_Recv(&rows, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
            MPI_Recv(&c[offset][0], rows*NCB, MPI_DOUBLE, source, mtype,
                     MPI_COMM_WORLD, &status);
            
        }

        printMatrix(c,NRA,NCB,"Result matrix");


        double finish = MPI_Wtime();
        printf("timetaken %f seconds.\n", finish - start);
    }



    if (id > MASTER)
     {
        mtype = FROM_MASTER;
        MPI_Recv(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&a, rows*NCA, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&b, NRB*NCB, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD, &status);

        for (k=0; k<NCB; k++) 
        {
            for (i=0; i<rows; i++) 
            {
                c[i][k] = 0.0;
                for (j=0; j<NCA; j++) 
                {
                    c[i][k] = c[i][k] + a[i][j] * b[j][k];
                }
            }
        }
        mtype = FROM_WORKER;
        MPI_Send(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
        MPI_Send(&c, rows*NCB, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD);
    }
    MPI_Finalize();
}


