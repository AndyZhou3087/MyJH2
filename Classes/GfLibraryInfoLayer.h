#ifndef __GFLIBRARYINFO_LAYER_H__
#define __GFLIBRARYINFO_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
USING_NS_CC;

class GfLibraryInfoLayer : public cocos2d::Layer
{
public:
	GfLibraryInfoLayer();
	~GfLibraryInfoLayer();
	virtual void onEnterTransitionDidFinish();
	static GfLibraryInfoLayer* create(std::string gfid);
	virtual void onExit();
	bool init(std::string gfid);
	void getGfAttr(std::string gfid);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	int status;
	cocos2d::ui::Text* attrlblArr[6];
	cocos2d::ui::Text* namelbl;
};

#endif
