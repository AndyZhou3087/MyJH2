#include "InnRoomLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "RandHeroLayer.h"

USING_NS_CC;

InnRoomLayer::InnRoomLayer()
{

}

InnRoomLayer::~InnRoomLayer()
{
	
}


InnRoomLayer* InnRoomLayer::create(Building* buidingData)
{
	InnRoomLayer *pRet = new(std::nothrow)InnRoomLayer();
	if (pRet && pRet->init(buidingData))
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
bool InnRoomLayer::init(Building* buidingData)
{
    if ( !Layer::init() )
    {
        return false;
    }

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("innRoomLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();
	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("innroomtitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//等级
	cocos2d::ui::Text* lvUIText = (cocos2d::ui::Text*)csbnode->getChildByName("lv");
	std::string str = StringUtils::format("%d%s", buidingData->level.getValue(), ResourceLang::map_lang["lvtext"].c_str());
	lvUIText->setString(str);

	//招募按钮
	cocos2d::ui::Widget* recruitbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("recruitbtn");
	recruitbtn->setTag(0);
	recruitbtn->addTouchEventListener(CC_CALLBACK_2(InnRoomLayer::onBtnClick, this));

	//招募按钮文字
	cocos2d::ui::ImageView* recruitbtntxt = (cocos2d::ui::ImageView*)recruitbtn->getChildByName("text");
	recruitbtntxt->loadTexture(ResourcePath::makeTextImgPath("recruit_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//升级按钮
	cocos2d::ui::Widget* lvUpbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("lvupbtn");
	lvUpbtn->setTag(1);
	lvUpbtn->addTouchEventListener(CC_CALLBACK_2(InnRoomLayer::onBtnClick, this));

	//升级按钮文字
	cocos2d::ui::ImageView* lvUpbtntxt = (cocos2d::ui::ImageView*)lvUpbtn->getChildByName("text");
	lvUpbtntxt->loadTexture(ResourcePath::makeTextImgPath("innroolv_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//关闭按钮
	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(2);
	closebtn->addTouchEventListener(CC_CALLBACK_2(InnRoomLayer::onBtnClick, this));

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

void InnRoomLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
		switch (tag)
		{
		case 0://招募
		{
			RandHeroLayer* layer = RandHeroLayer::create();
			this->addChild(layer);
			break;
		}
		case 1://升级
			break;
		case 2://关闭
			this->removeFromParentAndCleanup(true);
			break;
		default:
			break;
		}
	}
}

void InnRoomLayer::onExit()
{
	Layer::onExit();
}
