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


void transposeDiagonally(auto* matrix)
{//Transposes the matrix along the diagonal. Also known as "in-place". This does change the passed matrix
	auto tempMatrix = _2DSquareMatrix<int>(matrix->size());
	for(auto i = 0; i<matrix->size(); i++)
	{
		for(auto j = i; j < matrix->size(); j++)
		{
		auto tempVal = matrix->at(i).at(j);
		matrix->at(i).at(j) = matrix->at(j).at(i);
		matrix->at(j).at(i) = tempVal;		
		}
	}
}


auto global_array = _2DSquareMatrix<int>(10);
pthread_mutex_t array_mutex;

void *print(void *tid){

    int start, *mytid, end;
    int iterations = global_array.size()/NUM_THREADS;

    mytid = (int *)tid;

    sleep(1);
    start = (*mytid * iterations);
    end = start + iterations;

    //cout << "Thread " << *mytid << " doing iterations " << start << " to " << end << endl;

    pthread_mutex_lock (&array_mutex);
    for (int i = start; i < end; i++){
        for (int j = 0; j < global_array.size(); j++){
            cout << global_array[i][j] << "\t";
        }
        cout << "\n" << endl;
    }
    pthread_mutex_unlock(&array_mutex);

    pthread_exit(NULL);
    
}

void *diagonalTranspose(void *tid){

    int start, *mytid, end;
    int iterations = global_array.size()/NUM_THREADS;

    mytid = (int *)tid;

    sleep(1);
    start = (*mytid * iterations);
    end = start + iterations;

    //cout << "Thread " << *mytid << " doing iterations " << start << " to " << end << endl;

    pthread_mutex_lock (&array_mutex);
    for (int i = start; i < end; i++){
        for (int j = i; j < global_array.size(); j++){
            
            auto temp = global_array[i][j];
            global_array[i][j] = global_array[j][i];
            global_array[j][i] = temp;

       }
    }
    pthread_mutex_unlock(&array_mutex);

    pthread_exit(NULL);
    
}




int main(int argc, char **argv)
{

    srand(time(NULL));
   // auto my2dM = _2DSquareMatrix<int>(5);
    //PopulateRandomMatrix(&my2dM);
   // printMatrix(&my2dM);
   PopulateRandomMatrix(&global_array);
   printMatrix(&global_array);
    cout<<endl;
    //transposeMatrixByChunks(&my2dM,2);
    //transposeDiagonally(&my2dM);
    transposeDiagonally(&global_array);
    printMatrix(&global_array);

    pthread_t pthreads[NUM_THREADS];
    int rc, taskids[NUM_THREADS];
    pthread_attr_t attr;
    void *status;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (int i = 0; i < NUM_THREADS; i++){
        taskids[i] = i;
        cout << "main(): Creating thread - " << i << endl;

        rc = pthread_create(&pthreads[i], &attr, diagonalTranspose, (void *) &taskids[i]);

    }

    
    for (int i = 0; i < NUM_THREADS; i++){
        rc = pthread_join(pthreads[i], &status);

        cout << "Main: completed thread: " << i << endl;
    }

    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&array_mutex);

	//printMatrix(&my2dM);
    printMatrix(&global_array);
    cout << "Done" << endl;

     pthread_exit(NULL);

	return 0;
}
