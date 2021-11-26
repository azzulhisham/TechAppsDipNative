#pragma once

#include <string>
#include <vector>
#include <cmath>
#include "Row.h"

class Segment
{
public:
	static std::vector<std::vector<double>> Window();
	static std::vector<std::vector<Row>> Init(int length = 3);
	
private:
	static std::vector<Row> Segment1(int length, int mid);
	static std::vector<Row> Segment2(int length, int mid);
	static std::vector<Row> Segment3(int length, int mid);
	static std::vector<Row> Segment4(int length, int mid);
	static std::vector<Row> Segment5(int length, int mid);
	static std::vector<Row> Segment6(int length, int mid);
	static std::vector<Row> Segment7(int length, int mid);
	static std::vector<Row> Segment8(int length, int mid);
	// static std::vector<Row> Segment9(int length, int mid);
	// static std::vector<Row> Segment10(int length, int mid);
	// static std::vector<Row> Segment11(int length, int mid);
	// static std::vector<Row> Segment12(int length, int mid);
	// static std::vector<Row> Segment13(int length, int mid);
	// static std::vector<Row> Segment14(int length, int mid);
	// static std::vector<Row> Segment15(int length, int mid);
	// static std::vector<Row> Segment16(int length, int mid);
};
