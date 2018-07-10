#include "FightingLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MovingLabel.h"
#include "MyRes.h"
#include "FightHeroNode.h"
#include "MapBlockScene.h"

USING_NS_CC;

FightingLayer::FightingLayer()
{

}

FightingLayer::~FightingLayer()
{
	for (unsigned int i = 0; i < m_enemyHeros.size(); i++)
	{
		delete m_enemyHeros[i];
	}
}


FightingLayer* FightingLayer::create(std::vector<Npc*> enemyHeros)
{
	FightingLayer *pRet = new(std::nothrow)FightingLayer();
	if (pRet && pRet->init(enemyHeros))
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
bool FightingLayer::init(std::vector<Npc*> enemyHeros)
{
	if (!Layer::init())
	{
		return false;
	}

	m_enemyHeros = enemyHeros;
	//LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	//this->addChild(color);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* bg = Sprite::create(ResourcePath::makeImagePath("fightingbg.jpg"));
	bg->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
	this->addChild(bg);

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("fightLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	//°´Å¥
	cocos2d::ui::Widget* escapebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("escapebtn");
	escapebtn->addTouchEventListener(CC_CALLBACK_2(FightingLayer::onBtnClick, this));

	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)escapebtn->getChildByName("text");

	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("escapebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);


	for (int i = 0; i < 6; i++)
	{
		FightHeroNode * fightHeroNode = FightHeroNode::create();
		fightHeroNode->setPosition(145 + i%3*215, 435 -  i/3*240);
		fightHeroNode->setData(GlobalInstance::myCardHeros[i]);
		addChild(fightHeroNode, 0, i);
	}

	for (unsigned int i = 0; i < enemyHeros.size(); i++)
	{
		FightHeroNode * fightHeroNode = FightHeroNode::create();
		fightHeroNode->setPosition(145 + i % 3 * 215, 835 + i / 3 * 240);
		fightHeroNode->setData(m_enemyHeros[i], 1);
		addChild(fightHeroNode, 0, 6+i);
	}

	//ÆÁ±ÎÏÂ²ãµã»÷
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void FightingLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		this->removeFromParentAndCleanup(true);
	}
}

void FightingLayer::onExit()
{
	Layer::onExit();
}

void FightingLayer::myHeroDeath(int which)
{
	GlobalInstance::myCardHeros[which]->setState(HS_DEAD);
	//GlobalInstance::myCardHeros[which] = NULL;
	if (g_MapBlockScene != NULL)
		g_MapBlockScene->removeChildByTag(which);
}

void FightingLayer::pauseAtkSchedule()
{
	for (int i = 0; i < 6; i++)
	{
		FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(i);
		fnode->pauseTimeSchedule();
	}
	for (unsigned int i = 0; i < m_enemyHeros.size(); i++)
	{
		FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(6 + i);
		fnode->pauseTimeSchedule();
	}
}
void FightingLayer::resumeAtkSchedule()
{
	int ret = checkFightResult();
	if (ret < 0)
	{
		for (int i = 0; i < 6; i++)
		{
			FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(i);
			fnode->resumeTimeSchedule();
		}
		for (unsigned int i = 0; i < m_enemyHeros.size(); i++)
		{
			FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(6 + i);
			fnode->resumeTimeSchedule();
		}
	}
	else
	{
		this->removeFromParentAndCleanup(true);
		if (g_MapBlockScene != NULL)
			g_MapBlockScene->showFightResult(ret);
	}
}

void FightingLayer::showAtk(Npc* ndata)
{
	if (ndata->getId().length() <= 0)
	{
		for (unsigned int i = 0; i < m_enemyHeros.size(); i++)
		{
			if (m_enemyHeros[i] != NULL && m_enemyHeros[i]->getHp() > 0)
			{
				FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(6 + i);
				if (fnode->isVisible())
				{
					float atkhp = ndata->getAtk();
					fnode->hurt(atkhp);
					break;
				}
			}
		}
	}
	else
	{
		for (unsigned int i = 0; i < 6; i++)
		{
			if (GlobalInstance::myCardHeros[i] != NULL && GlobalInstance::myCardHeros[i]->getHp() > 0)
			{
				FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(i);
				if (fnode->isVisible())
				{
					float atkhp = ndata->getAtk();
					fnode->hurt(atkhp);

					break;
				}
			}
		}
	}	
}

int FightingLayer::checkFightResult()
{
	int index = 0;
	int esize = m_enemyHeros.size();
	for (int i = 0; i < esize; i++)
	{
		if (m_enemyHeros[i] == NULL || m_enemyHeros[i]->getHp() <= 0.000001f)
		{
			index++;
		}
	}
	if (index == esize)
		return 1;

	index = 0;
	for (int i = 0; i < 6; i++)
	{
		if (GlobalInstance::myCardHeros[i] == NULL)
		{
			index++;
		}
	}
	if (index == 6)
		return 0;

	return -1;
}