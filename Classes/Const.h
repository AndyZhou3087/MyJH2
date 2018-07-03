
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

#define MAPBLOCKWIDTH 72
#define MAPBLOCKHEIGHT 72

const static float creatorResLvUpPara[] = {0.5f, 0.9f, 0.7f, 0.8f, 1.0f};
//5个品质颜色
const static Color3B POTENTIALCOLOR[5] = { Color3B(36, 255, 82), Color3B(52, 179, 240), Color3B(252, 77, 226), Color3B(255, 214, 71), Color3B(255, 69, 69) };
#endif // _H_CONST_H_
