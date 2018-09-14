#include <jni.h>
#include "ShopLayer.h"
extern "C"
{
	JNIEXPORT void JNICALL Java_com_csfb_myjh_JNI_sendMessage(JNIEnv *env, jobject obj,jint msg);
	JNIEXPORT void JNICALL Java_com_csfb_myjh_JNI_postData(JNIEnv *env, jobject obj);
}

JNIEXPORT void JNICALL Java_com_csfb_myjh_JNI_sendMessage(JNIEnv *env, jobject obj,jint msg)
{
	ShopLayer::setMessage((PYARET)msg);
}

JNIEXPORT void JNICALL Java_com_csfb_myjh_JNI_postData(JNIEnv *env, jobject obj)
{
}

