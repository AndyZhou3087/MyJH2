#include "CommonFuncs.h"
#include "SoundManager.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "DataBase64.h"
#include "Utility.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include "../cocos2d//external//win32-specific/icon/include/iconv.h"
#include <io.h>
#else
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#endif

#define Button_ACTION (Sequence::create(ScaleTo::create(0.05f,0.95f),NULL))
#define Button_ACTION1 (Sequence::create(ScaleTo::create(0.05f,1),NULL))

int CommonFuncs::split(const std::string& str, std::vector<std::string>& ret_, std::string sep)
{
	if (str.empty())
	{
		return 0;
	}

	std::string tmp;
	std::string::size_type pos_begin = 0;//str.find_first_not_of(sep);
	std::string::size_type comma_pos = 0;

	while (pos_begin != std::string::npos)
	{
		comma_pos = str.find(sep, pos_begin);
		if (comma_pos != std::string::npos)
		{
			tmp = str.substr(pos_begin, comma_pos - pos_begin);
			pos_begin = comma_pos + sep.length();
		}
		else
		{
			tmp = str.substr(pos_begin);
			pos_begin = comma_pos;
		}

		//if (!tmp.empty())
		//{
			ret_.push_back(tmp);
			tmp.clear();
		//}
	}

	return 0;
}

std::string CommonFuncs::replace_all(std::string & s, std::string const & t, std::string const & w)
{
	std::string::size_type pos = s.find(t), t_size = t.size(), r_size = w.size();
	while (pos != std::string::npos){ // found 
		s.replace(pos, t_size, w);
		pos = s.find(t, pos + r_size);
	}
	return s;
}

std::string CommonFuncs::trim(std::string &s)
{
	if (!s.empty())
	{
		s.erase(0, s.find_first_not_of(" "));
		s.erase(s.find_last_not_of(" ") + 1);
	}
	return s;
}

int CommonFuncs::code_convert(const char *from_charset, const char *to_charset, const char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	iconv_t cd;
	const char *temp = inbuf;
	const char **pin = &temp;
	char **pout = &outbuf;
	memset(outbuf, 0, outlen);
    cd = iconv_open(to_charset, from_charset);
	if (cd == 0) return -1;
	if (iconv(cd, pin, &inlen, pout, &outlen) == -1) return -1;
	iconv_close(cd);
#else
//    iconv_t cd;
//    cd = iconv_open(to_charset, from_charset);
//	if (iconv(cd, &inbuf, &inlen, &outbuf, &outlen) == -1)
//	{
//		iconv_close(cd);
//		return  - 1;
//	}
	
#endif
	return 0;
}

std::string CommonFuncs::gbk2utf(const char *inbuf)
{
	std::string strRet;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	size_t inlen = strlen(inbuf);
	char * outbuf = new char[inlen * 2 + 2];

	if (code_convert("gb2312", "utf-8", inbuf, inlen, outbuf, inlen * 2 + 2) == 0)
	{
		strRet = outbuf;
	}
	delete[] outbuf;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	strRet = inbuf;
#endif
	return strRet;
}


void CommonFuncs::BtnAction(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == cocos2d::ui::Widget::TouchEventType::BEGAN || type == cocos2d::ui::Widget::TouchEventType::MOVED)
	{
		((cocos2d::ui::Widget*)pSender)->runAction(Button_ACTION);
	}
	else if (type == cocos2d::ui::Widget::TouchEventType::CANCELED)
	{
		((cocos2d::ui::Widget*)pSender)->runAction(Button_ACTION1);
	}
	else if (type == cocos2d::ui::Widget::TouchEventType::ENDED)
	{
		((cocos2d::ui::Widget*)pSender)->runAction(Button_ACTION1);
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
	}
}

void CommonFuncs::changeGray(cocos2d::Node* node)
{
	auto p = GLProgram::createWithFilenames(ResourcePath::makePath("gray.vsh"), ResourcePath::makePath("gray.fsh"));
	p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
	p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_COLOR, GLProgram::VERTEX_ATTRIB_COLOR);
	p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORDS);
	p->link();
	p->updateUniforms();

	node->setGLProgram(p);
}

void CommonFuncs::removeGray(cocos2d::Node* node)
{
	std::string str = GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP;//ShaderPositionTextureColor_noMVP;
	GLProgram * pProgram = GLProgramCache::getInstance()->getGLProgram(str);
	node->setGLProgram(pProgram);
}

void CommonFuncs::checkEnAndCnCount(std::string instr, int& encount, int& cncount)
{
	if (instr.length() <= 0)
		return;

	std::u32string utf32String;
	StringUtils::UTF8ToUTF32(instr, utf32String);

	int chiniseCount = 0;

	int engnishCount = 0;

	int len = utf32String.length();
	for (int i = 0; i < len; i++) 
	{
		// 判断是否为可显示英文字符
		char32_t s = utf32String[i];
		if ((s >= 32) && (s < 127))
		{
			engnishCount++;
		}
		//是否为汉字
		else if (s > 127) 
		{
			chiniseCount++;
		}
	}
	encount = engnishCount;
	cncount = chiniseCount;
}

void CommonFuncs::playCommonLvUpAnim(Node* target, std::string textstr)
{
	auto effectnode = CSLoader::createNode("effect/qianghuachenggong.csb");
	effectnode->setPosition(Vec2(360, 640));
	target->addChild(effectnode, 10, "qianghuachenggong");
	cocos2d::ui::ImageView* ziti = (cocos2d::ui::ImageView*)effectnode->getChildByName("ziti");
	ziti->loadTexture(ResourcePath::makeTextImgPath(textstr, GlobalInstance::getInstance()->getLang()), cocos2d::ui::Widget::TextureResType::PLIST);
	auto action = CSLoader::createTimeline("effect/qianghuachenggong.csb");
	effectnode->runAction(action);
	action->gotoFrameAndPlay(0, false);
}

cocos2d::Node* CommonFuncs::playResBoxEffect(cocos2d::Node* target, int qu)
{
	if (qu >= 3)
	{
		std::string effectstr = StringUtils::format("effect/resbox%deffect.csb", qu);
		auto effectnode = CSLoader::createNode(effectstr);
		effectnode->setPosition(Vec2(target->getContentSize().width/2, target->getContentSize().height/2));
		target->addChild(effectnode, 1, "resboxeffect");
		auto action = CSLoader::createTimeline(effectstr);
		effectnode->runAction(action);
		action->gotoFrameAndPlay(0, true);
		return effectnode;
	}
	return NULL;
}

void CommonFuncs::dfsFolder(std::string folderPath, std::vector<std::string> &vec_ret, int depth)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	_finddata_t FileInfo;
	std::string strfind = folderPath + "/*";
	long Handle = _findfirst(strfind.c_str(), &FileInfo);

	if (Handle == -1L)
	{
		return;
	}
	do {
		//判断是否有子目录
		if (FileInfo.attrib & _A_SUBDIR)
		{
			//这个语句很重要
			if ((strcmp(FileInfo.name, ".") != 0) && (strcmp(FileInfo.name, "..") != 0))
			{
				std::string newPath = folderPath + "/" + FileInfo.name;
				dfsFolder(newPath, vec_ret);
			}
		}
		else
		{
			std::string filename = (folderPath + "/" + FileInfo.name);
			log("filename = %s ", FileInfo.name);

			vec_ret.push_back(filename);
		}
	} while (_findnext(Handle, &FileInfo) == 0);

	_findclose(Handle);
#else
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	if ((dp = opendir(folderPath.c_str())) == NULL) {
		fprintf(stderr, "cannot open directory: %s\n", folderPath.c_str());
		return;
	}
	chdir(folderPath.c_str());
	while ((entry = readdir(dp)) != NULL) {
		lstat(entry->d_name, &statbuf);
		if (S_ISDIR(statbuf.st_mode)) {

			if (strcmp(".", entry->d_name) == 0 ||
				strcmp("..", entry->d_name) == 0)
				continue;
			printf("%*s%s/\n", depth, "", entry->d_name);
			dfsFolder(entry->d_name, vec_ret, depth + 4);
		}
		else {
			string filename = entry->d_name;
			char pathfile[256];
			memset(pathfile, 0x00, sizeof(pathfile));
			sprintf(pathfile, "%*s%s\n", depth, "", entry->d_name);
			vec_ret.push_back(pathfile);
		}
	}
	chdir("..");
	closedir(dp);
#endif
}

void CommonFuncs::encryptToFile(std::string oValue, std::string filename)
{
	std::string encryptvalue = encryptData(reinterpret_cast<const unsigned char*>(oValue.c_str()), oValue.length());
	Encrypt((char*)encryptvalue.c_str(), true);
	std::string encrypstr = StringUtils::format("%s", encryptvalue.c_str());

	FileUtils::getInstance()->writeStringToFile(encrypstr, filename);
}
