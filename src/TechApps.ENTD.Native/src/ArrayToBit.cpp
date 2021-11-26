#include "ArrayToBit.h"

int ArrayToBit::FromBit(int storage, int index)
{
	int mask = 1 << index;

	return (storage & mask) >> index;
}

int ArrayToBit::SumBit(int storage, int range)
{
	auto value = 0;
	for (int i = 0; i < range; i++)
	{
		value += ArrayToBit::FromBit(storage, i);
	}
	return value;
}

int ArrayToBit::ToBit(int storage, int index, int value)
{
	int mask = 1 << index;

	return (storage & ~mask) | (value << index);
}

std::vector<int> ArrayToBit::ToArray(int storage, int range)
{
	auto array = std::vector<int>(range);
	for (int i = 0; i < array.size(); i++)
	{
		array[i] = ArrayToBit::FromBit(storage, i);
	}
	return array;
}

int ArrayToBit::ToBit(std::vector<int> &array)
{
	int storage = 0;
	for (int i = 0; i < array.size(); i++)
	{
		storage = ArrayToBit::ToBit(storage, i, array[i]);
	}
	return storage;
}
