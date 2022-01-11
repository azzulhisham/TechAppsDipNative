#include <mkl.h>
#define _USE_MATH_DEFINES

#include <mkl_dfti.h>
#include <vector>
#include <cmath>
#include <limits>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>

#include "Index3D.h"

using std::vector;

class Dip
{
public:
    int windowX = 2;
    int windowZ = 2;
    int dX = 1;
    int dZ = 1;
    float pSize = 0.0001f;
    bool isInline = true;

    vector<vector<float>> Calculate(const vector<vector<float>> &data, const vector<MKL_Complex8> &kernelWindow);
    vector<vector<MKL_Complex8>> CustGaussian2D(int winX, int winZ, double theta);
    vector<MKL_Complex8> GenerateKernel(const vector<vector<MKL_Complex8>> &raw, int width, int height);


private:
    const float PI = 3.14159265358979323846f;
    
    int GetSizeKernel(int size);
    float rotgauss(int x, int y, double theta, double sigmax, double sigmay);
    vector<vector<float>> getDt(const vector<vector<float>> &data);
    vector<vector<float>> getDx(const vector<vector<float>> &data);
    vector<vector<MKL_Complex8>> PaddingArray(const vector<vector<MKL_Complex8>> &input, int rowSize, int colSize, int winX, int winZ, bool circ);
    vector<vector<MKL_Complex8>> ExtendArray(const vector<vector<MKL_Complex8>> &raw, int row, int col);
    vector<vector<MKL_Complex8>> ExIndex(const vector<vector<MKL_Complex8>> &m, int mcXFrom, int mcXTo, int mcYFrom, int mcYTo);
    vector<vector<MKL_Complex8>> Convolution(const vector<MKL_Complex8> &kernelWin, vector<vector<MKL_Complex8>> input);
    MKL_Complex8 ComplexDivision(const MKL_Complex8 &dividend, const MKL_Complex8 &divisor);
    MKL_Complex8 ComplexAtan(const MKL_Complex8 &v);
    float getMagnitude(const MKL_Complex8 &trace);
    float ComplexToScalar(const MKL_Complex8 &v);
};
