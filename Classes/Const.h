
#ifndef _H_CONST_H_
#define _H_CONST_H_
#include "cocos2d.h"

#define ANDOIRJNICLS "com/game/myjh"

#define FONT_NAME "fonts/simhei.ttf"

#define MANNAMECOUNT 474
#define FEMALENAMECOUNT 71

#define HERO_RESETTIME 3600//英雄刷新时间
#define RES_REFRESHTIME 60//资源刷新时间
#define MARKET_RESETTIME 3600//市场刷新时间

#define HEROPOWER_RESETTIME 300//体力刷新时间

#define TWENTYFOURHOURSTOSEC 86400//24小时描述

#define MAPBLOCKWIDTH 72
#define MAPBLOCKHEIGHT 72

#define RESCREATOREMAXLV 50
#define CLICKOFFSETP 20

#define BETCOSTCOIN 50 //

#define COINREFRESH_MARKET_NUM 20//元宝刷新市场
#define SILVERREFRESH_MARKET_NUM 2000//银两刷新市场

#define COINREFRESH_HERO_NUM 50//元宝刷新招募英雄
#define SILVERREFRESH_HERO_NUM 100//银两刷新招募英雄

#define TIPSCOUNT 8

#define MAP_BET 1

#define USE_TRANSCENE 1

#define MODIFYNICKNAMECOUNT_COIN 50

#define QUMAX 5

#define REPAIRTIME 60

//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//#define UMENG
//#include "UMeng/Analytics/MobClickCpp.h"
//#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#define encrypt_jsonxml 1 //加密
#endif
#define encrypt_jsonxml 1
const static float newguidetime = 0.3f;

const static float creatorResLvUpPara[] = {0.5f, 0.9f, 0.7f, 0.8f, 1.0f};
//5个品质颜色
const static cocos2d::Color3B POTENTIALCOLOR[5] = { cocos2d::Color3B(36, 255, 82), cocos2d::Color3B(52, 179, 240), cocos2d::Color3B(252, 77, 226), cocos2d::Color3B(255, 214, 71), cocos2d::Color3B(255, 69, 69) };

//镶嵌宝石加成
const static float STONE_BNS[][3] = { {38.0f, 121.0f, 299.0f},{19.0f, 61.0f, 150.0f},{375.0f,1210.0f, 2990.0f},{0.275f, 0.9f, 1.99f},{0.375f, 1.21f, 2.99f} };

//品质系数
const static float POTENTIAL_BNS[] = {1.0f, 1.27f, 1.63f, 2.10f, 2.75f};

//突破等级系数
const static float BREAK_BNS[] = { 1.0f, 1.1f, 1.12f, 1.15f, 1.19f, 1.24f, 1.3f};

//初始最大等级
const static int BASEMAXHEROLV = 50;

//最大等级后，根据解锁的章节开放多少级
const static int ADDMAXHEROLV[] = {0,0,0,0,5,5,0,0};

//强化最高等级
const static int STHRENTHMAXLV = 20;
//装备强化失败降级数
const static int COSTLV[] = { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 4 };
//装备强化成功概率
const static float ODDS[] = { 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 95.0f, 90.0f, 85.0f, 80.0f, 75.0f, 70.0f, 65.0f, 60.0f, 55.0f, 50.0f, 45.0f, 40.0f, 30.0f, 20.0f, 15.0f };
//装备强化失败后降级概率
const static int FAILCOSTLV[] = { 0, 0, 0, 0, 0, 10, 20, 25, 30, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90 };

//区分资源类型的字符
const static std::string RES_TYPES_CHAR[] = { "r","a","e","g","f","w","x", "q", "c", "s" ,"d","m","i","b","y","v","t","p","u","k","z","j","l"};

const static std::string defaultres = "r001-500-1;r002-500-1;r003-500-1;r004-500-1;t001-10-1;r005-500-1;r009-10-1;p001-5-1;v001-2-1;z001-2-1;w001-1-1-0-0-;x001-1-1-0-0-";

//迷宫地图个数
const static int mazes[] = {2, 5, 3, 3, 2};
const static int mazesfightbg[] = {2, 3, 2, 2, 3};

const static int VIPDAYS[] = { 30,30,365,30};

const static int ADDZANMAINTASKCOUNTS[] = {5, 10, 20, 30, 40, 50, 60, 80, 100, 120};

const static std::string qu4epiece[] = {"la008", "la004","la028","la040","la016","la024","la012","la020","le004", "lg004", "lf004"};
#endif // _H_CONST_H_
