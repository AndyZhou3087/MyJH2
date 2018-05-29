#include "Resource.h"
#include "tinyxml2/tinyxml2.h"


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

std::string ResourcePath::makeTextImgPath(std::string name, int langtype)
{
	std::string langname = ResourceLang::makeLangName(langtype);
	return StringUtils::format("text_%s/%s.png", langname.c_str(), name.c_str());
}

std::map<std::string, std::string> ResourceLang::map_lang;

std::string ResourceLang::makeLangName(int langtype)
{
	std::string name = "zh_cn";
	switch (langtype)
	{
	case EN:
		name = "en";
		break;
	case ZH_CN:
		name = "zh_cn";
		break;
	case ZH_HK:
		name = "zh_hk";
		break;
	default:
		break;
	}
	return name;
}

void ResourceLang::load(int langtype)
{
	map_lang.clear();
	parseXml(langtype);
}

void ResourceLang::parseXml(int langtype)
{

	tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();
	unsigned char pBuffer[5000];
	memset(pBuffer, 0x00, sizeof(pBuffer));
	std::string langname = StringUtils::format("lang/%s/strings.xml", ResourceLang::makeLangName(langtype).c_str());
	strcpy((char *)pBuffer, FileUtils::getInstance()->getStringFromFile(langname).c_str());
	int err = pDoc->Parse((const char*)pBuffer);
	if (err != 0)
	{
		delete pDoc;
		return;
	}
	tinyxml2::XMLElement *rootEle = pDoc->RootElement();
	//int total = rootEle->IntAttribute("total");
	//获取第一个节点属性
	//const XMLAttribute *attribute = rootEle->FirstAttribute();
	//打印节点属性名和值
	//log("attribute<em>name = %s,attribute</em>value = %s", attribute->Name(), attribute->Value());</p>
	tinyxml2::XMLElement *element = rootEle->FirstChildElement();
	while (element != NULL)
	{
		if (strcmp(element->Name(), "s") == 0)
		{
			map_lang[element->Attribute("name")] = element->GetText();
		}
		
		element = element->NextSiblingElement();
	}
	delete pDoc;
}
