#include <vector>
// #include <mkl.h>

using std::vector;

size_t NextPow2(size_t size)
{
	size--;
	size |= size >> 1;
	size |= size >> 2;
	size |= size >> 4;
	size |= size >> 8;
	size |= size >> 16;
	return size = size + 1;
}

// vector<float> InitKernel(int unit = 4)
// {
// 	size_t width = (unit + 1) * 2 + 1;
// 	size_t size = NextPow2(width);
// 	vector<float> kernel(size * size); // row major to match matlab
// 	auto win = Segment::Window();
// 	auto half = win.size() / 2;
	
// 	for (size_t i = 0; i < win.size(); i++) {
// 		for (size_t j = 0; j < win.size(); j++) {
// 			if (i < half) {
				
// 			}
// 		}
// 	}

// 	MKL_LONG fft[2] = { size,  size }; // col major col x row
// 	MKL_LONG status;
// 	DFTI_DESCRIPTOR_HANDLE kfh = NULL;
// 	status = DftiCreateDescriptor(&kfh, DFTI_SINGLE, DFTI_COMPLEX, 2, fft);
// 	status = DftiCommitDescriptor(kfh);
// 	status = DftiComputeForward(kfh, kernel.data()); // forward FFT
// 	status = DftiFreeDescriptor(&kfh);

// 	return kernel;
// }