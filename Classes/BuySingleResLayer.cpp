#include "BuySingleResLayer.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "Const.h"
#include "SoundManager.h"
#include "AnimationEffect.h"
#include "SoundManager.h"
#include "BuyCoinLayer.h"
#include "EventBusinessLayer.h"
#include "MapBlockScene.h"

BuySingleResLayer::BuySingleResLayer()
{
	m_isLongPress = false;
	m_longTouchNode = NULL;
	buycount = 1;
}


BuySingleResLayer::~BuySingleResLayer()
{

}

BuySingleResLayer* BuySingleResLayer::create(MSGAWDSDATA fproper)
{
	BuySingleResLayer *pRet = new(std::nothrow)BuySingleResLayer();
	if (pRet && pRet->init(fproper))
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

bool BuySingleResLayer::init(MSGAWDSDATA fproper)
{
	if (!Layer::init())
	{
		return false;
	}

	m_fproper = fproper;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color, 0, "colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("buySingleResLayer.csb"));
	this->addChild(csbnode);

	cocos2d::ui::ImageView* smallbg = (cocos2d::ui::ImageView*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	Node* topnode = csbnode->getChildByName("topnode");

	Node* contentnode = csbnode->getChildByName("contentnode");
	contentnode->setVisible(false);

	Node* bottomnode = csbnode->getChildByName("bottomnode");

	cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)topnode->getChildByName("resbox");
	std::string resid = fproper.rid;
	int qu = 0;
	std::string qustr = "ui/resbox.png";

	int smallbgheight = 1000;
	int t = 0;
	for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
	{
		if (resid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
			break;
	}
	if (t >= T_ARMOR && t <= T_FASHION)
	{
		qu = fproper.qu;
		qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
	}
	else if (t >= T_WG && t <= T_NG)
	{
		qu = GlobalInstance::map_GF[resid].qu;
		qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
	}

	else if (t >= T_RENS && t <= T_BOX)
	{
		qu = atoi(resid.substr(1).c_str()) - 1;
		qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
	}

	resbox->loadTexture(ResourcePath::makePath(qustr), cocos2d::ui::Widget::TextureResType::PLIST);

	CommonFuncs::playResBoxEffect(resbox, t, qu, 0);

	std::string str = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);

	cocos2d::ui::ImageView* resimg = (cocos2d::ui::ImageView*)topnode->getChildByName("res");
	resimg->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

	//名字
	cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)topnode->getChildByName("name");

	if (t > T_RES_0)
		namelbl->setTextColor(Color4B(POTENTIALCOLOR[qu]));

	std::string namestr = GlobalInstance::map_AllResources[resid].name;
	if (t >= T_ARMOR && t <= T_NG)
	{
		std::string st = StringUtils::format("potential_%d", qu);
		namestr = StringUtils::format(ResourceLang::map_lang["libraryinfoequipname"].c_str(), GlobalInstance::map_AllResources[resid].name.c_str(), ResourceLang::map_lang[st].c_str());
	}
	namelbl->setString(namestr);

	//描述
	cocos2d::ui::Text* desclbl = (cocos2d::ui::Text*)topnode->getChildByName("desctext");
	desclbl->setString(GlobalInstance::map_AllResources[resid].desc);

	for (int i = 0; i <= 5; i++)
	{
		str = StringUtils::format("attrtext_%d", i);
		attrlblArr[i] = (cocos2d::ui::Text*)contentnode->getChildByName(str);
	}

	if (t >= T_ARMOR && t <= T_NG)
	{
		contentnode->setVisible(true);

		getAttr(resid, t);

		cocos2d::ui::Text* jobtext = (cocos2d::ui::Text*)contentnode->getChildByName("attrtext_6");

		std::string jobstr;

		if (t >= T_WG && t <= T_NG)
		{
			for (unsigned int i = 0; i < GlobalInstance::map_GF[resid].vec_skillbns.size(); i++)
			{
				int m = GlobalInstance::map_GF[resid].vec_skillbns[i];
				if (m == 1)
				{
					if (jobstr.length() > 0)
						jobstr.append(ResourceLang::map_lang["comma"]);
					jobstr.append(GlobalInstance::map_AllResources[GlobalInstance::vec_herosAttr[i].id].name);
				}
			}

			cocos2d::ui::Text* skilltextlbl = (cocos2d::ui::Text*)contentnode->getChildByName("skilltext");
			skilltextlbl->setString(ResourceLang::map_lang["skilltext"]);

			int skilltype = GlobalInstance::map_GF[resid].skill;
			std::string skillids = StringUtils::format("sk%03d", skilltype);

			cocos2d::ui::Text* skillnamelbl = (cocos2d::ui::Text*)contentnode->getChildByName("skillname");
			std::string skillname = StringUtils::format(ResourceLang::map_lang["skillname"].c_str(), GlobalInstance::map_AllResources[skillids].name.c_str());
			skillnamelbl->setString(skillname);

			cocos2d::ui::Text* skilldesclbl = (cocos2d::ui::Text*)contentnode->getChildByName("skilldesc");
			Label* dlbl = (Label*)skilldesclbl->getVirtualRenderer();
			dlbl->setLineSpacing(5);

			std::string allskilldesc = StringUtils::format(ResourceLang::map_lang["gfkilldesc"].c_str(), GlobalInstance::map_GF[resid].skillrnd);

			std::string skilldesc = GlobalInstance::map_AllResources[skillids].desc;
			if (skilltype == SKILL_1 || skilltype == SKILL_13 || skilltype == SKILL_15 || skilltype == SKILL_20)
				skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[resid].skilleff1);
			else if (skilltype == SKILL_2)
				skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[resid].skilleff1 / 100);
			else if (skilltype == SKILL_3 || skilltype == SKILL_4)
				skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[resid].skilleff2 - 1);
			else if (skilltype == SKILL_5 || skilltype == SKILL_6)
				skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[resid].skilleff2, GlobalInstance::map_GF[resid].skilleff1);
			else if (skilltype == SKILL_7)
				skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[resid].skilleff1);
			else if (skilltype == SKILL_8)
				skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[resid].skilleff2, GlobalInstance::map_GF[resid].skilleff1);
			else if (skilltype == SKILL_9 || skilltype == SKILL_10 || skilltype == SKILL_11 || skilltype == SKILL_12 || skilltype == SKILL_14 || skilltype == SKILL_16 || skilltype == SKILL_18)
				skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[resid].skilleff1, GlobalInstance::map_GF[resid].skilleff2 - 1);
			else if (skilltype == SKILL_17)
				skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[resid].skilleff1, GlobalInstance::map_GF[resid].skilleff1, GlobalInstance::map_GF[resid].skilleff2 - 1);

			allskilldesc.append(skilldesc);
			skilldesclbl->setString(allskilldesc);
		}
		else 
		{
			contentnode->getChildByName("skilltext")->setVisible(false);
			contentnode->getChildByName("skillname")->setVisible(false);
			contentnode->getChildByName("skilldesc")->setVisible(false);
			smallbgheight = 900;
			topnode->setPositionY(970);
			contentnode->setPositionY(570);
			bottomnode->setPositionY(350);

			if (t == T_ARMOR)
			{
				for (unsigned int i = 0; i < GlobalInstance::map_Equip[resid].vec_bns.size(); i++)
				{
					float m = GlobalInstance::map_Equip[resid].vec_bns[i];
					if (m >= 1)
					{
						if (jobstr.length() > 0)
							jobstr.append(ResourceLang::map_lang["comma"]);
						jobstr.append(GlobalInstance::map_AllResources[GlobalInstance::vec_herosAttr[i].id].name);

					}
				}
			}
			else if (t >= T_EQUIP && t <= T_FASHION)
			{
				jobstr = ResourceLang::map_lang["fitalljob"];
			}
		}
		jobtext->setString(jobstr);

		cocos2d::ui::Text* vocationtextlbl = (cocos2d::ui::Text*)contentnode->getChildByName("vocationtext");
		vocationtextlbl->setString(ResourceLang::map_lang["fitjob"]);
	}
	else
	{
		smallbgheight = 650;
		topnode->setPositionY(840);
		bottomnode->setPositionY(500);
	}

	smallbg->setContentSize(Size(smallbg->getContentSize().width, smallbgheight));

	cocos2d::ui::Text* pricetext = (cocos2d::ui::Text*)bottomnode->getChildByName("pricetext");
	pricetext->setString(ResourceLang::map_lang["buypricetext"]);

	cocos2d::ui::Text* counttext = (cocos2d::ui::Text*)bottomnode->getChildByName("counttext");
	counttext->setString(ResourceLang::map_lang["buycounttext"]);

	totalpricelbl = (cocos2d::ui::Text*)bottomnode->getChildByName("totalprice");

	priceicon = (cocos2d::ui::ImageView*)bottomnode->getChildByName("priceicon");
	priceicon->ignoreContentAdaptWithSize(true);

	if (GlobalInstance::map_AllResources[resid].coinval > 0)
		priceicon->loadTexture(ResourcePath::makePath("ui/main_coin.png"), cocos2d::ui::Widget::TextureResType::PLIST);

	rescountlbl = (cocos2d::ui::Text*)bottomnode->getChildByName("rescount");

	updateData();

	updateCoinTextColor(0);
	this->schedule(schedule_selector(BuySingleResLayer::updateCoinTextColor), 1.0f);
	//按钮
	cocos2d::ui::Widget* buybtn = (cocos2d::ui::Widget*)bottomnode->getChildByName("buybtn");
	buybtn->setTag(100);
	buybtn->addTouchEventListener(CC_CALLBACK_2(BuySingleResLayer::onBtnClick, this));

	int langtype = GlobalInstance::getInstance()->getLang();

	//按钮文字
	cocos2d::ui::ImageView* buybtntxt = (cocos2d::ui::ImageView*)buybtn->getChildByName("text");
	buybtntxt->loadTexture(ResourcePath::makeTextImgPath("mapeventtext_6_1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//取消按钮
	cocos2d::ui::Widget* cancelbtn = (cocos2d::ui::Widget*)bottomnode->getChildByName("closebtn");
	cancelbtn->setTag(101);
	cancelbtn->addTouchEventListener(CC_CALLBACK_2(BuySingleResLayer::onBtnClick, this));

	//取消按钮文字
	cocos2d::ui::ImageView* cancelbtntxt = (cocos2d::ui::ImageView*)cancelbtn->getChildByName("text");
	cancelbtntxt->loadTexture(ResourcePath::makeTextImgPath("cancelbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//加号按钮
	cocos2d::ui::Widget* addbtn = (cocos2d::ui::Widget*)bottomnode->getChildByName("addbtn");
	addbtn->addTouchEventListener(CC_CALLBACK_2(BuySingleResLayer::onAddBtnClick, this));
	addbtn->setTag(1001);
	//减号按钮
	cocos2d::ui::Widget* subbtn = (cocos2d::ui::Widget*)bottomnode->getChildByName("subbtn");
	subbtn->addTouchEventListener(CC_CALLBACK_2(BuySingleResLayer::onSubBtnClick, this));
	subbtn->setTag(1002);

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

void BuySingleResLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);

	Node* clicknode = (Node*)pSender;
	int tag = clicknode->getTag();

	if (type == ui::Widget::TouchEventType::ENDED)
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		switch (tag)
		{
		case 100://购买按钮
		{
			std::string showstr;

			int saleval = 0;
			DynamicValueInt myrich;
			bool iscoinsale = false;
			if (GlobalInstance::map_AllResources[m_fproper.rid].coinval > 0)
			{
				saleval = GlobalInstance::map_AllResources[m_fproper.rid].coinval;
				myrich.setValue(GlobalInstance::getInstance()->getMyCoinCount().getValue());
				iscoinsale = true;
			}
			else
			{
				saleval = GlobalInstance::map_AllResources[m_fproper.rid].silverval*9/10;
				myrich.setValue(GlobalInstance::getInstance()->getMySoliverCount().getValue());
			}

			if (myrich.getValue() < buycount * saleval)
			{
				if (iscoinsale)
				{
					showstr = ResourceLang::map_lang["nomorecoin"];
					Layer* layer = BuyCoinLayer::create(buycount * saleval - myrich.getValue());
					Director::getInstance()->getRunningScene()->addChild(layer, 100, "buycoinlayer");
				}
				else
					showstr = ResourceLang::map_lang["nomoresilver"];

				MovingLabel::show(showstr);
			}
			else
			{
				SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUYRES);
				m_fproper.count = buycount;
				DynamicValueInt dv;
				dv.setValue(buycount * saleval);
				if (iscoinsale)
				{
					GlobalInstance::getInstance()->costMyCoinCount(dv);
					GlobalInstance::getInstance()->setPosCostCoin(COSTCOIN_EVENTBUISNESS, dv.getValue());
				}
				else
					GlobalInstance::getInstance()->costMySoliverCount(dv);

				EventBusinessLayer* layer = (EventBusinessLayer*)g_MapBlockScene->getChildByName("buisinesslayer");
				if (layer != NULL)
				{
					layer->buyRes(m_fproper);
					AnimationEffect::closeAniEffect(this);
				}
				else
				{
					MyRes::Add(m_fproper.rid, m_fproper.count, MYPACKAGE, m_fproper.qu, GlobalInstance::getInstance()->generateStoneCount(m_fproper.qu));
					AnimationEffect::closeAniEffect(this);
				}
			}

			break;
		}
		case 101:
			AnimationEffect::closeAniEffect(this);
			break;
		default:
			break;
		}
	}
}

void BuySingleResLayer::onAddBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	Node* clicknode = (Node*)pSender;
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		m_longTouchNode = clicknode;
		if (!isScheduled(schedule_selector(BuySingleResLayer::longTouchUpdate)))
			schedule(schedule_selector(BuySingleResLayer::longTouchUpdate), 0.1f);
	}
	else if (type == ui::Widget::TouchEventType::ENDED)
	{
		cancelLongTouch();
		addCount();
	}
	else if (type == ui::Widget::TouchEventType::CANCELED)
	{
		cancelLongTouch();
	}
}

void BuySingleResLayer::onSubBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	Node* clicknode = (Node*)pSender;
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		m_longTouchNode = clicknode;
		if (!isScheduled(schedule_selector(BuySingleResLayer::longTouchUpdate)))
			schedule(schedule_selector(BuySingleResLayer::longTouchUpdate), 0.1f);
	}
	else if (type == ui::Widget::TouchEventType::ENDED)
	{
		cancelLongTouch();
		subCount();
	}
	else if (type == ui::Widget::TouchEventType::CANCELED)
	{
		cancelLongTouch();
	}
}

void BuySingleResLayer::longTouchUpdate(float delay)
{
	m_isLongPress = true;
	if (m_longTouchNode != NULL) {
		if (m_longTouchNode->getTag() % 1000 == 1)
			addCount();
		else if (m_longTouchNode->getTag() % 1000 == 2)
			subCount();
	}
}

void BuySingleResLayer::cancelLongTouch()
{
	m_isLongPress = false;
	m_longTouchNode = NULL;
	unschedule(schedule_selector(BuySingleResLayer::longTouchUpdate));
}

void BuySingleResLayer::addCount()
{
	if (buycount >= m_fproper.count)
	{
		MovingLabel::show(ResourceLang::map_lang["outofstock"]);
	}
	else
	{
		buycount++;
		updateData();
	}
}

void BuySingleResLayer::subCount()
{
	if (buycount > 1)
	{
		buycount--;
		updateData();
	}
}

void BuySingleResLayer::updateData()
{	
	int saleval = 0;
	DynamicValueInt myrich;
	std::string resid = m_fproper.rid;
	if (GlobalInstance::map_AllResources[resid].coinval > 0)
	{
		saleval = GlobalInstance::map_AllResources[resid].coinval;
		myrich.setValue(GlobalInstance::getInstance()->getMyCoinCount().getValue());
	}
	else
	{
		saleval = GlobalInstance::map_AllResources[resid].silverval*9/10;
		myrich.setValue(GlobalInstance::getInstance()->getMySoliverCount().getValue());
	}

	std::string salestr = StringUtils::format("%d", buycount * saleval);
	totalpricelbl->setString(salestr);

	std::string countstr = StringUtils::format("%d/%d", buycount, m_fproper.count);
	rescountlbl->setString(countstr);
}

void BuySingleResLayer::getAttr(std::string gfid, int type)
{
	std::vector<float> vec_attrval;

	float hp = 0;
	float atk = 0;
	float df = 0;
	float atkspeed = 0;
	float crit = 0;
	float dodge = 0;

	if (type >= T_WG && type <= T_NG)
	{
		hp = GlobalInstance::map_GF[gfid].vec_hp[0];
		atk = GlobalInstance::map_GF[gfid].vec_atk[0];
		df = GlobalInstance::map_GF[gfid].vec_df[0];
		atkspeed = GlobalInstance::map_GF[gfid].vec_speed[0];
		crit = GlobalInstance::map_GF[gfid].vec_crit[0];
		dodge = GlobalInstance::map_GF[gfid].vec_avoid[0];
	}
	else
	{
		float ebns = POTENTIAL_BNS[m_fproper.qu];
		hp = GlobalInstance::map_Equip[gfid].maxhp * ebns;
		atk = GlobalInstance::map_Equip[gfid].atk * ebns;
		df = GlobalInstance::map_Equip[gfid].df * ebns;
		atkspeed = GlobalInstance::map_Equip[gfid].speed * ebns;
		crit = GlobalInstance::map_Equip[gfid].crit * ebns;
		dodge = GlobalInstance::map_Equip[gfid].avoid * ebns;
	}

	vec_attrval.push_back(hp);
	vec_attrval.push_back(atk);
	vec_attrval.push_back(df);
	vec_attrval.push_back(atkspeed);
	vec_attrval.push_back(crit);
	vec_attrval.push_back(dodge);


	for (int i = 0; i <= 5; i++)
	{
		std::string str = StringUtils::format("addattrtext_%d", i);
		str = StringUtils::format(ResourceLang::map_lang[str].c_str(), vec_attrval[i]);
		attrlblArr[i]->setString(str);
	}
}

void BuySingleResLayer::updateCoinTextColor(float dt)
{
	int saleval = 0;
	DynamicValueInt myrich;
	std::string resid = m_fproper.rid;
	if (GlobalInstance::map_AllResources[resid].coinval > 0)
	{
		saleval = GlobalInstance::map_AllResources[resid].coinval;
		myrich.setValue(GlobalInstance::getInstance()->getMyCoinCount().getValue());
	}
	else
	{
		saleval = GlobalInstance::map_AllResources[resid].silverval * 9 / 10;
		myrich.setValue(GlobalInstance::getInstance()->getMySoliverCount().getValue());
	}

	if (myrich.getValue() < buycount * saleval)
	{
		totalpricelbl->setColor(Color3B(255, 0, 0));
	}
	else
	{
		totalpricelbl->setColor(Color3B(255, 191, 0));
	}
}