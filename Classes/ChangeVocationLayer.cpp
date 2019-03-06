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
#include "SimpleResPopLayer.h"

USING_NS_CC;

ChangeVocationLayer::ChangeVocationLayer()
{
	for (int i = 0; i < 2; i++)
	{
		count[i] = NULL;
	}
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
	this->addChild(color, 0, "colorLayer");

	csbnode = CSLoader::createNode(ResourcePath::makePath("changeVocationLayer.csb"));
	this->addChild(csbnode);

	cocos2d::ui::ImageView* bg = (cocos2d::ui::ImageView*)csbnode->getChildByName("bg");

	int langtype = GlobalInstance::getInstance()->getLang();

	int c = (herodata->getLevel() + 1) / 10;
	needresid = StringUtils::format("d%03d", c);

	//关闭按钮
	cocos2d::ui::Button* closebtn = (cocos2d::ui::Button*)csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(ChangeVocationLayer::onBtnClick, this));
	closebtn->setTag(1000);

	Node* vnode[2];
	for (int i = 0; i < 2; i++)
	{
		std::string str = StringUtils::format("v%dnode", i+1);
		vnode[i] = csbnode->getChildByName(str);
	}

	int index = 1;
	if (forwhere == 0)
	{
		index = 2;
		vnode[0]->setPositionY(895);
		bg->setContentSize(Size(bg->getContentSize().width, 1130));
		closebtn->setPositionY(1160);
	}
	else
	{
		//vnode[0]->setPositionY(700);
		//vnode[1]->setVisible(false);
		//bg->setContentSize(Size(bg->getContentSize().width, 680));
		//closebtn->setPositionY(935);

		vnode[0]->setPositionY(640);
		vnode[1]->setVisible(false);
		bg->setContentSize(Size(bg->getContentSize().width, 620));
		closebtn->setPositionY(905);
	}
	for (int i = 0; i < index; i++)
	{
		cocos2d::ui::ImageView* vocationicon = (cocos2d::ui::ImageView*)vnode[i]->getChildByName("v");

		std::string vstr;
		int showvoc = 0;
		std::string btnstr;
		if (forwhere == 0)
		{
			showvoc = herodata->getVocation() + 4 * (i + 1);
			btnstr = StringUtils::format("changevocbtn_text%d", showvoc);
		}
		else
		{
			showvoc = herodata->getVocation();
			btnstr = "changevocbtnbreak_text";
		}
		vstr = StringUtils::format("ui/voc%dicon.png", showvoc);

		vocationicon->loadTexture(vstr, cocos2d::ui::Widget::TextureResType::PLIST);

		cocos2d::ui::Text* desc = (cocos2d::ui::Text*)vnode[i]->getChildByName("desc");
		std::string hstr = StringUtils::format("h%03d", showvoc + 1);
		desc->setString(GlobalInstance::map_AllResources[hstr].desc);

		for (int m = 0; m < 6; m++)
		{
			std::string attrtextname = StringUtils::format("attrtext_%d", m);
			cocos2d::ui::ImageView* attrtex = (cocos2d::ui::ImageView*)vnode[i]->getChildByName(attrtextname);

			attrtextname = StringUtils::format("attr%d_text", m);
			attrtex->loadTexture(ResourcePath::makeTextImgPath(attrtextname, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		}
		//if (forwhere == 0)
		{
			for (int m = 0; m < 6; m++)
			{
				for (int n = 0; n < 6; n++)
				{
					std::string attrname = StringUtils::format("attrvalblock%d_%d", m, n);
					Node* attrnode = vnode[i]->getChildByName(attrname);
					if (n + 1 > GlobalInstance::GlobalInstance::vec_herosAttr[showvoc].vec_attrbias[m])
					{
						attrnode->setVisible(false);
					}
				}
			}
		}

			
		cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)vnode[i]->getChildByName("resbox")->getChildByName("res");
		std::string respath = StringUtils::format("ui/%s.png", needresid.c_str());
		res->loadTexture(respath, cocos2d::ui::Widget::TextureResType::PLIST);
		res->addTouchEventListener(CC_CALLBACK_2(ChangeVocationLayer::onImgClick, this));

		cocos2d::ui::ImageView* changevocneed_text = (cocos2d::ui::ImageView*)vnode[i]->getChildByName("resbox")->getChildByName("changevocneed_text");
		changevocneed_text->loadTexture(ResourcePath::makeTextImgPath("changevocneed_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

		count[i] = (cocos2d::ui::Text*)vnode[i]->getChildByName("resbox")->getChildByName("count");

		cocos2d::ui::Button* actionbtn = (cocos2d::ui::Button*)vnode[i]->getChildByName("actionbtn");
		actionbtn->addTouchEventListener(CC_CALLBACK_2(ChangeVocationLayer::onBtnClick, this));
		actionbtn->setTag(1000 + i + 1);

		cocos2d::ui::ImageView* actionbtntext = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
		actionbtntext->ignoreContentAdaptWithSize(true);
		actionbtntext->loadTexture(ResourcePath::makeTextImgPath(btnstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

		//if (forwhere == 1)
		//{
		//	changevocneed_text->setPosition(Vec2(-210, -290));
		//	actionbtn->setPositionY(-290);
		//}
	}

	updateResCount(0);
	this->schedule(schedule_selector(ChangeVocationLayer::updateResCount), 1.0f);


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
			std::string str = StringUtils::format("v%dnode", i+1);
			nodes.push_back(csbnode->getChildByName(str)->getChildByName("actionbtn"));
		}
	}
	g_mainScene->showNewerGuideNode(step, nodes);
}

void ChangeVocationLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
		if (tag == 1000)
		{
			AnimationEffect::closeAniEffect(this);
		}
		else
		{
			if (m_forwhere == 0)//转职
			{
				if (MyRes::getMyResCount(needresid) < 1)
				{
					MovingLabel::show(ResourceLang::map_lang["reslack"]);

					if (NewGuideLayer::checkifNewerGuide(66))
						return;

					SimpleResPopLayer* layer = SimpleResPopLayer::create(needresid, 1, 1);
					this->addChild(layer);
					AnimationEffect::openAniEffect(layer);
				}
				else
				{
					MyRes::Use(needresid);
					SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BREAKUPSUCC);
					m_herodata->setChangeCount(m_herodata->getChangeCount() + 1);
					m_herodata->setVocation(m_herodata->getVocation() + tag%1000 * 4);
					GlobalInstance::getInstance()->saveHero(m_herodata);
					HeroAttrLayer* attlay = (HeroAttrLayer*)this->getParent();
					if (attlay != NULL)
					{
						attlay->changeHeroVocImg();
					}
					CommonFuncs::playCommonLvUpAnim(this->getParent(), "texiao_zzcg");
					AnimationEffect::closeAniEffect(this);
				}
			}
			else//突破
			{
				if (MyRes::getMyResCount(needresid) < 1)
				{
					MovingLabel::show(ResourceLang::map_lang["reslack"]);
					SimpleResPopLayer* layer = SimpleResPopLayer::create(needresid, 1, 1);
					this->addChild(layer);
					AnimationEffect::openAniEffect(layer);
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
					AnimationEffect::closeAniEffect(this);
				}
			}
		}
	}
}

void ChangeVocationLayer::onImgClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (NewGuideLayer::checkifNewerGuide(66))
			return;

		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		SimpleResPopLayer* layer = SimpleResPopLayer::create(needresid, 1, 1);
		this->addChild(layer);
		AnimationEffect::openAniEffect(layer);
	}
}

void ChangeVocationLayer::updateResCount(float dt)
{
	std::string str = StringUtils::format("%d/1", MyRes::getMyResCount(needresid));

	for (int i = 0; i < 2; i++)
	{
		if (count[i] != NULL)
		{
			count[i]->setString(str);

			if (MyRes::getMyResCount(needresid) >= 1)
			{
				count[i]->setColor(Color3B(255, 255, 255));
			}
			else
			{
				count[i]->setColor(Color3B(255, 0, 0));
			}
		}
	}
}

void ChangeVocationLayer::onExit()
{
	Layer::onExit();
}


