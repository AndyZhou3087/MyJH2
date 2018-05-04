#include "DataSave.h"
#include "cocos2d.h"
#include "MD5.h"
#include "Utility.h"

DataSave* DataSave::_Context = NULL;

DataSave* DataSave::getInstance() {
	if (_Context == NULL) {
		_Context = new DataSave;
	}
	return _Context;
}

void DataSave::purgeGameSave() {
	CC_SAFE_DELETE(_Context);
}

int DataSave::getBuildLv(std::string name)
{
	return loadIntDataByKey(name, 1);
}

void DataSave::setBuildLv(std::string name, int val)
{
	saveIntDataByKey(name, val);
}

int DataSave::getLocalLang()
{
	return loadIntDataByKey("lang", 1);
}

void DataSave::setLocalLang(int val)
{
	saveIntDataByKey("lang", val);
}