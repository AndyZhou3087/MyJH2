
#ifndef _H_CONST_H_
#define _H_CONST_H_
#include "cocos2d.h"

#define ANDOIRJNICLSNAME "com/csfb/myjh/Utils"

#define FONT_NAME "fonts/simhei.ttf"

#define FIRSTNAMECOUNT 10
#define LASTNAMECOUNT 10
#define HERO_RESETTIME 21600//英雄刷新时间
#define RES_REFRESHTIME 60//资源刷新时间
#define MARKET_RESETTIME 60//3600//市场刷新时间

#define HEROPOWER_RESETTIME 300//体力刷新时间

#define MAPBLOCKWIDTH 72
#define MAPBLOCKHEIGHT 72

#define RESCREATOREMAXLV 50

const static float creatorResLvUpPara[] = {0.5f, 0.9f, 0.7f, 0.8f, 1.0f};
//5个品质颜色
const static Color3B POTENTIALCOLOR[5] = { Color3B(36, 255, 82), Color3B(52, 179, 240), Color3B(252, 77, 226), Color3B(255, 214, 71), Color3B(255, 69, 69) };

//镶嵌宝石加成
const static float STONE_BNS[][3] = { {38.0f, 121.0f, 299.0f},{19.0f, 61.0f, 150.0f},{375.0f,1210.0f, 2990.0f},{0.375f, 1.21f, 2.99f},{0.375f, 1.21f, 2.99f} };

const static float POTENTIAL_BNS[] = {1.0f, 1.27f, 1.63f, 2.08f, 2.63f};

//突破等级系数
const static float BREAK_BNS[] = { 1.1f, 1.12f, 1.15f, 1.19f, 1.24f, 1.24f };

//区分资源类型的字符
const static std::string RES_TYPES_CHAR[] = { "r","a","e","g","f","w","x","s","c","d","m","b","y" };
#endif // _H_CONST_H_
