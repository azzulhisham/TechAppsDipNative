#include "Dip.h"
#include "DipJni.h"

#include <iostream>
using namespace std;

jfloatArray JNICALL Java_com_petronas_dip_Dip_run(JNIEnv *env, jclass cls, jfloatArray data, jint traceSize, jint winX, jint winZ, jint dx, jint dz, jfloat psizeCut, jfloat maxAmp)
{
  int dataLength = env->GetArrayLength(data);
  int sampleSize = dataLength / traceSize;

  //convert jfloatArray to vector<float>
  vector<float> input(dataLength);
  env->GetFloatArrayRegion(data, 0, dataLength, input.data());

  //convert input data to 2D array
  vector<vector<float>> inputData(traceSize, vector<float>(sampleSize));
  int dataRow = 0;
  int dataCnt = 0;
  float pCutter = maxAmp * psizeCut;
  for (int i = 0; i < dataLength; i++)
  {
    float absData = abs(input[i]);
    inputData[dataRow][dataCnt] = absData < pCutter ? 0 : input[i];

    dataCnt++;
    if (dataCnt >= sampleSize)
    {
      dataRow++;
      dataCnt = 0;
    }
  }

  //process
  Dip dip;
  dip.windowX = winX;
  dip.windowZ = winZ;
  dip.dX = dx;
  dip.dZ = dz;
  dip.pSize = psizeCut;
  vector<vector<MKL_Complex8>> kernel = dip.CustGaussian2D(dip.windowX, dip.windowZ, 0);
  vector<MKL_Complex8> kernelWin = dip.GenerateKernel(kernel, sampleSize, traceSize);
  vector<vector<float>> result = dip.Calculate(inputData, kernelWin);
  vector<float> resultData(result.size() * result[0].size());

  //convert 2D array to single array 176x251 => 251x176
  for (int i = 0; i < result.size(); i++) // 176
  {
    for (int j = 0; j < result[i].size(); j++) // 251
    {
      resultData[(j * result.size()) + i] = result[i][j];
    }
  }

  //convert vector float to jfloatArray
  jsize retResultSize = (jsize)resultData.size();
  jfloatArray retResult = env->NewFloatArray(retResultSize);
  env->SetFloatArrayRegion(retResult, 0, retResultSize, resultData.data());

  return retResult;
}
