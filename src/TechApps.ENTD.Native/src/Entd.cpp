#include "Entd.h"

const vector<vector<double>> ENTD::kernel = Segment::Window();

std::string ENTD::toString() {
    std::stringstream buffer;
    buffer << "Unit \t\t: " << Unit << std::endl;
    return buffer.str();
}

void ENTD::Initialize()
{
	_padding = Unit + 1;
	templates = Segment::Init(Unit);
}

vector<vector<double>> ENTD::Convolution2d(const vector<vector<float>> &values)
{
	auto padding = kernel.size() / 2;
	vector<vector<double>> convs(values.size() - padding * 2 , vector<double>(values.size() - padding * 2));
	auto vn = values.size() == 0 ? 0 : values[0].size();
	auto kn = kernel.size() == 0 ? 0 : kernel[0].size();

	for (int i = padding; i < values.size() - padding; i++) {
		for (int j = padding; j < vn - padding; j++) {
			double conv = 0;
			for (int k = 0; k < kernel.size(); k++) {
				for (int l = 0; l < kn; l++) {
					conv += values[i + l - padding][j + k - padding] * kernel[k][l];
				}
			}
			convs[i - padding][j - padding] = conv;
		}
	}
	return convs;
}

int ENTD::Detect(int plane, vector<int> &density, const vector<vector<float>> &values, int px, int py)
{
	auto convs = Convolution2d(values);
	auto mid = Unit + 1;	
	auto point = convs[mid + px][mid + py];
	auto convergences = 0;
	for (int d = 0; d < templates.size(); d++)
	{
		auto template_Keyword = templates[d];
		auto v = 0;
		for (int r = 0; r < template_Keyword.size(); r++)
		{
			vector<double> x(5);
			for (int o = 0; o < x.size(); o++)
			{
				auto cx = template_Keyword[r].Xi[o] - 1 + px;
				auto cy = template_Keyword[r].Xj[o] - 1 + py;
				x[o] = convs[cx][cy];
			}

			auto c1 = x[0] + x[4] - 2 * x[2];
			auto c2 = x[1] + x[3] - 2 * x[2];
			auto c3 = x[0] + x[2] - 2 * x[1];
			auto c4 = x[4] + x[2] - 2 * x[3];

			v += (c1 > point && c2 > c3 && c2 > c4) ? 1 : 0;
		}

		if (v == Unit)
		{
			convergences++;
			if (plane == 0)
				density[d]++;
		}
	}
	return convergences;
}
