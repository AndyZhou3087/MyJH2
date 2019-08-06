#include "TimeGiftLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MovingLabel.h"
#include "AnimationEffect.h"
#include "MyRes.h"
#include "ShopLayer.h"
#include "SoundManager.h"
#include "SimpleResPopLayer.h"
#include "EquipDescLayer.h"
#include "SelectEPieceLayer.h"

USING_NS_CC;

int p_price[] = {20, 60, 120};//原价
TimeGiftLayer::TimeGiftLayer()
{

}

TimeGiftLayer::~TimeGiftLayer()
{

}


TimeGiftLayer* TimeGiftLayer::create(ShopData* data)
{
	TimeGiftLayer *pRet = new(std::nothrow)TimeGiftLayer();
	if (pRet && pRet->init(data))
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
bool TimeGiftLayer::init(ShopData* data)
{
	if (!Layer::init())
	{
		return false;
	}

	m_data = data;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color, 0, "colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("timeGiftLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	//按钮
	cocos2d::ui::Button* buybtn = (cocos2d::ui::Button*)csbnode->getChildByName("buybtn");
	buybtn->addTouchEventListener(CC_CALLBACK_2(TimeGiftLayer::onBtnClick, this));
	buybtn->setTag(0);

	cocos2d::ui::Button* closebtn = (cocos2d::ui::Button*)csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(TimeGiftLayer::onBtnClick, this));
	closebtn->setTag(1000);

	cocos2d::ui::ImageView* buybtntext = (cocos2d::ui::ImageView*)buybtn->getChildByName("text");
	buybtntext->loadTexture(ResourcePath::makeTextImgPath("mapeventtext_6_1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	buybtntext->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath("mapeventtext_6_1", langtype))->getContentSize());

	cocos2d::ui::ImageView* title = (cocos2d::ui::ImageView*)csbnode->getChildByName("title");
	title->loadTexture(ResourcePath::makeTextImgPath("timegifttitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	lefttime = (cocos2d::ui::Text*)csbnode->getChildByName("lefttime");

	if (GlobalInstance::serverTimeGiftData.isopen)
	{
		updatetime(0);

		this->schedule(schedule_selector(TimeGiftLayer::updatetime), 1.0f);
	}
	else
	{
		lefttime->setVisible(false);
	}

	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desc->setString(data->desc);

	int startindex = 0;
	int cruindex = 0;

	for (unsigned int i = 0; i < GlobalInstance::vec_shopdata.size(); i++)
	{
		if (GlobalInstance::vec_shopdata[i].icon.compare("timegift0") == 0)
		{
			startindex = i;
		}
		if (GlobalInstance::vec_shopdata[i].icon.compare(data->icon) == 0)
		{
			cruindex = i;
			break;
		}
	}

	cocos2d::ui::Text* price_0 = (cocos2d::ui::Text*)csbnode->getChildByName("price_0");
	std::string str = StringUtils::format(ResourceLang::map_lang["timegiftprice_0"].c_str(), p_price[cruindex - startindex]);
	price_0->setString(str);

	DrawNode* lineNode = DrawNode::create();
	price_0->addChild(lineNode, 100000);
	lineNode->setLineWidth(2.0f);
	lineNode->drawLine(Vec2(-5, price_0->getContentSize().height/2), Vec2(price_0->getContentSize().width + 5, price_0->getContentSize().height / 2), Color4F(price_0->getTextColor()));

	cocos2d::ui::Text* price = (cocos2d::ui::Text*)csbnode->getChildByName("price");
	str = StringUtils::format(ResourceLang::map_lang["timegiftprice_1"].c_str(), data->price);
	price->setString(str);


	int startx[] = { 360, 270 ,210 };
	int offsetx[] = { 0, 180, 150 };

	int starty[] = { 730, 800 };
	int offsety[] = { 0, 170 };
	int ressize = data->res.size();

	int row = (ressize - 1) / 3;
	for (int i = 0; i < ressize; i++)
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
		else if (t == T_FRAGMENT)
		{
			//qu = 4;
			//str = StringUtils::format("ui/resbox_qu%d.png", qu);
		}
		else if (t == T_EPIECE)
		{
			//qu = 4;
			//str = StringUtils::format("ui/resbox_qu%d.png", qu);
			int r = GlobalInstance::getInstance()->createRandomNum(sizeof(qu4epiece) / sizeof(qu4epiece[0]));
			resid = qu4epiece[r];
			std::vector<std::string> vec_;
			vec_.push_back(resid);
			vec_.push_back(vec_res[1]);
			data->res[i] = vec_;
		}

		cocos2d::ui::ImageView* box = cocos2d::ui::ImageView::create(str, cocos2d::ui::Widget::TextureResType::PLIST);
		box->addTouchEventListener(CC_CALLBACK_2(TimeGiftLayer::onResclick, this));
		box->setName(resid);
		box->setTag(qu * 1000 + t);
		box->setTouchEnabled(true);
		this->addChild(box);

		int index = 0;
		if (i < row * 3)
			index = (3 - 1)%3;
		else
			index = (ressize - 1) % 3;

		int x = startx[index] + offsetx[index] * (i % 3);
		int y = starty[(ressize - 1) / 3] - offsety[(ressize - 1) / 3] * (i / 3);
		box->setPosition(Vec2(x, y));

		CommonFuncs::playResBoxEffect(box, t, qu, 0);

		str = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);
		//Sprite* res = Sprite::createWithSpriteFrameName(str);
		cocos2d::ui::ImageView* res = cocos2d::ui::ImageView::create(str, cocos2d::ui::Widget::TextureResType::PLIST);
		box->addChild(res, 0, "res");
		res->setPosition(Vec2(box->getContentSize().width / 2, box->getContentSize().height / 2));

		if (t == T_EPIECE)
		{
			box->setTouchEnabled(false);
			Sprite* pieceicon = Sprite::createWithSpriteFrameName("ui/pieceicon.png");
			pieceicon->setAnchorPoint(Vec2(0, 1));
			pieceicon->setPosition(10, box->getContentSize().height - 10);
			box->addChild(pieceicon);

			cocos2d::ui::ImageView* changeicon = cocos2d::ui::ImageView::create("ui/changepiece.png", cocos2d::ui::Widget::TextureResType::PLIST);
			//changeicon->addTouchEventListener(CC_CALLBACK_2(TimeGiftLayer::onResclick, this));
			changeicon->setPosition(Vec2(box->getContentSize().width - 15, box->getContentSize().height - 15));
			//changeicon->setTag(10000 + i);
			//changeicon->setTouchEnabled(true);
			box->addChild(changeicon);

			cocos2d::ui::ImageView* changeclick = cocos2d::ui::ImageView::create("ui/blank.png", cocos2d::ui::Widget::TextureResType::PLIST);
			changeclick->addTouchEventListener(CC_CALLBACK_2(TimeGiftLayer::onResclick, this));
			changeclick->setContentSize(Size(box->getContentSize()));
			changeclick->setPosition(Vec2(box->getContentSize().width/2, box->getContentSize().height/2));
			changeclick->setTag(10000 + i);
			changeclick->setTouchEnabled(true);
			box->addChild(changeclick);
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

void TimeGiftLayer::updatetime(float dt)
{
	std::string timestr = StringUtils::format(ResourceLang::map_lang["timegiftlefttime"].c_str(), GlobalInstance::serverTimeGiftData.lefttime / 3600, GlobalInstance::serverTimeGiftData.lefttime % 3600 / 60, GlobalInstance::serverTimeGiftData.lefttime % 3600 % 60);
	lefttime->setString(timestr);
}

void TimeGiftLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
		if (tag == 0)
		{
			ShopLayer::beginPay(this->getTag());
		}
		else if (tag == 1000)
		{
			AnimationEffect::closeAniEffect(this);
		}
	}
}

void TimeGiftLayer::onResclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		cocos2d::ui::ImageView* clickres = (cocos2d::ui::ImageView*)pSender;

		int tag = clickres->getTag();
		if (tag < 10000)
		{
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
		else
		{
			int w = tag % 10000;
			int count = atoi(m_data->res[w][1].c_str());
			SelectEPieceLayer* layer = SelectEPieceLayer::create(clickres->getParent()->getChildByName("res"), count, m_data);
			this->addChild(layer, 0, w);
			AnimationEffect::openAniEffect(layer);
		}
	}
}