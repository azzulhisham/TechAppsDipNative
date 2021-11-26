#pragma once

#include <vector>

class ArrayToBit final
{
public:
	static int FromBit(int storage, int index);

	static int SumBit(int storage, int range = 8);

	static int ToBit(int storage, int index, int value);

	static std::vector<int> ToArray(int storage, int range = 8);

	static int ToBit(std::vector<int> &array);
};
