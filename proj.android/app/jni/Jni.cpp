#include <jni.h>
#include "ShopLayer.h"
#include "MainScene.h"
extern "C"
{
	JNIEXPORT void JNICALL Java_com_game_myjh_JNI_sendMessage(JNIEnv *env, jclass jobj,jint msg, jstring para);
	JNIEXPORT void JNICALL Java_com_game_myjh_JNI_saveAllData(JNIEnv *env, jclass jobj);
}

JNIEXPORT void JNICALL Java_com_game_myjh_JNI_sendMessage(JNIEnv *env, jclass jobj,jint msg, jstring para)
{
	const char* parastr;
	parastr = env->GetStringUTFChars(para, JNI_FALSE);
	if(parastr != NULL) {
		ShopLayer::setMessage((PYARET)msg, parastr);
		env->ReleaseStringUTFChars(para, parastr);
	}
}

JNIEXPORT void JNICALL Java_com_game_myjh_JNI_saveAllData(JNIEnv *env, jclass jobj)
{
    MainScene::saveAllData();
}

