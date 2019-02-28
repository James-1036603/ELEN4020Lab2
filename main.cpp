#include <stdio.h>
#include <pthread.h>
#include <iostream>
#include <vector>

using namespace std;

template<typename T> vector<vector<T>> TDSquareMatrix(int dimension)
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



int main(int argc, char **argv)
{
	auto myNm = TDSquareMatrix<char>(5);
	return 0;
}
