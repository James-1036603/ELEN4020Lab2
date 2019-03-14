#include <stdio.h>
#include <pthread.h>
#include <iostream>
#include <vector>
#include <time.h>
#include <unistd.h>

#define NUM_THREADS     2

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

int main(int argc, char **argv)
{

    srand(time(NULL));
    auto my2dM = _2DSquareMatrix<int>(5);
    PopulateRandomMatrix(&my2dM);
    printMatrix(&my2dM);
    cout<<endl;

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
        thread_data_array[i].matrix = &my2dM;
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

        cout << "Main: completed thread: " << i << endl;
        cout << "exiting with staus " << status << endl;
    }

//destory attribute
    pthread_attr_destroy(&attr);

    printMatrix(&my2dM);
    cout << "Done" << endl;

//terminate all remaining threads
     pthread_exit(NULL);

	return 0;
}
