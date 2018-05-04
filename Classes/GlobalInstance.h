/****************************************************************
静态数据操作类
****************************************************************/
#ifndef _STATIC_DATA_H_
#define _STATIC_DATA_H_
#include "cocos2d.h"
USING_NS_CC;


class GlobalInstance
{
public:
	GlobalInstance();
	~GlobalInstance();
	/****************************
	类实例，单例模式
	****************************/
	static GlobalInstance* getInstance();
public:
	//语言
	CC_SYNTHESIZE(int, m_lang, Lang);
private:
	static GlobalInstance* _Context;//类实例
};
#endif

