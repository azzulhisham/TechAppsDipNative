#include <mkl.h>
#define _USE_MATH_DEFINES

#include <mkl_dfti.h>
#include <vector>
#include <cmath>
#include <limits>
#include <iostream>
#include <sstream>
#include <cmath>

#include "Index3D.h"

using std::vector;

class DIP
{
public:
    int windowX = 2;
    int windowZ = 2;
    int dX = 1;
    int dZ = 1;
    float pSize = 0.0001f;

    vector<vector<MKL_Complex8>> PaddingArray(vector<vector<MKL_Complex8>> input, int rowSize, int colSize, int winX, int winZ,  bool circ);
    vector<vector<MKL_Complex8>> ExtendArray(vector<vector<MKL_Complex8>> raw, int row, int col);  
    vector<vector<MKL_Complex8>> ExIndex(vector<vector<MKL_Complex8>> m, int mcXFrom, int mcXTo, int mcYFrom, int mcYTo); 
    vector<vector<MKL_Complex8>> Convolution(const vector<MKL_Complex8>& kernelWin, vector<vector<MKL_Complex8>> input); 

    int GetSizeKernel(int size);
    float rotgauss(int x, int y, double theta, double sigmax, double sigmay);
    vector<vector<MKL_Complex8>> GenerateKernel(vector<vector<MKL_Complex8>> raw, int width, int height);
    vector<vector<MKL_Complex8>> CustGaussian2D(int winX, int winZ, double theta);
    vector<vector<float>> CustGaussianDouble2D(int winX, int winZ, double theta);

    void TestPrint(Index3D index3d);
};
