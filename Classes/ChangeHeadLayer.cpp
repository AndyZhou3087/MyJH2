#include "ChangeHeadLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "DataSave.h"
#include "HeadInfoLayer.h"
#include "MainMenuLayer.h"
#include "SoundManager.h"

USING_NS_CC;

ChangeHeadLayer::ChangeHeadLayer()
{
	lastSelectIndex = -1;
}

ChangeHeadLayer::~ChangeHeadLayer()
{

}


ChangeHeadLayer* ChangeHeadLayer::create()
{
	ChangeHeadLayer *pRet = new(std::nothrow)ChangeHeadLayer();
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

// on "init" you need to initialize your instance
bool ChangeHeadLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("changeHeadLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::ImageView* selheadtitle = (cocos2d::ui::ImageView*)csbnode->getChildByName("selheadtitle");
	selheadtitle->loadTexture(ResourcePath::makeTextImgPath("selheadtitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Button* actionbtn = (cocos2d::ui::Button*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(ChangeHeadLayer::onBtnClick, this));
	actionbtn->setTag(100);

	cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	text->loadTexture(ResourcePath::makeTextImgPath("okbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ScrollView* scrollView = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollView");

	int itemheight = 190;
	int innerheight = itemheight * (8 / 3 + 1);
	int contentheight = scrollView->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollView->setInnerContainerSize(Size(600, innerheight));

	std::string str;
	for (int i = 0; i < 8; i++)
	{
		Sprite* headbox = Sprite::createWithSpriteFrameName("ui/main_menu_box.png");
		scrollView->addChild(headbox);
		headbox->setPosition(Vec2(76 + (i % 3) * 224, innerheight - i / 3 * itemheight - itemheight / 2));

		Sprite* select = Sprite::createWithSpriteFrameName("ui/selecthead.png");
		scrollView->addChild(select);
		select->setPosition(headbox->getPosition());
		select->setVisible(false);
		selectArr[i] = select;
		if (DataSave::getInstance()->getHeadId() == i + 4)
		{
			select->setVisible(true);
			lastSelectIndex = i;
		}

		str = StringUtils::format("ui/h_%d_0.png", i + 4);
		cocos2d::ui::ImageView* head = cocos2d::ui::ImageView::create(str, cocos2d::ui::Widget::TextureResType::PLIST);
		head->addTouchEventListener(CC_CALLBACK_2(ChangeHeadLayer::onBtnClick, this));
		head->setTouchEnabled(true);
		head->setTag(i);
		head->setPosition(Vec2(headbox->getPositionX(), headbox->getPositionY() + 8));
		scrollView->addChild(head);

		//ClippingNode* m_clippingNode = ClippingNode::create();
		//m_clippingNode->setInverted(false);//设置底板可见
		//m_clippingNode->setAlphaThreshold(0.5f);//设置透明度Alpha值为0
		//scrollView->addChild(m_clippingNode, 1);
		//m_clippingNode->setAnchorPoint(Vec2(0.5, 1));
		//m_clippingNode->setPosition(Vec2(headbox->getPositionX(), headbox->getPositionY() + 45));
		//cocos2d::ui::ImageView* head = cocos2d::ui::ImageView::create(str, cocos2d::ui::Widget::TextureResType::LOCAL);
		//head->addTouchEventListener(CC_CALLBACK_2(ChangeHeadLayer::onBtnClick, this));
		//head->setTouchEnabled(true);
		//head->setTag(i);
		//head->setAnchorPoint(Vec2(0.5, 1));
		//head->setPositionY(20);
		//m_clippingNode->addChild(head);
		//Node* stencil = Node::create();
		//Sprite* cnode = Sprite::createWithSpriteFrameName("ui/headclip.png");
		//cnode->setAnchorPoint(Vec2(0.5, 1));
		//stencil->addChild(cnode);
		//m_clippingNode->setStencil(stencil);

		//std::string s = StringUtils::format("vocation_%d", i + 4);
		//str = ResourceLang::map_lang[s];
		//Label *namelbl = Label::createWithTTF(str, FONT_NAME, 22);
		//namelbl->setColor(Color3B(255, 255, 255));
		//namelbl->setPosition(Vec2(headbox->getContentSize().width / 2, -20));
		//headbox->addChild(namelbl);
	}

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

void ChangeHeadLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* node = (Node*)pSender;
	int tag = node->getTag();
	if (tag == 100)
	{
		CommonFuncs::BtnAction(pSender, type);
	}
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (tag == 100)
		{
			DataSave::getInstance()->setHeadId(lastSelectIndex + 4);
			HeadInfoLayer * infolayer = (HeadInfoLayer*)this->getParent();
			if (infolayer != NULL)
			{
				infolayer->changeHead();
				MainMenuLayer * menulayer = (MainMenuLayer*)infolayer->getParent();
				if (menulayer != NULL)
				{
					menulayer->changeHead();
				}
			}
			AnimationEffect::closeAniEffect((Layer*)this);
		}
		else
		{
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
			if (lastSelectIndex == tag)
			{
				return;
			}
			else
			{
				selectArr[lastSelectIndex]->setVisible(false);
				selectArr[tag]->setVisible(true);
				lastSelectIndex = tag;
			}
		}
	}
}

void ChangeHeadLayer::onExit()
{
	Layer::onExit();
}