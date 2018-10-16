#include "MatchMainLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "MovingLabel.h"
#include "AnimationEffect.h"
#include "SoundManager.h"
#include "MainScene.h"
#include "Const.h"
#include "SelectMyHerosLayer.h"
#include "DataSave.h"

USING_NS_CC;

MatchMainLayer::MatchMainLayer()
{
	clickHero = -1;
}

MatchMainLayer::~MatchMainLayer()
{
	clickHero = -1;
}


MatchMainLayer* MatchMainLayer::create()
{
	MatchMainLayer *pRet = new(std::nothrow)MatchMainLayer();
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
bool MatchMainLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("matchMainLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();
	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("matchfighttitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//更换队形按钮
	cocos2d::ui::Button* changebtn = (cocos2d::ui::Button*)csbnode->getChildByName("changebtn");
	changebtn->setTag(1000);
	changebtn->addTouchEventListener(CC_CALLBACK_2(MatchMainLayer::onBtnClick, this));

	//按钮文字
	cocos2d::ui::ImageView* changebtntxt = (cocos2d::ui::ImageView*)changebtn->getChildByName("text");
	changebtntxt->loadTexture(ResourcePath::makeTextImgPath("changequene_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//保存队形按钮
	cocos2d::ui::Button* savebtn = (cocos2d::ui::Button*)csbnode->getChildByName("savebtn");
	savebtn->setTag(1001);
	savebtn->addTouchEventListener(CC_CALLBACK_2(MatchMainLayer::onBtnClick, this));

	cocos2d::ui::ImageView* savebtntxt = (cocos2d::ui::ImageView*)savebtn->getChildByName("text");
	savebtntxt->loadTexture(ResourcePath::makeTextImgPath("matchsave_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//保存队形按钮
	cocos2d::ui::Button* rulebtn = (cocos2d::ui::Button*)csbnode->getChildByName("rulebtn");
	rulebtn->setTag(1002);
	rulebtn->addTouchEventListener(CC_CALLBACK_2(MatchMainLayer::onBtnClick, this));

	cocos2d::ui::ImageView* rulebtntxt = (cocos2d::ui::ImageView*)rulebtn->getChildByName("text");
	rulebtntxt->loadTexture(ResourcePath::makeTextImgPath("matchrulebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//匹配按钮
	cocos2d::ui::Button* matchbtn = (cocos2d::ui::Button*)csbnode->getChildByName("matchbtn");
	matchbtn->setTag(1003);
	matchbtn->addTouchEventListener(CC_CALLBACK_2(MatchMainLayer::onBtnClick, this));

	cocos2d::ui::ImageView* matchbtntxt = (cocos2d::ui::ImageView*)matchbtn->getChildByName("text");
	matchbtntxt->loadTexture(ResourcePath::makeTextImgPath("matchbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	matchbtntxt->ignoreContentAdaptWithSize(true);

	//关闭按钮
	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1004);
	closebtn->addTouchEventListener(CC_CALLBACK_2(MatchMainLayer::onBtnClick, this));

	cocos2d::ui::Text* changehint = (cocos2d::ui::Text*)csbnode->getChildByName("changehint");
	changehint->setString(ResourceLang::map_lang["changelineuphint"]);

	cocos2d::ui::Text* matchlvtext = (cocos2d::ui::Text*)csbnode->getChildByName("matchlvtext");
	matchlvtext->setString(ResourceLang::map_lang["matchlvtext"]);

	cocos2d::ui::Text* matchexptext = (cocos2d::ui::Text*)csbnode->getChildByName("matchexptext");
	matchexptext->setString(ResourceLang::map_lang["matchexptext"]);

	matchlv = (cocos2d::ui::Text*)csbnode->getChildByName("matchlv");
	matchlv->setString(ResourceLang::map_lang["matchlvname_0"]);

	matchexp = (cocos2d::ui::Text*)csbnode->getChildByName("matchexp");
	matchexp->setString("0");

	needexp = (cocos2d::ui::Text*)csbnode->getChildByName("needexp");
	std::string str = StringUtils::format(ResourceLang::map_lang["neednextexpdesc"].c_str(), 0);
	needexp->setString(str);

	nextlvtext = (cocos2d::ui::Text*)csbnode->getChildByName("nextlvtext");
	nextlvtext->setString("");

	cocos2d::ui::Text* rewardtext = (cocos2d::ui::Text*)csbnode->getChildByName("rewardtext");
	rewardtext->setString(ResourceLang::map_lang["matchrewardtext"]);

	cocos2d::ui::Text* endtimetxt = (cocos2d::ui::Text*)csbnode->getChildByName("endtimetxt");
	endtimetxt->setString(ResourceLang::map_lang["matchendtimetext"]);

	endtime = (cocos2d::ui::Text*)csbnode->getChildByName("endtime");
	endtime->setString("");

	for (int i = 0; i < 6; i++)
	{
		Vec2 pos = Vec2(140 + i % 3 * 215, /*745 + */1060 - i / 3 * 250);
		Sprite* cardnodebg = Sprite::create(ResourcePath::makeImagePath("cardherobox_.png"));
		cardnodebg->setPosition(Vec2(pos.x, pos.y+14));
		this->addChild(cardnodebg, 0);

		m_myCardHerosNode[i] = CardHeroNode::create();
		m_myCardHerosNode[i]->setPosition(pos);
		this->addChild(m_myCardHerosNode[i], 1, i);
		m_myCardHerosNode[i]->setData(NULL);
	}

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(MatchMainLayer::onTouchBegan, this);

	listener->onTouchMoved = CC_CALLBACK_2(MatchMainLayer::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(MatchMainLayer::onTouchEnded, this);
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

bool MatchMainLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
	clickHero = -1;
	beginTouchPoint = touch->getLocation();
	for (int i = 0; i < 6; i++)
	{
		if (GlobalInstance::myCardHeros[i] != NULL)
		{
			float x = m_myCardHerosNode[i]->getPositionX();
			float y = m_myCardHerosNode[i]->getPositionY();

			if (beginTouchPoint.x >= x - 70 && beginTouchPoint.x <= x + 70 && beginTouchPoint.y >= y - 70 && beginTouchPoint.y <= y + 70)
			{
				m_myCardHerosNode[i]->setLocalZOrder(2);
				clickHero = i;
				break;
			}
		}
	}

	return true;
}

void MatchMainLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
	auto movedPoint = touch->getLocation();
	if (fabs(movedPoint.x - beginTouchPoint.x) >= CLICKOFFSETP || fabs(movedPoint.y - beginTouchPoint.y) >= CLICKOFFSETP)
	{
		if (clickHero >= 0)
		{
			auto touchPos = touch->getLocation();
			m_myCardHerosNode[clickHero]->setPosition(touchPos);
			m_myCardHerosNode[clickHero]->setIsDrading(true);
		}
	}
}

void MatchMainLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
	if (clickHero >= 0)
	{
		auto touchPos = touch->getLocation();
		log("touchPos.x = %.f, touchPos.y = %.f", touchPos.x, touchPos.y);
		m_myCardHerosNode[clickHero]->setPosition(touchPos);

		bool ischange = false;
		for (int i = 0; i < 6; i++)
		{
			if (i != clickHero)
			{
				float x = m_myCardHerosNode[i]->getPositionX();
				float y = m_myCardHerosNode[i]->getPositionY();

				if (touchPos.x >= x - 70 && touchPos.x <= x + 70 && touchPos.y >= y - 70 && touchPos.y <= y + 70)
				{
					CardHeroNode* cardnode = m_myCardHerosNode[clickHero];

					m_myCardHerosNode[clickHero]->setLocalZOrder(1);
					m_myCardHerosNode[clickHero]->runAction(MoveTo::create(0.2f, Vec2(140 + i % 3 * 215, /*745 + */1060 - i / 3 * 250)));
					m_myCardHerosNode[clickHero]->setTag(i);
					GlobalInstance::myCardHeros[clickHero]->setPos(i + 1);
					
					m_myCardHerosNode[i]->setTag(clickHero);
					if (GlobalInstance::myCardHeros[i] != NULL)
					{
						m_myCardHerosNode[i]->runAction(MoveTo::create(0.2f, Vec2(140 + clickHero % 3 * 215, /*745 + */1060 - clickHero / 3 * 250)));
						GlobalInstance::myCardHeros[i]->setPos(clickHero + 1);
					}
					else
					{
						m_myCardHerosNode[i]->setPosition(Vec2(140 + clickHero % 3 * 215, /*745 + */1060 - clickHero / 3 * 250));
						m_myCardHerosNode[i]->updateSelPosLbl();
					}

					m_myCardHerosNode[clickHero] = m_myCardHerosNode[i];
					m_myCardHerosNode[i] = cardnode;

					Hero * myhero = GlobalInstance::myCardHeros[clickHero];
					GlobalInstance::myCardHeros[clickHero] = GlobalInstance::myCardHeros[i];
					GlobalInstance::myCardHeros[i] = myhero;
					
					ischange = true;
					break;
				}
			}
		}
		if (!ischange)
		{
			m_myCardHerosNode[clickHero]->setPosition(Vec2(140 + clickHero % 3 * 215, /*745 + */1060 - clickHero / 3 * 250));
			m_myCardHerosNode[clickHero]->setLocalZOrder(1);
		}
		clickHero = -1;
	}
}


void MatchMainLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
		switch (tag)
		{
		case 1000://出城
			break;
		
		default:
			break;
		}
	}
}

void MatchMainLayer::onExit()
{
	Layer::onExit();
}

CardHeroNode* MatchMainLayer::getMyCardHeroNode(int index)
{
	return m_myCardHerosNode[index];
}