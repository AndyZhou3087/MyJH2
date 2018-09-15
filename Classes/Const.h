
#ifndef _H_CONST_H_
#define _H_CONST_H_
#include "cocos2d.h"

#define ANDOIRJNICLSNAME "com/csfb/myjh/Utils"

#define FONT_NAME "fonts/simhei.ttf"

#define FIRSTNAMECOUNT 495
#define LASTNAMECOUNT0 2306
#define LASTNAMECOUNT1 4159
#define HERO_RESETTIME 21600//英雄刷新时间
#define RES_REFRESHTIME 60//资源刷新时间
#define MARKET_RESETTIME 3600//市场刷新时间

#define HEROPOWER_RESETTIME 300//体力刷新时间

#define MAPBLOCKWIDTH 72
#define MAPBLOCKHEIGHT 72

#define RESCREATOREMAXLV 50

#define CLICKOFFSETP 8

const static float creatorResLvUpPara[] = {0.5f, 0.9f, 0.7f, 0.8f, 1.0f};
//5个品质颜色
const static Color3B POTENTIALCOLOR[5] = { Color3B(36, 255, 82), Color3B(52, 179, 240), Color3B(252, 77, 226), Color3B(255, 214, 71), Color3B(255, 69, 69) };

//镶嵌宝石加成
const static float STONE_BNS[][3] = { {38.0f, 121.0f, 299.0f},{19.0f, 61.0f, 150.0f},{375.0f,1210.0f, 2990.0f},{0.375f, 1.21f, 2.99f},{0.375f, 1.21f, 2.99f} };

const static float POTENTIAL_BNS[] = {1.0f, 1.27f, 1.63f, 2.08f, 2.63f};

//突破等级系数
const static float BREAK_BNS[] = { 1.0f, 1.1f, 1.12f, 1.15f, 1.19f, 1.24f };

//装备强化失败降级数
const static int COSTLV[] = { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 15, 16, 17, 18, 19 };
//装备强化成功概率
const static float ODDS[] = { 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 96.5f, 92.5f, 88.0f, 83.0f, 77.5f, 71.5f, 65.0f, 58.0f, 50.5f, 42.5f, 34.0f, 25.0f, 15.5f, 5.5f };

//区分资源类型的字符
const static std::string RES_TYPES_CHAR[] = { "r","a","e","g","f","w","x","s","c","d","m","b","y","v","t","p" };

const static std::string defaultres = "r001-500-1;r002-500-1;r003-500-1;r004-500-1;t001-10-1;r005-500-1;w001-1-1-0-0-;x001-1-1-0-0-";

#endif // _H_CONST_H_
