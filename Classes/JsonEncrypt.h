#ifndef __JsonEncrypt_H__
#define __JsonEncrypt_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include <io.h>

USING_NS_CC;
class JsonEncrypt : public cocos2d::Layer
{
public:
	JsonEncrypt();
	~JsonEncrypt();
	static cocos2d::Scene* createScene();

	virtual void onExit();
	virtual bool init();

private:
	CREATE_FUNC(JsonEncrypt);
	
	void setEncrypt(std::string sValue, std::string filename);

	std::string parseJsonXml(std::string s);

	void dfsFolder(std::string folderPath, int depth = 0);
};

#endif
