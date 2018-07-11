#pragma once
#ifndef __MaterialDescLayer_LAYER_H__
#define __MaterialDescLayer_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
USING_NS_CC;

class MaterialDescLayer : public cocos2d::Layer
{
public:
	MaterialDescLayer();
	~MaterialDescLayer();

	static MaterialDescLayer* create(std::string resid);
	virtual void onExit();
	bool init(std::string resid);

	void setResId(std::string resid);

private:
	std::string m_resid;
	cocos2d::ui::Text* m_text;

private:
	void loadMaterialDesc();
	void delayRemoveSelf(float dt);
};

#endif
