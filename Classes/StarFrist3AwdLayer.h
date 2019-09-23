#ifndef __STARFIRST3_LAYER_H__
#define __STARFIRST3_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
USING_NS_CC;

class StarFrist3AwdLayer : public cocos2d::Layer
{
public:
	StarFrist3AwdLayer();
	~StarFrist3AwdLayer();

	static StarFrist3AwdLayer* create(std::vector<std::string> vec_awdstr);
	bool init(std::vector<std::string> vec_awdstr);

private:

};

#endif
