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

StrengthenLayer::StrengthenLayer()
{
	coincount.setValue(0);
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

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	langtype = GlobalInstance::getInstance()->getLang();

	m_equip = res_equip;

	cocos2d::ui::Text* title = (cocos2d::ui::Text*)csbnode->getChildByName("title");
	title->setString(ResourceLang::map_lang["strengthtitle"]);

	int qu = m_equip->getQU().getValue();
	std::string qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
	cocos2d::ui::ImageView* resbox_qu = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox_qu");
	resbox_qu->loadTexture(qustr, cocos2d::ui::Widget::TextureResType::PLIST);

	CommonFuncs::playResBoxEffect(resbox_qu, qu);

	std::string str = GlobalInstance::getInstance()->getResUIFrameName(m_equip->getId(), qu);

	cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	res->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* name = (cocos2d::ui::Text*)csbnode->getChildByName("name");

	std::string namestr = GlobalInstance::map_AllResources[m_equip->getId()].name;
	if (m_equip->getLv().getValue() > 0)
		namestr = StringUtils::format("+%d%s", m_equip->getLv().getValue(), namestr.c_str());
	name->setString(namestr);

	name->setTextColor(Color4B(POTENTIALCOLOR[qu]));

	for (int i = 0; i < 3; i++)
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
		str = StringUtils::format("%d/%d", MyRes::getMyResCount(restr), m_equip->getLv().getValue() + 1);//一级需求一个
		rescount->setString(str);

		coincount.setValue(coincount.getValue() + GlobalInstance::map_AllResources[restr].coinval * (m_equip->getLv().getValue() + 1));
		if (MyRes::getMyResCount(restr) < m_equip->getLv().getValue() + 1)
		{
			rescount->setColor(Color3B(255, 0, 0));
		}
	}

	cocos2d::ui::Text* tipstext = (cocos2d::ui::Text*)csbnode->getChildByName("tipstext");
	if (m_equip->getLv().getValue() >= 6)
	{
		str = StringUtils::format(ResourceLang::map_lang["tipsstrengthtext"].c_str(), ODDS[m_equip->getLv().getValue()], COSTLV[m_equip->getLv().getValue()]);
	}
	else
	{
		str = StringUtils::format(ResourceLang::map_lang["tipsstrengthtext1"].c_str(), ODDS[m_equip->getLv().getValue()], COSTLV[m_equip->getLv().getValue()]); 
	}
	
	tipstext->setString(str);

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

		if (m_equip->getLv().getValue() >= sizeof(COSTLV) / sizeof(COSTLV[0]))
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
			GlobalInstance::getInstance()->setPosCostCoin(4, dvl.getValue());
		}

		//记录每日任务
		Quest::setDailyTask(STRENG_EQUIP, 1);
		Quest::setAchieveTypeCount(STRENG_EQUIP, 1);

		int r = GlobalInstance::getInstance()->createRandomNum(1000);
		int odds = ODDS[m_equip->getLv().getValue()] * 10;
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
		}
		else
		{
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_STRENTHFAIL);
			DynamicValueInt elv;
			elv.setValue(m_equip->getLv().getValue() - COSTLV[m_equip->getLv().getValue()]);
			m_equip->setLv(elv);
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
		std::string restr = StringUtils::format("q00%d", tag + 1);
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		SimpleResPopLayer* layer = SimpleResPopLayer::create(restr, 2);
		this->addChild(layer);
		AnimationEffect::openAniEffect(layer);
	}
}