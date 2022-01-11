#include "dip.h"
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

vector<vector<float>> Dip::getDt(const vector<vector<float>> &data)
{
    auto height = data[0].size(); // sample size
    auto width = data.size();
    vector<vector<float>> o(height, vector<float>(width)); //column major (follow matlab)

    for (int w = 0; w < width; w++)
    {
        for (int h = 1, i = 0; h < height; h++, i++)
        {
            float tempDiff = data[w][h] - data[w][h - 1];
            o[i][w] = tempDiff;
        }
    }

    return o;
}

vector<vector<float>> Dip::getDx(const vector<vector<float>> &data)
{
    auto height = data[0].size();
    auto width = data.size();
    vector<vector<float>> o(height, vector<float>(width)); //column major (follow matlab)

    for (int h = 0; h < height; h++)
    {
        for (int w = 1, i = 0; w < width; w++, i++)
        {
            float tempDiff = data[w][h] - data[w - 1][h];
            o[h][i] = tempDiff;
        }
    }

    return o;
}

vector<vector<float>> Dip::Calculate(const vector<vector<float>> &data, const vector<MKL_Complex8> &kernelWindow)
{
    vector<vector<float>> dt = getDt(data);
    vector<vector<float>> dx = getDx(data);
    vector<vector<MKL_Complex8>> dtdt(dt.size(), vector<MKL_Complex8>(dt[0].size()));
    vector<vector<MKL_Complex8>> dtdx(dt.size(), vector<MKL_Complex8>(dt[0].size()));

    for (int i = 0; i < dt.size(); i++)
    {
        for (int j = 0; j < dt[0].size(); j++)
        {
            dtdx[i][j].real = dt[i][j] * dx[i][j];
            dtdt[i][j].real = dt[i][j] * dt[i][j];
        }
    }

    //convolution
    vector<vector<MKL_Complex8>> resultDtDx = Convolution(kernelWindow, dtdx);
    vector<vector<MKL_Complex8>> resultDtDt = Convolution(kernelWindow, dtdt);
    vector<vector<MKL_Complex8>> pp(resultDtDx.size(), vector<MKL_Complex8>(resultDtDx[0].size()));

    for (int i = 0; i < resultDtDx.size(); i++)
    {
        for (int j = 0; j < resultDtDx[0].size(); j++)
        {

            //just added
            if (abs(resultDtDt[i][j].real) <= 1e-6)
            {
                MKL_Complex8 tmp;
                tmp.real = 0;
                tmp.imag = 0;
                pp[i][j] = tmp;

                continue;
            }

            pp[i][j] = ComplexDivision(resultDtDx[i][j], resultDtDt[i][j]);
            pp[i][j].real *= -1;
            pp[i][j].imag *= -1;
        }
    }

    vector<vector<MKL_Complex8>> resultPp = Convolution(kernelWindow, pp);

    vector<vector<float>> newSlice(resultPp.size(), vector<float>(resultPp[0].size()));

    for (int i = 0; i < resultPp.size(); i++)
    {
        for (int j = 0; j < resultPp[0].size(); j++)
        {
            MKL_Complex8 value;
            value.real = (resultPp[i][j].real * dZ) / dX;
            value.imag = (resultPp[i][j].imag * dZ) / dX;
            MKL_Complex8 b = ComplexAtan(value);
            newSlice[i][j] = ComplexToScalar(b);
        }
    }
    return newSlice;
}

float Dip::ComplexToScalar(const MKL_Complex8 &c)
{
    float s = getMagnitude(c) * 180 / PI;
    return c.real < 0 ? s * -1 : s;
}

MKL_Complex8 Dip::ComplexAtan(const MKL_Complex8 &value)
{
    complex<float> tmp(value.real, value.imag);
    complex<float> right(0.0f - tmp.imag(), tmp.real());
    complex<float> tmp1(1 - right.real(), 0 - right.imag());
    complex<float> tmp2(1 + right.real(), 0 + right.imag());

    //log a complex number
    //log(x + iy) = log(sqrt(pow(x, 2) + pow(y, 2))) + i atan(y/x)
    complex<float> logTmp1(logf(powf(powf(tmp1.real(), 2) + powf(tmp1.imag(), 2), 0.5f)), atan(tmp1.imag() / tmp1.real()));
    complex<float> logTmp2(logf(powf(powf(tmp2.real(), 2) + powf(tmp2.imag(), 2), 0.5f)), atan(tmp2.imag() / tmp2.real()));

    complex<float> log1MinusLog2(logTmp1.real() - logTmp2.real(), logTmp1.imag() - logTmp2.imag());

    complex<float> multiplier(0.0f, 0.5f);

    float atanReal = (multiplier.real() * log1MinusLog2.real()) - (multiplier.imag() * log1MinusLog2.imag());
    float atanImag = (multiplier.real() * log1MinusLog2.imag()) + (multiplier.imag() * log1MinusLog2.real());

    MKL_Complex8 b;
    b.real = atanReal;
    b.imag = atanImag;
    //---------------------------------------------------------------------------
    return b;
}

vector<vector<MKL_Complex8>> Dip::Convolution(const vector<MKL_Complex8> &kernelWin, vector<vector<MKL_Complex8>> input)
{
    int colSize = (int)(input[0].size());
    int rowSize = (int)(input.size());
    vector<vector<MKL_Complex8>> padded = PaddingArray(input, rowSize, colSize, GetSizeKernel(windowZ), GetSizeKernel(windowX), false);

    // cout << windowX << windowZ << endl;
    //convert matrix format into data array
    vector<MKL_Complex8> paddedArray(padded.size() * padded[0].size());
    vector<MKL_Complex8> conv(padded.size() * padded[0].size());
    int temp = 0;

    for (int r = 0; r < padded.size(); r++)
    {
        for (int c = 0; c < padded[0].size(); c++)
        {
            paddedArray[temp] = padded[r][c];
            temp += 1;
        }
    }

    auto kn = paddedArray.size();
    long padColSize = (long)(padded[0].size());
    long padRowSize = (long)(padded.size());

    //Fourier Forward - MatLab
    MKL_LONG fft[2]{padRowSize, padColSize};
    MKL_LONG status;
    DFTI_DESCRIPTOR_HANDLE fh = NULL;

    status = DftiCreateDescriptor(&fh, DFTI_SINGLE, DFTI_COMPLEX, 2, fft);
    status = DftiCommitDescriptor(fh);
    status = DftiComputeForward(fh, paddedArray.data()); // forward FFT
    status = DftiFreeDescriptor(&fh);

    // pointwise multiple kernel x inputMatrix (x + yi)*(u + vi) = (xu - yv) + (xv + yu)i
    for (size_t i = 0; i < kn; i++)
    {
        conv[i].real = (kernelWin[i].real * paddedArray[i].real) - (kernelWin[i].imag * paddedArray[i].imag);
        conv[i].imag = (kernelWin[i].real * paddedArray[i].imag) + (kernelWin[i].imag * paddedArray[i].real);
    }

    //Fourier Backward - MatLab
    DFTI_DESCRIPTOR_HANDLE bh = NULL;
    status = DftiCreateDescriptor(&bh, DFTI_SINGLE, DFTI_COMPLEX, 2, fft);
    status = DftiSetValue(bh, DFTI_BACKWARD_SCALE, 1.0f / kn);
    status = DftiCommitDescriptor(bh);
    status = DftiComputeBackward(bh, conv.data()); // backward FFT
    status = DftiFreeDescriptor(&bh);

    //convert data array into matrix after backward computation
    vector<vector<MKL_Complex8>> resultConv(padded.size(), vector<MKL_Complex8>(padded[0].size()));
    temp = 0;
    for (int r = 0; r < padded.size(); r++)
    {
        for (int c = 0; c < padded[0].size(); c++)
        {
            resultConv[r][c] = conv[temp];
            temp += 1;
        }
    }

    //return actual data size based on input data size
    vector<vector<MKL_Complex8>> result(rowSize, vector<MKL_Complex8>(colSize));

    for (int r = 0; r < rowSize; r++)
    {
        for (int c = 0; c < colSize; c++)
        {
            result[r][c] = resultConv[r][c];
        }
    }

    return result;
}

vector<vector<MKL_Complex8>> Dip::PaddingArray(const vector<vector<MKL_Complex8>> &input, int colSize, int rowSize, int winX, int winZ, bool circ)
{

    int xMmid = (int)floor(winX / 2.0);
    int yMmid = (int)floor(winZ / 2.0);

    colSize += xMmid;
    rowSize += yMmid;

    vector<vector<MKL_Complex8>> extended = ExtendArray(input, colSize, rowSize);

    if (!circ)
    {
        return extended;
    }

    int mcMx = xMmid;
    int mcMy = yMmid;

    int meMx = mcMx + colSize - 1;
    int meMy = mcMy + rowSize - 1;

    vector<vector<MKL_Complex8>> newKernal = ExIndex(extended, mcMx, meMx - 1, mcMy, meMy - 1);

    return newKernal;
}

vector<vector<MKL_Complex8>> Dip::ExtendArray(const vector<vector<MKL_Complex8>> &raw, int col, int row)
{
    vector<vector<MKL_Complex8>> expanded(col, vector<MKL_Complex8>(row));

    for (int i = 0; i < col; i++)
    {
        for (int j = 0; j < row; j++)
        {
            if (i >= raw.size() || j >= raw[0].size())
                continue;

            expanded[i][j] = raw[i][j];
        }
    }

    return expanded;
}

vector<vector<MKL_Complex8>> Dip::ExIndex(const vector<vector<MKL_Complex8>> &m, int mcXFrom, int mcXTo, int mcYFrom, int mcYTo)
{
    vector<vector<MKL_Complex8>> newMatrix(m.size(), vector<MKL_Complex8>(m[0].size()));

    for (int i = 0, x = mcXFrom; i < m.size(); x++, i++) //row
    {
        for (int j = 0, y = mcYFrom; j < m[0].size(); y++, j++) //col
        {
            if (x == m.size())
                x = 0;

            if (y == m[0].size())
                y = 0;

            newMatrix[i][j] = m[x][y];
        }
    }

    return newMatrix;
}

vector<MKL_Complex8> Dip::GenerateKernel(const vector<vector<MKL_Complex8>> &raw, int width, int height)
{
    vector<vector<MKL_Complex8>> expandRaw = PaddingArray(raw, width, height, (int)(raw.size()), (int)(raw[0].size()), true);
    vector<MKL_Complex8> paddedArray(expandRaw.size() * expandRaw[0].size());
    for (int r = 0, temp = 0; r < expandRaw.size(); r++)
    {
        for (int c = 0; c < expandRaw[0].size(); c++, temp++)
        {
            paddedArray[temp] = expandRaw[r][c];
        }
    }

    auto kn = paddedArray.size();
    long padColSize = (long)(expandRaw[0].size());
    long padRowSize = (long)(expandRaw.size());
    MKL_LONG fft[2]{padRowSize, padColSize};
    MKL_LONG status;
    DFTI_DESCRIPTOR_HANDLE fh = NULL;

    status = DftiCreateDescriptor(&fh, DFTI_SINGLE, DFTI_COMPLEX, 2, fft);
    status = DftiCommitDescriptor(fh);
    status = DftiComputeForward(fh, paddedArray.data()); // forward FFT
    status = DftiFreeDescriptor(&fh);

    return paddedArray;
}

int Dip::GetSizeKernel(int size)
{
    return (2 * size) + 1;
}

float Dip::rotgauss(int x, int y, double theta, double sigmax, double sigmay)
{

    theta = (theta / 180) * PI;
    double xm = (x)*cos(theta) - (y)*sin(theta);
    double ym = (x)*sin(theta) + (y)*cos(theta);
    double u = pow((xm / sigmax), 2) + pow((ym / sigmay), 2);
    return (float)exp((-1.0 * u) / 2.0);
}

vector<vector<MKL_Complex8>> Dip::CustGaussian2D(int winX, int winZ, double theta)
{
    int x = GetSizeKernel(winX);
    int y = GetSizeKernel(winZ);

    vector<vector<float>> p(y, vector<float>(x));

    float sumofgauss = 0.0f;
    int rbegin = -1 * (int)round(x / 2.0);
    int cbegin = -1 * (int)round(y / 2.0);

    for (int r = 0; r < x; r++)
    {
        for (int c = 0; c < y; c++)
        {
            p[c][r] = rotgauss(rbegin + (r + 1), cbegin + (c + 1), theta, winX, winZ);
            sumofgauss += p[c][r];
        }
    }

    vector<vector<MKL_Complex8>> kernel(y, vector<MKL_Complex8>(x));

    for (int i = 0; i < kernel.size(); i++)
    {
        for (int j = 0; j < kernel[i].size(); j++)
        {
            kernel[i][j].real = p[i][j] / sumofgauss;
        }
    }

    return kernel;
}

MKL_Complex8 Dip::ComplexDivision(const MKL_Complex8 &dividend, const MKL_Complex8 &divisor)
{
    float conjugate = powf(divisor.real, 2) + powf(divisor.imag, 2);
    MKL_Complex8 multiply1;
    multiply1.real = divisor.real * dividend.real;
    multiply1.imag = divisor.real * dividend.imag;

    MKL_Complex8 multiply2;
    multiply2.real = (divisor.imag * -1) * dividend.imag * -1;
    multiply2.imag = (divisor.imag * -1) * dividend.real;

    MKL_Complex8 result;
    result.real = (multiply1.real + multiply2.real) / conjugate;
    result.imag = (multiply1.imag + multiply2.imag) / conjugate;

    return result;
}

float Dip::getMagnitude(const MKL_Complex8 &trace)
{
    double real = trace.real;
    double imag = trace.imag;
    if (std::isinf(real) || std::isinf(imag) || std::isnan(real) || std::isnan(imag))
        return 0;
    double v = sqrt((real * real) + (imag * imag));
    return (float)(std::isinf(v) || std::isnan(v) ? 0 : v);
}
