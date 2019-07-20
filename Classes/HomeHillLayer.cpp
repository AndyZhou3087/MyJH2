#include "HomeHillLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "ResCreator.h"
#include "HillResNode.h"
#include "Const.h"
#include "ConsumeResActionLayer.h"
#include "MovingLabel.h"
#include "AnimationEffect.h"
#include "NewGuideLayer.h"
#include "MainScene.h"

USING_NS_CC;

HomeHillLayer::HomeHillLayer()
{
	m_hillResNode = NULL;
}

HomeHillLayer::~HomeHillLayer()
{
	
}


HomeHillLayer* HomeHillLayer::create(Building* buidingData)
{
	HomeHillLayer *pRet = new(std::nothrow)HomeHillLayer();
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
bool HomeHillLayer::init(Building* buidingData)
{
    if ( !Layer::init() )
    {
        return false;
    }

	m_buidingData = buidingData;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	csbnode = CSLoader::createNode(ResourcePath::makePath("homeHillLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	//空闲工人数
	cocos2d::ui::Text* leftfarmertext = (cocos2d::ui::Text*)csbnode->getChildByName("leftfarmertext");
	leftfarmertext->setString(ResourceLang::map_lang["leftfarmertext"]);

	m_leftfarmercount = (cocos2d::ui::Text*)csbnode->getChildByName("leftfarmerlbl");

	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("homehilltitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//等级
	lvUIlbl = (cocos2d::ui::Text*)csbnode->getChildByName("lv");
	std::string str = StringUtils::format("%d%s", buidingData->level.getValue() + 1, ResourceLang::map_lang["lvtext"].c_str());
	lvUIlbl->setString(str);

	//招募按钮
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->setTag(1000);
	actionbtn->addTouchEventListener(CC_CALLBACK_2(HomeHillLayer::onBtnClick, this));

	//招募按钮文字
	cocos2d::ui::ImageView* recruitbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	recruitbtntxt->loadTexture(ResourcePath::makeTextImgPath("hillrecruit", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//升级按钮
	cocos2d::ui::Widget* lvUpbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("lvupbtn");
	lvUpbtn->setTag(1001);
	lvUpbtn->addTouchEventListener(CC_CALLBACK_2(HomeHillLayer::onBtnClick, this));

	//升级按钮文字
	cocos2d::ui::ImageView* lvUpbtntxt = (cocos2d::ui::ImageView*)lvUpbtn->getChildByName("text");
	lvUpbtntxt->loadTexture(ResourcePath::makeTextImgPath("hilllv_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//关闭按钮
	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1002);
	closebtn->addTouchEventListener(CC_CALLBACK_2(HomeHillLayer::onBtnClick, this));

	cocos2d::ui::Text* hintdesc = (cocos2d::ui::Text*)csbnode->getChildByName("hintdesc");
	hintdesc->setString(ResourceLang::map_lang["homehillhintdesc"]);

	//资源滚动控件
	m_contentscroll = (cocos2d::ui::ScrollView*)csbnode->getChildByName("contentscroll");

	m_contentscroll->setScrollBarEnabled(false);
	m_contentscroll->setBounceEnabled(true);

	//倒计时
	m_timebar = (cocos2d::ui::LoadingBar*)csbnode->getChildByName("timebar");
	m_timebar->setPercent(100);

	m_timelbl = (cocos2d::ui::Text*)csbnode->getChildByName("timelbl");

	refreshResUi();

	updateTime(0);
	this->schedule(schedule_selector(HomeHillLayer::updateTime), 1.0f);

	this->scheduleOnce(schedule_selector(HomeHillLayer::delayShowNewerGuide), newguidetime);

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

void HomeHillLayer::delayShowNewerGuide(float dt)
{
	/*if (GlobalInstance::getInstance()->getUnlockHomehillCondition() && !NewGuideLayer::checkifNewerGuide(15))
	{*/
	if (!NewGuideLayer::checkifNewerGuide(15))
	{
		if (NewGuideLayer::checkifNewerGuide(16))
		{
			showNewerGuide(16);
		}
		else if (NewGuideLayer::checkifNewerGuide(18))
		{
			showNewerGuide(18);
		}
		else if (NewGuideLayer::checkifNewerGuide(20))
		{
			showNewerGuide(20);
		}
		else if (NewGuideLayer::checkifNewerGuide(21))
		{
			showNewerGuide(21);
		}
		else if (NewGuideLayer::checkifNewerGuide(82))
		{
			showNewerGuide(82);
		}
		else if (NewGuideLayer::checkifNewerGuide(83))
		{
			showNewerGuide(83);
		}
		else if (NewGuideLayer::checkifNewerGuide(84))
		{
			showNewerGuide(84);
		}
		//else if (NewGuideLayer::checkifNewerGuide(85))
		//{
		//	showNewerGuide(85);
		//}
	}
}

void HomeHillLayer::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	if (step == 16)
	{
		nodes.push_back(csbnode->getChildByName("lvupbtn"));
	}
	else if (step == 18)
	{
		nodes.push_back(csbnode->getChildByName("actionbtn"));
	}
	else if (step == 20 || step == 21 || step == 82 || step == 83 || step == 84)
	{
		if (m_hillResNode != NULL)
		{
			nodes.push_back(m_hillResNode->getChildByName("csbnode")->getChildByName("addbtn"));
		}
	}
	//else if (step == 85)
	//{
	//	nodes.push_back(csbnode->getChildByName("closebtn"));
	//}
	g_mainScene->showNewerGuideNode(step, nodes);
}

void HomeHillLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
		switch (tag)
		{
		case 1000://招募
		{
			ConsumeResActionLayer* layer = ConsumeResActionLayer::create(NULL, CA_EMPLOYFARMER);
			this->addChild(layer);
			AnimationEffect::openAniEffect((Layer*)layer);
			break;
		}
		case 1001://升级
		{
			//等级是否最大
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
		}
		case 1002://关闭
			AnimationEffect::closeAniEffect(this);
			break;
		default:
			break;
		}
	}
}

void HomeHillLayer::refreshResUi()
{
	m_contentscroll->removeAllChildrenWithCleanup(true);

	int size = GlobalInstance::vec_resCreators.size();
	int itemheight = 190;
	int innerheight = itemheight * size;
	int contentheight = m_contentscroll->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	m_contentscroll->setInnerContainerSize(Size(650, innerheight));

	for (unsigned int i = 0; i < GlobalInstance::vec_resCreators.size(); i++)
	{
		ResCreator* data = GlobalInstance::vec_resCreators[i];
		if (data->getLv().getValue() >= 0)
		{
			HillResNode* resnode = HillResNode::create(data);

			if (i < 8)
			{
				resnode->setPosition(Vec2(m_contentscroll->getContentSize().width + 600, innerheight - i * itemheight - itemheight / 2));
				resnode->runAction(EaseSineIn::create(MoveBy::create(0.15f + i * 0.07f, Vec2(-m_contentscroll->getContentSize().width / 2 - 600, 0))));
			}
			else
				resnode->setPosition(Vec2(m_contentscroll->getContentSize().width / 2, innerheight - i * itemheight - itemheight / 2));
			m_contentscroll->addChild(resnode, 0, i);

			//做引导处理
			if (i == 0)
			{
				m_hillResNode = resnode;
			}
		}
	}
}

void HomeHillLayer::lvup()
{
	std::string str = StringUtils::format("%d%s", m_buidingData->level.getValue() + 1, ResourceLang::map_lang["lvtext"].c_str());
	lvUIlbl->setString(str);

	std::string cid = StringUtils::format("r%03d", m_buidingData->level.getValue() + 1);
	DynamicValueInt dv;
	dv.setValue(0);
	GlobalInstance::vec_resCreators[m_buidingData->level.getValue()]->setLv(dv);
	GlobalInstance::getInstance()->saveResCreatorData();
	refreshResUi();
}

void HomeHillLayer::updateTime(float dt)
{
	int pastime = GlobalInstance::servertime - GlobalInstance::getInstance()->getRefreshResTime();
	int lefttime = RES_REFRESHTIME - pastime;
	std::string timestr = StringUtils::format("%02d:%02d:%02d", lefttime / 3600, lefttime % 3600 / 60, lefttime % 3600 % 60);
	m_timelbl->setString(timestr);
	m_timebar->setPercent(lefttime * 100 / RES_REFRESHTIME);
	std::string str = StringUtils::format("%d/%d", GlobalInstance::getInstance()->getTotalFarmers() - GlobalInstance::getInstance()->getWorkingFarmerCount(), GlobalInstance::getInstance()->getTotalFarmers());
	m_leftfarmercount->setString(str);
}

void HomeHillLayer::onExit()
{
	Layer::onExit();
}
