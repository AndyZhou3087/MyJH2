#include "JsonEncrypt.h"
#include "Resource.h"
#include "MainScene.h"
#include "DataSave.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "CommonFuncs.h"
#include "Utility.h"
#include "json.h"

USING_NS_CC;

std::string jsonArr[17] = { "achieve","branchtask","buildings","dailytask","equip","gf","heroatr","mainmap","maintask","npc","npcatr","resaction","shop",
"skill","submap","suitequip","tbox" };

JsonEncrypt::JsonEncrypt()
{

}

JsonEncrypt::~JsonEncrypt()
{

}

Scene* JsonEncrypt::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto mainLayer = JsonEncrypt::create();

	// add layer as a child to scene
	scene->addChild(mainLayer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool JsonEncrypt::init()
{
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("LoadingLayer.csb"));
	this->addChild(csbnode);

	//º”√‹
	for (int i = 0; i < sizeof(jsonArr) / sizeof(jsonArr[0]); i++)
	{
		std::string str = StringUtils::format("json/%s.json", jsonArr[i].c_str());
		std::string filestr = FileUtils::getInstance()->getStringFromFile(ResourcePath::makePath(str));
		str = StringUtils::format("jsonEnc/%s.json", jsonArr[i].c_str());
		setEncrypt(filestr, ResourcePath::makePath(str));
	}

	for (int j = 1; j < 6; j++)
	{
		std::string str = StringUtils::format("json/boxevent%d.json", j);
		std::string filestr = FileUtils::getInstance()->getStringFromFile(ResourcePath::makePath(str));
		str = StringUtils::format("jsonEnc/boxevent%d.json", j);
		setEncrypt(filestr, ResourcePath::makePath(str));

		str = StringUtils::format("json/event%d.json", j);
		filestr = FileUtils::getInstance()->getStringFromFile(ResourcePath::makePath(str));
		str = StringUtils::format("jsonEnc/event%d.json", j);
		setEncrypt(filestr, ResourcePath::makePath(str));
	}

	std::string sfilestr = FileUtils::getInstance()->getStringFromFile(ResourcePath::makePath("lang/zh_cn/allresdesc.json"));
	setEncrypt(sfilestr, ResourcePath::makePath("jsonEnc/allresdesc.json"));

	for (int m = 1; m < 6; m++)
	{
		for (int n = 1; n < 6; n++)
		{
			for (int q = 1; q < 6; q++)
			{
				std::string str = StringUtils::format("mapdata/m%d-%d-%d.xml", m, n, q);
				std::string filestr = FileUtils::getInstance()->getStringFromFile(ResourcePath::makePath(str));
				if (filestr.length() > 0)
				{
					str = StringUtils::format("xmlEnc/m%d-%d-%d.xml", m, n, q);
					setEncrypt(filestr, ResourcePath::makePath(str));
				}
			}
		}
	}
	std::string filestr = FileUtils::getInstance()->getStringFromFile(ResourcePath::makePath("mapdata/m0-0-0.xml"));
	if (filestr.length() > 0)
	{
		setEncrypt(filestr, ResourcePath::makePath("xmlEnc/m0-0-0.xml"));
	}


	return true;
}

void JsonEncrypt::setEncrypt(std::string sValue, std::string filename)
{
	std::string encryptvalue = encryptData(reinterpret_cast<const unsigned char*>(sValue.c_str()), sValue.length());
	Encrypt((char*)encryptvalue.c_str(), true);
	std::string encrypstr = StringUtils::format("%s", encryptvalue.c_str());

	FILE* fp = fopen(filename.c_str(), "wt");
	if (fp != NULL)
	{
		fwrite(encrypstr.c_str(), 1, encrypstr.size(), fp);
		fclose(fp);
	}
}

std::string JsonEncrypt::parseJsonXml(std::string s)
{
	Encrypt((char*)s.c_str(), false);
	std::string parseStr = parseData(s.c_str());
	return parseStr;
}

void JsonEncrypt::onExit()
{
	Layer::onExit();
}
