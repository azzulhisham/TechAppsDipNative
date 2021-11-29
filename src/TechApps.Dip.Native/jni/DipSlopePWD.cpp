#include "dip.h"
#include <iomanip>

using namespace std;


int main() {

    DIP dip;

	// vector<vector<int>> c_array = {{ 1, 2, 3, 4, 10 },
	//                                { 5, 6, 7, 8, 50 },
	//                                { 9, 10, 11, 12, 90 },
	//                                { 13, 14, 15, 16, 130 }};

    // int colSize = (int)(c_array[0].size());
    // int rowSize = (int)(c_array.size());                               

	// dip.PaddingArray(c_array, colSize, rowSize, 1, 1, false);

	return 0;
}

vector<vector<int>> DIP::PaddingArray(vector<vector<int>> input, int colSize, int rowSize, int winX, int winZ,  bool circ) {

    int xMmid = (int)floor(winX / 2.0);
    int yMmid = (int)floor(winZ / 2.0);

    colSize += xMmid;
    rowSize += yMmid;

    vector<vector<int>> extended = ExtendArray(input, colSize, rowSize);

    if (!circ) {
        return extended;
    }

    int mcMx = xMmid;
    int mcMy = yMmid;
    
    int meMx = mcMx + colSize - 1;
    int meMy = mcMy + rowSize - 1;

    vector<vector<int>> newKernal = ExIndex(extended, mcMx, meMx - 1, mcMy, meMy - 1);

    return newKernal;
    
}

vector<vector<int>> DIP::ExtendArray(vector<vector<int>> raw, int col, int row) {

    vector<vector<int>> expanded(row);
    for(auto i=0; i<row; i++){
        expanded[i] = vector<int>(col);
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

vector<vector<int>> DIP::ExIndex(vector<vector<int>> m, int mcXFrom, int mcXTo, int mcYFrom, int mcYTo){

    vector<vector<int>> newMatrix(m.size());
    for(auto i=0; i<m.size(); i++){
        newMatrix[i] = vector<int>(m[0].size());
    }    

    for (int i = 0, x=mcYFrom; i < m.size(); x++, i++)  //row
    {
        for (int j = 0, y = mcXFrom; j < m[0].size(); y++, j++)  //col
        {
            if (x == m.size())
                x = 0;

            if (y == m[0].size())
                y = 0;

            newMatrix[i][j] = m[i][j];
        }
    }

    return newMatrix;
}





