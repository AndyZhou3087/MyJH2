#include "ExchangePieceLayer.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyMenu.h"
#include "AnimationEffect.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "StoreHouseLayer.h"
#include "MainScene.h"
#include "ResDescLayer.h"

ExchangePieceLayer::ExchangePieceLayer()
{

}


ExchangePieceLayer::~ExchangePieceLayer()
{

}


ExchangePieceLayer* ExchangePieceLayer::create(std::string exchangeeid, int exchangecount)
{
	ExchangePieceLayer*pRet = new(std::nothrow)ExchangePieceLayer();
	if (pRet && pRet->init(exchangeeid, exchangecount))
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

bool ExchangePieceLayer::init(std::string exchangeeid, int exchangecount)
{
	if (!Layer::init())
	{
		return false;
	}
	m_exchangerid = exchangeeid;
	m_count = exchangecount;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node *csbnode = CSLoader::createNode(ResourcePath::makePath("selectEquipLayer.csb"));
	this->addChild(csbnode);

	cocos2d::ui::ImageView* bg = (cocos2d::ui::ImageView*)csbnode->getChildByName("bg");
	bg->setContentSize(Size(bg->getContentSize().width, 580));

	scrollview = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollview");
	scrollview->setScrollBarEnabled(false);
	scrollview->setBounceEnabled(true);

	scrollview->setContentSize(Size(scrollview->getContentSize().width, 500));

	loadscrollview();

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

void ExchangePieceLayer::loadscrollview()
{
	int itemheight = 160;
	int ressize = sizeof(qu4epiece)/sizeof(qu4epiece[0]);

	int realcount = ressize - 1;

	int row = realcount % 4 == 0 ? realcount / 4 : (realcount / 4 + 1);
	int innerheight = itemheight * row;

	int contentheight = scrollview->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollview->setInnerContainerSize(Size(scrollview->getContentSize().width, innerheight));

	int index = 0;
	for (int m = 0; m < ressize; m++)
	{

		if (qu4epiece[m].compare(m_exchangerid) == 0)
			continue;

		std::string qustr = "ui/resbox.png";

		Sprite * qubox = Sprite::createWithSpriteFrameName(qustr);

		MenuItemSprite* boxItem = MenuItemSprite::create(
			qubox,
			qubox,
			qubox,
			CC_CALLBACK_1(ExchangePieceLayer::onclick, this));
		boxItem->setTag(m);

		boxItem->setPosition(Vec2(qubox->getContentSize().width / 2 + 20 + index % 4 * itemheight, innerheight - itemheight / 2 - index / 4 * itemheight));

		MyMenu* menu = MyMenu::create();
		menu->addChild(boxItem, 0, "item");
		menu->setTouchlimit(scrollview);
		menu->setPosition(Vec2(0, 0));

		scrollview->addChild(menu, 0, m);

		std::string resid = qu4epiece[m];

		std::string str = GlobalInstance::getInstance()->getResUIFrameName(resid, 0);

		Sprite * res = Sprite::createWithSpriteFrameName(str);
		res->setPosition(Vec2(boxItem->getContentSize().width / 2, boxItem->getContentSize().height / 2));
		boxItem->addChild(res);

		Sprite* pieceicon = Sprite::createWithSpriteFrameName("ui/pieceicon.png");
		pieceicon->setAnchorPoint(Vec2(0, 1));
		pieceicon->setPosition(10, boxItem->getContentSize().height - 10);
		boxItem->addChild(pieceicon);

		std::string namestr = GlobalInstance::map_AllResources[resid].name;

		Label *namelbl = Label::createWithTTF(namestr, FONT_NAME, 20);
		namelbl->setColor(Color3B(26, 68, 101));
		namelbl->setPosition(Vec2(boxItem->getContentSize().width / 2, -10));
		boxItem->addChild(namelbl, 0, "resname");

		std::string countstr = StringUtils::format("%d/%d", 1, MyRes::getMyResCount(resid));
		Label *countlbl = Label::createWithTTF(countstr, FONT_NAME, 23);
		countlbl->setAnchorPoint(Vec2(1, 0));
		countlbl->setColor(Color3B::WHITE);
		countlbl->setPosition(Vec2(boxItem->getContentSize().width - 10, 10));
		boxItem->addChild(countlbl);
		index++;
	}
}

void ExchangePieceLayer::onclick(Ref* pSender)
{
	SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
	Node* node = (Node*)pSender;

	int tag = node->getTag();

	std::string resid = qu4epiece[tag];

	std::string str = StringUtils::format(ResourceLang::map_lang["exchangepiecesucc"].c_str(), GlobalInstance::map_AllResources[resid].name.c_str());
	MovingLabel::show(str);
	MyRes::Add(resid, 1);
	MyRes::Use(m_exchangerid, m_count);

	if (MyRes::getMyResCount(m_exchangerid) > 0)
	{
		ResDescLayer* parentlayer = (ResDescLayer*)this->getParent();
		parentlayer->updateCountlbl();
		AnimationEffect::closeAniEffect(this);
	}
	else
	{
		AnimationEffect::closeAniEffect((Layer*)this->getParent());
	}


	if (g_mainScene != NULL)
	{
		StoreHouseLayer* mlayer = (StoreHouseLayer*)g_mainScene->getChildByName("3storehouse");
		if (mlayer != NULL)
		{
			mlayer->updateUI();
		}
	}
}