#include "CardHeroNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "SelectMyHerosLayer.h"
#include "AnimationEffect.h"
#include "SoundManager.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MovingLabel.h"
#include "HeroAttrLayer.h"

CardHeroNode::CardHeroNode()
{
	m_isdraging = false;
}


CardHeroNode::~CardHeroNode()
{

}

CardHeroNode* CardHeroNode::create()
{
	CardHeroNode *pRet = new(std::nothrow)CardHeroNode();
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

bool CardHeroNode::init()
{
	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("cardHeroNode.csb"));
	this->addChild(csbnode, 0, "csbnode");

	desclbl = (cocos2d::ui::Text*)csbnode->getChildByName("desc");

	//头像框
	headbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("hbox");
	headbox->addTouchEventListener(CC_CALLBACK_2(CardHeroNode::onClick, this));
	headbox->setSwallowTouches(false);
	//头像
	headimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("head");

	this->setContentSize(headbox->getContentSize());

	cardnamebox = csbnode->getChildByName("cardnamebox");
	//名字
	namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");

	powericon = (cocos2d::ui::ImageView*)csbnode->getChildByName("powericon");
	powericon->loadTexture(ResourcePath::makeTextImgPath("powericon_text", GlobalInstance::getInstance()->getLang()), cocos2d::ui::Widget::TextureResType::PLIST);

	//体力
	powertext = (cocos2d::ui::Text*)csbnode->getChildByName("powertext");

	cocos2d::ui::ImageView* powerclick = (cocos2d::ui::ImageView*)csbnode->getChildByName("powerclick");
	powerclick->addTouchEventListener(CC_CALLBACK_2(CardHeroNode::onPowerClick, this));

	//等级

	lvlbl = (cocos2d::ui::Text*)csbnode->getChildByName("lv");

	vocationbox = (cocos2d::ui::Widget*)csbnode->getChildByName("vocationbox");

	vocationicon = (cocos2d::ui::ImageView*)vocationbox->getChildByName("v");

	for (int i = 1; i <= 5; i++)
	{
		std::string str = StringUtils::format("star%d", i);
		stars[i - 1] = csbnode->getChildByName(str);
		stars[i - 1]->setVisible(false);
	}

	return true;
}

void CardHeroNode::onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (m_isdraging)
		{
			m_isdraging = false;
			return;
		}
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);

		if (m_herodata == NULL)
		{
			Layer* layer = SelectMyHerosLayer::create(HS_TAKEON);
			layer->setTag(this->getTag());
			this->getParent()->addChild(layer, 3, "selectmyheroslayer");
			AnimationEffect::openAniEffect((Layer*)layer);
		}
		else
		{
			Layer* layer = HeroAttrLayer::create(m_herodata, 0);
			this->getParent()->addChild(layer, 3, this->getTag());
			AnimationEffect::openAniEffect((Layer*)layer);
		}
	}
}

void CardHeroNode::onPowerClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		std::string str;
		int powercount = m_herodata->getPower().getValue();
		if (m_herodata->getPower().getValue() < 100)
		{
			int pasttime = GlobalInstance::servertime - m_herodata->getPowerTime();
			int lefttime = HEROPOWER_RESETTIME - pasttime % HEROPOWER_RESETTIME;
			std::string str = StringUtils::format(ResourceLang::map_lang["poweraddtext"].c_str(), lefttime / 60, lefttime % 60);
			MovingLabel::show(str);
		}
	}
}

void CardHeroNode::setData(Hero* herodata)
{
	m_herodata = herodata;

	if (herodata != NULL)
	{
		std::string str = StringUtils::format("cardh_%d_%d.png", herodata->getVocation(), herodata->getSex());
		headimg->loadTexture(ResourcePath::makeImagePath(str), cocos2d::ui::Widget::TextureResType::LOCAL);
		headimg->setVisible(true);

		str = StringUtils::format("cardherobox_%d.png", herodata->getPotential());
		headbox->loadTexture(ResourcePath::makeImagePath(str), cocos2d::ui::Widget::TextureResType::LOCAL);

		powericon->setVisible(true);

		desclbl->setVisible(false);

		cardnamebox->setVisible(true);

		updatePowerCount(0);
		this->schedule(schedule_selector(CardHeroNode::updatePowerCount), 1);

	}
	else
	{
		this->unschedule(schedule_selector(CardHeroNode::updatePowerCount));

		headbox->loadTexture(ResourcePath::makeImagePath("cardherobox_.png"), cocos2d::ui::Widget::TextureResType::LOCAL);
		headimg->setVisible(false);
		lvlbl->setVisible(false);
		namelbl->setVisible(false);

		vocationbox->setVisible(false);
		powertext->setVisible(false);
		powericon->setVisible(false);

		cardnamebox->setVisible(false);

		updateSelPosLbl();
		for (int i = 1; i <= 5; i++)
		{
			stars[i - 1]->setVisible(false);
		}
	}
}

void CardHeroNode::updateSelPosLbl()
{
	desclbl->setVisible(true);
	std::string indexstr = StringUtils::format("selheronum%d", this->getTag() + 1);
	std::string descstr = StringUtils::format(ResourceLang::map_lang["selherohinttext"].c_str(), ResourceLang::map_lang[indexstr].c_str());
	desclbl->setString(descstr);
}

void CardHeroNode::updatePowerCount(float dt)
{
	if (m_herodata != NULL)
	{
		std::string str = StringUtils::format("lv.%d", m_herodata->getLevel() + 1);
		lvlbl->setString(str);
		lvlbl->setVisible(true);

		namelbl->setString(m_herodata->getName());
		namelbl->setVisible(true);

		if (m_herodata->getVocation() >= 4)
		{
			str = StringUtils::format("ui/cardvocation%d.png", m_herodata->getVocation());
			vocationicon->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);
			vocationbox->setVisible(true);
		}
		else
			vocationbox->setVisible(false);

		int startx[] = { 0, -7, -15, -22, -30 };

		for (int i = 0; i < m_herodata->getChangeCount() - 1; i++)
		{
			stars[i]->setVisible(true);
			stars[i]->setPositionX(startx[(m_herodata->getChangeCount() - 2)] + 15 * i);
		}

		int pasttime = GlobalInstance::servertime - m_herodata->getPowerTime();
		int lefttime = HEROPOWER_RESETTIME - pasttime % HEROPOWER_RESETTIME;

		DynamicValueInt dv;
		int count = pasttime / HEROPOWER_RESETTIME;
		if (count > 0)
		{
			m_herodata->setPowerTime(GlobalInstance::servertime);
			if (count + m_herodata->getPower().getValue() > 100)
				count = 100;
			else
				count += m_herodata->getPower().getValue();
			dv.setValue(count);

			m_herodata->setPower(dv);
			GlobalInstance::getInstance()->saveHero(m_herodata);
		}

		std::string str = StringUtils::format("%d/100", m_herodata->getPower().getValue());
		powertext->setString(str);
		powertext->setVisible(true);
		if (m_herodata->getPower().getValue() <= 0)
			powertext->setTextColor(Color4B(Color3B(255,35,35)));
		else
			powertext->setTextColor(Color4B(Color3B(101, 76, 36)));
	}
}
