#include "SetInStoneLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MyRes.h"
#include "TakeOnLayer.h"
#include "SelectEquipLayer.h"
#include "AnimationEffect.h"

USING_NS_CC;

static bool isChangeStone = false;
SetInStoneLayer::SetInStoneLayer()
{
}

SetInStoneLayer::~SetInStoneLayer()
{
	isChangeStone = false;
}


SetInStoneLayer* SetInStoneLayer::create(ResBase* res, int which, Hero* herodata)
{
	SetInStoneLayer *pRet = new(std::nothrow)SetInStoneLayer();
	if (pRet && pRet->init(res, which, herodata))
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
bool SetInStoneLayer::init(ResBase* res, int which, Hero* herodata)
{
	if (!Layer::init())
	{
		return false;
	}

	m_herodata = herodata;

	m_res = res;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("setinStoneLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	//°´Å¥
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(SetInStoneLayer::onBtnClick, this));
	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("stonein_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	actionbtn->setTag(1000);

	cocos2d::ui::Widget* takeoffbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn_0");
	takeoffbtn->addTouchEventListener(CC_CALLBACK_2(SetInStoneLayer::onBtnClick, this));
	cocos2d::ui::ImageView* takeoffbtntxt = (cocos2d::ui::ImageView*)takeoffbtn->getChildByName("text");
	takeoffbtntxt->loadTexture(ResourcePath::makeTextImgPath("takeoffbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	takeoffbtn->setTag(1001);

	std::string stoneid;

	if (m_res->getType() == T_STONE)
	{
		stoneid = res->getId();
		takeoffbtn->setVisible(false);
	}
	else
	{
		Equip* equip = (Equip*)m_res;
		stoneid = equip->vec_stones[which];
		actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("replacebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		actionbtn->setPositionX(230);
		actionbtn->setTag(1002);
	}
	//Equip* equip = (Equip*)res;
	//if (equip->vec_stones[which].length() > 1)
	//{
	//	isChangeStone = true;

	//}

	cocos2d::ui::ImageView* resbox_qu = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox_qu");

	std::string qustr = "ui/resbox.png";
	resbox_qu->loadTexture(qustr, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* p_res = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	std::string str = StringUtils::format("ui/%s.png", stoneid.c_str());
	p_res->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	namelbl->setString(GlobalInstance::map_AllResources[stoneid].name);

	int intid = atoi(stoneid.substr(1).c_str()) - 1;
	int intv = intid / 3;
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
	str = StringUtils::format(ResourceLang::map_lang[str].c_str(), STONE_BNS[intv][intid%3]);
	if (textindex >= 0 && textindex <= 2)
		str.append("%");
	attrlbl->setString(str);

	//ÆÁ±ÎÏÂ²ãµã»÷
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

void SetInStoneLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		int tag = ((Node*)pSender)->getTag();
		switch (tag)
		{
		case 1000:
		{
			if (isChangeStone)
			{
				TakeOnLayer* takeOnLayer = (TakeOnLayer*)this->getParent()->getParent()->getParent();
				takeOnLayer->chageStone(m_res, this->getTag());
				this->getParent()->getParent()->removeFromParentAndCleanup(true);
			}
			else
			{
				TakeOnLayer* takeOnLayer = (TakeOnLayer*)this->getParent()->getParent();
				takeOnLayer->setInStone(m_res, this->getTag());
				this->getParent()->removeFromParentAndCleanup(true);
			}
			break;
		}
		case 1001:
		{
			Equip* equip = (Equip*)m_res;
			TakeOnLayer* takeOnLayer = (TakeOnLayer*)this->getParent();
			takeOnLayer->setOutStone(equip->vec_stones[this->getTag()], this->getTag());
			AnimationEffect::closeAniEffect((Layer*)this);
		}
		break;
		case 1002:
		{
			isChangeStone = true;
			SelectEquipLayer* slayer = SelectEquipLayer::create(T_STONE, m_herodata);
			this->addChild(slayer, 0, this->getTag());
			AnimationEffect::openAniEffect((Layer*)slayer);
			break;
		}

		default:
			break;
		}		
	}
}

void SetInStoneLayer::onExit()
{
	Layer::onExit();
}