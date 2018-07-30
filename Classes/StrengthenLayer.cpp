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

#define SLIVERCOUNT 100

StrengthenLayer::StrengthenLayer()
{

}


StrengthenLayer::~StrengthenLayer()
{
	MyRes::saveData();
}


StrengthenLayer* StrengthenLayer::create(Equip* res_equip)
{
	StrengthenLayer *pRet = new(std::nothrow)StrengthenLayer();

	if (pRet && pRet->init(res_equip))
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

bool StrengthenLayer::init(Equip* res_equip)
{
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);

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

	std::string str = StringUtils::format("ui/%s.png", m_equip->getId().c_str());
	if (qu == 3)
	{
		str = StringUtils::format("ui/%s-2.png", m_equip->getId().c_str());
	}
	else if (qu == 4)
	{
		str = StringUtils::format("ui/%s-3.png", m_equip->getId().c_str());
	}

	cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	res->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* lv = (cocos2d::ui::Text*)csbnode->getChildByName("lv");
	str = StringUtils::format("LV.%d", m_equip->getLv().getValue());
	lv->setString(str);

	cocos2d::ui::Text* name = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	name->setString(GlobalInstance::map_AllResources[m_equip->getId()].name);

	for (int i = 0; i < 3; i++)
	{
		str = StringUtils::format("resname_%d", i);
		cocos2d::ui::Text* resname = (cocos2d::ui::Text*)csbnode->getChildByName(str);
		std::string restr = StringUtils::format("q00%d", i + 1);
		resname->setString(GlobalInstance::map_AllResources[restr].name);

		str = StringUtils::format("rescount_%d", i);
		cocos2d::ui::Text* rescount = (cocos2d::ui::Text*)csbnode->getChildByName(str);
		str = StringUtils::format("%d/%d", MyRes::getMyResCount(restr), m_equip->getLv().getValue());//一级需求一个
		rescount->setString(str);

		if (MyRes::getMyResCount(restr) < m_equip->getLv().getValue())
		{
			rescount->setColor(Color3B(255, 0, 0));
		}
	}

	cocos2d::ui::Text* tipstext = (cocos2d::ui::Text*)csbnode->getChildByName("tipstext");
	if (m_equip->getLv().getValue() > 6)
	{
		str = StringUtils::format(ResourceLang::map_lang["tipsstrengthtext"].c_str(), ODDS[m_equip->getLv().getValue() - 1], COSTLV[m_equip->getLv().getValue() - 1]);
	}
	else
	{
		str = StringUtils::format(ResourceLang::map_lang["tipsstrengthtext1"].c_str(), ODDS[m_equip->getLv().getValue() - 1], COSTLV[m_equip->getLv().getValue() - 1]); 
	}
	
	tipstext->setString(str);

	//强化按钮
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(StrengthenLayer::onBtnClick, this));
	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("strenthbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* silvercount = (cocos2d::ui::Text*)csbnode->getChildByName("silvercount");
	str = StringUtils::format("x%d", SLIVERCOUNT);
	silvercount->setString(str);


	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		this->removeFromParentAndCleanup(true);
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
		if (m_equip->getLv().getValue() >= sizeof(COSTLV) / sizeof(COSTLV[0]))
		{
			MovingLabel::show(ResourceLang::map_lang["mostlv"]);
			return;
		}

		for (int i = 0; i < 3; i++)
		{
			std::string restr = StringUtils::format("q00%d", i + 1);
			if (MyRes::getMyResCount(restr) < m_equip->getLv().getValue())
			{
				MovingLabel::show(ResourceLang::map_lang["reslack"]);
				return;
			}
		}
		if (GlobalInstance::getInstance()->getMySoliverCount().getValue() < SLIVERCOUNT)
		{
			MovingLabel::show(ResourceLang::map_lang["nomoresilver"]);
			return;
		}

		for (int i = 0; i < 3; i++)
		{
			std::string restr = StringUtils::format("q00%d", i + 1);
			if (MyRes::getMyResCount(restr) >= m_equip->getLv().getValue())
			{
				MyRes::Use(restr);
			}
		}
		DynamicValueInt dvl;
		dvl.setValue(SLIVERCOUNT);
		GlobalInstance::getInstance()->costMySoliverCount(dvl);

		int r = GlobalInstance::getInstance()->createRandomNum(1000);
		int odds = ODDS[m_equip->getLv().getValue() - 1] * 10;
		if (r <= odds)
		{
			DynamicValueInt elv;
			elv.setValue(m_equip->getLv().getValue() + 1);
			m_equip->setLv(elv);

			MovingLabel::show(ResourceLang::map_lang["strengthsuccess"]);
		}
		else
		{
			DynamicValueInt elv;
			elv.setValue(m_equip->getLv().getValue() - COSTLV[m_equip->getLv().getValue() - 1]);
			m_equip->setLv(elv);
			MovingLabel::show(ResourceLang::map_lang["strengthfail"]);
		}
		this->removeFromParentAndCleanup(true);
	}
}