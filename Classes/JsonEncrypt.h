#ifndef __JsonEncrypt_H__
#define __JsonEncrypt_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#ifdef _WIN32
#include <io.h>
#else             
//#include <unistd.h>
//#include <stdio.h>
//#include <dirent.h>
//#include <sys/stat.h>
#endif

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

	void dfsFolder(std::string folderPath, std::string foldername, int depth = 0);
};

#endif
