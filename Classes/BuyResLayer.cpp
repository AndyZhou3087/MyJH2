#include "BuyResLayer.h"
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

BuyResLayer::BuyResLayer()
{

}

BuyResLayer::~BuyResLayer()
{

}


BuyResLayer* BuyResLayer::create(std::vector<MSGAWDSDATA> vec_rewards, int putwhere, bool inmarket)
{
	BuyResLayer *pRet = new(std::nothrow)BuyResLayer();
	if (pRet && pRet->init(vec_rewards, putwhere, inmarket))
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
bool BuyResLayer::init(std::vector<MSGAWDSDATA> vec_res, int putwhere, bool inmarket)
{
	if (!Layer::init())
	{
		return false;
	}
	m_putwhere = putwhere;
	m_vecres = vec_res;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 128));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("buyResLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	cocos2d::ui::Text* title = (cocos2d::ui::Text*)csbnode->getChildByName("title");
	title->setString(ResourceLang::map_lang["buytext"]);

	//按钮1
	cocos2d::ui::Widget* okbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("okbtn");
	okbtn->addTouchEventListener(CC_CALLBACK_2(BuyResLayer::onBtnClick, this));
	okbtn->setTag(1);
	//按钮1文字
	cocos2d::ui::ImageView* okbtntxt = (cocos2d::ui::ImageView*)okbtn->getChildByName("text");
	okbtntxt->loadTexture(ResourcePath::makeTextImgPath("okbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//按钮2
	cocos2d::ui::Widget* cancelbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("cancelbtn");
	cancelbtn->addTouchEventListener(CC_CALLBACK_2(BuyResLayer::onBtnClick, this));
	cancelbtn->setTag(0);
	//按钮2文字
	cocos2d::ui::ImageView* cancelbtntxt = (cocos2d::ui::ImageView*)cancelbtn->getChildByName("text");
	cancelbtntxt->loadTexture(ResourcePath::makeTextImgPath("cancelbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	int startx[] = { 360, 270 ,210 };
	int offsetx[] = { 0, 180, 150 };
	int rewardsize = vec_res.size();

	for (int i = 0; i < 3; i++)
	{
		std::string str = StringUtils::format("resbox_%d", i);
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
			else if (t == T_EPIECE)
			{
				Sprite* pieceicon = Sprite::createWithSpriteFrameName("ui/pieceicon.png");
				pieceicon->setAnchorPoint(Vec2(0, 1));
				pieceicon->setPosition(10, resbox->getContentSize().height - 10);
				resbox->addChild(pieceicon);
			}

			CommonFuncs::playResBoxEffect(resbox, t, qu, 0);

			resbox->setPositionX(startx[rewardsize - 1] + offsetx[rewardsize - 1] * i);
			resbox->loadTexture(boxstr, cocos2d::ui::Widget::TextureResType::PLIST);
			std::string resstr = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);//StringUtils::format("ui/%s.png", resid.c_str());
			res->loadTexture(resstr, cocos2d::ui::Widget::TextureResType::PLIST);

			namelbl->setString(GlobalInstance::map_AllResources[resid].name);
			std::string countstr = StringUtils::format("x%d", count);
			countlbl->setString(countstr);

			if (GlobalInstance::map_AllResources[resid].coinval > 0)
			{
				needcoincount.setValue(needcoincount.getValue() + count * GlobalInstance::map_AllResources[resid].coinval);
			}
			else
			{
				needcoincount.setValue(needcoincount.getValue() + count * GlobalInstance::map_AllResources[resid].silverval / 10);
			}
		}
		else
		{
			resbox->setVisible(false);
		}
	}

	std::string descstr = GlobalInstance::map_AllResources[vec_res[0].rid].desc;
	if (inmarket)
	{
		needcoincount.setValue(needcoincount.getValue() * 12 / 10);
		descstr.append(ResourceLang::map_lang["reswheregettext"]);
	}

	cocos2d::ui::Text* countlbl = (cocos2d::ui::Text*)csbnode->getChildByName("cnumbl");
	std::string str = StringUtils::format("%d", needcoincount.getValue());
	countlbl->setString(str);
	
	cocos2d::ui::Text* desclbl = (cocos2d::ui::Text*)csbnode->getChildByName("desc");

	Label* descRenderLable = (Label*)desclbl->getVirtualRenderer();
	descRenderLable->setLineSpacing(10);

	desclbl->setString(descstr);

	//屏蔽下层点击
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

void BuyResLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		switch (node->getTag())
		{
		case 0:
			AnimationEffect::closeAniEffect(this);
			break;
		case 1:
		{

			if (GlobalInstance::getInstance()->getMyCoinCount().getValue() < needcoincount.getValue())
			{
				MovingLabel::show(ResourceLang::map_lang["nomorecoin"]);

				Layer* layer = BuyCoinLayer::create(needcoincount.getValue() - GlobalInstance::getInstance()->getMyCoinCount().getValue());
				Director::getInstance()->getRunningScene()->addChild(layer, 10000, "buycoinlayer");
				return;
			}

			if (m_vecres.size() == 1 && m_vecres[0].rid.compare("p001") == 0)//体力药水
			{
				for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
				{
					Hero* hero = GlobalInstance::vec_myHeros[i];
					if (hero->getPower().getValue() > 50)
					{
						DynamicValueInt dal;
						dal.setValue(100);
						hero->setPower(dal);
					}
					else
					{
						DynamicValueInt dal;
						dal.setValue(hero->getPower().getValue() + 50);
						hero->setPower(dal);
					}
				}
			}
			else
			{
				for (unsigned int i = 0; i < m_vecres.size(); i++)
				{
					int qu = m_vecres[i].qu;
					int st = GlobalInstance::getInstance()->generateStoneCount(qu);

					if (m_vecres[i].rid.compare("r013") == 0)
					{
						DynamicValueInt intdv;
						intdv.setValue(m_vecres[i].count*1000);
						GlobalInstance::getInstance()->addMySoliverCount(intdv);
					}
					else
						MyRes::Add(m_vecres[i].rid, m_vecres[i].count, m_putwhere, qu, st);
				}
			}
			GlobalInstance::getInstance()->costMyCoinCount(needcoincount);
			setMarketData();

			AnimationEffect::closeAniEffect(this);
		}
			break;
		default:
			break;
		}
	}
}

void BuyResLayer::setMarketData()
{
	std::map<std::string, int> map_marketres;
	int lv = Building::map_buildingDatas["5market"]->level.getValue();
	for (int v = 0; v <= Building::map_buildingDatas["5market"]->level.getValue(); v++)
	{
		int vsize = Building::map_buildingDatas["5market"]->vec_exdata.size();
		if (v < vsize)
		{
			for (unsigned int i = 0; i < Building::map_buildingDatas["5market"]->vec_exdata[v].size(); i++)
			{
				std::vector<std::string> vec_tmp;
				CommonFuncs::split(Building::map_buildingDatas["5market"]->vec_exdata[v][i], vec_tmp, "-");

				std::string resid = vec_tmp[0];
				int stockcount = atoi(vec_tmp[1].c_str());
				map_marketres[resid] = stockcount;
			}
		}
	}

	std::string stockstr = DataSave::getInstance()->getMarketStock();
	if (stockstr.length() > 0)
	{
		std::vector<std::string> vec_one;
		CommonFuncs::split(stockstr, vec_one, ";");
		for (unsigned int i = 0; i < vec_one.size(); i++)
		{
			std::vector<std::string> vec_tmp;
			CommonFuncs::split(vec_one[i], vec_tmp, "-");
			std::string resid = vec_tmp[0];
			int stockcount = atoi(vec_tmp[1].c_str());
			map_marketres[resid] = stockcount;
		}
	}


	for (unsigned int i = 0; i < m_vecres.size(); i++)
	{
		std::string rid = m_vecres[i].rid;
		if (map_marketres.find(rid) != map_marketres.end())
		{
			map_marketres[rid] -= m_vecres[i].count;
			if (map_marketres[rid] < 0)
				map_marketres[rid] = 0;
		}
	}

	std::string str;

	std::map<std::string, int>::iterator it;

	for (it = map_marketres.begin(); it != map_marketres.end(); it++)
	{
		std::string onestr = StringUtils::format("%s-%d;",it->first.c_str(), map_marketres[it->first]);
		str.append(onestr);
	}

	if (str.length() > 0)
	{
		DataSave::getInstance()->setMarketStock(str.substr(0, str.length() - 1));
	}
}
