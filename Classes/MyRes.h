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

	static void Add(std::string resid, int count = 1, int inwhere = MYSTORAGE, int qu = 0, int stonescount = 0);

	//根据索引，做删除操作简单
	//1--成功，0--失败，不够时失败
	static int Use(int iterindex, int count = 1);

	//像装备，功法同一个ID会有多个，无法只根据ID查找
	static int Use(ResBase* res, int count = 1, int inwhere = MYSTORAGE);

	//基础资源，可有多个数量的用此方法
	static int Use(std::string resid, int count = 1, int inwhere = MYSTORAGE);

	static int getMyPackageCount();
public:

	static std::vector<ResBase* > vec_MyResources;//我的资源，包括背包，仓库，装备上的
	static void saveData();
private:
	static ResBase* getMyRes(std::string resid, int inwhere = MYSTORAGE);
};

#endif
