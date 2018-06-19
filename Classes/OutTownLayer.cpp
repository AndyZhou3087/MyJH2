#include "OutTownLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "MovingLabel.h"
#include "MyRes.h"
#include "MainMapScene.h"

USING_NS_CC;

std::string carryResids[] = {"r001", "r011", "r012"};
OutTownLayer::OutTownLayer()
{
	for (int i=0;i<3;i++)
		caryycount[i] = 0;
}

OutTownLayer::~OutTownLayer()
{
	
}


OutTownLayer* OutTownLayer::create()
{
	OutTownLayer *pRet = new(std::nothrow)OutTownLayer();
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
bool OutTownLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("outTownLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();
	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("outtowntitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//按钮
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->setTag(1000);
	actionbtn->addTouchEventListener(CC_CALLBACK_2(OutTownLayer::onBtnClick, this));

	//按钮文字
	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("goout_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//关闭按钮
	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1001);
	closebtn->addTouchEventListener(CC_CALLBACK_2(OutTownLayer::onBtnClick, this));

	cocos2d::ui::Text* foodname = (cocos2d::ui::Text*)csbnode->getChildByName("r001box")->getChildByName("namelbl");
	foodname->setString(GlobalInstance::map_AllResources["r001"].name);

	cocos2d::ui::Text* carrytextlbl = (cocos2d::ui::Text*)csbnode->getChildByName("carrytextlbl");
	carrytextlbl->setString(ResourceLang::map_lang["carrytext"]);

	carrylbl = (cocos2d::ui::Text*)csbnode->getChildByName("carrylbl");


	for (int i = 0; i < 6; i++)
	{
		m_myCardHerosNode[i] = CardHeroNode::create();
		m_myCardHerosNode[i]->setPosition(Vec2(140 + i%3*215, /*745 + */1030-i/3*250));
		this->addChild(m_myCardHerosNode[i], 0, i);
		m_myCardHerosNode[i]->setData(GlobalInstance::myCardHeros[i]);
	}
	std::string str;
	for (int i = 0; i < 1; i++)
	{
		str = StringUtils::format("addbtn%d", i);
		cocos2d::ui::Button* addbtn = (cocos2d::ui::Button*)csbnode->getChildByName(str);
		addbtn->addTouchEventListener(CC_CALLBACK_2(OutTownLayer::onAddBtnClick, this));

		str = StringUtils::format("subbtn%d", i);
		cocos2d::ui::Button* subbtn = (cocos2d::ui::Button*)csbnode->getChildByName(str);
		subbtn->addTouchEventListener(CC_CALLBACK_2(OutTownLayer::onSubBtnClick, this));

		str = StringUtils::format("carrycount%d", i);
		caryycountlbl[i] = (cocos2d::ui::Text*)csbnode->getChildByName("carrycount0");

		caryycount[i] = MyRes::getMyResCount(carryResids[i], MYPACKAGE);
	}

	//还可以带多少食物
	caryycount[0] = GlobalInstance::getInstance()->getTotalCaryy() - caryycount[0] - caryycount[1] - caryycount[2];
	if (caryycount[0] > MyRes::getMyResCount(carryResids[0]))
		caryycount[0] = MyRes::getMyResCount(carryResids[0]) + MyRes::getMyResCount(carryResids[0], MYPACKAGE);
	else
		caryycount[0] += MyRes::getMyResCount(carryResids[0], MYPACKAGE);


	updateCaryyCountLbl();

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

void OutTownLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
		switch (tag)
		{
		case 1000://出城
		{
			bool isselecthero = false;
			for (int i = 0; i < 6; i++)
			{
				if (GlobalInstance::myCardHeros[i] != NULL)
				{
					isselecthero = true;
					break;
				}
			}
			if (!isselecthero)
			{
				MovingLabel::show(ResourceLang::map_lang["noherosout"]);
				return;
			}
			

			for (int i = 0; i < 3; i++)
			{
				int addcount = caryycount[i] - MyRes::getMyResCount(carryResids[i], MYPACKAGE);
				if (addcount != 0)
				{
					MyRes::Add(carryResids[i], addcount, MYPACKAGE);
					MyRes::Add(carryResids[i], -addcount);
				}
			}
			GlobalInstance::getInstance()->parseMapJson();
			Director::getInstance()->replaceScene(TransitionFade::create(2.0f, MainMapScene::createScene()));
			
			break;
		}
		case 1001://关闭
			this->removeFromParentAndCleanup(true);
			break;
		default:
			break;
		}
	}
}

void OutTownLayer::onAddBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		std::string name = clicknode->getName();
		int index = atoi(name.substr(name.length() - 1).c_str());
		if (isCarryOver())
		{
			MovingLabel::show(ResourceLang::map_lang["carryovertext"]);
		}
		else
		{
			if (caryycount[index] >= MyRes::getMyResCount(carryResids[index]))
			{
				std::string str = StringUtils::format(ResourceLang::map_lang["lacktext"].c_str(), GlobalInstance::map_AllResources[carryResids[index]].name.c_str());
				MovingLabel::show(str);
				return;
			}
			caryycount[index]++;
			updateCaryyCountLbl();
		}
	}
}

void OutTownLayer::updateCaryyCountLbl()
{
	std::string str = StringUtils::format("%d/%d", caryycount[0] + caryycount[1]+ caryycount[2], GlobalInstance::getInstance()->getTotalCaryy());
	carrylbl->setString(str);

	str = StringUtils::format("%d/%d", caryycount[0], MyRes::getMyResCount(carryResids[0]) + MyRes::getMyResCount(carryResids[0], MYPACKAGE));
	caryycountlbl[0]->setString(str);
}

void OutTownLayer::onSubBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		std::string name = clicknode->getName();
		int index = atoi(name.substr(name.length() - 1).c_str());

		if (caryycount[index] <= 0)
			return;

		caryycount[index]--;
		updateCaryyCountLbl();
	}
}

bool OutTownLayer::isCarryOver()
{
	int count = 0;
	for (int i = 0; i < 3; i++)
	{
		count += caryycount[i];
	}
	return count >= GlobalInstance::getInstance()->getTotalCaryy();
}

void OutTownLayer::onExit()
{
	Layer::onExit();
}

CardHeroNode* OutTownLayer::getMyCardHeroNode(int index)
{
	return m_myCardHerosNode[index];
}