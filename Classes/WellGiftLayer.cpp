#include "WellGiftLayer.h"
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

USING_NS_CC;

std::string vipname[] = {"vip0", "vip1", "vip2"};
WellGiftLayer::WellGiftLayer()
{
	isgetingvip = false;
}

WellGiftLayer::~WellGiftLayer()
{

}


WellGiftLayer* WellGiftLayer::create()
{
	WellGiftLayer *pRet = new(std::nothrow)WellGiftLayer();
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
bool WellGiftLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color, 0, "colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("wellGiftLayer.csb"));
	this->addChild(csbnode);
	langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::ImageView* title = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	title->loadTexture(ResourcePath::makeTextImgPath("welltitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(WellGiftLayer::onBtnClick, this));
	closebtn->setTag(0);

	desc = (cocos2d::ui::Text*)csbnode->getChildByName("desc");

	for (int i = 0; i < 6; i++)
	{
		std::string str = "ui/resbox.png";

		box[i] = cocos2d::ui::ImageView::create(str, cocos2d::ui::Widget::TextureResType::PLIST);
		box[i]->addTouchEventListener(CC_CALLBACK_2(WellGiftLayer::onResclick, this));
		box[i]->setTouchEnabled(true);
		this->addChild(box[i]);
		box[i]->setVisible(false);
		//box->setPosition(Vec2(x, y));

		str = GlobalInstance::getInstance()->getResUIFrameName("r001", 0);
		cocos2d::ui::ImageView* res = cocos2d::ui::ImageView::create(str, cocos2d::ui::Widget::TextureResType::PLIST);
		box[i]->addChild(res, 0, "res");
		res->setPosition(Vec2(box[i]->getContentSize().width / 2, box[i]->getContentSize().height / 2));

		Label *namelbl = Label::createWithTTF("", FONT_NAME, 23);
		namelbl->setPosition(Vec2(box[i]->getContentSize().width / 2, -15));
		namelbl->setColor(Color3B(255, 241, 205));
		namelbl->enableOutline(Color4B(94, 76, 61, 255), 2);
		box[i]->addChild(namelbl, 0, "namelbl");

		Label *countlbl = Label::createWithTTF("", FONT_NAME, 23);
		countlbl->setAnchorPoint(Vec2(1, 0));
		countlbl->setColor(Color3B::WHITE);
		countlbl->setPosition(Vec2(box[i]->getContentSize().width - 10, 10));
		box[i]->addChild(countlbl, 0, "countlbl");
	}

	for (int i = 1; i <= 3; i++)
	{
		ShopData* shopdata = NULL;

		for (unsigned int m = 0; m < GlobalInstance::vec_shopdata.size(); m++)
		{
			if (GlobalInstance::vec_shopdata[m].icon.compare(vipname[i - 1]) == 0)
			{
				shopdata = &GlobalInstance::vec_shopdata[m];
				break;
			}

		}
		std::string name = StringUtils::format("buybtn%d", i);
		cocos2d::ui::Widget* buybtn =  (cocos2d::ui::Widget*)csbnode->getChildByName(name);
		buybtn->addTouchEventListener(CC_CALLBACK_2(WellGiftLayer::onBtnClick, this));
		buybtn->setTag(1000+i);
		buybtn->setUserData((void*)shopdata);

		cocos2d::ui::ImageView* buybtntext = (cocos2d::ui::ImageView*)buybtn->getChildByName("text");
		buybtntext->loadTexture(ResourcePath::makeTextImgPath("mapeventtext_6_1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		buybtntext->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath("mapeventtext_6_1", langtype))->getContentSize());

		name = StringUtils::format("vip%dclick", i);
		cocos2d::ui::Widget* vipclick = (cocos2d::ui::Widget*)csbnode->getChildByName(name);
		vipclick->addTouchEventListener(CC_CALLBACK_2(WellGiftLayer::onBtnClick, this));
		vipclick->setTag(2000 + i);
		vipclick->setUserData((void*)shopdata);

		cocos2d::ui::Text* price = (cocos2d::ui::Text*)vipclick->getChildByName("price");
		std::string str = StringUtils::format(ResourceLang::map_lang["shoppricetext"].c_str(), shopdata->price);
		Label* lbl = (Label*)price->getVirtualRenderer();
		lbl->enableBold();
		price->setString(str);

		if (i == 3)
			selectVip(shopdata);
	}

/*	if (m_type == 1)
	{
		int id = atoi(data->icon.substr(3, 1).c_str());
		std::string vipid = StringUtils::format("vip%d", id + 2);
		HttpDataSwap::init(NULL)->paySuccNotice(vipid, data->price);
		buybtn->setVisible(false);
		price->setVisible(false);
	}
	else if (m_type == 2)
	{
		buybtn->setVisible(false);
		price->setVisible(false);
	}

	if (data->type == GIFT)
	{
		str = StringUtils::format("shoptext_%d", data->type);
		desc->setString(ResourceLang::map_lang[str]);
	}
	else if (data->type == VIP)
	{
		str = StringUtils::format("shoptext_%d", m_data->type);
		str = StringUtils::format(ResourceLang::map_lang[str].c_str(), m_data->name.c_str());
		desc->setString(str);

		HttpDataSwap::init(this)->vipIsOn();

	}*/

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

void WellGiftLayer::selectVip(ShopData* data)
{
	int startx[] = { 360, 270 ,210 };
	int offsetx[] = { 0, 180, 150 };

	int starty[] = { 760, 810 };
	int offsety[] = { 0, 170 };
	int ressize = data->res.size();

	int row = (ressize - 1) / 3;

	for (int i = 0; i < 6; i++)
	{
		if (i < ressize)
		{
			std::string str = "ui/resbox.png";

			std::vector<std::string> vec_res = data->res[i];
			std::string resid = vec_res[0];
			int count = atoi(vec_res[1].c_str());
			int qu = 0;

			if (vec_res.size() > 2)
			{
				qu = atoi(vec_res[2].c_str());
			}

			int t = 0;
			for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
			{
				if (resid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
					break;
			}
			if (t >= T_RENS && t <= T_BOX)
			{
				qu = atoi(resid.substr(1).c_str()) - 1;
				str = StringUtils::format("ui/resbox_qu%d.png", qu);
			}
			else if (t >= T_ARMOR && t <= T_FASHION)
			{
				str = StringUtils::format("ui/resbox_qu%d.png", qu);
			}
			else if (t >= T_WG && t <= T_NG)
			{
				qu = GlobalInstance::map_GF[resid].qu;
				str = StringUtils::format("ui/resbox_qu%d.png", qu);
			}
			else if (t >= T_HEROCARD && t <= T_ARMCARD)
			{
				qu = 3;
				str = StringUtils::format("ui/resbox_qu%d.png", qu);
			}
			box[i]->setVisible(true);
			box[i]->setName(resid);
			box[i]->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
			int index = 0;
			if (i < row * 3)
				index = (3 - 1) % 3;
			else
				index = (ressize - 1) % 3;

			int x = startx[index] + offsetx[index] * (i % 3);
			int y = starty[(ressize - 1) / 3] - offsety[(ressize - 1) / 3] * (i / 3);
			box[i]->setPosition(Vec2(x, y));

			CommonFuncs::playResBoxEffect(box[i], t, qu, 0);

			str = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);
			cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)box[i]->getChildByName("res");
			res->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

			Label *namelbl = (Label*)box[i]->getChildByName("namelbl");
			namelbl->setString(GlobalInstance::map_AllResources[resid].name);

			std::string countstr = StringUtils::format("%d", count);
			Label *countlbl = (Label*)box[i]->getChildByName("countlbl");
			countlbl->setString(countstr);
		}
		else
		{
			box[i]->setVisible(false);
		}
	}
	std::string str = StringUtils::format("shoptext_%d", data->type);
	str = StringUtils::format(ResourceLang::map_lang[str].c_str(), data->name.c_str());
	desc->setString(str);
}

void WellGiftLayer::onResclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		cocos2d::ui::ImageView* clickres = (cocos2d::ui::ImageView*)pSender;
		std::string resid = clickres->getName();
		SimpleResPopLayer* layer = SimpleResPopLayer::create(resid, 3);
		this->addChild(layer);
		AnimationEffect::openAniEffect(layer);
	}
}

void WellGiftLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
		if (tag == 0)
		{
			AnimationEffect::closeAniEffect((Layer*)this);
		}
		else if (tag > 1000 && tag < 2000)
		{
			//isgetingvip = true;
			//WaitingProgress* wp = WaitingProgress::create(ResourceLang::map_lang["getmonthlywaiting"]);
			//this->addChild(wp, 0, "waitingprogress");

			//int id = atoi(m_data->icon.substr(3, 1).c_str());
			//std::string vipid = StringUtils::format("vip%d", id + 2);

			//HttpDataSwap::init(this)->getMonthlyReward(vipid);
		}
		else if (tag > 2000)
		{
			selectVip((ShopData*)node->getUserData());
		}
	}
}


void WellGiftLayer::getVipReward()
{
	//for (unsigned int i = 0; i < m_data->res.size(); i++)
	//{
	//	std::vector<std::string> vec_res = m_data->res[i];
	//	std::string resid = vec_res[0];
	//	int count = atoi(vec_res[1].c_str());
	//	int qu = 0;
	//	if (vec_res.size() > 2)
	//	{
	//		qu = atoi(vec_res[2].c_str());
	//	}
	//	int stonescount = GlobalInstance::getInstance()->generateStoneCount(qu);

	//	if (resid.compare("r006") == 0)
	//	{
	//		DynamicValueInt dvint;
	//		dvint.setValue(count);
	//		GlobalInstance::getInstance()->addMySoliverCount(dvint);
	//	}
	//	else if (resid.compare("r012") == 0)
	//	{
	//		DynamicValueInt dvint;
	//		dvint.setValue(count);
	//		GlobalInstance::getInstance()->addMyCoinCount(dvint);
	//	}
	//	else
	//	{
	//		MyRes::Add(resid, count, MYSTORAGE, qu, stonescount);
	//	}
	//}
}

void WellGiftLayer::onFinish(int code)
{
	//if (!isgetingvip)
	//{
	//	if (code == 0)
	//	{
	//		if (GlobalInstance::map_buyVipDays.size() > 0)
	//		{
	//			if (m_type != 0)
	//			{
	//				buybtn->setVisible(true);
	//				buybtn->setTag(1);
	//				buybtntext->loadTexture(ResourcePath::makeTextImgPath("msgallget_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	//				buybtntext->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath("msgallget_text", langtype))->getContentSize());
	//				buybtntext->setScale(0.7f);
	//			}
	//			//price->setVisible(false);
	//			std::map<std::string, int>::iterator it;
	//			for (it = GlobalInstance::map_buyVipDays.begin(); it != GlobalInstance::map_buyVipDays.end(); ++it)
	//			{
	//				if (m_data->icon.compare(it->first) == 0)
	//				{
	//					lefttime->setVisible(true);
	//					std::string str = StringUtils::format(ResourceLang::map_lang["lefttimetext"].c_str(), it->second);
	//					lefttime->setString(str);
	//					break;
	//				}
	//			}
	//		}
	//	}
	//	else
	//	{
	//		MovingLabel::show(ResourceLang::map_lang["matchnetworkerr"]);
	//	}
	//}
	//else
	//{
	//	isgetingvip = false;
	//	this->removeChildByName("waitingprogress");

	//	if (code == 0)
	//	{
	//		getVipReward();
	//		AnimationEffect::closeAniEffect(this);
	//	}
	//	else
	//	{
	//		MovingLabel::show(ResourceLang::map_lang["matchnetworkerr"]);
	//	}

	//}
}