#include "SelectMyHerosLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "AnimationEffect.h"
#include "NewGuideLayer.h"
#include "MainScene.h"
#include "Const.h"

USING_NS_CC;

SelectMyHerosLayer::SelectMyHerosLayer()
{

}

SelectMyHerosLayer::~SelectMyHerosLayer()
{
	
}


SelectMyHerosLayer* SelectMyHerosLayer::create(int wheretype)
{
	SelectMyHerosLayer *pRet = new(std::nothrow)SelectMyHerosLayer();
	if (pRet && pRet->init(wheretype))
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
bool SelectMyHerosLayer::init(int wheretype)
{
    if ( !Layer::init() )
    {
        return false;
    }

	m_wheretype = wheretype;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("simpleLayer.csb"));
	this->addChild(csbnode, 0, "selct");
	int langtype = GlobalInstance::getInstance()->getLang();
	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->ignoreContentAdaptWithSize(true);

	std::string titlenamstr = "outtowntitle";
	if (wheretype == HS_ONCHALLENGE)
		titlenamstr = "matchfighttitle";

	titleimg->loadTexture(ResourcePath::makeTextImgPath(titlenamstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)csbnode->getChildByName("desc");

	int size = GlobalInstance::vec_myHeros.size();
	if (size > 0)
		desc->setVisible(false);
	else
		desc->setString(ResourceLang::map_lang["heroempty"]);

	//关闭按钮
	closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(SelectMyHerosLayer::onBtnClick, this));

	//我的英雄滚动控件
	m_contentscroll = (cocos2d::ui::ScrollView*)csbnode->getChildByName("contentscroll");

	m_contentscroll->setScrollBarEnabled(false);
	m_contentscroll->setBounceEnabled(true);

	GlobalInstance::getInstance()->heroSortByLv();

	refreshMyHerosUi();

	this->scheduleOnce(schedule_selector(SelectMyHerosLayer::delayShowNewerGuide), newguidetime);

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

void SelectMyHerosLayer::delayShowNewerGuide(float dt)
{
	if (!NewGuideLayer::checkifNewerGuide(45))
	{
		/*if (NewGuideLayer::checkifNewerGuide(FOURTHGUIDESTEP))
		{
			if (NewGuideLayer::checkifNewerGuide(47))
			{
				showNewerGuide(47);
			}
			else if (NewGuideLayer::checkifNewerGuide(49))
			{
				showNewerGuide(49);
			}
			else if (NewGuideLayer::checkifNewerGuide(51))
			{
				showNewerGuide(51);
			}
		}*/
		if (NewGuideLayer::checkifNewerGuide(FOURTHGUIDESTEP))
		{
			if (NewGuideLayer::checkifNewerGuide(47))
			{
				showNewerGuide(47);
			}
			else if (NewGuideLayer::checkifNewerGuide(48))
			{
				showNewerGuide(48);
			}
			else if (NewGuideLayer::checkifNewerGuide(49))
			{
				showNewerGuide(49);
			}
			else if (NewGuideLayer::checkifNewerGuide(50))
			{
				showNewerGuide(50);
			}
		}
	}
}

void SelectMyHerosLayer::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	if (step == 47)
	{
		if (m_contentscroll->getChildByTag(0) != NULL)
			nodes.push_back(m_contentscroll->getChildByTag(0)->getChildByName("csbnode")->getChildByName("actionbtn"));
	}
	else if (step == 48)
	{
		if (m_contentscroll->getChildByTag(1) != NULL)
			nodes.push_back(m_contentscroll->getChildByTag(1)->getChildByName("csbnode")->getChildByName("actionbtn"));
	}
	else if (step == 49)
	{
		if (m_contentscroll->getChildByTag(2) != NULL)
			nodes.push_back(m_contentscroll->getChildByTag(2)->getChildByName("csbnode")->getChildByName("actionbtn"));
	}
	else if (step == 50)
	{
		nodes.push_back(closebtn);
	}
	if (nodes.size() > 0)
		g_mainScene->showNewerGuideNode(step, nodes);
}

void SelectMyHerosLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		AnimationEffect::closeAniEffect((Layer*)this);
	}
}

void SelectMyHerosLayer::onExit()
{
	Layer::onExit();
}

void SelectMyHerosLayer::refreshMyHerosUi()
{
	m_contentscroll->removeAllChildrenWithCleanup(true);

	int count = 0;
	for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
	{
		Hero* herodata = GlobalInstance::vec_myHeros[i];
		if (herodata->getState() != HS_DEAD && herodata->getState() != HS_TRAINING)
			count++;
	}

	int size = count;
	int itemheight = 170;
	int innerheight = itemheight * size;
	int contentheight = m_contentscroll->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	m_contentscroll->setInnerContainerSize(Size(650, innerheight));

	int c = 0;
	for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
	{
		Hero* herodata = GlobalInstance::vec_myHeros[i];
		if (herodata->getState() != HS_DEAD && herodata->getState() != HS_TRAINING)
		{
			MyHeroNode* heronode = MyHeroNode::create(herodata, m_wheretype);
			heronode->setPosition(Vec2(m_contentscroll->getContentSize().width / 2, innerheight - c * itemheight - itemheight / 2));
			m_contentscroll->addChild(heronode, 0, i);
			c++;
		}
	}
}

MyHeroNode* SelectMyHerosLayer::getMyHeroNode(int index)
{
	return (MyHeroNode*)m_contentscroll->getChildByTag(index);
}

