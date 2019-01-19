#include "RandHeroNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "HeroAttrLayer.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "SoundManager.h"
#include "GlobalInstance.h"

RandHeroNode::RandHeroNode()
{

}


RandHeroNode::~RandHeroNode()
{

}

RandHeroNode* RandHeroNode::create()
{
	RandHeroNode *pRet = new(std::nothrow)RandHeroNode();
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

bool RandHeroNode::init()
{
	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("randHeroNode.csb"));
	this->addChild(csbnode, 0, "csbnode");
	bgitem = (cocos2d::ui::Widget*)csbnode->getChildByName("randheadbox");
	bgitem->addTouchEventListener(CC_CALLBACK_2(RandHeroNode::onClick, this));
	bgitem->setEnabled(false);

	//头像框
	headbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("hbox");
	
	//头像
	headimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("headimg");

	//名字
	namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");

	//职业
	vocationtextlbl = (cocos2d::ui::Text*)csbnode->getChildByName("vocationtext");
	vocationtextlbl->setString(ResourceLang::map_lang["vocationtext"]);

	vocationlbl = (cocos2d::ui::Text*)csbnode->getChildByName("vocation");

	//品质
	potentialtextlbl = (cocos2d::ui::Text*)csbnode->getChildByName("potentialtext");
	potentialtextlbl->setString(ResourceLang::map_lang["potentialtext"]);

	potentiallbl = (cocos2d::ui::Text*)csbnode->getChildByName("potential");

	isrecruitedWidget = (cocos2d::ui::Text*)csbnode->getChildByName("isrecruited_text");
	markRecruited(false);

	return true;
}

void RandHeroNode::onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		Layer* layer = HeroAttrLayer::create((Hero*)this->getUserData());
		this->getParent()->addChild(layer, 0, this->getTag());
		AnimationEffect::openAniEffect((Layer*)layer);
	}
}

void RandHeroNode::setData(Hero* herodata, bool isRandAnim)
{
	m_herodata = herodata;
	if (!isRandAnim)
	{
		std::string str = StringUtils::format("ui/h_%d_%d.png", herodata->getVocation(), herodata->getSex());
		headimg->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

		str = StringUtils::format("ui/herobox_%d.png", herodata->getPotential());
		headbox->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

		str = StringUtils::format("vocation_%d", herodata->getVocation());
		vocationlbl->setString(ResourceLang::map_lang[str]);

		str = StringUtils::format("potential_%d", herodata->getPotential());
		potentiallbl->setString(ResourceLang::map_lang[str]);

		potentialtextlbl->setColor(Color3B(POTENTIALCOLOR[herodata->getPotential()]));
		potentiallbl->setColor(Color3B(POTENTIALCOLOR[herodata->getPotential()]));

		namelbl->setString(herodata->getName());
		bgitem->setEnabled(true);
	}
	else
	{
		bgitem->setEnabled(false);
		namelbl->setString("");
		this->schedule(schedule_selector(RandHeroNode::refreshAnim), 0.2f, 7, 0.0f);
		this->scheduleOnce(schedule_selector(RandHeroNode::finishAnim), 1.8f);
	}

	if (herodata->getState() == HS_OWNED)
		markRecruited(true);
	else if (herodata->getState() == HS_READY)
		markRecruited(false);

	this->setUserData((void*)herodata);
}

void RandHeroNode::markRecruited(bool ismask)
{
	isrecruitedWidget->setVisible(ismask);
}

void RandHeroNode::refreshAnim(float dt)
{
	int v = GlobalInstance::getInstance()->createRandomNum(BASEHEROMAX);
	std::string str = StringUtils::format("ui/h_%d_%d.png", v, 0);
	headimg->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

	int p = GlobalInstance::getInstance()->createRandomNum(5);
	str = StringUtils::format("ui/herobox_%d.png", p);
	headbox->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

	str = StringUtils::format("vocation_%d", v);
	vocationlbl->setString(ResourceLang::map_lang[str]);

	str = StringUtils::format("potential_%d", p);
	potentiallbl->setString(ResourceLang::map_lang[str]);

	potentialtextlbl->setColor(Color3B(POTENTIALCOLOR[p]));
	potentiallbl->setColor(Color3B(POTENTIALCOLOR[p]));

}

void RandHeroNode::finishAnim(float dt)
{
	std::string str = StringUtils::format("ui/h_%d_%d.png", m_herodata->getVocation(), m_herodata->getSex());
	headimg->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

	str = StringUtils::format("ui/herobox_%d.png", m_herodata->getPotential());
	headbox->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

	str = StringUtils::format("vocation_%d", m_herodata->getVocation());
	vocationlbl->setString(ResourceLang::map_lang[str]);

	str = StringUtils::format("potential_%d", m_herodata->getPotential());
	potentiallbl->setString(ResourceLang::map_lang[str]);

	potentialtextlbl->setColor(Color3B(POTENTIALCOLOR[m_herodata->getPotential()]));
	potentiallbl->setColor(Color3B(POTENTIALCOLOR[m_herodata->getPotential()]));

	namelbl->setString(m_herodata->getName());
	bgitem->setEnabled(true);
}