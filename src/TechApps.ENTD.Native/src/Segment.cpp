#include "Segment.h"

std::vector<std::vector<double>> Segment::Window()
{
	std::vector<std::vector<double>> kernel {
	        {1.21306131942527, 1.60653065971263, 1.21306131942527},
                {1.60653065971263, 2               , 1.60653065971263},
                {1.21306131942527, 1.60653065971263, 1.21306131942527},
	};
	return kernel;
}

std::vector<std::vector<Row>> Segment::Init(int length)
{
	auto mid = static_cast<int>(std::round(((length + 1) * 2 + 1) / 2.0));
	
	std::vector<std::vector<Row>> D_test {
        	Segment1(length, mid),
	        Segment2(length, mid),
        	Segment3(length, mid),
	        Segment4(length, mid),
        	Segment5(length, mid),
	        Segment6(length, mid),
        	Segment7(length, mid),
	        Segment8(length, mid),
	};

	return D_test;
}

std::vector<Row> Segment::Segment1(int length, int mid)
{
	auto rows = std::vector<Row>();

	for (int row = 0, a = mid; row < length; row++, a--)
	{
		Row R ;
		R.Xi = std::vector<int>();
		R.Xj = std::vector<int>(); 

		for (int count = 0, b = 2; count < 5; count++, b--)
		{
			R.Xi.push_back(a);
			R.Xj.push_back(mid - b);
		}
		rows.push_back(R);
	}
	return rows;
}

std::vector<Row> Segment::Segment2(int length, int mid)
{ 

	auto rows = std::vector<Row>();

	for (int row = 0, x = mid, y = mid; row < length; row++, x--, y += row % 2)
	{
		Row R;
		R.Xi = std::vector<int>();
		R.Xj = std::vector<int>();
		R.Xi.push_back(x - 1);
		R.Xj.push_back(y - 2);
		R.Xi.push_back(x - 1);
		R.Xj.push_back(y - 1);
		R.Xi.push_back(x - 0);
		R.Xj.push_back(y - 0);
		R.Xi.push_back(x + 1);
		R.Xj.push_back(y + 1);
		R.Xi.push_back(x + 1);
		R.Xj.push_back(y + 2);
		rows.push_back(R);
	}
	return rows;
}

std::vector<Row> Segment::Segment3(int length, int mid)
{
	auto rows = std::vector<Row>();

	for (int row = 0, x = mid, y = mid; row < length; row++, x--, y++)
	{
		Row R;
		R.Xi = std::vector<int>();
		R.Xj = std::vector<int>();

		for (int count = 0, a = 2, b = 2; count < 5; count++, a--, b--)
		{
			R.Xi.push_back(x - a);
			R.Xj.push_back(y - b);
		}
		rows.push_back(R);
	}
	return rows;
}

std::vector<Row> Segment::Segment4(int length, int mid)
{
	auto rows = std::vector<Row>();

	for (int row = 0, x = mid, y = mid; row < length; row++, x -= (row + 1) % 2, y++)
	{
		Row R;
		R.Xi = std::vector<int>();
		R.Xj = std::vector<int>();

		R.Xi.push_back(x - 2);
		R.Xj.push_back(y - 1);
		R.Xi.push_back(x - 1);
		R.Xj.push_back(y - 1);
		R.Xi.push_back(x - 0);
		R.Xj.push_back(y - 0);
		R.Xi.push_back(x + 1);
		R.Xj.push_back(y + 1);
		R.Xi.push_back(x + 2);
		R.Xj.push_back(y + 1);
		rows.push_back(R);
	}
	return rows;
}

std::vector<Row> Segment::Segment5(int length, int mid)
{
	auto rows = std::vector<Row>();

	for (int row = 0, b = mid; row < length; row++, b++)
	{
		Row R;

		R.Xi = std::vector<int>();
		R.Xj = std::vector<int>();

		for (int count = 0, a = 2; count < 5; count++, a--)
		{
			R.Xi.push_back(mid - a);
			R.Xj.push_back(b);
		}
		rows.push_back(R);
	}
	return rows;
}

std::vector<Row> Segment::Segment6(int length, int mid)
{
	auto rows = std::vector<Row>();

	for (int row = 0, x = mid, y = mid; row < length; row++, x += row % 2, y++)
	{
		Row R;
		R.Xi = std::vector<int>();
		R.Xj = std::vector<int>();

		R.Xi.push_back(x - 2);
		R.Xj.push_back(y + 1);
		R.Xi.push_back(x - 1);
		R.Xj.push_back(y + 1);
		R.Xi.push_back(x + 0);
		R.Xj.push_back(y + 0);
		R.Xi.push_back(x + 1);
		R.Xj.push_back(y - 1);
		R.Xi.push_back(x + 2);
		R.Xj.push_back(y - 1);
		rows.push_back(R);
	}
	return rows;
}

std::vector<Row> Segment::Segment7(int length, int mid)
{
	auto rows = std::vector<Row>();

	for (int row = 0, x = mid, y = mid; row < length; row++, x++, y++)
	{
		Row R;

		R.Xi = std::vector<int>();
		R.Xj = std::vector<int>();
		for (int count = 0, a = 2, b = 2; count < 5; count++, b--, a--)
		{
			R.Xi.push_back(x - a);
			R.Xj.push_back(y + b);
		}
		rows.push_back(R);
	}
	return rows;
}

std::vector<Row> Segment::Segment8(int length, int mid)
{
	auto rows = std::vector<Row>();

	for (int row = 0, x = mid, y = mid; row < length; row++, x++, y += (row + 1) % 2)
	{
		Row R;

		R.Xi = std::vector<int>();
		R.Xj = std::vector<int>();

		R.Xi.push_back(x - 1);
		R.Xj.push_back(y + 2);
		R.Xi.push_back(x - 1);
		R.Xj.push_back(y + 1);
		R.Xi.push_back(x + 0);
		R.Xj.push_back(y + 0);
		R.Xi.push_back(x + 1);
		R.Xj.push_back(y - 1);
		R.Xi.push_back(x + 1);
		R.Xj.push_back(y - 2);
		rows.push_back(R);
	}
	return rows;
}

// std::vector<Row> Segment::Segment9(int length, int mid)
// {
// 	auto rows = std::vector<Row>();

// 	for (int row = 0, a = mid; row < length; row++, a++)
// 	{
// 		Row R;

// 		R.Xi = std::vector<int>();
// 		R.Xj = std::vector<int>();

// 		for (int count = 0, b = 2; count < 5; count++, b--)
// 		{
// 			R.Xi.push_back(a);
// 			R.Xj.push_back(mid + b);
// 		}
// 		rows.push_back(R);
// 	}
// 	return rows;
// }

// std::vector<Row> Segment::Segment10(int length, int mid)
// {
// 	auto rows = std::vector<Row>();

// 	for (int row = 0, x = mid, y = mid; row < length; row++, x++, y -= row % 2)
// 	{
// 		Row R;

// 		R.Xi = std::vector<int>();
// 		R.Xj = std::vector<int>();

// 		R.Xi.push_back(x + 1);
// 		R.Xj.push_back(y + 2);
// 		R.Xi.push_back(x + 1);
// 		R.Xj.push_back(y + 1);
// 		R.Xi.push_back(x + 0);
// 		R.Xj.push_back(y + 0);
// 		R.Xi.push_back(x - 1);
// 		R.Xj.push_back(y - 1);
// 		R.Xi.push_back(x - 1);
// 		R.Xj.push_back(y - 2);
// 		rows.push_back(R);
// 	}
// 	return rows;
// }

// std::vector<Row> Segment::Segment11(int length, int mid)
// {
// 	auto rows = std::vector<Row>();

// 	for (int row = 0, x = mid, y = mid; row < length; row++, x++, y--)
// 	{
// 		Row R;

// 		R.Xi = std::vector<int>();
// 		R.Xj = std::vector<int>();

// 		for (int count = 0, a = 2, b = 2; count < 5; count++, a--, b--)
// 		{
// 			R.Xi.push_back(x + a);
// 			R.Xj.push_back(y + b);
// 		}
// 		rows.push_back(R);
// 	}
// 	return rows;
// }

// std::vector<Row> Segment::Segment12(int length, int mid)
// {
// 	auto rows = std::vector<Row>();

// 	for (int row = 0, x = mid, y = mid; row < length; row++, x += row % 2, y--)
// 	{
// 		Row R;

// 		R.Xi = std::vector<int>();
// 		R.Xj = std::vector<int>();

// 		R.Xi.push_back(x + 2);
// 		R.Xj.push_back(y + 1);
// 		R.Xi.push_back(x + 1);
// 		R.Xj.push_back(y + 1);
// 		R.Xi.push_back(x - 0);
// 		R.Xj.push_back(y - 0);
// 		R.Xi.push_back(x - 1);
// 		R.Xj.push_back(y - 1);
// 		R.Xi.push_back(x - 2);
// 		R.Xj.push_back(y - 1);
// 		rows.push_back(R);
// 	}
// 	return rows;
// }

// std::vector<Row> Segment::Segment13(int length, int mid)
// {
// 	auto rows = std::vector<Row>();

// 	for (int row = 0, b = mid; row < length; row++, b--)
// 	{
// 		Row R;

// 		R.Xi = std::vector<int>();
// 		R.Xj = std::vector<int>();

// 		for (int count = 0, a = 2; count < 5; count++, a--)
// 		{
// 			R.Xi.push_back(mid + a);
// 			R.Xj.push_back(b);
// 		}
// 		rows.push_back(R);
// 	}
// 	return rows;
// }

// std::vector<Row> Segment::Segment14(int length, int mid)
// {
// 	auto rows = std::vector<Row>();

// 	for (int row = 0, x = mid, y = mid; row < length; row++, x -= row % 2, y--)
// 	{
// 		Row R;
// 		R.Xi = std::vector<int>();
// 		R.Xj = std::vector<int>();

// 		R.Xi.push_back(x + 2);
// 		R.Xj.push_back(y - 1);
// 		R.Xi.push_back(x + 1);
// 		R.Xj.push_back(y - 1);
// 		R.Xi.push_back(x + 0);
// 		R.Xj.push_back(y + 0);
// 		R.Xi.push_back(x - 1);
// 		R.Xj.push_back(y + 1);
// 		R.Xi.push_back(x - 2);
// 		R.Xj.push_back(y + 1);
// 		rows.push_back(R);
// 	}
// 	return rows;
// }

// std::vector<Row> Segment::Segment15(int length, int mid)
// {
// 	auto rows = std::vector<Row>();

// 	for (int row = 0, x = mid, y = mid; row < length; row++, x--, y--)
// 	{
// 		Row R;

// 		R.Xi = std::vector<int>();
// 		R.Xj = std::vector<int>();

// 		for (int count = 0, a = 2, b = 2; count < 5; count++, a--, b--)
// 		{
// 			R.Xi.push_back(x + a);
// 			R.Xj.push_back(y - b);
// 		}
// 		rows.push_back(R);
// 	}
// 	return rows;
// }

// std::vector<Row> Segment::Segment16(int length, int mid)
// {
// 	auto rows = std::vector<Row>();

// 	for (int row = 0, x = mid, y = mid; row < length; row++, x--, y -= row % 2)
// 	{
// 		Row R;

// 		R.Xi = std::vector<int>();
// 		R.Xj = std::vector<int>();

// 		R.Xi.push_back(x + 1);
// 		R.Xj.push_back(y - 2);
// 		R.Xi.push_back(x + 1);
// 		R.Xj.push_back(y - 1);
// 		R.Xi.push_back(x + 0);
// 		R.Xj.push_back(y + 0);
// 		R.Xi.push_back(x - 1);
// 		R.Xj.push_back(y + 1);
// 		R.Xi.push_back(x - 1);
// 		R.Xj.push_back(y + 2);
// 		rows.push_back(R);
// 	}
// 	return rows;
// }
