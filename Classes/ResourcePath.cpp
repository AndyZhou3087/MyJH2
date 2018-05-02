#include "ResourcePath.h"


std::string ResourcePath::makeImagePath(std::string imageName)
{
	return StringUtils::format("images/%s", imageName.c_str());
}

std::string ResourcePath::makeSoundPath(std::string soundName)
{
	return StringUtils::format("sounds/%s", soundName.c_str());
}

std::string ResourcePath::makeResPath(std::string resName)
{
	return StringUtils::format("res/%s", resName.c_str());
}

std::string ResourcePath::makeFontPath(std::string fontName)
{
	return StringUtils::format("fonts/%s", fontName.c_str());
}

std::string ResourcePath::makePath(std::string pathname)
{
	return StringUtils::format("%s", pathname.c_str());
}
