#include <stdio.h>
#include <pthread.h>
#include <iostream>
#include <vector>

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
            cout<<myMatrix.at(i).at(j)<<" ";
        }
        cout<<endl;
    }
}

void PopulateRandomMatrux(auto* inMatrix)
{//Populate Matrix with random values between 0 and 9
    auto N = inMatrix->size();
    for(auto i = 0; i<N; i++)
    {
        for(auto j = 0; j<N; j++)
        {
            myMatrix.at(i).at(j) = rand()%10;
        }
    }
}



int main(int argc, char **argv)
{
    
	return 0;
}
