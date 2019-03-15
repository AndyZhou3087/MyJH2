#include <jni.h>
#include "ShopLayer.h"
#include "MainScene.h"
extern "C"
{
	JNIEXPORT void JNICALL Java_com_game_myjh_JNI_sendMessage(JNIEnv *env, jclass jobj,jint msg);
	JNIEXPORT void JNICALL Java_com_game_myjh_JNI_saveAllData(JNIEnv *env, jclass jobj);
}

JNIEXPORT void JNICALL Java_com_game_myjh_JNI_sendMessage(JNIEnv *env, jclass jobj,jint msg)
{
	ShopLayer::setMessage((PYARET)msg);
}

JNIEXPORT void JNICALL Java_com_game_myjh_JNI_saveAllData(JNIEnv *env, jclass jobj)
{
    MainScene::saveAllData();
}

