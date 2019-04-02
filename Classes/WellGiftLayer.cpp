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
#include "EquipDescLayer.h"

USING_NS_CC;

std::string vipname[] = {"vip0", "vip1", "vip2"};

WellGiftLayer::WellGiftLayer()
{
	selectvip = 3;
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
		vipclick[i-1] = (cocos2d::ui::Widget*)csbnode->getChildByName(name);
		vipclick[i-1]->addTouchEventListener(CC_CALLBACK_2(WellGiftLayer::onBtnClick, this));
		vipclick[i-1]->setTag(2000 + i);
		vipclick[i-1]->setUserData((void*)shopdata);
		vipclickoripos[i - 1] = vipclick[i - 1]->getPosition();

		cocos2d::ui::Text* price = (cocos2d::ui::Text*)vipclick[i-1]->getChildByName("price");
		std::string str = StringUtils::format(ResourceLang::map_lang["shoppricetext"].c_str(), shopdata->price);
		Label* lbl = (Label*)price->getVirtualRenderer();
		lbl->enableBold();
		price->setString(str);

		if (i == 3)
			selectVip(shopdata);

		name = StringUtils::format("vip%dleftday", i);
		leftdayslbl[i-1] = (cocos2d::ui::Text*)csbnode->getChildByName(name);
		leftdayslbl[i - 1]->setVisible(false);
		leftdayslbl[i - 1]->setString("");
		leftdayslbl[i - 1]->setUserData((void*)shopdata);
	}

	WaitingProgress* wp = WaitingProgress::create(ResourceLang::map_lang["datawaitingtext"]);
	this->addChild(wp, 0, "waitingprogress");
	HttpDataSwap::init(this)->vipIsOn();

	this->schedule(schedule_selector(WellGiftLayer::updateUI), 1);
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
				qu = atoi(resid.substr(1).c_str()) + 2;
				str = StringUtils::format("ui/resbox_qu%d.png", qu);
			}
			box[i]->setVisible(true);
			box[i]->setName(resid);
			box[i]->setTag(qu * 1000 + t);
			box[i]->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
			int index = 0;
			if (i < row * 3)
				index = (3 - 1) % 3;
			else
				index = (ressize - 1) % 3;

			int x = startx[index] + offsetx[index] * (i % 3);
			int y = starty[(ressize - 1) / 3] - offsety[(ressize - 1) / 3] * (i / 3);
			box[i]->setPosition(Vec2(x, y));

			box[i]->removeChildByName("resboxeffect");

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
	str = StringUtils::format(ResourceLang::map_lang[str].c_str(), VIPDAYS[selectvip-1], data->name.c_str());
	desc->setString(str);

	for (int i = 0; i < 3; i++)
	{
		if (i + 1 != selectvip)
		{
			vipclick[i]->stopAllActions();
			vipclick[i]->setPosition(vipclickoripos[i]);
		}
		else
			vipclick[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.3f, Vec2(0, 5)), MoveBy::create(0.6f, Vec2(0, -5)), MoveBy::create(0.5f, Vec2(0, 0)), NULL)));
	}
}

void WellGiftLayer::onResclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		cocos2d::ui::ImageView* clickres = (cocos2d::ui::ImageView*)pSender;
		std::string resid = clickres->getName();
		int t = clickres->getTag() % 1000;

		if (t >= T_ARMOR && t <= T_NG)
		{
			Layer* layer = EquipDescLayer::create(resid, clickres->getTag() / 1000, 1);
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
			for (unsigned int i = 0; i < GlobalInstance::vec_shopdata.size(); i++)
			{
				if (GlobalInstance::vec_shopdata[i].icon.compare(vipname[tag%1000-1]) == 0)
				{
					ShopLayer::beginPay(i);
					break;
				}
			}
		}
		else if (tag > 2000)
		{
			int stag = tag % 2000;
			if (selectvip == stag)
				return;
			selectvip = stag;
			selectVip((ShopData*)node->getUserData());
		}
	}
}

void WellGiftLayer::onFinish(int code)
{
	this->removeChildByName("waitingprogress");
	if (code == 0)
	{
		updateUI(0);
	}
	else
	{
		MovingLabel::show(ResourceLang::map_lang["matchnetworkerr"]);
	}
}

void WellGiftLayer::updateUI(float dt)
{
	if (GlobalInstance::map_buyVipDays.size() > 0)
	{
		std::map<std::string, int>::iterator it;
		for (it = GlobalInstance::map_buyVipDays.begin(); it != GlobalInstance::map_buyVipDays.end(); ++it)
		{
			for (int i = 0; i < 3; i++)
			{
				ShopData* mdata = (ShopData*)leftdayslbl[i]->getUserData();
				if (mdata->icon.compare(it->first) == 0)
				{
					leftdayslbl[i]->setVisible(true);
					std::string str = StringUtils::format(ResourceLang::map_lang["lefttimetext"].c_str(), it->second);
					leftdayslbl[i]->setString(str);
					break;
				}
			}
		}
	}
}