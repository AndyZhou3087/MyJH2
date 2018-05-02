#ifndef __RESOURCE_PATH__
#define __RESOURCE_PATH__

#include "cocos2d.h"
USING_NS_CC;

static std::string FONT_NAME = "";

class ResourcePath
{
public:

	static std::string makeImagePath(std::string imageName);
	static std::string makeSoundPath(std::string soundName);
	static std::string makeFontPath(std::string fontName);
	static std::string makeResPath(std::string resName);
	static std::string makePath(std::string pathname);

};

#endif
