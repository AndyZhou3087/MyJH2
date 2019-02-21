#include "HospitalLayer.h"
#include <algorithm>
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "MyMenu.h"
#include "GlobalInstance.h"
#include "Resource.h"
#include "MyHeroNode.h"
#include "AnimationEffect.h"
#include "NewGuideLayer.h"
#include "MainScene.h"

HospitalLayer::HospitalLayer()
{
	dcount = -1;
}


HospitalLayer::~HospitalLayer()
{

}

bool HospitalLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	m_csbnode = CSLoader::createNode(ResourcePath::makePath("hospitalLayer.csb"));
	this->addChild(m_csbnode);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	int langtype = GlobalInstance::getInstance()->getLang();

	//БъЬт
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)m_csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("hospital_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	closebtn = (cocos2d::ui::Widget*)m_csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(HospitalLayer::onBtnClick, this));

	scrollview = (cocos2d::ui::ScrollView*)m_csbnode->getChildByName("scrollview");
	scrollview->setScrollBarEnabled(false);
	scrollview->setBounceEnabled(true);

	cointext = (cocos2d::ui::Text*)m_csbnode->getChildByName("cointext");
	std::string str = StringUtils::format("%d", GlobalInstance::getInstance()->getMyCoinCount().getValue());
	cointext->setString(str);

	silvertext = (cocos2d::ui::Text*)m_csbnode->getChildByName("silvertext");
	str = StringUtils::format("%d", GlobalInstance::getInstance()->getMySoliverCount().getValue());
	silvertext->setString(str);

	hintdesc = (cocos2d::ui::Text*)m_csbnode->getChildByName("hintdesc");
	hintdesc->setString("");

	updateContent();

	//this->scheduleOnce(schedule_selector(HospitalLayer::delayShowNewerGuide), newguidetime);

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}

void HospitalLayer::delayShowNewerGuide(float dt)
{
	if (vec_deadheros.size() <= 0)
	{
		GlobalInstance::getInstance()->saveNewerGuide(64, false);
	}
	if (!NewGuideLayer::checkifNewerGuide(63))
	{
		if (vec_deadheros.size() > 0)
		{
			if (NewGuideLayer::checkifNewerGuide(64))
			{
				showNewerGuide(64);
			}
		}
		else if (NewGuideLayer::checkifNewerGuide(65))
		{
			showNewerGuide(65);
		}
	}
}

void HospitalLayer::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	if (step == 64)
	{
		nodes.push_back(vec_deadNodes[0]->getChildByName("csbnode")->getChildByName("actionbtn"));
	}
	else if (step == 65)
	{
		nodes.push_back(closebtn);
	}
	if (g_mainScene != NULL)
	{
		g_mainScene->showNewerGuideNode(step, nodes);
	}
}

void HospitalLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		AnimationEffect::closeAniEffect((Layer*)this);
	}
}

void HospitalLayer::updateContent()
{
	std::string str = StringUtils::format("%d", GlobalInstance::getInstance()->getMyCoinCount().getValue());
	cointext->setString(str);
	str = StringUtils::format("%d", GlobalInstance::getInstance()->getMySoliverCount().getValue());
	silvertext->setString(str);

	int scrollpos = scrollview->getScrolledPercentVertical();

	scrollview->removeAllChildrenWithCleanup(true);
	vec_deadheros.clear();
	vec_deadNodes.clear();
	for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
	{
		Hero* hero = GlobalInstance::vec_myHeros[i];
		if (hero->getState() == HS_DEAD)
		{
			vec_deadheros.push_back(GlobalInstance::vec_myHeros[i]);
		}
	}

	int size = vec_deadheros.size();


	if (vec_deadheros.size() <= 0)
	{
		delayShowNewerGuide(0);
		hintdesc->setString(ResourceLang::map_lang["hospitalhintdesc"]);
	}

	int itemheight = 170;
	int innerheight = itemheight * size;
	int contentheight = scrollview->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollview->setInnerContainerSize(Size(scrollview->getContentSize().width, innerheight));

	for (int i = 0; i < size; i++)
	{
		Hero* hero = vec_deadheros[i];
		MyHeroNode* node = MyHeroNode::create(hero, HS_DEAD);
		scrollview->addChild(node);
		node->setPosition(Vec2(scrollview->getContentSize().width + 600, innerheight - i * itemheight - itemheight / 2));
		vec_deadNodes.push_back(node);
		if (i == 0)
		{
			node->runAction(Sequence::create(EaseSineIn::create(MoveBy::create(0.15f + i*0.07f, Vec2(-scrollview->getContentSize().width / 2 - 600, 0))), CallFunc::create(CC_CALLBACK_0(HospitalLayer::todoNewGuide, this)), NULL));
		}
		else
		{
			node->runAction(EaseSineIn::create(MoveBy::create(0.15f + i*0.07f, Vec2(-scrollview->getContentSize().width / 2 - 600, 0))));
		}
		//node->setPosition(Vec2(319, innerheight - i*itemheight - itemheight*0.5));
	}

	if (scrollpos > 0)
	{
		scrollview->jumpToPercentVertical(scrollpos);
	}
}

void HospitalLayer::todoNewGuide()
{
	delayShowNewerGuide(0);
}