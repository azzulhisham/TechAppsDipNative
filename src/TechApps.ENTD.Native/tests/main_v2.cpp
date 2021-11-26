#include <vector>
#include <tuple>
#include "Entd.h"
#include <SegyData.h>
#include <SegyRead.h>
#include <SegyWrite.h>
#include <PspaException.h>
#include "gtest/gtest.h"

using namespace std;
using namespace PagosaGeo;

TEST(ENTDTest, arrayToBitTest) {
  vector<int> zero { 0,0,0,0,0,0,0,0 };
  vector<int> ones { 1,1,1,1,1,1,1,1 };
  vector<int> odds { 0,1,0,1,0,1,0,1 };
  ASSERT_EQ (0, ArrayToBit::ToBit(zero));
	ASSERT_EQ (255, ArrayToBit::ToBit(ones)); 
  ASSERT_EQ (170, ArrayToBit::ToBit(odds));
}


vector<vector<float>> getSurface(string fileName, int padding = 0, int z = 0) 
{
	try
	{
		SegyRead inputData(fileName);
		auto rng = inputData.range();		
		auto nSample = (int) rng.numSmps();
		auto nILine = rng.numInlines();
		auto nXLine = rng.numCrosslines();
    vector<vector<float>> surface(nILine + padding * 2);
    for (size_t i = 0, l = 0; i < surface.size(); i++) {
      vector<float> trace(nXLine + padding * 2);
      if (i >= padding && i < surface.size() - padding) {
        for (size_t j = padding; j < trace.size() - padding; j++) {
          SeisTrace trc;
          inputData.readTrace(trc);
          vector<float> tx(nSample);
          trc.getValues(tx.data(), nSample);
          trace[j] = tx[z];
        }
      }
      surface[i] = trace;
    }    
    return surface;
	} catch (PspaException& ex) {    
		ex.addSource("Main -- run1dTool()");
		std::cout << ex.toString() << endl;
    return {};
	}
}

TEST(ENTDTest, segmentTest) {
  auto s = Segment::Init();
  ASSERT_EQ(8, s.size());
  ASSERT_EQ(5, s[0][0].Xi[0]);
}

// TODO enable only in local
void surfaceTest() {
  ENTD fn;
  auto file = "../../../data/F3_demo.sgy";
  vector<vector<float>> surface = getSurface(file, fn.Unit + 2);
  vector<vector<float>> output;
  vector<int> density;
  tie(output, density) = fn.Apply(surface);
}

int main(int argc, char * argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
