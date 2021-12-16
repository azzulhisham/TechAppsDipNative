#include "dip.h"
#include <iomanip>

#include <fstream>
#include <string>

using namespace std;


// int main() {

//     DIP dip;


// 	vector<vector<int>> inputMatrix = {{ -1, 2, 3, -4, 5, -6, 7, 8, 9, -10, 11 },
//                                    { -12, 13, 14, -15, 16, -16, 18, 19, 20, -21, 22 },
//                                    { -22, 23, 24, -25, 26, -26, 28, 29, 30, -31, 32 },
//                                    { -32, 33, 34, -35, 36, -36, 38, 39, 40, -41, 42 },
//                                    { -32, 33, 34, -35, 36, -36, 38, 39, 40, -41, 42 },
//                                    { -52, 53, 54, -55, 56, -56, 58, 59, 60, -61, 22 },
//             };

//     // vector<vector<vector<int>>> in = { 
//     //                                     { { 1, 2, 3, -1 }, { 4, 5, 6, -2 }, { 4, 5, 6, -2 } },
//     //                                     { { 7, 8, 9, -3 }, { 10, 11, 12, -4 }, { 4, 5, 6, -2 } } 
//     //                                  };


//     //unit testing
//     vector<vector<MKL_Complex8>> c_array(inputMatrix.size(), vector<MKL_Complex8>(inputMatrix[0].size()));
//     for (int i = 0; i < c_array.size(); i++) {
//         for (int j = 0; j < c_array[0].size(); j++) {
//             c_array[i][j].real = (float)inputMatrix[i][j];
//         }
//     }
                             
//     vector<vector<MKL_Complex8>> kernel = dip.CustGaussian2D(dip.windowX, dip.windowZ, 0);
//     vector<vector<MKL_Complex8>> kernelWin = dip.GenerateKernel(kernel, (int)(inputMatrix.size()), (int)(inputMatrix[0].size()));

//     vector<MKL_Complex8> kernelWinArray(kernelWin.size() * kernelWin[0].size());
//     int temp = 0;

//     for(int r=0; r<kernelWin.size(); r++){
//         for(int c=0; c<kernelWin[0].size(); c++){
//             kernelWinArray[temp] = kernelWin[r][c];
//             temp += 1;
//         }
//     }


//     vector<vector<MKL_Complex8>> result = dip.Convolution(kernelWinArray, c_array); 
// 	//vector<vector<MKL_Complex8>> result = dip.PaddingArray(c_array, 11, 6, 4, 4, false);

//     cout.precision(11);
//     for (int r = 0; r < result.size(); r++)
//     {
//         for (int c = 0; c < result[0].size(); c++)
//         {
//             cout << result[r][c].real << "\t";
//         }
//         cout << endl;
//     }    


// 	return 0;
// }


// int main(){
//     cout << "Test Calculate Function..............." << endl;

//     int sampleSize = 28;
//     vector<float> input(784);
//     int cnt = 0;

//     fstream newfile;  
//     newfile.open("C:\\Users\\zulhisham\\Downloads\\inputData007.txt",ios::in); //open a file to perform read operation using file object
//     if (newfile.is_open()){   //checking whether the file is open
//         string tp;
//         while(getline(newfile, tp)){ //read data from file object and put it into string.
//             //cout << tp << "\n"; //print the data of the string

//             input[cnt] = std::stof(tp);
//             cnt++;
//         }
//         newfile.close(); //close the file object.
//     }

//     cout << "App Started..............." << endl;
//     // for(int i=0; i<input.size(); i++){
//     //     cout << input[i] << endl;
//     // }

//     //initialize 3D vector/list
//     int dataLength = (int)(input.size());
//     int traceSize = sampleSize;
//     int numberOfTrace = dataLength/traceSize;
//     vector<vector<vector<float>>> inputData(1, vector<vector<float>>(numberOfTrace, vector<float>(traceSize)));

//     int dataRow = 0;
//     int dataCnt = 0;

//     //convert input data to 3D array
//     for(int i=0; i<dataLength; i++) {
//         inputData[0][dataRow][dataCnt] = input[i];
//         dataCnt += 1;

//         if(dataCnt >= traceSize) {
//             dataRow += 1;
//             dataCnt = 0;
//         }
//     }

//     DIP dip;
//     Index3D min(0);
//     Index3D max((int)(inputData.size())-1, (int)(inputData[0].size())-1, (int)(inputData[0][0].size())-1);

//     vector<vector<MKL_Complex8>> kernel = dip.CustGaussian2D(dip.windowX, dip.windowZ, 0);
//     vector<vector<MKL_Complex8>> kernelWindow = dip.GenerateKernel(kernel, (int)(inputData[0].size()), (int)(inputData[0][0].size()));

//     vector<MKL_Complex8>kernelWin(kernelWindow.size() * kernelWindow[0].size());

//     //convert 2D array to single array
//     dataCnt = 0;
//     for(int i=0; i<kernelWindow.size(); i++) {
//         for(int j=0; j<kernelWindow[0].size(); j++) {
//             kernelWin[dataCnt] = kernelWindow[i][j];
//             dataCnt += 1;
//         }
//     }  

//     //process
//     vector<vector<float>> result = dip.Calculate(inputData, min, max, 0, kernelWin);  
//     vector<float>resultData(result.size() * result[0].size());

//     //convert 2D array to single array
//     dataCnt = 0;
//     for(int i=0; i<result.size(); i++) {
//         for(int j=0; j<result[0].size(); j++) {
//             resultData[dataCnt] = result[i][j];
//             dataCnt += 1;
//         }
//     }  

//     //ofstream newfileout;  
//     //newfileout.open("C:\\Users\\zulhisham\\Downloads\\cplusplus.txt",ios::out); //open a file to perform read operation using file object

//     for(int i=0; i<resultData.size(); i++){
//         cout << resultData[i] << endl;
//         //newfileout << resultData[i] << endl;
//     }

//     //newfileout.close(); //close the file object.

//     return 0;
// }

vector<vector<float>> DIP::getDxCrossline(vector<vector<vector<float>>> data, int w1, int h1, int cursor) {

    int width = w1;
    int height = h1;

    vector<vector<float>> dt(height, vector<float>(width)); //column major (follow matlab)

    for (int z = 0; z < height; z++)
    {
        for (int x = 1,w=0; x < width; x++, w++)
        {
            float tempDiff = data[x][cursor][z] - data[x - 1][cursor][z];
            dt[z][w] = tempDiff;
        }
        //var diffTrace = DiffOrder1(trace);
    }

    return dt;
}

vector<vector<float>> DIP::getDtCrossline(vector<vector<vector<float>>> data, int w1, int h1, int cursor) {

    int width = w1;
    int height = h1;

    vector<vector<float>> dt(height, vector<float>(width)); //column major (follow matlab)

    for (int x = 0; x < width; x++)
    {
        for (int z = 1, y = 0; z < height; z++, y++)
        {
            float tempDiff = data[x][cursor][z] - data[x][cursor][z - 1];
            dt[y][x] = tempDiff;
        }
                //var diffTrace = DiffOrder1(trace);
    }

    return dt;
}

vector<vector<float>> DIP::getDtInline(vector<vector<vector<float>>> data, int w1, int h1, int cursor){

    int width = w1;
    int height = h1;

    vector<vector<float>> dt(height, vector<float>(width));
    
    for (int x = 0; x < width; x++)
    {
        for (int z = 1, y = 0; z < height; z++, y++)
        {
            float tempDiff = data[cursor][x][z] - data[cursor][x][z-1];
            dt[y][x] = tempDiff; //column major (follow matlab)
        }
    }

    return dt;
}

vector<vector<float>> DIP::getDxInline(vector<vector<vector<float>>> data, int w1, int h1, int cursor) {

    int width = w1;
    int height = h1;

    vector<vector<float>> dt(height, vector<float>(width));//column major (follow matlab)

    for (int z = 0; z < height; z++)
    {
        for (int x = 1, w=0; x < width; x++, w++)
        {
            float tempDiff = data[cursor][x][z] - data[cursor][x - 1][z];
            dt[z][w] = tempDiff;
        }
        //var diffTrace = DiffOrder1(trace);
    }

    return dt;
}

vector<vector<float>> DIP::Calculate(vector<vector<vector<float>>> data, Index3D min, Index3D max, int cursor, const vector<MKL_Complex8>& kernelWindow) {

    int height = (max.K - min.K) + 1;
    int widthInLine = (max.J - min.J) + 1;
    int widthXLine = (max.I - min.I) + 1;

    vector<vector<float>> dt = DIP::isInline ? DIP::getDtInline(data, widthInLine, height, cursor) : DIP::getDtCrossline(data, widthXLine, height, cursor); //Min.X and Max.X is Width of the Slice

    int widthXline = (max.I - min.I) + 1;
    int heightXline = (max.K - min.K) + 1;

    int widthInline = (max.J - min.J) + 1;
    int heightInline = (max.K - min.K) + 1;

    vector<vector<float>> dx = DIP::isInline ? DIP::getDxInline(data, widthInline, heightInline, cursor) : DIP::getDxCrossline(data, widthXline, heightXline, cursor); //Min.X and Max.X is Width of the Slice

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

            //TODO
            //pp[i][j] = Complex32.Divide(resultDtDx[i][j], resultDtDt[i][j]) * -1;
            pp[i][j] = ComplexDivision(resultDtDx[i][j], resultDtDt[i][j]);
            pp[i][j].real *= -1;
            pp[i][j].imag *= -1;
        }
    }

    vector<vector<MKL_Complex8>> resultPp = Convolution(kernelWindow, pp);

    vector<vector<float>> newSlice(resultPp.size(), vector<float>(resultPp[0].size()));
    vector<vector<float>> phases(resultPp.size(), vector<float>(resultPp[0].size()));

    for (int i = 0; i < resultPp.size(); i++)
    {
        for (int j = 0; j < resultPp[0].size(); j++)
        {
            MKL_Complex8 value;
            value.real = (resultPp[i][j].real * DIP::dZ) / DIP::dX;
            value.imag = (resultPp[i][j].imag * DIP::dZ) / DIP::dX;
            complex<float> tmp(value.real, value.imag);

            
            //------  the process below implemented the atan() of a comlex number  ------

            //complex<float> tmp(-0.0681083f, -7.81696E-07f);   // for debug
            complex<float> right(0.0f - tmp.imag(), tmp.real());
            complex<float> tmp1(1 - right.real(), 0 - right.imag());
            complex<float> tmp2(1 + right.real(), 0 + right.imag());

            //log a complex number
            //log(x + iy) = log(sqrt(pow(x, 2) + pow(y, 2))) + i atan(y/x)
            complex<float> logTmp1(logf(powf(powf(tmp1.real(),2) + powf(tmp1.imag(),2),0.5f)), atan(tmp1.imag()/tmp1.real()));
            complex<float> logTmp2(logf(powf(powf(tmp2.real(),2) + powf(tmp2.imag(),2),0.5f)), atan(tmp2.imag()/tmp2.real()));

            complex<float> log1MinusLog2(logTmp1.real() - logTmp2.real(), logTmp1.imag() - logTmp2.imag());

            complex<float> multiplier(0.0f, 0.5f);
            
            float atanReal = (multiplier.real() * log1MinusLog2.real()) - (multiplier.imag() * log1MinusLog2.imag());
		    float atanImag = (multiplier.real() * log1MinusLog2.imag()) + (multiplier.imag() * log1MinusLog2.real());

            complex<float> complexAtan(atanReal, atanImag);
            //---------------------------------------------------------------------------

            MKL_Complex8 b;
            b.real = complexAtan.real();
            b.imag = complexAtan.imag();

            float chk = (float)(getMagnitude(b));

            if(b.real < 0)
                newSlice[i][j] = (float)(getMagnitude(b) * -1 * 180 / PI);
            else
                newSlice[i][j] = (float)(getMagnitude(b) * 180 / PI);

            // if(b.real < 0)
            //     newSlice[i][j] = (float)((b.real * -1) * -1 * 180 / PI);
            // else
            //     newSlice[i][j] = (float)((b.real * -1) * 180 / PI);            
        }
    }

    return newSlice;    

}

vector<vector<MKL_Complex8>> DIP::Convolution(const vector<MKL_Complex8>& kernelWin, vector<vector<MKL_Complex8>> input) {

    DIP dip;
    
    int colSize = (int)(input[0].size());
    int rowSize = (int)(input.size());
    vector<vector<MKL_Complex8>> padded = dip.PaddingArray(input, rowSize, colSize, dip.GetSizeKernel(dip.windowZ), dip.GetSizeKernel(dip.windowX), false);

    //convert matrix format into data array
    vector<MKL_Complex8> paddedArray(padded.size() * padded[0].size());
    vector<MKL_Complex8> conv(padded.size() * padded[0].size());
    int temp = 0;

    for(int r=0; r<padded.size(); r++){
        for(int c=0; c<padded[0].size(); c++){
            paddedArray[temp] = padded[r][c];
            temp += 1;
        }
    }


	auto kn = paddedArray.size();
    long padColSize = (long)(padded[0].size());
    long padRowSize = (long)(padded.size());

    //Fourier Forward - MatLab
	MKL_LONG fft[2] { padRowSize, padColSize };
	MKL_LONG status;
	DFTI_DESCRIPTOR_HANDLE fh = NULL;

	status = DftiCreateDescriptor(&fh, DFTI_SINGLE, DFTI_COMPLEX, 2, fft);
	status = DftiCommitDescriptor(fh);
	status = DftiComputeForward(fh, paddedArray.data()); // forward FFT
	status = DftiFreeDescriptor(&fh);

	// pointwise multiple kernel x inputMatrix (x + yi)*(u + vi) = (xu - yv) + (xv + yu)i
	for (size_t i = 0; i < kn; i++) {
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
    vector<vector<MKL_Complex8>> resultConv(padded.size());
    for(auto i=0; i<padded.size(); i++){
        resultConv[i] = vector<MKL_Complex8>(padded[0].size());
    }

    temp = 0;

    for(int r=0; r<padded.size(); r++){
        for(int c=0; c<padded[0].size(); c++){
            resultConv[r][c] = conv[temp];
            temp += 1;
        }
    }

    //return actual data size based on input data size
    vector<vector<MKL_Complex8>> result(rowSize);
    for(auto i=0; i<rowSize; i++){
        result[i] = vector<MKL_Complex8>(colSize);
    }

    for(int r=0; r<rowSize; r++){
        for(int c=0; c<colSize; c++){
            result[r][c] =  resultConv[r][c];
        }
    }

    return result;
}

vector<vector<MKL_Complex8>> DIP::PaddingArray(vector<vector<MKL_Complex8>> input, int colSize, int rowSize, int winX, int winZ,  bool circ) {

    int xMmid = (int)floor(winX / 2.0);
    int yMmid = (int)floor(winZ / 2.0);

    colSize += xMmid;
    rowSize += yMmid;

    vector<vector<MKL_Complex8>> extended = ExtendArray(input, colSize, rowSize);

    if (!circ) {
        return extended;
    }


    int mcMx = xMmid;
    int mcMy = yMmid;
    
    int meMx = mcMx + colSize - 1;
    int meMy = mcMy + rowSize - 1;

    vector<vector<MKL_Complex8>> newKernal = ExIndex(extended, mcMx, meMx - 1, mcMy, meMy - 1); 

    return newKernal;
    
}

vector<vector<MKL_Complex8>> DIP::ExtendArray(vector<vector<MKL_Complex8>> raw, int col, int row) {

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

vector<vector<MKL_Complex8>> DIP::ExIndex(vector<vector<MKL_Complex8>> m, int mcXFrom, int mcXTo, int mcYFrom, int mcYTo){

     vector<vector<MKL_Complex8>> newMatrix(m.size(), vector<MKL_Complex8>(m[0].size())); 

    for (int i = 0, x=mcXFrom; i < m.size(); x++, i++)  //row
    {
        for (int j = 0, y = mcYFrom; j < m[0].size(); y++, j++)  //col
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

vector<vector<MKL_Complex8>> DIP::GenerateKernel(vector<vector<MKL_Complex8>> raw, int width, int height) {

    vector<vector<MKL_Complex8>> expandRaw = PaddingArray(raw, width, height, (int)(raw.size()), (int)(raw[0].size()), true);

    vector<MKL_Complex8> paddedArray(expandRaw.size() * expandRaw[0].size());
    vector<MKL_Complex8> kernel(expandRaw.size() * expandRaw[0].size());
    int temp = 0;

    for(int r=0; r<expandRaw.size(); r++){
        for(int c=0; c<expandRaw[0].size(); c++){
            paddedArray[temp] = expandRaw[r][c];
            temp += 1;
        }
    }

	auto kn = paddedArray.size();
    long padColSize = (long)(expandRaw[0].size());
    long padRowSize = (long)(expandRaw.size());

	MKL_LONG fft[2] { padRowSize, padColSize };
	MKL_LONG status;
	DFTI_DESCRIPTOR_HANDLE fh = NULL;

	status = DftiCreateDescriptor(&fh, DFTI_SINGLE, DFTI_COMPLEX, 2, fft);
	status = DftiCommitDescriptor(fh);
	status = DftiComputeForward(fh, paddedArray.data()); // forward FFT
	status = DftiFreeDescriptor(&fh);

    vector<vector<MKL_Complex8>> result(expandRaw.size());
    for(auto i=0; i<expandRaw.size(); i++){
        result[i] = vector<MKL_Complex8>(expandRaw[0].size());
    }

    temp = 0;

    for(int r=0; r<expandRaw.size(); r++){
        for(int c=0; c<expandRaw[0].size(); c++){
            result[r][c] = paddedArray[temp];
            temp += 1;
        }
    }

    return result;

}

int DIP::GetSizeKernel(int size) {

    return (2 * size) + 1;

}

float DIP::rotgauss(int x, int y, double theta, double sigmax, double sigmay) {

    theta = (theta / 180) * PI;
    double xm = (x) * cos(theta) - (y) * sin(theta);
    double ym = (x) * sin(theta) + (y) * cos(theta);
    double u = pow((xm / sigmax), 2) + pow((ym / sigmay), 2);
    return (float)exp((-1.0 * u) / 2.0);

}

vector<vector<MKL_Complex8>> DIP::CustGaussian2D(int winX, int winZ, double theta) {

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

    vector<vector<MKL_Complex8>> kernel(p.size(), vector<MKL_Complex8>(p[0].size()));
    
    for (int i = 0; i < p.size(); i++)
    {
        for (int j = 0; j < p[0].size(); j++)
        {
            kernel[i][j].real = p[i][j] / sumofgauss;
        }
    }

    return kernel;

}

vector<vector<float>> DIP::CustGaussianDouble2D(int winX, int winZ, double theta) {

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

    vector<vector<float>> kernel(p.size(), vector<float>(p[0].size()));
    
    for (int i = 0; i < p.size(); i++)
    {
        for (int j = 0; j < p[0].size(); j++)
        {
            kernel[i][j] = p[i][j] / sumofgauss;
        }
    }

    return kernel;

}

MKL_Complex8 DIP::ComplexDivision(MKL_Complex8 dividend, MKL_Complex8 divisor) {

    double conjugate = pow(divisor.real, 2) + pow(divisor.imag, 2);

    MKL_Complex8 multiply1; 
    multiply1.real = divisor.real * dividend.real;
    multiply1.imag = divisor.real * dividend.imag;

    MKL_Complex8 multiply2; 
    multiply2.real = (divisor.imag * -1) * dividend.imag * -1;
    multiply2.imag = (divisor.imag * -1) * dividend.real;

    MKL_Complex8 result; 
    result.real = (float)((multiply1.real + multiply2.real) / conjugate);
    result.imag = (float)((multiply1.imag + multiply2.imag) / conjugate); 

    return result;   

}

float DIP::getMagnitude(const MKL_Complex8& trace) {

	double real = trace.real;
	double imag = trace.imag;
	if (std::isinf(real) || std::isinf(imag) || std::isnan(real) || std::isnan(imag))
		return 0;
	double v = sqrt((real * real) + (imag * imag));
	return (float)(std::isinf(v) || std::isnan(v) ? 0 : v);

}



