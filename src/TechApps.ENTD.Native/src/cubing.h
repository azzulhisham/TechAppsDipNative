#include<vector>

using namespace std;

vector<vector<vector<float>>> getSubCube(vector<vector<vector<float>>> seisCube,int iPos , int jPos, int kPos);

vector<vector<float>> getSurface1(vector<vector<vector<float>>> cube);

vector<vector<float>> getSurface2(vector<vector<vector<float>>> cube);

vector<vector<float>> getSurface3(vector<vector<vector<float>>> cube);

vector<vector<float>> getSurface4(vector<vector<vector<float>>> cube);

vector<vector<float>> getSurface5(vector<vector<vector<float>>> cube);

vector<vector<float>> getSurface6(vector<vector<vector<float>>> cube);

vector<vector<float>> getSurface7(vector<vector<vector<float>>> cube);

vector<vector<float>> getSurface8(vector<vector<vector<float>>> cube);

vector<vector<float>> getSurface9(vector<vector<vector<float>>> cube);

vector<vector<float>> expand(vector<vector<float>>, int padding = 1);