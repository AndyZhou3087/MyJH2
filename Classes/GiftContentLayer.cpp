#include "GiftContentLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MovingLabel.h"
#include "MainScene.h"
#include "AnimationEffect.h"
#include "ShopNode.h"
#include "DynamicValue.h"
#include "ShopLayer.h"

USING_NS_CC;

GiftContentLayer::GiftContentLayer()
{

}

GiftContentLayer::~GiftContentLayer()
{

}


GiftContentLayer* GiftContentLayer::create(ShopData* data, int tag)
{
	GiftContentLayer *pRet = new(std::nothrow)GiftContentLayer();
	if (pRet && pRet->init(data, tag))
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
bool GiftContentLayer::init(ShopData* data, int tag)
{
	if (!Layer::init())
	{
		return false;
	}

	m_data = data;
	m_tag = tag;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color, 0, "colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("giftDescLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	//°´Å¥
	cocos2d::ui::Button* buybtn = (cocos2d::ui::Button*)csbnode->getChildByName("buybtn");
	buybtn->addTouchEventListener(CC_CALLBACK_2(GiftContentLayer::onBtnClick, this));

	cocos2d::ui::ImageView* title = (cocos2d::ui::ImageView*)csbnode->getChildByName("title");
	std::string str = StringUtils::format("text_%s", data->icon.c_str());
	title->loadTexture(ResourcePath::makeTextImgPath(str, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* lefttime = (cocos2d::ui::Text*)csbnode->getChildByName("lefttime");

	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	cocos2d::ui::Text* price = (cocos2d::ui::Text*)csbnode->getChildByName("price");
	str = StringUtils::format(ResourceLang::map_lang["shoppricetext"].c_str(), data->price);
	price->setString(str);
	if (data->type == GIFT)
	{
		str = StringUtils::format("shoptext_%d", data->type);
		desc->setString(str);
	}
	else if (data->type == VIP)
	{
		str = StringUtils::format("shoptext_%d", data->type);
		str = StringUtils::format(ResourceLang::map_lang[str].c_str(), data->name.c_str());
		desc->setString(str);
		std::map<std::string, int>::iterator it;
		for (it = GlobalInstance::map_buyVipDays.begin(); it != GlobalInstance::map_buyVipDays.end(); ++it)
		{
			if (data->icon.compare(it->first) == 0)
			{
				lefttime->setVisible(true);
				str = StringUtils::format(ResourceLang::map_lang["lefttimetext"].c_str(), it->second);
				lefttime->setString(str);
			}
		}
	}

	std::vector<int> startx;
	std::vector<int> starty;
	if (data->res.size() < 4)
	{
		for (int i = 0; i < 3; i++)
		{
			startx.push_back(202 + i * 160);
			starty.push_back(660);
		}
	}
	else if (data->res.size() < 5)
	{
		for (int i = 0; i < 3; i++)
		{
			startx.push_back(202 + i * 160);
			starty.push_back(730);
		}
		startx.push_back(362);
		starty.push_back(573);
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			startx.push_back(202 + i * 160);
			starty.push_back(730);
		}
		for (int j = 0; j < 2; j++)
		{
			startx.push_back(274 + j * 176);
			starty.push_back(566);
		}
	}
	for (unsigned int i = 0; i < data->res.size(); i++)
	{
		std::vector<std::string> vec_res = data->res[i];
		std::string resid = vec_res[0];
		int count = atoi(vec_res[1].c_str());
		int qu = atoi(vec_res[2].c_str());
		std::string str = StringUtils::format("ui/resbox_qu%d.png", qu);
		Sprite* box = Sprite::createWithSpriteFrameName(str);
		this->addChild(box);
		box->setPosition(Vec2(startx[i], starty[i]));
		str = StringUtils::format("ui/%s.png", resid.c_str());
		if (qu == 3)
		{
			str = StringUtils::format("ui/%s-2.png", resid.c_str());
		}
		else if (qu == 4)
		{
			str = StringUtils::format("ui/%s-3.png", resid.c_str());
		}
		Sprite* res = Sprite::createWithSpriteFrameName(str);
		box->addChild(res);
		res->setPosition(Vec2(box->getContentSize().width / 2, box->getContentSize().height / 2));

		Label *namelbl = Label::createWithTTF(GlobalInstance::map_AllResources[resid].name, FONT_NAME, 23);
		namelbl->setPosition(Vec2(box->getContentSize().width / 2, -15));
		namelbl->setColor(Color3B(35, 74, 79));
		box->addChild(namelbl);

		std::string countstr = StringUtils::format("%d", count);
		Label *countlbl = Label::createWithTTF(countstr, FONT_NAME, 23);
		countlbl->setAnchorPoint(Vec2(1, 0));
		countlbl->setColor(Color3B::WHITE);
		countlbl->setPosition(Vec2(box->getContentSize().width - 10, 10));
		box->addChild(countlbl);
	}

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

void GiftContentLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		ShopLayer::beginPay(m_tag);
		AnimationEffect::closeAniEffect((Layer*)this);
	}
}