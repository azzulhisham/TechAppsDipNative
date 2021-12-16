#include "com_petronas_dip_CallNative.h"
#include "dip.h"

#include <iostream>
using namespace std;


jfloatArray JNICALL Java_com_petronas_dip_CallNative_run
  (JNIEnv *env, jclass cls, jfloatArray data, jint traceSize, jint winX, jint winZ, jint dx, jint dz, jfloat psizeCut, jint sampleSize, jfloat maxAmp, jfloat sampleRate) {

    DIP dip;

    //dip.windowX = winX;
    //dip.windowZ = winZ;
    dip.windowX = 2;
    dip.windowZ = 2;

    //dip.dX = dx;
    //dip.dZ = dz;
    dip.dX = 1;
    dip.dZ = 1;

    dip.pSize = psizeCut;

    int dataLength = env->GetArrayLength(data);
    int numberOfTrace = dataLength/traceSize;

    //initialize 3D vector/list
    vector<vector<vector<float>>> inputData(1, vector<vector<float>>(numberOfTrace, vector<float>(traceSize)));

    //convert jfloatArray to float
    vector<float> input(dataLength);
	  env->GetFloatArrayRegion(data, 0, dataLength, input.data());

    int dataRow = 0;
    int dataCnt = 0;

    float _maxAmp = (float)maxAmp;
    float _psizeCut = (float)psizeCut;
    float pCutter = _maxAmp * _psizeCut;

    //convert input data to 3D array
    for(int i=0; i<dataLength; i++) {
      float absData = abs(input[i]);

      if (absData < pCutter){
        inputData[0][dataRow][dataCnt] = 0;
      }
      else {
        inputData[0][dataRow][dataCnt] = input[i];
      }
      
      dataCnt += 1;

      if(dataCnt >= traceSize) {
          dataRow += 1;
          dataCnt = 0;
      }
    }

    Index3D min(0);
    Index3D max((int)(inputData.size())-1, (int)(inputData[0].size())-1, (int)(inputData[0][0].size())-1);

    vector<vector<MKL_Complex8>> kernel = dip.CustGaussian2D(dip.windowX, dip.windowZ, 0);
    vector<vector<MKL_Complex8>> kernelWindow = dip.GenerateKernel(kernel, (int)(inputData[0][0].size()), (int)(inputData[0].size()));

    vector<MKL_Complex8>kernelWin(kernelWindow.size() * kernelWindow[0].size());

    //convert 2D array to single array
    dataCnt = 0;
    for(int i=0; i<kernelWindow.size(); i++) {
        for(int j=0; j<kernelWindow[0].size(); j++) {
            kernelWin[dataCnt] = kernelWindow[i][j];
            dataCnt += 1;
        }
    }    

    // //convert 3D array size to single array
    // vector<float> output(inputData.size() * inputData[0].size() * inputData[0][0].size());

    // //convert 3D array to single array
    // dataCnt = 0;
    // for(int i=0; i<inputData.size(); i++) {
    //     for(int j=0; j<inputData[0].size(); j++) {
    //         for(int k=0; k<inputData[0][0].size(); k++) {
    //             output[dataCnt] = inputData[i][j][k];
    //             dataCnt += 1;
    //         }
    //     }
    // }

    //process
    vector<vector<float>> result = dip.Calculate(inputData, min, max, 0, kernelWin);  
    vector<float>resultData(result.size() * result[0].size());

    //convert 2D array to single array
    dataCnt = 0;
    for(int i=0; i<result.size(); i++) {
        for(int j=0; j<result[0].size(); j++) {
          resultData[(j * result.size()) + i] = result[i][j];
            //resultData[dataCnt] = result[i][j];
            //dataCnt += 1;
        }
    }     

    //convert float to jfloatArray
    int retResultSize = (int)(result.size()) * (int)(result[0].size());
    jfloatArray retResult = env->NewFloatArray(retResultSize);
    env->SetFloatArrayRegion(retResult, 0, retResultSize, resultData.data());

    //jfloatArray retResult = env->NewFloatArray(dataLength);
    //env->SetFloatArrayRegion(retResult, 0, dataLength, input.data());

    return retResult;
}

jdouble JNICALL Java_com_petronas_dip_CallNative_multiple
  (JNIEnv *env, jclass cls, jdouble param1, jdouble param2) {

	return param1 * param2;
}

jdouble JNICALL Java_com_petronas_dip_CallNative_sum
  (JNIEnv *env, jclass cls, jdouble param1, jdouble param2) {

	return param1 + param2;
}

jstring JNICALL Java_com_petronas_dip_CallNative_comboItem
  (JNIEnv *env, jclass cls) {

	return env -> NewStringUTF("Hello World From C++");
}

jstring JNICALL Java_com_petronas_dip_CallNative_comboCustomItem
  (JNIEnv *env, jclass cls, jstring item) {

	const char *nativeString = env->GetStringUTFChars(item, nullptr);

	return env -> NewStringUTF(nativeString);
}
