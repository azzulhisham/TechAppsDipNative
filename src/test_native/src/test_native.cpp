//============================================================================
// Name        : test_native.cpp
// Author      : Zulhisham
// Version     :
// Copyright   : Your copyright notice
// Description : Testing Java Native
//============================================================================


#include "com_petronas_dip_CallNative.h"
#include <iostream>
using namespace std;

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
