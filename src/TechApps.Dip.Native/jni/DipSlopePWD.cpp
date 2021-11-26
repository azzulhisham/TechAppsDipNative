#include "dip.h"
#include <iomanip>

using namespace std;

const int COLSIZE = 5;
const int ROWSIZE = 4;



vector<vector<int>> ExtendArray(vector<vector<int>> raw, int col, int row) {

	//vector<vector<MKL_Complex8>> expanded = new Complex32[col, row];
//    for (int i = 0; i < col; i++)
//    {
//        for (int j = 0; j < row; j++)
//        {
//            if (i >= raw.GetLength(0) || j >= raw.GetLength(1))
//                continue;
//
//            expanded[i, j] = raw[i, j];
//        }
//    }


    for (int i = 0; i < ROWSIZE; ++i) {
        for (int j = 0; j < COLSIZE; ++j) {
            raw[i][j] *= 2;
        }
    }

    //return reinterpret_cast<int *>(raw);
    return raw;
}


void PaddingArray(vector<vector<int>> input, int colSize, int rowSize, int winX, int winZ,  bool circ = false) {

    int xMmid = (int)floor(winX / 2.0);
    int yMmid = (int)floor(winZ / 2.0);

    colSize += xMmid;
    rowSize += yMmid;

    vector<vector<int>> extended = ExtendArray(input, colSize, rowSize);

    cout << "modified array\n";
    for (int i = 0; i < extended.size(); ++i) {
        cout << " [ ";
        for (int j = 0; j < extended[0].size(); ++j) {
            //cout << setw(2) << *(extended + (i * COLSIZE) + j) << ", ";
            cout << setw(2) << extended[i][j] << ", ";
        }
        cout << "]" << endl;
    }  
}


int main() {

	vector<vector<int>> c_array = {{ 1, 2, 3, 4, 10 },
	                               { 5, 6, 7, 8, 50 },
	                               { 9, 10, 11, 12, 90 },
	                               { 13, 14, 15, 16, 130 }};

	PaddingArray(c_array, COLSIZE, ROWSIZE, 1, 1, false);

    cout << c_array.size() << "*" << c_array[0].size() << endl;
	cout << "App end..........." << endl;
	return 0;
}


