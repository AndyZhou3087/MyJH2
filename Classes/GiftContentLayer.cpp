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
#include "MyRes.h"
#include "WaitingProgress.h"
#include "SoundManager.h"
#include "SimpleResPopLayer.h"
#include "EquipDescLayer.h"

USING_NS_CC;
GiftContentLayer::GiftContentLayer()
{
	isgetingvip = false;
}

GiftContentLayer::~GiftContentLayer()
{
}


GiftContentLayer* GiftContentLayer::create(ShopData* data, int tag, int type)
{
	GiftContentLayer *pRet = new(std::nothrow)GiftContentLayer();
	if (pRet && pRet->init(data, tag, type))
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
bool GiftContentLayer::init(ShopData* data, int tag, int type)
{
	if (!Layer::init())
	{
		return false;
	}

	m_type = type;
	m_data = data;
	m_tag = tag;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color, 0, "colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("giftDescLayer.csb"));
	this->addChild(csbnode);
	langtype = GlobalInstance::getInstance()->getLang();

	//°´Å¥
	buybtn = (cocos2d::ui::Button*)csbnode->getChildByName("buybtn");
	buybtn->addTouchEventListener(CC_CALLBACK_2(GiftContentLayer::onBtnClick, this));
	buybtn->setTag(0);
	buybtntext = (cocos2d::ui::ImageView*)buybtn->getChildByName("text");
	buybtntext->loadTexture(ResourcePath::makeTextImgPath("mapeventtext_6_1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	buybtntext->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath("mapeventtext_6_1", langtype))->getContentSize());

	cocos2d::ui::Button* closebtn = (cocos2d::ui::Button*)csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(GiftContentLayer::onBtnClick, this));
	closebtn->setTag(1001);
	closebtn->setVisible(false);

	cocos2d::ui::ImageView* title = (cocos2d::ui::ImageView*)csbnode->getChildByName("title");
	title->ignoreContentAdaptWithSize(true);
	std::string str = StringUtils::format("text_%s", data->icon.c_str());

	if (data->type == COIN)
		str = "text_gd";

	title->loadTexture(ResourcePath::makeTextImgPath(str, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	lefttime = (cocos2d::ui::Text*)csbnode->getChildByName("lefttime");
	lefttime->setAnchorPoint(Vec2(0, 0.5));
	lefttime->setPosition(Vec2(443, 436));

	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	price = (cocos2d::ui::Text*)csbnode->getChildByName("price");
	str = StringUtils::format(ResourceLang::map_lang["shoppricetext"].c_str(), data->price);
	price->setString(str);

	if (m_type == 1)
	{
		buybtn->setVisible(false);
		price->setVisible(false);
	}
	else if (m_type == 2)
	{
		buybtn->setVisible(false);
		price->setVisible(false);
	}
	else
	{
		closebtn->setVisible(true);
	}

	if (data->type == GIFT || data->type == COIN)
	{
		str = StringUtils::format("shoptext_%d", data->type);
		desc->setString(ResourceLang::map_lang[str]);
	}
	else if (data->type == VIP)
	{
		str = StringUtils::format("shoptext_%d", m_data->type);
		str = StringUtils::format(ResourceLang::map_lang[str].c_str(), VIPDAYS[m_tag], m_data->name.c_str());
		desc->setString(str);

		HttpDataSwap::init(this)->vipIsOn();

	}

	int ressize = data->res.size();
	int startx[] = { 360, 270 ,210 };
	int offsetx[] = { 0, 180, 150 };

	int starty[] = { 660, 730 };
	int offsety[] = { 0, 160 };
	int row = (ressize - 1) / 3;
	for (int i = 0; i < ressize; i++)
	{
		std::string str = "ui/resbox.png";

		std::vector<std::string> vec_res = data->res[i];
		std::string resid = vec_res[0];
		int count = atoi(vec_res[1].c_str());
		int qu = 0;

		if (vec_res.size() > 2)
			qu = atoi(vec_res[2].c_str());

		int t = 0;
		for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
		{
			if (resid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
				break;
		}
		if (t >= T_WG && t <= T_NG)
		{
			qu = GlobalInstance::map_GF[resid].qu;
			str = StringUtils::format("ui/resbox_qu%d.png", qu);
		}		
		else if (t >= T_RENS && t <= T_BOX)
		{
			qu = atoi(resid.substr(1).c_str()) - 1;
			str = StringUtils::format("ui/resbox_qu%d.png", qu);
		}
		else if (t == T_FRAGMENT)
		{
			//qu = 0;
			//str = StringUtils::format("ui/resbox_qu%d.png", qu);
		}
		if(data->icon.compare(0, 8, "timegift") == 0 && t == T_EPIECE)
		{
			int r = GlobalInstance::getInstance()->createRandomNum(sizeof(qu4epiece)/sizeof(qu4epiece[0]));
			resid = qu4epiece[r];
			std::vector<std::string> vec_; 
			vec_.push_back(resid);
			vec_.push_back(vec_res[1]);
			data->res[i] = vec_;
		}

		cocos2d::ui::ImageView* box = cocos2d::ui::ImageView::create(str, cocos2d::ui::Widget::TextureResType::PLIST);
		box->addTouchEventListener(CC_CALLBACK_2(GiftContentLayer::onResclick, this));
		box->setName(resid);
		box->setTag(qu*1000 + t);
		box->setTouchEnabled(true);

		CommonFuncs::playResBoxEffect(box, t, qu, 0);

		this->addChild(box);

		int index = 0;
		if (i < row * 3)
			index = (3 - 1) % 3;
		else
			index = (ressize - 1) % 3;

		int x = startx[index] + offsetx[index] * (i % 3);
		int y = starty[(ressize - 1) / 3] - offsety[(ressize - 1) / 3] * (i / 3);

		box->setPosition(Vec2(x, y));

		if (vec_res.size() > 2)
		{
			qu = atoi(vec_res[2].c_str());
		}
		str = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);
		Sprite* res = Sprite::createWithSpriteFrameName(str);
		box->addChild(res);
		res->setPosition(Vec2(box->getContentSize().width / 2, box->getContentSize().height / 2));

		if (t == T_EPIECE)
		{
			Sprite* pieceicon = Sprite::createWithSpriteFrameName("ui/pieceicon.png");
			pieceicon->setAnchorPoint(Vec2(0, 1));
			pieceicon->setPosition(10, box->getContentSize().height - 10);
			box->addChild(pieceicon);
		}

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
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void GiftContentLayer::onResclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		cocos2d::ui::ImageView* clickres = (cocos2d::ui::ImageView*)pSender;
		std::string resid = clickres->getName();

		int t = clickres->getTag()%1000;
		
		if (t >= T_ARMOR && t <= T_NG)
		{
			Layer* layer = EquipDescLayer::create(resid, clickres->getTag()/1000, 1);
			this->addChild(layer);
			AnimationEffect::openAniEffect(layer);
		}
		else
		{
			SimpleResPopLayer* layer = SimpleResPopLayer::create(resid, 3);
			this->addChild(layer);
			AnimationEffect::openAniEffect(layer);
		}
	}
}

void GiftContentLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		cocos2d::ui::Widget* node = (cocos2d::ui::Widget*)pSender;
		int tag = node->getTag();
		if (tag == 1001)
		{
			AnimationEffect::closeAniEffect(this);
		}
		else if (tag == 0)
		{
			ShopLayer::beginPay(m_tag);
		}
		else
		{
			node->setEnabled(false);
			isgetingvip = true;
			WaitingProgress* wp = WaitingProgress::create(ResourceLang::map_lang["getmonthlywaiting"]);
			Director::getInstance()->getRunningScene()->addChild(wp, 0, "waitingprogress");

			int id = atoi(m_data->icon.substr(3, 1).c_str());
			std::string vipid = StringUtils::format("vip%d", id + 2);

			HttpDataSwap::init(this)->getMonthlyReward(vipid);

		}
	}
}


void GiftContentLayer::getVipReward()
{
	for (unsigned int i = 0; i < m_data->res.size(); i++)
	{
		std::vector<std::string> vec_res = m_data->res[i];
		std::string resid = vec_res[0];
		int count = atoi(vec_res[1].c_str());
		int qu = 0;
		if (vec_res.size() > 2)
		{
			qu = atoi(vec_res[2].c_str());
		}
		int stonescount = GlobalInstance::getInstance()->generateStoneCount(qu);

		if (resid.compare("r006") == 0)
		{
			DynamicValueInt dvint;
			dvint.setValue(count);
			GlobalInstance::getInstance()->addMySoliverCount(dvint);
		}
		else if (resid.compare("r012") == 0)
		{
			DynamicValueInt dvint;
			dvint.setValue(count);
			GlobalInstance::getInstance()->addMyCoinCount(dvint);
		}
		else
		{
			MyRes::Add(resid, count, MYSTORAGE, qu, stonescount);
		}
	}
}

void GiftContentLayer::onFinish(int code)
{
	if (!isgetingvip)
	{
		if (code == 0)
		{
			if (GlobalInstance::map_buyVipDays.size() > 0)
			{
				if (m_type != 0)
				{
					buybtn->setVisible(true);
					buybtn->setTag(1);
					buybtntext->loadTexture(ResourcePath::makeTextImgPath("msgallget_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
					buybtntext->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath("msgallget_text", langtype))->getContentSize());
					buybtntext->setScale(0.7f);
				}
				//price->setVisible(false);            
				std::map<std::string, int>::iterator it;
				for (it = GlobalInstance::map_buyVipDays.begin(); it != GlobalInstance::map_buyVipDays.end(); ++it)
				{
					if (m_data->icon.compare(it->first) == 0)
					{
						lefttime->setVisible(true);
						std::string str = StringUtils::format(ResourceLang::map_lang["lefttimetext"].c_str(), it->second);
						lefttime->setString(str);
						break;
					}
				}
			}
		}
		else
		{
			MovingLabel::show(ResourceLang::map_lang["matchnetworkerr"]);
		}
	}
	else
	{
		isgetingvip = false;
		Director::getInstance()->getRunningScene()->removeChildByName("waitingprogress");

		if (code == 0)
		{
			getVipReward();
			AnimationEffect::closeAniEffect(this);
		}
		else
		{
			buybtn->setEnabled(true);
			MovingLabel::show(ResourceLang::map_lang["matchnetworkerr"]);
		}

	}
}
