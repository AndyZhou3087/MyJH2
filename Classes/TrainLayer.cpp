#include "TrainLayer.h"
#include "Resource.h"
#include "Const.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "ConsumeResActionLayer.h"
#include "MovingLabel.h"
#include "DataSave.h"
#include "Hero.h"
#include "MyHeroNode.h"
#include "AnimationEffect.h"
#include "RepairBuildingLayer.h"

USING_NS_CC;

TrainLayer::TrainLayer()
{

}

TrainLayer::~TrainLayer()
{
	GlobalInstance::getInstance()->saveMyHeros();
}


TrainLayer* TrainLayer::create(Building* buidingData)
{
	TrainLayer *pRet = new(std::nothrow)TrainLayer();
	if (pRet && pRet->init(buidingData))
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
bool TrainLayer::init(Building* buidingData)
{
	if (!Layer::init())
	{
		return false;
	}

	if (g_mainScene != NULL)
	{
		if (g_mainScene->traintip != NULL)
		{
			g_mainScene->traintip->setVisible(false);
		}
	}

	m_buidingData = buidingData;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("trainLayer.csb"));
	this->addChild(csbnode);

	langtype = GlobalInstance::getInstance()->getLang();
	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("train_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//等级
	lvUIlbl = (cocos2d::ui::Text*)csbnode->getChildByName("lv");
	std::string str = StringUtils::format("%d%s", buidingData->level.getValue() + 1, ResourceLang::map_lang["lvtext"].c_str());
	lvUIlbl->setString(str);

	//升级按钮
	cocos2d::ui::Widget* lvUpbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("lvupbtn");
	lvUpbtn->setTag(1000);
	lvUpbtn->addTouchEventListener(CC_CALLBACK_2(TrainLayer::onBtnClick, this));

	//升级按钮文字
	cocos2d::ui::ImageView* lvUpbtntxt = (cocos2d::ui::ImageView*)lvUpbtn->getChildByName("text");
	lvUpbtntxt->loadTexture(ResourcePath::makeTextImgPath("trainlv_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//关闭按钮
	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1001);
	closebtn->addTouchEventListener(CC_CALLBACK_2(TrainLayer::onBtnClick, this));

	//滚动控件
	m_contentscroll = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollview");
	m_contentscroll->setScrollBarEnabled(false);
	m_contentscroll->setBounceEnabled(true);

	loadData();
	updateContent();

	repairbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("repairbtn");
	repairbtn->setTag(2000);
	repairbtn->addTouchEventListener(CC_CALLBACK_2(TrainLayer::onBtnClick, this));

	repairbtn->runAction(RepeatForever::create(Sequence::create(RotateTo::create(0.1f, 10), RotateTo::create(0.1f, 0), RotateTo::create(0.1f, -10), RotateTo::create(0.1f, 0), DelayTime::create(0.5f), NULL)));

	repairtimelbl = (cocos2d::ui::Text*)repairbtn->getChildByName("time");
	repairtimelbl->setString("");

	updateRepairTime(0);
	this->schedule(schedule_selector(TrainLayer::updateRepairTime), 1.0f);

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

void TrainLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
		switch (tag)
		{
		case 1000://升级
			if (m_buidingData->level.getValue() < m_buidingData->maxlevel.getValue() - 1)
			{
				ConsumeResActionLayer* layer = ConsumeResActionLayer::create(m_buidingData, CA_BUILDINGLVUP);
				this->addChild(layer);
				AnimationEffect::openAniEffect((Layer*)layer);
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["maxlv"]);
			}
			break;
		case 1001://关闭
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		case 2000:
		{
			RepairBuildingLayer* layer = RepairBuildingLayer::create(m_buidingData->name);
			addChild(layer);
		}
		break;
		default:
			break;
		}
	}
}

void TrainLayer::loadData()
{
	
}

void TrainLayer::updateContent()
{
	m_contentscroll->removeAllChildrenWithCleanup(true);

	int itemheight = 170;

	int size = GlobalInstance::vec_myHeros.size();


	std::vector<Hero*> vec_trainheros;
	for (int i = 0; i < size; i++)
	{
		Hero* hero = GlobalInstance::vec_myHeros[i];
		if (hero->getState()!=HS_DEAD)
		{
			vec_trainheros.push_back(hero);
		}
	}

	int innerheight = itemheight * vec_trainheros.size();
	int contentheight = m_contentscroll->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	m_contentscroll->setInnerContainerSize(Size(650, innerheight));

	for (unsigned int i = 0; i < vec_trainheros.size(); i++)
	{
		Hero* hero = vec_trainheros[i];
		MyHeroNode* node = MyHeroNode::create(hero, HS_TRAINING);

		node->setPosition(Vec2(m_contentscroll->getContentSize().width + 600, innerheight - i * itemheight - itemheight / 2));
		node->runAction(EaseSineIn::create(MoveBy::create(0.15f + i*0.07f, Vec2(-m_contentscroll->getContentSize().width / 2 - 600, 0))));

		m_contentscroll->addChild(node, 0, i);

	}
}

void TrainLayer::lvup()
{
	std::string str = StringUtils::format("%d%s", m_buidingData->level.getValue() + 1, ResourceLang::map_lang["lvtext"].c_str());
	lvUIlbl->setString(str);

	loadData();
	updateContent();
}

void TrainLayer::updateRepairTime(float dt)
{
	updateRepairUi();
}

void TrainLayer::updateRepairUi()
{
	int repairstate = GlobalInstance::map_buildingrepairdata["4trainigroom"].state;
	if (repairstate > 0)
	{
		if (repairstate == 3)
		{
			int pasttime = GlobalInstance::servertime - GlobalInstance::map_buildingrepairdata["4trainigroom"].repairtime;

			if (pasttime >= REPAIRTIME)
			{
				repairbtn->setVisible(false);
			}
			else
			{
				repairtimelbl->setVisible(true);
				int lefttime = REPAIRTIME - pasttime;
				std::string strlbl = StringUtils::format("%02d:%02d", lefttime / 60, lefttime % 60);
				repairtimelbl->setString(strlbl);
				repairbtn->stopAllActions();
				repairbtn->setRotation(0);
			}
		}
		else
		{
			repairtimelbl->setVisible(false);
		}
	}
	else
	{
		repairbtn->setVisible(false);
	}
}