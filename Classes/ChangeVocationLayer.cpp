#include "ChangeVocationLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "DataSave.h"
#include "ResCreator.h"
#include "HeroAttrLayer.h"
#include "AnimationEffect.h"
#include "SoundManager.h"
#include "NewGuideLayer.h"
#include "MainScene.h"

USING_NS_CC;

ChangeVocationLayer::ChangeVocationLayer()
{

}

ChangeVocationLayer::~ChangeVocationLayer()
{

}


ChangeVocationLayer* ChangeVocationLayer::create(Hero* herodata, int forwhere)
{
	ChangeVocationLayer *pRet = new(std::nothrow)ChangeVocationLayer();
	if (pRet && pRet->init(herodata, forwhere))
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
bool ChangeVocationLayer::init(Hero* herodata, int forwhere)
{
	if (!Layer::init())
	{
		return false;
	}

	m_herodata = herodata;
	m_forwhere = forwhere;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	csbnode = CSLoader::createNode(ResourcePath::makePath("changeVocationLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	int c = (herodata->getLevel() + 1) / 10;
	needresid = StringUtils::format("d%03d", c);

	//name
	cocos2d::ui::Text* name = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	name->setString(herodata->getName());

	//按钮1
	cocos2d::ui::Button* accbtn1 = (cocos2d::ui::Button*)csbnode->getChildByName("accbtn1");
	accbtn1->addTouchEventListener(CC_CALLBACK_2(ChangeVocationLayer::onBtnClick, this));
	//按钮2
	cocos2d::ui::Button* accbtn2 = (cocos2d::ui::Button*)csbnode->getChildByName("accbtn2");
	accbtn2->addTouchEventListener(CC_CALLBACK_2(ChangeVocationLayer::onBtnClick, this));

	//按钮3
	cocos2d::ui::Button* accbtn3 = (cocos2d::ui::Button*)csbnode->getChildByName("accbtn3");
	accbtn3->setTag(0);
	accbtn3->addTouchEventListener(CC_CALLBACK_2(ChangeVocationLayer::onBtnClick, this));
	accbtn3->setTitleText(ResourceLang::map_lang["changeclosetext"]);

	cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox")->getChildByName("res");
	std::string respath = StringUtils::format("ui/%s.png", needresid.c_str());
	res->loadTexture(respath, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* rewardlabel = (cocos2d::ui::Text*)csbnode->getChildByName("rewardlabel");

	cocos2d::ui::Text* content = (cocos2d::ui::Text*)csbnode->getChildByName("content");

	cocos2d::ui::Text* count_0 = (cocos2d::ui::Text*)csbnode->getChildByName("count_0");
	std::string str = StringUtils::format("%d/1", MyRes::getMyResCount(needresid));
	count_0->setString(str);

	if (MyRes::getMyResCount(needresid) >= 1)
	{
		count_0->setColor(Color3B(255, 255, 255));
	}
	else
	{
		count_0->setColor(Color3B(255, 0, 0));
	}
	
	if (forwhere == 0)
	{
		int a;
		int b;
		S_HeroAttr aatt;
		S_HeroAttr batt;
		for (unsigned int i = 0; i < GlobalInstance::vec_herosAttr.size(); i++)
		{
			if (i == herodata->getVocation() + 4)
			{
				a = i;
				aatt = GlobalInstance::vec_herosAttr[i];
			}
			if (i == herodata->getVocation() + 8)
			{
				b = i;
				batt = GlobalInstance::vec_herosAttr[i];
			}
		}
		str = StringUtils::format(ResourceLang::map_lang["changetext1"].c_str(), GlobalInstance::map_AllResources[aatt.id].name.c_str());
		accbtn1->setTitleText(str);
		accbtn1->setTag(a);

		str = StringUtils::format(ResourceLang::map_lang["changetext1"].c_str(), GlobalInstance::map_AllResources[batt.id].name.c_str());
		accbtn2->setTitleText(str);
		accbtn2->setTag(b);
		
		str = StringUtils::format("%s:%s\n\n%s:%s", GlobalInstance::map_AllResources[aatt.id].name.c_str(), GlobalInstance::map_AllResources[aatt.id].desc.c_str(), GlobalInstance::map_AllResources[batt.id].name.c_str(), GlobalInstance::map_AllResources[batt.id].desc.c_str());
		content->setString(str);
		rewardlabel->setString(ResourceLang::map_lang["changecondition"]);
	}
	else
	{
		content->setString(ResourceLang::map_lang["changesuretext"]);
		rewardlabel->setString(ResourceLang::map_lang["breakcondition"]);

		accbtn1->setTitleText(ResourceLang::map_lang["breaktext"]);
		accbtn1->setTag(1);
		accbtn2->setTitleText(ResourceLang::map_lang["changeclosetext"]);
		accbtn2->setTag(0);

		accbtn3->setVisible(false);
	}

	this->scheduleOnce(schedule_selector(ChangeVocationLayer::delayShowNewerGuide), newguidetime);

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void ChangeVocationLayer::delayShowNewerGuide(float dt)
{
	if (GlobalInstance::getInstance()->getHerosChangeLevelCount() > 0 && !NewGuideLayer::checkifNewerGuide(67))
	{
		if (NewGuideLayer::checkifNewerGuide(72))
		{
			showNewerGuide(72);
		}
	}
}

void ChangeVocationLayer::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	if (step == 72)
	{
		for (int i = 0; i < 2; i++)
		{
			std::string str = StringUtils::format("accbtn%d", i + 1);
			nodes.push_back(csbnode->getChildByName(str));
		}
	}
	g_mainScene->showNewerGuideNode(step, nodes);
}

void ChangeVocationLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
	int tag = clicknode->getTag();
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (m_forwhere == 0)
		{
			if (tag != 0)
			{
				if (MyRes::getMyResCount(needresid) < 1)
				{
					MovingLabel::show(ResourceLang::map_lang["reslack"]);
					return;
				}
				else
				{
					MyRes::Use(needresid);
				}
			}
			if (tag == m_herodata->getVocation() + 4 || tag == m_herodata->getVocation() + 8)
			{
				SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BREAKUPSUCC);
				m_herodata->setChangeCount(m_herodata->getChangeCount() + 1);
				m_herodata->setVocation(tag);
				GlobalInstance::getInstance()->saveHero(m_herodata);
				HeroAttrLayer* attlay = (HeroAttrLayer*)this->getParent();
				if (attlay != NULL)
				{
					attlay->changeButton();
				}
				CommonFuncs::playCommonLvUpAnim(this->getParent(), "texiao_zzcg");
			}
			AnimationEffect::closeAniEffect((Layer*)this);
		}
		else
		{
			if (tag != 0)
			{
				if (MyRes::getMyResCount(needresid) < 1)
				{
					MovingLabel::show(ResourceLang::map_lang["reslack"]);
					return;
				}
				else
				{
					MyRes::Use(needresid);
					m_herodata->setChangeCount(m_herodata->getChangeCount() + 1);
					GlobalInstance::getInstance()->saveHero(m_herodata);
					HeroAttrLayer* attlay = (HeroAttrLayer*)this->getParent();
					if (attlay != NULL)
					{
						attlay->changeButton();
					}
					CommonFuncs::playCommonLvUpAnim(this->getParent(), "texiao_tpcg");
				}
			}
			AnimationEffect::closeAniEffect((Layer*)this);
		}
	}
}

void ChangeVocationLayer::onExit()
{
	Layer::onExit();
}


