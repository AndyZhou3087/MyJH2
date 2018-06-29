#include "MapEventLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"

USING_NS_CC;

MapEventLayer::MapEventLayer()
{

}

MapEventLayer::~MapEventLayer()
{
	
}


MapEventLayer* MapEventLayer::create(int eventindex)
{
	MapEventLayer *pRet = new(std::nothrow)MapEventLayer();
	if (pRet && pRet->init(eventindex))
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
bool MapEventLayer::init(int eventindex)
{
    if ( !Layer::init() )
    {
        return false;
    }

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("mapEventLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	std::string str = StringUtils::format("images/event%d.jpg", eventindex);
	cocos2d::ui::ImageView* eventimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("eventimg");
	eventimg->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::LOCAL);
	
	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		this->removeFromParentAndCleanup(true);
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

void MapEventLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{

	}
}

void MapEventLayer::onExit()
{
	Layer::onExit();
}