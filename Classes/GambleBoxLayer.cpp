#include "GambleBoxLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MovingLabel.h"
#include "AnimationEffect.h"
#include "MapEventLayer.h"
#include "DynamicValue.h"

USING_NS_CC;

GambleBoxLayer::GambleBoxLayer()
{

}

GambleBoxLayer::~GambleBoxLayer()
{

}


GambleBoxLayer* GambleBoxLayer::create(int isWin, int diceid, int cost)
{
	GambleBoxLayer *pRet = new(std::nothrow)GambleBoxLayer();
	if (pRet && pRet->init(isWin, diceid, cost))
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
bool GambleBoxLayer::init(int isWin, int diceid, int cost)
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color, 0, "colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("gambleBoxLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	//按钮1
	cocos2d::ui::Widget* okbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("okbtn");
	okbtn->addTouchEventListener(CC_CALLBACK_2(GambleBoxLayer::onBtnClick, this));
	okbtn->setTag(1);
	//按钮1文字
	cocos2d::ui::ImageView* okbtntxt = (cocos2d::ui::ImageView*)okbtn->getChildByName("text");
	okbtntxt->loadTexture(ResourcePath::makeTextImgPath("continuebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//按钮2
	cocos2d::ui::Widget* cancelbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("cancelbtn");
	cancelbtn->addTouchEventListener(CC_CALLBACK_2(GambleBoxLayer::onBtnClick, this));
	cancelbtn->setTag(0);
	//按钮2文字
	cocos2d::ui::ImageView* cancelbtntxt = (cocos2d::ui::ImageView*)cancelbtn->getChildByName("text");
	cancelbtntxt->loadTexture(ResourcePath::makeTextImgPath("backbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* dice = (cocos2d::ui::ImageView*)csbnode->getChildByName("dice");
	std::string str = StringUtils::format("ui/dice%d.png", diceid);
	dice->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* win = (cocos2d::ui::Text*)csbnode->getChildByName("win");
	str = StringUtils::format("gamble_%d", isWin);
	win->setString(ResourceLang::map_lang[str]);

	cocos2d::ui::Text* gettext = (cocos2d::ui::Text*)csbnode->getChildByName("gettext");
	gettext->setString(ResourceLang::map_lang["gettext"]);
	if (isWin == 0)
	{
		gettext->setVisible(false);
		/*DynamicValueInt dvl;
		dvl.setValue(cost);
		GlobalInstance::getInstance()->costMyCoinCount(dvl);*/
	}
	else
	{
		cost = cost * 2;
		cocos2d::ui::Text* count = (cocos2d::ui::Text*)gettext->getChildByName("count");
		str = StringUtils::format("%d", cost);
		count->setString(str);
		DynamicValueInt dvl;
		dvl.setValue(cost);
		GlobalInstance::getInstance()->addMyCoinCount(dvl);
	}


	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	/*listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		AnimationEffect::closeAniEffect((Layer*)this);
	};*/
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void GambleBoxLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
		switch (tag)
		{
		case 0:
		{
			MapEventLayer* eventlayer = (MapEventLayer*)this->getParent();
			if (eventlayer != NULL)
			{
				AnimationEffect::closeAniEffect((Layer*)this);
				AnimationEffect::closeAniEffect((Layer*)eventlayer);
			}
		}
			break;
		case 1:
		{
			MapEventLayer* eventlayer = (MapEventLayer*)this->getParent();
			if (eventlayer != NULL)
			{
				eventlayer->continueGamble();
			}
			AnimationEffect::closeAniEffect((Layer*)this);
		}
		default:
			break;
		}
	}
}