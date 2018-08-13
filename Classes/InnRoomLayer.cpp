#include "InnRoomLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "RandHeroLayer.h"
#include "ConsumeResActionLayer.h"
#include "MovingLabel.h"
#include "DataSave.h"
#include "AnimationEffect.h"

USING_NS_CC;

InnRoomLayer::InnRoomLayer()
{

}

InnRoomLayer::~InnRoomLayer()
{
	
}


InnRoomLayer* InnRoomLayer::create(Building* buidingData)
{
	InnRoomLayer *pRet = new(std::nothrow)InnRoomLayer();
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
bool InnRoomLayer::init(Building* buidingData)
{
    if ( !Layer::init() )
    {
        return false;
    }

	m_buidingData = buidingData;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("innRoomLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();
	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("innroomtitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//等级
	lvUIlbl = (cocos2d::ui::Text*)csbnode->getChildByName("lv");
	std::string str = StringUtils::format("%d%s", buidingData->level.getValue() + 1, ResourceLang::map_lang["lvtext"].c_str());
	lvUIlbl->setString(str);

	//招募按钮
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->setTag(1000);
	actionbtn->addTouchEventListener(CC_CALLBACK_2(InnRoomLayer::onBtnClick, this));

	//招募按钮文字
	cocos2d::ui::ImageView* recruitbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	recruitbtntxt->loadTexture(ResourcePath::makeTextImgPath("recruit_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//升级按钮
	cocos2d::ui::Widget* lvUpbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("lvupbtn");
	lvUpbtn->setTag(1001);
	lvUpbtn->addTouchEventListener(CC_CALLBACK_2(InnRoomLayer::onBtnClick, this));

	//升级按钮文字
	cocos2d::ui::ImageView* lvUpbtntxt = (cocos2d::ui::ImageView*)lvUpbtn->getChildByName("text");
	lvUpbtntxt->loadTexture(ResourcePath::makeTextImgPath("innroolv_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//关闭按钮
	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1002);
	closebtn->addTouchEventListener(CC_CALLBACK_2(InnRoomLayer::onBtnClick, this));

	//我的英雄滚动控件
	m_contentscroll = (cocos2d::ui::ScrollView*)csbnode->getChildByName("contentscroll");

	m_contentscroll->setScrollBarEnabled(false);
	m_contentscroll->setBounceEnabled(true);

	GlobalInstance::getInstance()->heroSortByLv();

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

void InnRoomLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
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
			RandHeroLayer* layer = RandHeroLayer::create();
			this->addChild(layer);
			AnimationEffect::openAniEffect((Layer*)layer);
			break;
		}
		case 1001://升级
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
		case 1002://关闭
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		default:
			break;
		}
	}
}

void InnRoomLayer::refreshMyHerosUi()
{
	m_contentscroll->removeAllChildrenWithCleanup(true);

	int count = 0;
	for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
	{
		Hero* herodata = GlobalInstance::vec_myHeros[i];
		if (herodata->getState() != HS_DEAD)
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
		if (herodata->getState() != HS_DEAD)
		{
			MyHeroNode* heronode = MyHeroNode::create(herodata);
			heronode->setPosition(Vec2(m_contentscroll->getContentSize().width + 600, innerheight - c * itemheight - itemheight / 2));
			heronode->runAction(EaseSineIn::create(MoveBy::create(0.10f + c*0.05f, Vec2(-m_contentscroll->getContentSize().width / 2 - 600, 0))));
			m_contentscroll->addChild(heronode, 0, i);
			c++;
		}
	}
}
void InnRoomLayer::lvup()
{
	std::string str = StringUtils::format("%d%s", m_buidingData->level.getValue() + 1, ResourceLang::map_lang["lvtext"].c_str());
	lvUIlbl->setString(str);
}


MyHeroNode* InnRoomLayer::getMyHeroNode(int index)
{
	return (MyHeroNode*)m_contentscroll->getChildByTag(index);
}

void InnRoomLayer::fireHero(int index)
{
	GlobalInstance::getInstance()->fireHero(index);
	refreshMyHerosUi();
}

void InnRoomLayer::onExit()
{
	Layer::onExit();
}
