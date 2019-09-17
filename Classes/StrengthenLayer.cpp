#include "StrengthenLayer.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "Equip.h"
#include "json.h"
#include "MyRes.h"
#include "HeroAttrLayer.h"
#include "SelectEquipLayer.h"
#include "EquipDescLayer.h"
#include "Quest.h"
#include "MovingLabel.h"
#include "TakeOnLayer.h"
#include "AnimationEffect.h"
#include "EquipDescLayer.h"
#include "StoreHouseLayer.h"
#include "SimpleResPopLayer.h"
#include "BuyCoinLayer.h"
#include "GiftContentLayer.h"

StrengthenLayer::StrengthenLayer()
{
	coincount.setValue(0);
	boxeffectnode = NULL;
	luckcount.setValue(0);
}


StrengthenLayer::~StrengthenLayer()
{
	MyRes::saveData();
}


StrengthenLayer* StrengthenLayer::create(Equip* res_equip, int forwhere)
{
	StrengthenLayer *pRet = new(std::nothrow)StrengthenLayer();

	if (pRet && pRet->init(res_equip,forwhere))
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

bool StrengthenLayer::init(Equip* res_equip, int forwhere)
{
	if (!Layer::init())
	{
		return false;
	}

	m_forwhere = forwhere;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	csbnode = CSLoader::createNode(ResourcePath::makePath("strengthenLayer.csb"));
	this->addChild(csbnode);

	langtype = GlobalInstance::getInstance()->getLang();

	m_equip = res_equip;

	cocos2d::ui::Text* title = (cocos2d::ui::Text*)csbnode->getChildByName("title");
	title->setString(ResourceLang::map_lang["strengthtitle"]);

	int qu = m_equip->getQU().getValue();
	std::string qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
	resbox_qu = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox_qu");
	resbox_qu->loadTexture(qustr, cocos2d::ui::Widget::TextureResType::PLIST);

	if (boxeffectnode == NULL)
		boxeffectnode = CommonFuncs::playResBoxEffect(resbox_qu, m_equip->getType(), qu, m_equip->getLv().getValue());

	std::string str = GlobalInstance::getInstance()->getResUIFrameName(m_equip->getId(), qu);

	cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	res->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* name = (cocos2d::ui::Text*)csbnode->getChildByName("name");

	std::string namestr = GlobalInstance::map_AllResources[m_equip->getId()].name;
	if (m_equip->getLv().getValue() > 0)
		namestr = StringUtils::format("+%d%s", m_equip->getLv().getValue(), namestr.c_str());
	name->setString(namestr);

	name->setTextColor(Color4B(POTENTIALCOLOR[qu]));

	for (int i = 0; i < 4; i++)
	{

		str = StringUtils::format("resbox_%d", i);
		cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);
		resbox->addTouchEventListener(CC_CALLBACK_2(StrengthenLayer::onResClick, this));
		resbox->setTag(i);

		str = StringUtils::format("resname_%d", i);
		cocos2d::ui::Text* resname = (cocos2d::ui::Text*)csbnode->getChildByName(str);
		std::string restr = StringUtils::format("q00%d", i + 1);
		resname->setString(GlobalInstance::map_AllResources[restr].name);


		str = StringUtils::format("rescount_%d", i);
		cocos2d::ui::Text* rescount = (cocos2d::ui::Text*)csbnode->getChildByName(str);
		if (i < 3)
		{
			str = StringUtils::format("%d/%d", MyRes::getMyResCount(restr), m_equip->getLv().getValue() + 1);//一级需求一个
			rescount->setString(str);

			coincount.setValue(coincount.getValue() + GlobalInstance::map_AllResources[restr].coinval * (m_equip->getLv().getValue() + 1));
			if (MyRes::getMyResCount(restr) < m_equip->getLv().getValue() + 1)
			{
				rescount->setColor(Color3B(255, 0, 0));
			}
		}
		else
		{
			res4img = (cocos2d::ui::ImageView*)resbox->getChildByName("res");
			res4lbl = rescount;

		}
	}

	updateLuckycountlbl(0);
	this->schedule(schedule_selector(StrengthenLayer::updateLuckycountlbl), 1);
	updateSuccRnd();

	//强化按钮
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(StrengthenLayer::onBtnClick, this));
	actionbtn->setTag(1000);
	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("strenthbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);


	//立即强化按钮
	cocos2d::ui::Widget* dractionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn_0");
	dractionbtn->addTouchEventListener(CC_CALLBACK_2(StrengthenLayer::onBtnClick, this));
	dractionbtn->setTag(1001);
	cocos2d::ui::ImageView* drationbtntxt = (cocos2d::ui::ImageView*)dractionbtn->getChildByName("text");
	drationbtntxt->loadTexture(ResourcePath::makeTextImgPath("drstrenth_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* silvercount = (cocos2d::ui::Text*)csbnode->getChildByName("silvercount");
	str = StringUtils::format("x%d", coincount.getValue());
	silvercount->setString(str);


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

void StrengthenLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();

		if (m_equip->getLv().getValue() >= STHRENTHMAXLV)
		{
			MovingLabel::show(ResourceLang::map_lang["mostlv"]);
			return;
		}

		if (tag == 1000)
		{
			for (int i = 0; i < 3; i++)
			{
				std::string restr = StringUtils::format("q00%d", i + 1);
				if (MyRes::getMyResCount(restr) < m_equip->getLv().getValue() + 1)
				{
					MovingLabel::show(ResourceLang::map_lang["reslack"]);
					return;
				}
			}

			for (int i = 0; i < 3; i++)
			{
				std::string restr = StringUtils::format("q00%d", i + 1);
				if (MyRes::getMyResCount(restr) >= m_equip->getLv().getValue() + 1)
				{
					MyRes::Use(restr, m_equip->getLv().getValue() + 1);
				}
			}
		}
		else
		{
			if (GlobalInstance::getInstance()->getMyCoinCount().getValue() < coincount.getValue())
			{
				MovingLabel::show(ResourceLang::map_lang["nomorecoin"]);

				Layer* layer = BuyCoinLayer::create(coincount.getValue() - GlobalInstance::getInstance()->getMyCoinCount().getValue());
				Director::getInstance()->getRunningScene()->addChild(layer, 100, "buycoinlayer");
				return;
			}
			DynamicValueInt dvl;
			dvl.setValue(coincount.getValue());
			GlobalInstance::getInstance()->costMyCoinCount(dvl);
			GlobalInstance::getInstance()->setPosCostCoin(COSTCOIN_STRENTH, dvl.getValue());
		}

		//记录每日任务
		Quest::setDailyTask(STRENG_EQUIP, 1);
		Quest::setAchieveTypeCount(STRENG_EQUIP, 1);

		int r = GlobalInstance::getInstance()->createRandomNum(1000);
		int odds = ODDS[m_equip->getLv().getValue()] * 10 + luckcount.getValue() * 50;

		MyRes::Use("q004", luckcount.getValue());

		if (odds > 1000)
			odds = 1000;

		if (m_equip->getLv().getValue() + 1 == 18)
		{
			if (odds > 800)
				odds = 800;
		}
		else if (m_equip->getLv().getValue() + 1 == 19)
		{
			if (odds > 700)
				odds = 700;
		}
		else if (m_equip->getLv().getValue() + 1 == 20)
		{
			if (odds > 600)
				odds = 600;
		}

		if (r <= odds)
		{
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_STRENTHOK);
			DynamicValueInt elv;
			elv.setValue(m_equip->getLv().getValue() + 1);
			m_equip->setLv(elv);

			if (this->getParent()->getChildByName("qianghuachenggong")!=NULL)
			{
				this->getParent()->removeChildByName("qianghuachenggong");
			}
			//特效
			CommonFuncs::playCommonLvUpAnim(this->getParent(), "texiao_ziti");

			if (boxeffectnode == NULL)
				boxeffectnode = CommonFuncs::playResBoxEffect(resbox_qu, m_equip->getType(), m_equip->getQU().getValue(), m_equip->getLv().getValue());
		
			std::string contentstr = StringUtils::format(ResourceLang::map_lang["newtemplet11"].c_str(), GlobalInstance::getInstance()->getMyNickName().c_str(), GlobalInstance::map_AllResources[m_equip->getId()].name.c_str(), m_equip->getLv().getValue());
			MainScene::addNews(contentstr, 2);
		}
		else
		{
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_STRENTHFAIL);
			

			int r1 = GlobalInstance::getInstance()->createRandomNum(1000) + 1;
			int failrnd = FAILCOSTLV[m_equip->getLv().getValue()] * 10;
			if (r1 <= failrnd)
			{
				DynamicValueInt elv;
				elv.setValue(m_equip->getLv().getValue() - COSTLV[m_equip->getLv().getValue()]);
				m_equip->setLv(elv);
			}
			MovingLabel::show(ResourceLang::map_lang["strengthfail"]);

		}
		if (m_forwhere == 0)
		{
			TakeOnLayer* takon = (TakeOnLayer*)this->getParent();
			if (takon != NULL)
			{
				takon->updateAttr();
			}
		}
		else if (m_forwhere == 1)
		{
			EquipDescLayer* takon = (EquipDescLayer*)this->getParent();
			if (takon != NULL)
			{
				takon->updateAttr();
				StoreHouseLayer* storeHouseLayer = (StoreHouseLayer*)takon->getParent();
				if (storeHouseLayer != NULL)
					storeHouseLayer->updateUI();
			}
		}

		AnimationEffect::closeAniEffect((Layer*)this);
	}
}

void StrengthenLayer::onResClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		
		Node* node = (Node*)pSender;
		int tag = node->getTag();

		if (tag < 3)
		{
			std::string restr = StringUtils::format("q00%d", tag + 1);
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
			SimpleResPopLayer* layer = SimpleResPopLayer::create(restr, 2);
			this->addChild(layer);
			AnimationEffect::openAniEffect(layer);
		}
		else if(tag == 3){

			if (m_equip->getLv().getValue() >= STHRENTHMAXLV)
			{
				MovingLabel::show(ResourceLang::map_lang["mostlv"]);
				return;
			}

			std::string luckresid = "q004";

			bool ismax = false;
			int odds = ODDS[m_equip->getLv().getValue()] + (luckcount.getValue() + 1) * 5;
			if (odds > 100)
				ismax = true;

			if (m_equip->getLv().getValue() + 1 == 18)
			{
				if (odds > 80)
					ismax = true;
			}
			else if (m_equip->getLv().getValue() + 1 == 19)
			{
				if (odds > 70)
					ismax = true;
			}
			else if (m_equip->getLv().getValue() + 1 == 20)
			{
				if (odds > 60)
					ismax = true;
			}

			if (ismax)
			{
				MovingLabel::show(ResourceLang::map_lang["strenthaddrndmax"]);
				return;
			}

			luckcount.setValue(luckcount.getValue() + 1);
			int q004count = MyRes::getMyResCount(luckresid);
			if (q004count >= luckcount.getValue())
			{
				std::string resstr = StringUtils::format("ui/%s.png", luckresid.c_str());
				res4img->ignoreContentAdaptWithSize(true);
				res4img->loadTexture(resstr, cocos2d::ui::Widget::TextureResType::PLIST);
					
				updateLuckycountlbl(0);
				//std::string str = StringUtils::format(ResourceLang::map_lang["strenthaddrnd"].c_str(), luckcount.getValue()*5);
				MovingLabel::show(ResourceLang::map_lang["strenthaddrnd"]);

				updateSuccRnd();
			}
			else
			{
				luckcount.setValue(q004count);
				std::string str = StringUtils::format(ResourceLang::map_lang["notenouph"].c_str(), GlobalInstance::map_AllResources[luckresid].name.c_str());
				MovingLabel::show(str);

				for (unsigned int i = 0; i < GlobalInstance::vec_shopdata.size(); i++)
				{
					if (GlobalInstance::vec_shopdata[i].icon.compare("luckygift") == 0)
					{
						GiftContentLayer* layer = GiftContentLayer::create(&GlobalInstance::vec_shopdata[i], i);
						g_mainScene->addChild(layer, 10, "GiftContentLayer");
						AnimationEffect::openAniEffect((Layer*)layer);
						break;
					}
				}
			}
		}
	}
}

void StrengthenLayer::updateLuckycountlbl(float dt)
{
	std::string countstr = StringUtils::format("%d/%d", luckcount.getValue(), MyRes::getMyResCount("q004"));
	res4lbl->setString(countstr);
}

void StrengthenLayer::updateSuccRnd()
{
	cocos2d::ui::Text* tipstext = (cocos2d::ui::Text*)csbnode->getChildByName("tipstext");
	std::string str;
	if (m_equip->getLv().getValue() < STHRENTHMAXLV)
	{

		float odds = ODDS[m_equip->getLv().getValue()] + luckcount.getValue() * 5;
		if (odds > 100.0f)
			odds = 100.0f;

		if (m_equip->getLv().getValue() + 1 == 18)
		{
			if (odds > 80.0f)
				odds = 80.0f;
		}
		else if (m_equip->getLv().getValue() + 1 == 19)
		{
			if (odds > 70.0f)
				odds = 70.0f;
		}
		else if (m_equip->getLv().getValue() + 1 == 20)
		{
			if (odds > 60.0f)
				odds = 60.0f;
		}
		if (m_equip->getLv().getValue() >= 6)
		{
			str = StringUtils::format(ResourceLang::map_lang["tipsstrengthtext"].c_str(), odds, COSTLV[m_equip->getLv().getValue()]);
		}
		else
		{
			str = StringUtils::format(ResourceLang::map_lang["tipsstrengthtext1"].c_str(), odds, COSTLV[m_equip->getLv().getValue()]);
		}
	}
	else
	{
		str = ResourceLang::map_lang["mostlv"];
	}

	tipstext->setString(str);
}