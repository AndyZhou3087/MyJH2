#include "SelectMyHerosLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"

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
	this->addChild(color);
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("selectMyHerosLayer.csb"));
	this->addChild(csbnode, 0, "selct");
	int langtype = GlobalInstance::getInstance()->getLang();
	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");

	std::string titlenamstr = "outtowntitle";
	if (wheretype == HS_ONCHALLENGE)
		titlenamstr = "";

	titleimg->loadTexture(ResourcePath::makeTextImgPath(titlenamstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//关闭按钮
	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(SelectMyHerosLayer::onBtnClick, this));

	//我的英雄滚动控件
	m_contentscroll = (cocos2d::ui::ScrollView*)csbnode->getChildByName("contentscroll");

	m_contentscroll->setScrollBarEnabled(false);
	m_contentscroll->setBounceEnabled(true);

	refreshMyHerosUi();

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

void SelectMyHerosLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		this->removeFromParentAndCleanup(true);
	}
}

void SelectMyHerosLayer::onExit()
{
	Layer::onExit();
}

void SelectMyHerosLayer::refreshMyHerosUi()
{
	m_contentscroll->removeAllChildrenWithCleanup(true);

	int size = GlobalInstance::vec_myHeros.size();
	int itemheight = 150;
	int innerheight = itemheight * size;
	int contentheight = m_contentscroll->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	m_contentscroll->setInnerContainerSize(Size(650, innerheight));

	for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
	{
		Hero* herodata = GlobalInstance::vec_myHeros[i];
		MyHeroNode* heronode = MyHeroNode::create(herodata, m_wheretype);
		heronode->setPosition(Vec2(m_contentscroll->getContentSize().width / 2, innerheight - i * itemheight - itemheight / 2));
		m_contentscroll->addChild(heronode, 0, i);
	}
}

MyHeroNode* SelectMyHerosLayer::getMyHeroNode(int index)
{
	return (MyHeroNode*)m_contentscroll->getChildByTag(index);
}

