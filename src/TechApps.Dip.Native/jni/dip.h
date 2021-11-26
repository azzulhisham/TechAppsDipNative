#include <mkl.h>
#include <mkl_dfti.h>
#include <vector>
#include <cmath>
#include <limits>
#include <iostream>
#include <sstream>

using std::vector;

class DIP
{
public:
    void PaddingArray(vector<vector<int>> input, int colSize, int rowSize, int winX, int winZ,  bool circ);
    vector<vector<int>> ExtendArray(vector<vector<int>> raw, int col, int row);    
};
