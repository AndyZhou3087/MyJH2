#include "HeroLvupLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "RandHeroNode.h"
#include "InnRoomLayer.h"
#include "MainScene.h"
#include "MovingLabel.h"
#include "OutTownLayer.h"
#include "SelectMyHerosLayer.h"
#include "MapBlockScene.h"
#include "SelectEquipLayer.h"
#include "MyRes.h"
#include "DynamicValue.h"
#include "HeroAttrLayer.h"

USING_NS_CC;

#define S001EXP 100
#define S002EXP 200
#define S003EXP 300
#define S004EXP 400

HeroLvupLayer::HeroLvupLayer()
{

}

HeroLvupLayer::~HeroLvupLayer()
{
	
}


HeroLvupLayer* HeroLvupLayer::create(Hero* herodata)
{
	HeroLvupLayer *pRet = new(std::nothrow)HeroLvupLayer();
	if (pRet && pRet->init(herodata))
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
bool HeroLvupLayer::init(Hero* herodata)
{
	if (!Layer::init())
	{
		return false;
	}

	m_heroData = herodata;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 250));
	this->addChild(color);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("heroLvupLayer.csb"));
	this->addChild(csbnode);
	langtype = GlobalInstance::getInstance()->getLang();

	//英雄全身图
	cocos2d::ui::ImageView* herofullimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("hfull");
	herofullimg->ignoreContentAdaptWithSize(true);
	std::string str = StringUtils::format("hfull_%d_%d.png", herodata->getVocation(), herodata->getSex());
	herofullimg->loadTexture(ResourcePath::makeImagePath(str), cocos2d::ui::Widget::TextureResType::LOCAL);

	
	//属性信息
	Node* heroattrbottom = csbnode->getChildByName("heroattrbottom");

	//品质
	cocos2d::ui::ImageView* heroattrqu = (cocos2d::ui::ImageView*)heroattrbottom->getChildByName("heroattrqu");
	str = StringUtils::format("heroattrqu_%d", herodata->getPotential());
	heroattrqu->loadTexture(ResourcePath::makeTextImgPath(str, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//名字TextField控件效果不好，改用EditBox
	cocos2d::ui::Text* nametext = (cocos2d::ui::Text*)heroattrbottom->getChildByName("nametext");
	nametext->setString(herodata->getName());

	cocos2d::ui::Button* btn = (cocos2d::ui::Button*)heroattrbottom->getChildByName("backbtn");
	btn->addTouchEventListener(CC_CALLBACK_2(HeroLvupLayer::onBtnClick, this));


	//角色职业
	cocos2d::ui::Text* vocation = (cocos2d::ui::Text*)heroattrbottom->getChildByName("vocation");
	str = StringUtils::format("vocation_%d", herodata->getVocation());
	vocation->setString(ResourceLang::map_lang[str]);

	//角色属性文字
	for (int i = 0; i <= 5; i++)
	{
		str = StringUtils::format("attrtext_%d", i);
		cocos2d::ui::Text* attrtext = (cocos2d::ui::Text*)heroattrbottom->getChildByName(str);
		attrtext->setString(ResourceLang::map_lang[str]);
	}

	//生命值
	hplbl = (cocos2d::ui::Text*)heroattrbottom->getChildByName("hp");

	//int hp = herodata->getHp();

	//std::string attrstr = StringUtils::format("%d/%d", hp, (int)herodata->getMaxHp());
	//hplbl->setString(attrstr);

	//攻击值
	atkbl = (cocos2d::ui::Text*)heroattrbottom->getChildByName("atk");
	//attrstr = StringUtils::format("%d", (int)herodata->getAtk());
	//atkbl->setString(attrstr);

	//防御值
	dflbl = (cocos2d::ui::Text*)heroattrbottom->getChildByName("df");
	//attrstr = StringUtils::format("%d", (int)herodata->getDf());
	//dflbl->setString(attrstr);

	//攻击速度值
	atkspeedlbl = (cocos2d::ui::Text*)heroattrbottom->getChildByName("atkspeed");
	//attrstr = StringUtils::format("%.3f", 1.0f/herodata->getAtkSpeed());
	//atkspeedlbl->setString(attrstr);

	//暴击值
	critlbl = (cocos2d::ui::Text*)heroattrbottom->getChildByName("crit");
	//attrstr = StringUtils::format("%.3f%%", herodata->getCrit());
	//critlbl->setString(attrstr);

	//闪避值
	dodgelbl = (cocos2d::ui::Text*)heroattrbottom->getChildByName("dodge");
	//attrstr = StringUtils::format("%.3f%%", herodata->getDodge());
	//dodgelbl->setString(attrstr);


	for (int i = 1; i < 5; i++)
	{
		std::string str = StringUtils::format("s00%dimg", i);
		cocos2d::ui::ImageView* goodimg = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);
		goodimg->addTouchEventListener(CC_CALLBACK_2(HeroLvupLayer::onGoodsClick, this));
		goodimg->setTag(i);

		str = StringUtils::format("s00%dtext", i); 
		goodarr[i] = (cocos2d::ui::Text*)csbnode->getChildByName(str);
		str = StringUtils::format("s00%d", i);
		str = StringUtils::format("%d", MyRes::getMyResCount(str));
		goodarr[i]->setString(str);
	}


	//等级
	lvUIText = (cocos2d::ui::Text*)heroattrbottom->getChildByName("lv");
	str = StringUtils::format("Lv.%d", herodata->getLevel() + 1);
	lvUIText->setString(str);

	int curlvexp = 0;
	int nextlvexp = 0;
	int expsize = GlobalInstance::vec_herosAttr[herodata->getVocation()].vec_exp.size();

	if (herodata->getLevel() >= expsize)
		nextlvexp = GlobalInstance::vec_herosAttr[herodata->getVocation()].vec_exp[expsize - 1];
	else
		nextlvexp = GlobalInstance::vec_herosAttr[herodata->getVocation()].vec_exp[herodata->getLevel()];

	if (herodata->getLevel() > 0)
		curlvexp = GlobalInstance::vec_herosAttr[herodata->getVocation()].vec_exp[herodata->getLevel() - 1];

	//经验值
	explbl = (cocos2d::ui::Text*)heroattrbottom->getChildByName("exp");
	str = StringUtils::format("%d/%d", herodata->getExp().getValue() - curlvexp, nextlvexp - curlvexp);
	explbl->setString(str);

	expbar = (cocos2d::ui::LoadingBar*)heroattrbottom->getChildByName("heroattrexpbar");
	float percent = (herodata->getExp().getValue() - curlvexp)*100.0f / (nextlvexp - curlvexp);
	expbar->setPercent(percent);

	updataAtrrUI(0);
	this->schedule(schedule_selector(HeroLvupLayer::updataAtrrUI), 1.0f);

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

void HeroLvupLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	cocos2d::ui::Button* clicknode = (cocos2d::ui::Button*)pSender;

	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		this->removeFromParentAndCleanup(true);
	}
}

void HeroLvupLayer::onGoodsClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	cocos2d::ui::Button* clicknode = (cocos2d::ui::Button*)pSender;
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (m_heroData->getLevel() + 1 == m_heroData->getMaxLevel())
		{
			return;
		}

		int tag = clicknode->getTag();
		int count;
		switch (tag)
		{
		case 1:
			count = S001EXP;
			break;
		case 2:
			count = S002EXP;
			break;
		case 3:
			count = S003EXP;
			break;
		case 4:
			count = S004EXP;
			break;
		default:
			break;
		}

		if ((m_heroData->getLevel() + 1) % 10 == 0 && (m_heroData->getVocation() < 4 || (m_heroData->getLevel() + 1) / 10 == m_heroData->getChangeCount() + 1))
		{
			HeroAttrLayer* attlay = (HeroAttrLayer*)this->getParent();
			if (attlay!=NULL)
			{
				attlay->changeButton();
				MovingLabel::show(ResourceLang::map_lang["changebreak"]);
				return;
			}
		}

		std::string str = StringUtils::format("s00%d", tag);
		if (MyRes::getMyResCount(str) >= 1)
		{
			MyRes::Use(str);
			DynamicValueInt dal;
			dal.setValue(m_heroData->getExp().getValue() + count);
			m_heroData->setExp(dal);
			std::string s = StringUtils::format(ResourceLang::map_lang["winexp"].c_str(), count);
			MovingLabel::show(s, Color4B(0, 128, 0, 255), Vec2(360,320));
		}
		else
		{
			MovingLabel::show(ResourceLang::map_lang["reslack"]);
		}
		str = StringUtils::format("%d", MyRes::getMyResCount(str));
		goodarr[tag]->setString(str);
	}
}

void HeroLvupLayer::updataAtrrUI(float dt)
{
	int hp = m_heroData->getHp();
	std::string attrstr = StringUtils::format("%d/%d", hp, (int)m_heroData->getMaxHp());
	hplbl->setString(attrstr);

	//攻击值
	attrstr = StringUtils::format("%d", (int)m_heroData->getAtk());
	atkbl->setString(attrstr);

	//防御值
	attrstr = StringUtils::format("%d", (int)m_heroData->getDf());
	dflbl->setString(attrstr);

	//攻击速度值
	attrstr = StringUtils::format("%.3f", m_heroData->getAtkSpeed());
	atkspeedlbl->setString(attrstr);

	//暴击值
	attrstr = StringUtils::format("%.3f%%", m_heroData->getCrit());
	critlbl->setString(attrstr);

	//闪避值
	attrstr = StringUtils::format("%.3f%%", m_heroData->getDodge());
	dodgelbl->setString(attrstr);

	//等级
	std::string str = StringUtils::format("Lv.%d", m_heroData->getLevel() + 1);
	lvUIText->setString(str);

	int curlvexp = 0;
	int nextlvexp = 0;
	int expsize = GlobalInstance::vec_herosAttr[m_heroData->getVocation()].vec_exp.size();

	if (m_heroData->getLevel() >= expsize)
		nextlvexp = GlobalInstance::vec_herosAttr[m_heroData->getVocation()].vec_exp[expsize - 1];
	else
		nextlvexp = GlobalInstance::vec_herosAttr[m_heroData->getVocation()].vec_exp[m_heroData->getLevel()];

	if (m_heroData->getLevel() > 0)
		curlvexp = GlobalInstance::vec_herosAttr[m_heroData->getVocation()].vec_exp[m_heroData->getLevel() - 1];

	str = StringUtils::format("%d/%d", m_heroData->getExp().getValue() - curlvexp, nextlvexp - curlvexp);
	explbl->setString(str);

	float percent = (m_heroData->getExp().getValue() - curlvexp)*100.0f / (nextlvexp - curlvexp);
	expbar->setPercent(percent);

	for (int i = 1; i < 4; i++)
	{
		str = StringUtils::format("s00%d", i);
		str = StringUtils::format("%d", MyRes::getMyResCount(str));
		goodarr[i]->setString(str);
	}
}

void HeroLvupLayer::onExit()
{
	Layer::onExit();
}
