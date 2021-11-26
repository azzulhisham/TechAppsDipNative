//#pragma once
#include "Segment.h"
#include <vector>
#include <tuple>
// #include "ArrayToBit.h"
// #include <mkl.h>
// #include <mkl_dfti.h>
#include <iostream>
#include <sstream>

using namespace std;

class ENTD
{
public:
	int Unit = 0;
	double Rotation = 0;

	void Initialize();
	int Detect(int plane, vector<int> &density, const vector<vector<float>> &convs, int px = 0, int py = 0);
	std::string toString();

private:
	static const vector<vector<double>> kernel;
	vector<vector<Row>> templates;
	int _padding = 0;
	static vector<vector<double>> Convolution2d(const vector<vector<float>> &values);
};
