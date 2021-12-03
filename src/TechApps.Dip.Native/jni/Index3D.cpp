#include "dip.h"


using namespace std;

Index3D::Index3D(int i, int j, int k){
    
    I = i;
    J = j;
    K = k;

}

Index3D::Index3D(int v){

    I = v;
    J = v;
    K = v;

}

void Index3D::Test() {
    cout << I << ", " << J << ", " << K << endl;
}