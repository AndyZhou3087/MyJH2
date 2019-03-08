#include "BuySelectLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "MyRes.h"
#include "DataSave.h"
#include "Building.h"
#include "MovingLabel.h"
#include "BuyCoinLayer.h"

USING_NS_CC;

BuySelectLayer::BuySelectLayer()
{

}

BuySelectLayer::~BuySelectLayer()
{

}


BuySelectLayer* BuySelectLayer::create(std::vector<MSGAWDSDATA> vec_rewards, int putwhere)
{
	BuySelectLayer *pRet = new(std::nothrow)BuySelectLayer();
	if (pRet && pRet->init(vec_rewards, putwhere))
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
bool BuySelectLayer::init(std::vector<MSGAWDSDATA> vec_res, int putwhere)
{
	if (!Layer::init())
	{
		return false;
	}
	m_putwhere = putwhere;
	m_vecres = vec_res;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 128));
	this->addChild(color,0,"colorLayer");

	csbnode = CSLoader::createNode(ResourcePath::makePath("buySelectLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	cocos2d::ui::Text* title = (cocos2d::ui::Text*)csbnode->getChildByName("titlename");
	title->setString(ResourceLang::map_lang["buytext"]);

	//按钮1
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(BuySelectLayer::onBtnClick, this));
	actionbtn->setTag(1000);
	//按钮1文字
	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("mapeventtext_6_1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	actionbtntxt->ignoreContentAdaptWithSize(true);

	//按钮2
	cocos2d::ui::Widget* cancelbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("cancelbtn");
	cancelbtn->addTouchEventListener(CC_CALLBACK_2(BuySelectLayer::onBtnClick, this));
	cancelbtn->setTag(1001);
	//按钮2文字
	cocos2d::ui::ImageView* cancelbtntxt = (cocos2d::ui::ImageView*)cancelbtn->getChildByName("text");
	cancelbtntxt->loadTexture(ResourcePath::makeTextImgPath("cancelbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	int startx[] = { 360, 270 ,210 };
	int offsetx[] = { 0, 180, 150 };
	int rewardsize = vec_res.size();

	for (int i = 0; i < 3; i++)
	{
		std::string str = StringUtils::format("resbox%d", i);
		cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);

		cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)resbox->getChildByName("res");

		cocos2d::ui::Text* countlbl = (cocos2d::ui::Text*)resbox->getChildByName("countlbl");

		cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)resbox->getChildByName("name");

		if (i < rewardsize)
		{
			std::string boxstr = "ui/resbox.png";
			int t = 0;
			int qu = vec_res[i].qu;
			std::string resid = vec_res[i].rid;
			int count = vec_res[i].count;
			for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
			{
				if (resid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
					break;
			}
			if (t >= T_ARMOR && t <= T_FASHION)
			{
				boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
			}
			else if (t >= T_WG && t <= T_NG)
			{
				qu = GlobalInstance::map_GF[resid].qu;
				boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
			}
			else if (t >= T_RENS && t <= T_BOX)
			{
				qu = atoi(resid.substr(1).c_str()) - 1;
				boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
			}

			CommonFuncs::playResBoxEffect(resbox, qu);

			resbox->setPositionX(startx[rewardsize - 1] + offsetx[rewardsize - 1] * i);
			resbox->loadTexture(boxstr, cocos2d::ui::Widget::TextureResType::PLIST);
			resbox->setTag(i);
			resbox->addTouchEventListener(CC_CALLBACK_2(BuySelectLayer::onBtnClick, this));

			std::string resstr = StringUtils::format("ui/%s.png", resid.c_str());
			res->loadTexture(resstr, cocos2d::ui::Widget::TextureResType::PLIST);

			namelbl->setString(GlobalInstance::map_AllResources[resid].name);
			std::string countstr = StringUtils::format("%d", count);
			countlbl->setString(countstr);
		}
		else
		{
			resbox->setVisible(false);
		}
	}

	select = csbnode->getChildByName("select");

	updateSelect(0);

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

void BuySelectLayer::updateSelect(int index)
{
	selctindex = index;
	std::string resboxstr = StringUtils::format("resbox%d", index);
	Node* resbox = csbnode->getChildByName(resboxstr);
	select->setPosition(resbox->getPosition());

	std::string resid = m_vecres[index].rid;
	int count = m_vecres[index].count;

	needcoincount.setValue(count * GlobalInstance::map_AllResources[resid].silverval / 10);

	cocos2d::ui::Text* countlbl = (cocos2d::ui::Text*)csbnode->getChildByName("countlbl");
	std::string str = StringUtils::format("%d", needcoincount.getValue());
	countlbl->setString(str);
}

void BuySelectLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();

		if (tag == 1000)
		{
			if (GlobalInstance::getInstance()->getMyCoinCount().getValue() < needcoincount.getValue())
			{
				MovingLabel::show(ResourceLang::map_lang["nomorecoin"]);

				Layer* layer = BuyCoinLayer::create(needcoincount.getValue() - GlobalInstance::getInstance()->getMyCoinCount().getValue());
				Director::getInstance()->getRunningScene()->addChild(layer, 10000, "buycoinlayer");
				return;
			}

			int qu = m_vecres[selctindex].qu;
			int st = GlobalInstance::getInstance()->generateStoneCount(qu);
			MyRes::Add(m_vecres[selctindex].rid, m_vecres[selctindex].count, m_putwhere, qu, st);

			GlobalInstance::getInstance()->costMyCoinCount(needcoincount);

			AnimationEffect::closeAniEffect(this);
		}
		else if (tag == 1001)
		{
			AnimationEffect::closeAniEffect(this);
		}
		else
		{
			updateSelect(tag);
		}
	}
}