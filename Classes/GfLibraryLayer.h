#ifndef _GFLIBRARY_LAYER_H_
#define _GFLIBRARY_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
USING_NS_CC;

class GfLibraryLayer :public Layer
{
public:
	GfLibraryLayer();
	~GfLibraryLayer();

	virtual bool init(int type);

	static GfLibraryLayer* create(int type);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void onclick(Ref* pSender);

	void updateScrollviewContent();

	static bool larger_gfQu(std::string a, std::string b);
private:
	cocos2d::ui::ScrollView* scrollview;
	int mtype;
};
#endif

