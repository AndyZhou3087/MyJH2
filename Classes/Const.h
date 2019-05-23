
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
#define CLICKOFFSETP 10

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

#define REPAIRTIME 600

//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//#define UMENG
//#include "UMeng/Analytics/MobClickCpp.h"
//#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#define encrypt_jsonxml 1 //加密
#endif

const static float newguidetime = 0.3f;

const static float creatorResLvUpPara[] = {0.5f, 0.9f, 0.7f, 0.8f, 1.0f};
//5个品质颜色
const static cocos2d::Color3B POTENTIALCOLOR[5] = { cocos2d::Color3B(36, 255, 82), cocos2d::Color3B(52, 179, 240), cocos2d::Color3B(252, 77, 226), cocos2d::Color3B(255, 214, 71), cocos2d::Color3B(255, 69, 69) };

//镶嵌宝石加成
const static float STONE_BNS[][3] = { {38.0f, 121.0f, 299.0f},{19.0f, 61.0f, 150.0f},{375.0f,1210.0f, 2990.0f},{0.375f, 1.21f, 2.99f},{0.375f, 1.21f, 2.99f} };

const static float POTENTIAL_BNS[] = {1.0f, 1.27f, 1.63f, 2.10f, 2.70f};

//突破等级系数
const static float BREAK_BNS[] = { 1.0f, 1.1f, 1.12f, 1.15f, 1.19f, 1.24f };

//初始最大等级
const static int BASEMAXHEROLV = 50;

//最大等级后，根据解锁的章节开放多少级
const static int ADDMAXHEROLV[] = {0,0,0,0,5,5,0};

//装备强化失败降级数
const static int COSTLV[] = { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 15, 16, 17, 18, 19 };
//装备强化成功概率
const static float ODDS[] = { 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 96.5f, 92.5f, 88.0f, 83.0f, 77.5f, 71.5f, 65.0f, 58.0f, 50.5f, 42.5f, 34.0f, 25.0f, 15.5f, 5.5f };

//区分资源类型的字符
const static std::string RES_TYPES_CHAR[] = { "r","a","e","g","f","w","x", "q", "c", "s" ,"d","m","i","b","y","v","t","p","u","k","z"};

const static std::string defaultres = "r001-500-1;r002-500-1;r003-500-1;r004-500-1;t001-10-1;r005-500-1;r009-10-1;p001-5-1;v001-2-1;z001-2-1;w001-1-1-0-0-;x001-1-1-0-0-";

//迷宫地图个数
const static int mazes[] = {2, 5, 3, 3, 2};
const static int mazesfightbg[] = {2, 3, 2, 2, 3};

const static int VIPDAYS[] = { 30,30,365 };
#endif // _H_CONST_H_
