#include "dip.h"
#include "index3d.h"

#include <iomanip>

using namespace std;

void Index3D::Test(Index3D input) {
    cout << input.I << ", " << input.J << ", " << input.K << endl;
}

int main() {

    DIP dip;

	vector<vector<int>> inputMatrix = {{ -1, 2, 3, -4, 5, -6, 7, 8, 9, -10, 11 },
                                   { -12, 13, 14, -15, 16, -16, 18, 19, 20, -21, 22 },
                                   { -22, 23, 24, -25, 26, -26, 28, 29, 30, -31, 32 },
                                   { -32, 33, 34, -35, 36, -36, 38, 39, 40, -41, 42 },
                                   { -32, 33, 34, -35, 36, -36, 38, 39, 40, -41, 42 },
                                   { -52, 53, 54, -55, 56, -56, 58, 59, 60, -61, 22 },
            };

    vector<vector<vector<int>>> in = { 
                                        { { 1, 2, 3, -1 }, { 4, 5, 6, -2 }, { 4, 5, 6, -2 } },
                                        { { 7, 8, 9, -3 }, { 10, 11, 12, -4 }, { 4, 5, 6, -2 } } 
                                     };

    cout << in.size() << endl; 
    cout << in[0].size() << endl; 
    cout << in[0][0].size() << endl; 
    cout << in[1][2][2] << endl; 
    cout << endl;

    Index3D aa;
    Index3D bb;

    aa.InitializeIJK(in[1][1][2], in[1][2][2], in[1][2][3]); 
    bb.InitializeV(in[0][0][3]);
    //aa.I = in[1][2][2];
    //aa.J = in[1][2][3];  
    //aa.K = 0;

    cout << aa.I << ", " << aa.J << ", " << aa.K << endl;
    cout << bb.I << ", " << bb.J << ", " << bb.K << endl;

    bb.Test(aa);

    //unit testing
    // vector<vector<MKL_Complex8>> c_array(inputMatrix.size(), vector<MKL_Complex8>(inputMatrix[0].size()));
    // for (int i = 0; i < c_array.size(); i++) {
    //     for (int j = 0; j < c_array[0].size(); j++) {
    //         c_array[i][j].real = (float)inputMatrix[i][j];
    //     }
    // }
                             
    // vector<vector<MKL_Complex8>> kernel = dip.CustGaussian2D(dip.windowX, dip.windowZ, 0);
    // vector<vector<MKL_Complex8>> kernelWin = dip.GenerateKernel(kernel, (int)(inputMatrix.size()), (int)(inputMatrix[0].size()));

    // vector<MKL_Complex8> kernelWinArray(kernelWin.size() * kernelWin[0].size());
    // int temp = 0;

    // for(int r=0; r<kernelWin.size(); r++){
    //     for(int c=0; c<kernelWin[0].size(); c++){
    //         kernelWinArray[temp] = kernelWin[r][c];
    //         temp += 1;
    //     }
    // }

    // cout << "Kernel win : row=" << c_array.size() << " x col=" << c_array[0].size() << endl;


    //vector<vector<MKL_Complex8>> result = dip.Convolution(kernelWinArray, c_array); 
	//vector<vector<MKL_Complex8>> result = dip.PaddingArray(c_array, 11, 6, 4, 4, false);

    // cout.precision(11);
    // for (int r = 0; r < result.size(); r++)
    // {
    //     for (int c = 0; c < result[0].size(); c++)
    //     {
    //         cout << result[r][c].real << "\t";
    //     }
    //     cout << endl;
    // }    


	return 0;
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

vector<vector<MKL_Complex8>> DIP::PaddingArray(vector<vector<MKL_Complex8>> input, int rowSize, int colSize, int winX, int winZ,  bool circ) {

    int xMmid = (int)floor(winX / 2.0);
    int yMmid = (int)floor(winZ / 2.0);

    colSize += xMmid;
    rowSize += yMmid;

    vector<vector<MKL_Complex8>> extended = ExtendArray(input, rowSize, colSize);

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

vector<vector<MKL_Complex8>> DIP::ExtendArray(vector<vector<MKL_Complex8>> raw, int row, int col) {

    vector<vector<MKL_Complex8>> expanded(row);
    for(auto i=0; i<row; i++){
        expanded[i] = vector<MKL_Complex8>(col);
    }

    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            if (i >= raw.size() || j >= raw[0].size())
                continue;

            expanded[i][j] = raw[i][j];
        }
    }

    return expanded;

}

vector<vector<MKL_Complex8>> DIP::ExIndex(vector<vector<MKL_Complex8>> m, int mcXFrom, int mcXTo, int mcYFrom, int mcYTo){

    vector<vector<MKL_Complex8>> newMatrix(m.size());
    for(auto i=0; i<m.size(); i++){
        newMatrix[i] = vector<MKL_Complex8>(m[0].size());
    }    

    for (int i = 0, x=mcYFrom; i < m.size(); x++, i++)  //row
    {
        for (int j = 0, y = mcXFrom; j < m[0].size(); y++, j++)  //col
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

    theta = (theta / 180) * M_PI;
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

void Index3D::InitializeIJK(int i, int j, int k){
    
    I = i;
    J = j;
    K = k;

}

void Index3D::InitializeV(int v){
    
    I = v;
    J = v;
    K = v;

}


