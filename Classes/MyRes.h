#ifndef __MYRES_H_
#define __MYRES_H_

#include "cocos2d.h"
#include "ResBase.h"
#include "Equip.h"
#include "GongFa.h"
class MyRes
{
public:

	static int getMyResCount(std::string resid, int inwhere = MYSTORAGE);

	static void Add(std::string resid, int count = 1, int inwhere = MYSTORAGE, int qu = 0);

	static int getMyPackageCount();
public:

	static std::vector<ResBase* > vec_MyResources;//我的资源，包括背包，仓库，装备上的
	static void saveData();
private:
	static ResBase* getMyRes(std::string resid, int inwhere = MYSTORAGE);
};

#endif
