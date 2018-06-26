#include "ResDescLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MovingLabel.h"
#include "MyRes.h"
#include "StoreHouseLayer.h"

USING_NS_CC;

ResDescLayer::ResDescLayer()
{
	status = S_OTHER;
}

ResDescLayer::~ResDescLayer()
{
	
}


ResDescLayer* ResDescLayer::create(ResBase* res, int fromwhere)
{
	ResDescLayer *pRet = new(std::nothrow)ResDescLayer();
	if (pRet && pRet->init(res, fromwhere))
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
bool ResDescLayer::init(ResBase* res, int fromwhere)
{
    if ( !Layer::init() )
    {
        return false;
    }

	m_res = res;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("resDescLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	namelbl->setString(GlobalInstance::map_AllResources[res->getId()].name);

	cocos2d::ui::Text* desclbl = (cocos2d::ui::Text*)csbnode->getChildByName("desclbl");
	desclbl->setString(GlobalInstance::map_AllResources[res->getId()].desc);

	std::string str = StringUtils::format(ResourceLang::map_lang["rescount"].c_str(), res->getCount().getValue());

	cocos2d::ui::Text* coutlbl = (cocos2d::ui::Text*)csbnode->getChildByName("coutlbl");
	coutlbl->setString(str);

	//按钮
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(ResDescLayer::onBtnClick, this));
	
	std::string btntextstr;

	if (res->getType() == T_BOX)
	{
		btntextstr = "usebtn_text";
		status = S_CAN_USE;
	}
	else
	{
		if (GlobalInstance::map_AllResources[res->getId()].vec_needres.size() > 0)
		{
			status = S_CAN_DECOMPOSE;
			btntextstr = "decomposebtn_text";
		}
		else
			btntextstr = "closebtn_text";
	}
	if (btntextstr.length() > 0)
	{
		//按钮文字
		cocos2d::ui::ImageView* btntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
		btntxt->loadTexture(ResourcePath::makeTextImgPath(btntextstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	}
	else
	{
		actionbtn->setVisible(false);
	}

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

void ResDescLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (status == S_CAN_DECOMPOSE)
		{
			StoreHouseLayer* storelayer = (StoreHouseLayer*)this->getParent();
			if (storelayer != NULL)
				storelayer->decompose(m_res);
		}
		this->removeFromParentAndCleanup(true);
	}
}

void ResDescLayer::onExit()
{
	Layer::onExit();
}