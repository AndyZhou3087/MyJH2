#include "TimeGiftLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MovingLabel.h"
#include "AnimationEffect.h"
#include "MyRes.h"
#include "ShopLayer.h"

USING_NS_CC;

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

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("timeGiftLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	//°´Å¥
	cocos2d::ui::Button* buybtn = (cocos2d::ui::Button*)csbnode->getChildByName("buybtn");
	buybtn->addTouchEventListener(CC_CALLBACK_2(TimeGiftLayer::onBtnClick, this));
	buybtn->setTag(0);

	cocos2d::ui::ImageView* buybtntext = (cocos2d::ui::ImageView*)buybtn->getChildByName("text");
	buybtntext->loadTexture(ResourcePath::makeTextImgPath("mapeventtext_6_1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	buybtntext->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath("mapeventtext_6_1", langtype))->getContentSize());

	cocos2d::ui::ImageView* title = (cocos2d::ui::ImageView*)csbnode->getChildByName("title");
	title->loadTexture(ResourcePath::makeTextImgPath("timegifttitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	lefttime = (cocos2d::ui::Text*)csbnode->getChildByName("lefttime");

	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desc->setString(data->desc);

	cocos2d::ui::Text* price_0 = (cocos2d::ui::Text*)csbnode->getChildByName("price_0");
	std::string str = StringUtils::format(ResourceLang::map_lang["timegiftprice_0"].c_str(), data->price);
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

	int starty[] = { 710, 810 };
	int offsety[] = { 0, 180 };
	int ressize = data->res.size();
	for (int i = 0; i < ressize; i++)
	{
		std::string str = "ui/resbox.png";

		std::vector<std::string> vec_res = data->res[i];
		std::string resid = vec_res[0];
		int count = atoi(vec_res[1].c_str());
		int qu = 0;

		int t = 0;
		for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
		{
			if (resid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
				break;
		}
		if (t == T_RENS || t == T_DAN || t == T_MIJI || t == T_BOX)
		{
			qu = atoi(resid.substr(1).c_str()) - 1;
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

		Sprite* box = Sprite::createWithSpriteFrameName(str);
		this->addChild(box);

		box->setPosition(Vec2(startx[(ressize - 1)%3] + offsetx[(ressize - 1) % 3] * (i%3), starty[(ressize-1)/3] + offsety[(ressize - 1) / 3] * (i/3)));

		CommonFuncs::playResBoxEffect(box, qu);

		if (vec_res.size() > 2)
		{
			qu = atoi(vec_res[2].c_str());
		}
		str = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);
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
	}
}