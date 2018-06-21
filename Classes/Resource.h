#ifndef __RESOURCE_PATH__
#define __RESOURCE_PATH__

#include "cocos2d.h"
USING_NS_CC;

/******************************
资源处理类
********************************/

//语言，主要是简体和繁体
typedef enum
{
	EN,
	ZH_CN,//简体
	ZH_HK//繁体
}LANGTYPE;

class ResourcePath
{
public:

	static std::string makeImagePath(std::string imageName);
	static std::string makeSoundPath(std::string soundName);
	static std::string makeFontPath(std::string fontName);
	static std::string makeResPath(std::string resName);
	static std::string makePath(std::string pathname);

	/************************************
	处理带文字图片的路径
	***********************************/
	static std::string makeTextImgPath(std::string name, int langtype);

};

//语言处理
class ResourceLang
{
	
public:
	static std::map<std::string, std::string> map_lang;//xml文件数据
	static std::string makeLangName(int langtype);//将int语言类型转换成zh_cn,zh_hk
	static void load(int langtype);//价值文字数据
private:
	static void parseXml(int langtype);//解析文字xml

};

#endif
