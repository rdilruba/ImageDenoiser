// Author: R.Dilruba Kose
// Student number : 2015400219
// Compling & Working
/*  I give processor number n+1 in command line n processors do the job 1 is main send and receive data*/
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    // Find out rank, size
    int processor;
    MPI_Comm_rank(MPI_COMM_WORLD, &processor);
    int pnum;
    MPI_Comm_size(MPI_COMM_WORLD, &pnum);
    int P = pnum-1; // processor number other than main one
    char *infile = argv[1];  // reading input file name
    char *outfile = argv[2]; // reading output file name
    float  beta = atof(argv[3]); // reading beta number
    float  pi = atof(argv[4]); // reading pi number
    float gamma = 0.5*log((1-pi)/pi);
    int  T = 500000; // iteration number
    int counter = 200; // size of the picture
   
    srand((unsigned)time(NULL));
    if (processor == 0) { // if processor is the main one do the readings and send data
        int i, j;
        FILE *reader = fopen(infile, "r");
        FILE *writer = fopen(outfile, "w");
        int a = 0;
        int k,l;
        int myarray[counter][counter];
        /* reading the array of photo */
        for(k=0; k<counter; k++)
        {
            for(l=0; l<counter; l++)
            {
                fscanf(reader,"%d",&a);
                myarray[k][l] = a;
            }
        }
        fclose(reader);



        // the final array that will be recieved from processors
        int sarray[counter][counter];


        // sending array to proccessors
        for(i = 1 ; i <= P ; i++){
            MPI_Send(myarray[(i-1)*counter/P], counter*counter/P, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        // receiving the final array from proccessors
        for(i = 1 ; i <= P ; i++){
            MPI_Recv(sarray[(i-1)*counter/P], counter*counter/P, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
      
      	/*writing final array to output file */
        for(k=0; k<counter; k++)
        {
            for(l=0; l<counter; l++)
            {
                fprintf(writer,"%d ",sarray[k][l]);
            }
            fprintf(writer,"\n");
        }
        fclose(writer);
    }
    else{ // if processor is not the main do the algorithm

        int f,d,j; // for integers
        int x,y,sum=0; // sum is used for summation of the surrounding elements
        int prob = 0; // prob is used for probavility of changing the cell
        int rarray[counter/P][counter]; // the array recevied from main to work on
        int tempup[counter]; // the array of our upper processors last row
        int tempdown[counter]; // the array of our lower processors first row
        int sup[counter]; // our first row to send upper processor
        int sdown[counter]; // our last row to send bottom processor
        int zarray[counter/P][counter]; // copy of initial array to use for algorithm

        double ran; // random number to check probability
        //receiving the array
        MPI_Recv(rarray, counter*counter/P, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        /* copying initial array */
        for(f=0; f<counter/P; f++)
            for(d=0; d<counter; d++)
                zarray[f][d] = rarray[f][d];

        /* algorithm for . It looks for a cell and try to change it in each iteration*/
        for(j=0; j<T; j++) {

            int t,p;
            //copying our first row
            for(t=0; t<counter; t++)
                sup[t] = rarray[0][t];
            //copying our last row
            for(p=0; p<counter; p++)
                sdown[p] = rarray[200/P-1][p];

            if (processor != 1 ) // if processor is not the first one send upper processor to our first row
                MPI_Send(sup, counter, MPI_INT, processor - 1, 0, MPI_COMM_WORLD);
            if (processor != P ) // if processor is not the last one send lower processor to our last row
                MPI_Send(sdown, counter, MPI_INT, processor + 1, 0, MPI_COMM_WORLD);
            sum=0;
            // random x and y for cell
            x = rand() % (counter/P);
            y = rand() % counter;
            if(processor!=1) // if processor is not the first one get upper processor's last row
                MPI_Recv(tempup, counter, MPI_INT, processor - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


            if(processor!=P) // if processor is not the last one get lower processor's first row
                MPI_Recv(tempdown, counter, MPI_INT, processor + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            /* this for iterates over the surrounding elements of the chosen cell and calculate their sum
            if x is not in our domain looks up for it from upper or lower processors*/
            for(f=x-1; f<x+2; f++)
                for(d=y-1; d<y+2; d++) {

                    if(f==x && d==y)
                        continue;
                    if(f<0 && !(d<0 && d>=counter)) {
                        sum += tempup[d];

                    }
                    else if(f>=counter/P && !(d<0 && d>=counter)) {
                        sum += tempdown[d];
                    }
                    else if(!(d<0 && d>=counter))
                        sum += rarray[f][d];

                }
            // calculating the probability of changing the cell according to algorithm
            prob = (-2 * gamma * zarray[x][y] * rarray[x][y]) - (2 * beta * rarray[x][y] *sum);
            ran=log((double)rand()/RAND_MAX);
            if(ran< prob) { // checks if random number is less then changes the cell
                rarray[x][y] = -1 * rarray[x][y];
               }

        }
        // sending final array to the main processor
        MPI_Send(rarray[0], counter*counter/P, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
}
