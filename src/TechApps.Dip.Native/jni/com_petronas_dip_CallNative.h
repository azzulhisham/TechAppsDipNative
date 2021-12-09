/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_petronas_dip_CallNative */

#ifndef _Included_com_petronas_dip_CallNative
#define _Included_com_petronas_dip_CallNative
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_petronas_dip_CallNative
 * Method:    run
 * Signature: ([FIIIIIFIFF)[F
 */
JNIEXPORT jfloatArray JNICALL Java_com_petronas_dip_CallNative_run
  (JNIEnv *, jclass, jfloatArray, jint, jint, jint, jint, jint, jfloat, jint, jfloat, jfloat);

/*
 * Class:     com_petronas_dip_CallNative
 * Method:    multiple
 * Signature: (DD)D
 */
JNIEXPORT jdouble JNICALL Java_com_petronas_dip_CallNative_multiple
  (JNIEnv *, jclass, jdouble, jdouble);

/*
 * Class:     com_petronas_dip_CallNative
 * Method:    sum
 * Signature: (DD)D
 */
JNIEXPORT jdouble JNICALL Java_com_petronas_dip_CallNative_sum
  (JNIEnv *, jclass, jdouble, jdouble);

/*
 * Class:     com_petronas_dip_CallNative
 * Method:    comboItem
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_petronas_dip_CallNative_comboItem
  (JNIEnv *, jclass);

/*
 * Class:     com_petronas_dip_CallNative
 * Method:    comboCustomItem
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_petronas_dip_CallNative_comboCustomItem
  (JNIEnv *, jclass, jstring);

#ifdef __cplusplus
}
#endif
#endif
