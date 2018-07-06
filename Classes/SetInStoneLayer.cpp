#include "SetInStoneLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MyRes.h"
#include "TakeOnLayer.h"

USING_NS_CC;

SetInStoneLayer::SetInStoneLayer()
{
	isChangeStone = false;
}

SetInStoneLayer::~SetInStoneLayer()
{

}


SetInStoneLayer* SetInStoneLayer::create(ResBase* res, int which)
{
	SetInStoneLayer *pRet = new(std::nothrow)SetInStoneLayer();
	if (pRet && pRet->init(res, which))
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
bool SetInStoneLayer::init(ResBase* res, int which)
{
	if (!Layer::init())
	{
		return false;
	}

	m_res = res;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("setinStoneLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	cocos2d::ui::ImageView* resbox_qu = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox_qu");

	std::string qustr = "ui/resbox.png";
	resbox_qu->loadTexture(qustr, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* p_res = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	std::string str = StringUtils::format("ui/%s.png", res->getId().c_str());
	p_res->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	namelbl->setString(GlobalInstance::map_Equip[res->getId()].name);

	int intv = (atoi(res->getId().substr(1).c_str()) - 1) / 3;
	int textindex = 0;
	if (intv == 0)
		textindex = 1;
	else if (intv == 1)
		textindex = 2;
	else if (intv == 2)
		textindex = 0;
	else if (intv == 3)
		textindex = 5;
	else if (intv == 4)
		textindex = 4;

	cocos2d::ui::Text* attrlbl = (cocos2d::ui::Text*)csbnode->getChildByName("attrtext_0");

	str = StringUtils::format("addattrtext_%d", textindex);
	str = StringUtils::format(ResourceLang::map_lang[str].c_str(), stonebns[intv]);
	if (textindex >= 0 && textindex <= 2)
		str.append("%");
	attrlbl->setString(str);


	//°´Å¥
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(SetInStoneLayer::onBtnClick, this));
	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("stonein_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//Equip* equip = (Equip*)res;
	//if (equip->vec_stones[which].length() > 1)
	//{
	//	isChangeStone = true;
	//	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("replacebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	//}

	//ÆÁ±ÎÏÂ²ãµã»÷
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

void SetInStoneLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		TakeOnLayer* takeOnLayer = (TakeOnLayer*)this->getParent()->getParent();
		if (!isChangeStone)
			takeOnLayer->setInStone(m_res, this->getTag());
		else
			takeOnLayer->chageStone(m_res, this->getTag());
		this->getParent()->removeFromParentAndCleanup(true);
	}
}

void SetInStoneLayer::onExit()
{
	Layer::onExit();
}