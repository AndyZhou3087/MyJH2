#ifndef _BOOKLIBRARY_LAYER_H_
#define _BOOKLIBRARY_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
USING_NS_CC;

class BookLibraryLayer :public Layer
{
public:
	BookLibraryLayer();
	~BookLibraryLayer();

	virtual bool init();

	static BookLibraryLayer* create();

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void onclick(Ref* pSender);

	void updateScrollviewContent();

private:
	cocos2d::ui::ScrollView* scrollview;
	std::vector<std::string> vec_res;
};
#endif

