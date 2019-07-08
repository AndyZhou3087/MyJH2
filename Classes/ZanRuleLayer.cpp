#include "ZanRuleLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "AnimationEffect.h"

USING_NS_CC;

ZanRuleLayer::ZanRuleLayer()
{

}

ZanRuleLayer::~ZanRuleLayer()
{

}


ZanRuleLayer* ZanRuleLayer::create()
{
	ZanRuleLayer*pRet = new(std::nothrow)ZanRuleLayer();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

// on "init" you need to initialize your instance
bool ZanRuleLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 160));
	this->addChild(color, 0, "colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("matchRuleLayer.csb"));
	this->addChild(csbnode);

	cocos2d::ui::Text* title = (cocos2d::ui::Text*)csbnode->getChildByName("title");
	title->setString(ResourceLang::map_lang["zanruletitle"]);

	cocos2d::ui::Text* title2 = (cocos2d::ui::Text*)csbnode->getChildByName("title2");
	title2->setString(ResourceLang::map_lang["zanrulecontent"]);
	title2->setVisible(false);

	cocos2d::ui::ScrollView* scrollView = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollView");
	std::string str = CommonFuncs::replace_all(ResourceLang::map_lang["zanruletext"], "\\n", "\n");

	if (GlobalInstance::zanrwdinfo.length() > 0)
	{
		std::string ruleawardtext;

		ruleawardtext = CommonFuncs::replace_all(GlobalInstance::zanrwdinfo, "\\n", "\n");
		str.append(ruleawardtext);
	}

	Label* contentlbl = Label::createWithTTF(str, FONT_NAME, 25);
	contentlbl->setAnchorPoint(Vec2(0, 1));
	contentlbl->setColor(Color3B(255, 255, 255));
	contentlbl->enableShadow(Color4B::BLACK, Size(1, -1));
	contentlbl->setLineBreakWithoutSpace(true);
	contentlbl->setMaxLineWidth(scrollView->getContentSize().width);
	contentlbl->setLineSpacing(8);
	scrollView->addChild(contentlbl);
	int innerheight = contentlbl->getStringNumLines() * 33;//contentlbl->getHeight();
	int contentheight = scrollView->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollView->setInnerContainerSize(Size(scrollView->getContentSize().width, innerheight));
	contentlbl->setPosition(Vec2(0, innerheight));
	

	//�����²���
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		AnimationEffect::closeAniEffect((Layer*)this);
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}