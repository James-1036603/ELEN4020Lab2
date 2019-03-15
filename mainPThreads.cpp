#include <stdio.h>
#include <pthread.h>
#include <iostream>
#include <vector>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#define NUM_THREADS     4

using namespace std;

template<typename T> vector<vector<T>> _2DSquareMatrix(int dimension)
//Template 2D square matrix
{
    vector<vector<T>> temp2DMatrix;
    for(auto i = 0; i < dimension; i++)
    {
        vector<T> tempRow(dimension);
        temp2DMatrix.push_back(tempRow);
    }

    return temp2DMatrix;
}

void printMatrix(const auto* inMatrix)
{//Prints an NxN matrix
    auto N = inMatrix->size();
    for(auto i = 0; i<N; i++)
    {
        for(auto j = 0; j<N; j++)
        {
            cout<<inMatrix->at(i).at(j)<<"\t";
        }
        cout <<endl;
    }
}

void PopulateRandomMatrix(auto* inMatrix)
{//Populate Matrix with random values between 0 and 9
    auto N = inMatrix->size();
    for(auto i = 0; i<N; i++)
    {
        for(auto j = 0; j<N; j++)
        {
            inMatrix->at(i).at(j) = rand()% (N+1);
        }
    }
}

void SerialMatrixTranspose(auto* inMatrix)
//Transposes a NxN matrix without any threading
{

    auto N = inMatrix->size();

    for(auto i = 0; i < N; i++)
    {
        for(auto j = i; j < N; j++){
            auto tempVar = inMatrix->at(i).at(j);
            inMatrix->at(i).at(j) = inMatrix->at(j).at(i);
            inMatrix->at(j).at(i) = tempVar;
        }
    }
}


void transposeMatrixByChunks(auto* matrix, size_t chunkSize)
{//Transpose a matrix by dividing it into specified sized chunks. The matrix will have to be a factor of the chunk size to chunk evenly. THIS DOES NOT RETURN A MTRIX> MERELY TRANSPOSES AND DISPLAYS
//Issue is you can't return the new smaller chuncked matrix to the others without a printing method
    if(matrix->size()%chunkSize==0)
    {
        auto resultingMatrixSize = matrix->size()/chunkSize;
        cout<<"RS: "<<resultingMatrixSize<<endl;
        auto myTemp = _2DSquareMatrix<vector<vector<int>>>(resultingMatrixSize);
        auto temp2DMatrixA = _2DSquareMatrix<int>(chunkSize);


        for(auto i = 0; i < resultingMatrixSize; i++)//Cols
        {//Assign the values of the matrix to the temp matrices

            for(auto j = 0; j < resultingMatrixSize; j++)//Rows
            {

                for(auto k = 0; k < chunkSize; k++)
                {

                    for(auto l = 0; l < chunkSize; l++)
                    {
                        //Iterate along each row and col of the passed matrix to a temp matrix. Assign that temp matrix to the big larger matrix once it reaches an iteration of chunkSize
                        temp2DMatrixA.at(k).at(l) = matrix->at(k+(i*chunkSize)).at(l+(j*chunkSize));
                    }
                }
                myTemp.at(i).at(j) = temp2DMatrixA;
            }

        }

        //Transpose the matrix by first transposing the  smaller matrices and then the larger
        for(auto i = 0; i<resultingMatrixSize; i++)
            for(auto j = 0; j<resultingMatrixSize; j++)
                SerialMatrixTranspose(&myTemp.at(i).at(j));
        SerialMatrixTranspose(&myTemp);


        for(auto i = 0; i<resultingMatrixSize; i++){


            for(auto j = 0; j<resultingMatrixSize; j++){
                printMatrix(&myTemp.at(j).at(i));
                cout<<endl;
              }
              cout<<"----------"<<endl;
            }
    }

}

//creating struct of arguments to pass into routine
struct thread_data {
    int thread_id;
    vector<vector<int>> * matrix;    
};

//array of argument structs
struct thread_data thread_data_array[NUM_THREADS];

//diagonal transpose routine
void *finalDTranspose (void * thread_arguments) {
    auto taskid = 0;
    vector<vector<int>> *array;
    struct thread_data *arguments;

    //setting local arguments to externally declared struct arguments
    arguments = (struct thread_data *) thread_arguments;
    taskid = arguments->thread_id;
    array = arguments->matrix;

    //allocating a certain number of iterations to each thread
    auto iterations = array->size()/NUM_THREADS;
    auto start = (taskid * iterations);
    auto end = start + iterations;


//performing diagonal transposition
       for (int i = start; i < end; i++){
        for (int j = i; j < array->size(); j++){
            
            auto temp = array->at(i).at(j);
            array->at(i).at(j) = array->at(j).at(i);
            array->at(j).at(i) = temp;

       }
    }

//terminate thread after successful completion of routine
    pthread_exit(NULL);
}

struct chunks_arg {
    int thread_id;
    vector<vector<int>> * matrix;
    size_t chunk_size;
};

struct chunks_arg chunks_arg_array[NUM_THREADS];

void *transposeByChunks(void * chunks_arguments){

    auto taskid = 0;
    vector<vector<int>> *array;
    struct chunks_arg *arguments;
    auto chunkSize = 0;

    arguments = (struct chunks_arg *) chunks_arguments;
    taskid = arguments->thread_id;
    array = arguments->matrix;
    chunkSize = arguments->chunk_size;

    auto smaller_matrix_size = array->size()/chunkSize;

    auto iterations = smaller_matrix_size/NUM_THREADS;
    auto start = (taskid * iterations);
    auto end = start + iterations;

     if(array->size()%chunkSize==0)
    {
        auto myTemp = _2DSquareMatrix<vector<vector<int>>>(smaller_matrix_size);
        auto temp2DMatrixA = _2DSquareMatrix<int>(chunkSize);


        for(auto i = start; i < end; i++)//Cols
        {//Assign the values of the matrix to the temp matrices

            for(auto j = 0; j < smaller_matrix_size; j++)//Rows
            {

                for(auto k = 0; k < chunkSize; k++)
                {

                    for(auto l = 0; l < chunkSize; l++)
                    {
                        //Iterate along each row and col of the passed matrix to a temp matrix. Assign that temp matrix to the big larger matrix once it reaches an iteration of chunkSize
                        temp2DMatrixA.at(k).at(l) = array->at(k+(i*chunkSize)).at(l+(j*chunkSize));
                    }
                }
                myTemp.at(i).at(j) = temp2DMatrixA;
            }

        }

        //Transpose the matrix by first transposing the  smaller matrices and then the larger
        for(auto i = start; i<end; i++)
            for(auto j = 0; j<smaller_matrix_size; j++)
                SerialMatrixTranspose(&myTemp.at(i).at(j));
        SerialMatrixTranspose(&myTemp);
    }

    pthread_exit(NULL);

}

void threadedDiagonalTranspose(auto * array) 
{
    pthread_t pthreads[NUM_THREADS];
    int rc, taskids[NUM_THREADS];
    pthread_attr_t attr;
    void *status;

        //initialise attribute 
    pthread_attr_init(&attr);

    //configure attribute to signal that threads are joinable
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (int i = 0; i < NUM_THREADS; i++){
        taskids[i] = i;
        thread_data_array[i].matrix = array;
        thread_data_array[i].thread_id = taskids[i];

        rc = pthread_create(&pthreads[i], &attr, finalDTranspose, (void *) &thread_data_array[i]);
        if(rc){
            cout << "ERROR; return code from pthread_create() is " << rc << endl;
            exit(-1);
        }
    }

    //join threads or wait until all threads complete routines
    for (int i = 0; i < NUM_THREADS; i++){
        rc = pthread_join(pthreads[i], &status);
        if(rc){
            cout << "Error: Unable to join " << rc << endl;
            exit(-1);
        }
    }

//destory attribute
    pthread_attr_destroy(&attr);

}

void threadedChunksTranspose (auto * array)
{
    auto chunk_size = 4;

    pthread_t pthreads[NUM_THREADS];
    int rc, taskids[NUM_THREADS];
    pthread_attr_t attr;
    void *status;


    //initialise attribute 
    pthread_attr_init(&attr);

    //configure attribute to signal that threads are joinable
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (int i = 0; i < NUM_THREADS; i++){
        taskids[i] = i;
        chunks_arg_array[i].matrix = array;
        chunks_arg_array[i].thread_id = taskids[i];
        chunks_arg_array[i].chunk_size = chunk_size;

        rc = pthread_create(&pthreads[i], &attr, transposeByChunks, (void *) &chunks_arg_array[i]);
        if(rc){
            cout << "ERROR; return code from pthread_create() is " << rc << endl;
            exit(-1);
        }
    }

    //join threads or wait until all threads complete routines
    for (int i = 0; i < NUM_THREADS; i++){
        rc = pthread_join(pthreads[i], &status);
        if(rc){
            cout << "Error: Unable to join " << rc << endl;
            exit(-1);
        }
    }

    //destory attribute
    pthread_attr_destroy(&attr);

}

void performComparison(auto* matrix)
{//Perform an analysis of one thread to specified threads
  struct timeval start,end;
  cout<<"Performing comparison on size: "<<matrix->size()<<endl;

  //transposeDiagonally
  gettimeofday(&start, NULL);
  threadedDiagonalTranspose(matrix);
  gettimeofday(&end, NULL);
  double time_taken = (end.tv_sec - start.tv_sec) * 1e6;
  time_taken = (end.tv_sec - start.tv_sec) * 1e6;
  time_taken = (time_taken + (end.tv_usec - start.tv_usec))*1e-6;
  cout<<"time taken diagonal"<<fixed<<time_taken<<" sec"<<endl;

  //transposeMatrixByChunks
  gettimeofday(&start, NULL);
  threadedChunksTranspose(matrix);
  gettimeofday(&end, NULL);
  time_taken = (end.tv_sec - start.tv_sec) * 1e6;
  time_taken = (time_taken + (end.tv_usec - start.tv_usec))*1e-6;
  cout<<"time taken chunks "<< fixed<<time_taken<<" sec"<<endl;

  cout << "\n" << endl;
}

int main(int argc, char **argv)
{

    srand(time(NULL));

    //Matrix of N 128
    auto my2dM = _2DSquareMatrix<int>(128);
    PopulateRandomMatrix(&my2dM);
    performComparison(&my2dM);

    //Matrix of N 1024
    my2dM = _2DSquareMatrix<int>(1024);
    PopulateRandomMatrix(&my2dM);
    performComparison(&my2dM);

    //Matrix of N 2048
    my2dM = _2DSquareMatrix<int>(2048);
    PopulateRandomMatrix(&my2dM);
    performComparison(&my2dM);

    //Matrix of N 4096
    my2dM = _2DSquareMatrix<int>(4096);
    PopulateRandomMatrix(&my2dM);
    performComparison(&my2dM);


    //terminate all remaining threads
     pthread_exit(NULL);

	return 0;
}
