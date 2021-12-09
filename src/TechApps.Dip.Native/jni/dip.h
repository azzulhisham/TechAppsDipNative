#include <mkl.h>
#define _USE_MATH_DEFINES

#include <mkl_dfti.h>
#include <vector>
#include <cmath>
#include <limits>
#include <iostream>
#include <sstream>
#include <cmath>
#include<complex>

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
    bool isInline = true;

    vector<vector<float>> Calculate(vector<vector<vector<float>>> data, Index3D min, Index3D max, int cursor, const vector<MKL_Complex8>& kernelWindow);
    vector<vector<MKL_Complex8>> PaddingArray(vector<vector<MKL_Complex8>> input, int rowSize, int colSize, int winX, int winZ,  bool circ);
    vector<vector<MKL_Complex8>> ExtendArray(vector<vector<MKL_Complex8>> raw, int row, int col);  
    vector<vector<MKL_Complex8>> ExIndex(vector<vector<MKL_Complex8>> m, int mcXFrom, int mcXTo, int mcYFrom, int mcYTo); 
    vector<vector<MKL_Complex8>> Convolution(const vector<MKL_Complex8>& kernelWin, vector<vector<MKL_Complex8>> input); 
    vector<vector<MKL_Complex8>> GenerateKernel(vector<vector<MKL_Complex8>> raw, int width, int height);
    vector<vector<MKL_Complex8>> CustGaussian2D(int winX, int winZ, double theta);

private:
    const float PI = 3.14159265358979323846f;

    int GetSizeKernel(int size);
    float rotgauss(int x, int y, double theta, double sigmax, double sigmay);
    vector<vector<float>> CustGaussianDouble2D(int winX, int winZ, double theta);    

    vector<vector<float>> getDtInline(vector<vector<vector<float>>> data, int w1, int h1, int cursor);
    vector<vector<float>> getDxInline(vector<vector<vector<float>>> data, int w1, int h1, int cursor);
    vector<vector<float>> getDtCrossline(vector<vector<vector<float>>> data, int w1, int h1, int cursor);
    vector<vector<float>> getDxCrossline(vector<vector<vector<float>>> data, int w1, int h1, int cursor);

    MKL_Complex8 ComplexDivision(MKL_Complex8 dividend, MKL_Complex8 divisor);
    float getMagnitude(const MKL_Complex8& trace);
};
