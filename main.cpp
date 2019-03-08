#include <stdio.h>
#include <pthread.h>
#include <iostream>
#include <vector>
#include <time.h>

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
            cout<<inMatrix->at(i).at(j)<<" ";
        }
        cout<<endl;
    }
}

void PopulateRandomMatrix(auto* inMatrix)
{//Populate Matrix with random values between 0 and 9
    auto N = inMatrix->size();
    for(auto i = 0; i<N; i++)
    {
        for(auto j = 0; j<N; j++)
        {
            inMatrix->at(i).at(j) = rand()%10;
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


int main(int argc, char **argv)
{

    srand(time(NULL));
    auto my2dM = _2DSquareMatrix<int>(6);
    PopulateRandomMatrix(&my2dM);
    printMatrix(&my2dM);
    cout<<endl;
    //transposeMatrixByChunks(&my2dM,2);
    transposeDiagonally(&my2dM);
	printMatrix(&my2dM);


	return 0;
}
