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

	//创建该路径
#ifdef _WIN32
	CreateDirectoryA("../Resources/ResourcesEnc/", NULL);
	CreateDirectoryA("../Resources/ResourcesEnc/json/", NULL);
	CreateDirectoryA("../Resources/ResourcesEnc/mapdata/", NULL);
	CreateDirectoryA("../Resources/ResourcesEnc/lang/", NULL);
	CreateDirectoryA("../Resources/ResourcesEnc/lang/zh_cn/", NULL);
#else
	/*mkdir("../Resources/ResourcesEnc/json/", S_IRWXU | S_IRWXG | S_IRWXO);
	mkdir("../Resources/ResourcesEnc/mapdata/", S_IRWXU | S_IRWXG | S_IRWXO);
	mkdir("../Resources/ResourcesEnc/lang/zh_cn/", S_IRWXU | S_IRWXG | S_IRWXO);*/
#endif

	//遍历目录
	dfsFolder("../Resources/jsonbackup/json", "json");
	dfsFolder("../Resources/jsonbackup/mapdata", "mapdata");

	std::string sfilestr = FileUtils::getInstance()->getStringFromFile(ResourcePath::makePath("jsonbackup/lang/zh_cn/allresdesc.json"));
	setEncrypt(sfilestr, ResourcePath::makePath("ResourcesEnc/lang/zh_cn/allresdesc.json"));


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

void JsonEncrypt::dfsFolder(std::string folderPath, std::string foldername, int depth)
{
#ifdef WIN32
	_finddata_t FileInfo;
	string strfind = folderPath + "\\*";
	long Handle = _findfirst(strfind.c_str(), &FileInfo);

	if (Handle == -1L)
	{
		//cerr << "can not match the folder path" << endl;
		exit(-1);
	}
	do {
		//判断是否有子目录
		if (FileInfo.attrib & _A_SUBDIR)
		{
			//这个语句很重要
			if ((strcmp(FileInfo.name, ".") != 0) && (strcmp(FileInfo.name, "..") != 0))
			{
				string newPath = folderPath + "\\" + FileInfo.name;
				dfsFolder(newPath, foldername);
			}
		}
		else
		{
			string filename = (folderPath + "\\" + FileInfo.name);
			log("filename = %s ", FileInfo.name);

			std::string str = StringUtils::format("jsonbackup/%s/%s", foldername.c_str(), FileInfo.name);
			std::string filestr = FileUtils::getInstance()->getStringFromFile(ResourcePath::makePath(str));
			str = StringUtils::format("ResourcesEnc/%s/%s", foldername.c_str(), FileInfo.name);
			setEncrypt(filestr, ResourcePath::makePath(str));

			//cout << folderPath << "\\" << FileInfo.name << " " << endl;
		}
	} while (_findnext(Handle, &FileInfo) == 0);

	_findclose(Handle);
//#else
//	DIR *dp;
//	struct dirent *entry;
//	struct stat statbuf;
//	if ((dp = opendir(folderPath.c_str())) == NULL) {
//		fprintf(stderr, "cannot open directory: %s\n", folderPath.c_str());
//		return;
//	}
//	chdir(folderPath.c_str());
//	while ((entry = readdir(dp)) != NULL) {
//		lstat(entry->d_name, &statbuf);
//		if (S_ISDIR(statbuf.st_mode)) {
//
//			if (strcmp(".", entry->d_name) == 0 ||
//				strcmp("..", entry->d_name) == 0)
//				continue;
//			printf("%*s%s/\n", depth, "", entry->d_name);
//			dfsFolder(entry->d_name, depth + 4);
//		}
//		else {
//			string filename = entry->d_name;
//			54             printf("%*s%s\n", depth, "", entry->d_name);
//		}
//	}
//	chdir("..");
//	closedir(dp);
#endif
}

void JsonEncrypt::onExit()
{
	Layer::onExit();
}
