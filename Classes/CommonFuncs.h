#ifndef _COMMONFUNCS_H_
#define _COMMONFUNCS_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

class CommonFuncs
{
public:
	/****************************
	字符串分割
	@param str 字符串
	@param ret_ 分割后的字符串vector
	@param sep 分割的符号
	****************************/
	static int split(const std::string& str, std::vector<std::string>& ret_, std::string sep = ",");

	/****************************
	字符串替换(替换所有)
	@param s 源字符串
	@param t 需要替换的字符串
	@param w 替换成的字符串
	@return 替换后的字符串
	****************************/
	static std::string replace_all(std::string & s, std::string const & t, std::string const & w);


	/****************************
	字符串替换（替换一次）
	@param s 源字符串
	@param t 需要替换的字符串
	@param w 替换成的字符串
	@return 替换后的字符串
	****************************/
	static std::string replace(std::string& s, std::string const& t, std::string const& w);

	/****************************
	去掉首尾空格
	@param s 源字符串
	@return 去掉首尾空格的字符串
	****************************/
	static std::string trim(std::string &s);

	/****************************
	字符串转换UTF8
	@param inbuf 字符串
	@return 转换后的字符串
	win32平台写在代码里的中文需要调用转换函数
	android，ios平台只需要将文件保存为utf-8即可
	****************************/
	static std::string gbk2utf(const char *inbuf);

	/****************************
	按钮点击
	****************************/
	static void BtnAction(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	/****************************
	图片变灰操作
	****************************/
	static void changeGray(cocos2d::Node* node);

	/****************************
	图片变灰还原操作
	****************************/
	static void removeGray(cocos2d::Node* node);

	/****************************
	计算中英文长度
	****************************/
	static void checkEnAndCnCount(std::string instr, int& encount, int& cncount);

	/****************************
	通用升级动画，强化，招募，转职，任务完成，突破
	****************************/
	static void playCommonLvUpAnim(cocos2d::Node* target, std::string textstr, cocos2d::Vec2 pos = cocos2d::Vec2(360, 640));

	/****************************
	资源边框特效，紫色以上
	****************************/
	static cocos2d::Node* playResBoxEffect(cocos2d::Node* target, int type, int qu, int lv);

	/****************************
	文件目录遍历
	@param folderpaht 目录
	@param depth深度默认0，暂时无用
	@return 文件的绝对路径列表
	****************************/
	static void dfsFolder(std::string folderPath, std::vector<std::string> &vec_ret, int depth = 0);

	/****************************
	加密数据到文件
	@param oValue 原始数据
	@param 文件路径filename
	****************************/
	static void encryptToFile(std::string oValue, std::string filename);

private:

	/****************************
	字符串转换
	@param from_charset 转换前字符编码
	@param to_charset 转换后字符编码
	@param inbuf 需要转换的字符串

	@param inbuf 需要转换的字符串的长度
	@param outbuf 转换后的字符串
	@param outlen 转换后的字符长度
	****************************/
	static int code_convert(const char *from_charset, const char *to_charset, const char *inbuf, size_t inlen, char *outbuf, size_t outlen);

};

#endif
