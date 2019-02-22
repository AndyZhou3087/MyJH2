#include "OutTownLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "MovingLabel.h"
#include "MyRes.h"
#include "MainMapScene.h"
#include "AnimationEffect.h"
#include "SoundManager.h"
#include "MarketLayer.h"
#include "MainScene.h"
#include "NewGuideLayer.h"
#include "Const.h"
#include "SelectMyHerosLayer.h"
#include "DataSave.h"
#include "MyTransitionScene.h"

USING_NS_CC;

std::string carryResids[] = {"r001", "r011", "r012"};
OutTownLayer::OutTownLayer()
{
	m_isLongPress = false;
	m_longTouchNode = NULL;

	for (int i=0;i<3;i++)
		caryycount[i] = 0;
}

OutTownLayer::~OutTownLayer()
{
	clickHero = -1;
	if ((GlobalInstance::getInstance()->getHerosChangeLevelCount() > 0 && NewGuideLayer::checkifNewerGuide(66)) || (GlobalInstance::getInstance()->getHerosLevelCount(15) > 0 && NewGuideLayer::checkifNewerGuide(73)))
	{
		if (g_mainScene != NULL)
		{
			g_mainScene->delayShowNewerGuide(0);
		}
	}

	for (int i = 0; i < 6; i++)
	{
		Hero* hero = GlobalInstance::myCardHeros[i];
		if (hero != NULL)
		{
			for (int k = T_ARMOR; k <= T_NG; k++)
			{
				ResBase* eqres = hero->getEquipable(k);
				if (eqres != NULL)
				{
					hero->setEquipable((Equipable*)eqres, eqres->getType());
				}
			}
		}
	}
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
	this->addChild(color,0,"colorLayer");
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("outTownLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();
	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("outtowntitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//按钮
	actionbtn = (cocos2d::ui::Button*)csbnode->getChildByName("actionbtn");
	actionbtn->setTag(1000);
	actionbtn->addTouchEventListener(CC_CALLBACK_2(OutTownLayer::onBtnClick, this));

	//按钮文字
	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("goout_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//关闭按钮
	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1001);
	closebtn->addTouchEventListener(CC_CALLBACK_2(OutTownLayer::onBtnClick, this));

	//更换队形按钮
	changebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("changebtn");
	changebtn->setTag(1002);
	changebtn->addTouchEventListener(CC_CALLBACK_2(OutTownLayer::onBtnClick, this));

	cocos2d::ui::ImageView* changetxt = (cocos2d::ui::ImageView*)changebtn->getChildByName("text");
	changetxt->loadTexture(ResourcePath::makeTextImgPath("changequene_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* foodname = (cocos2d::ui::Text*)csbnode->getChildByName("r001box")->getChildByName("namelbl");
	foodname->setString(GlobalInstance::map_AllResources["r001"].name);

	cocos2d::ui::Text* versionname = (cocos2d::ui::Text*)csbnode->getChildByName("v001box")->getChildByName("namelbl");

	std::string vstr = StringUtils::format(ResourceLang::map_lang["vision"].c_str(), MyRes::getMyResCount("v001"));
	versionname->setString(vstr);

	cocos2d::ui::Text* gocityname = (cocos2d::ui::Text*)csbnode->getChildByName("t001box")->getChildByName("namelbl");

	std::string gstr = StringUtils::format(ResourceLang::map_lang["gocitycard"].c_str(), MyRes::getMyResCount("t001"));
	gocityname->setString(gstr);

	cocos2d::ui::Text* carrytextlbl = (cocos2d::ui::Text*)csbnode->getChildByName("carrytextlbl");
	carrytextlbl->setString(ResourceLang::map_lang["carrytext"]);

	carrylbl = (cocos2d::ui::Text*)csbnode->getChildByName("carrylbl");

	cocos2d::ui::Text* changehint = (cocos2d::ui::Text*)csbnode->getChildByName("changehint");
	changehint->setString(ResourceLang::map_lang["changelineuphint"]);

	cocos2d::ui::Text* hintdesc = (cocos2d::ui::Text*)csbnode->getChildByName("hintdesc");
	hintdesc->setString(ResourceLang::map_lang["outtownhintdesc"]);

	for (int i = 0; i < 6; i++)
	{
		Vec2 pos = Vec2(140 + i % 3 * 215, /*745 + */1060 - i / 3 * 250);
		Sprite* cardnodebg = Sprite::create(ResourcePath::makeImagePath("cardherobox_.png"));
		cardnodebg->setPosition(Vec2(pos.x, pos.y+14));
		this->addChild(cardnodebg, 0);

		m_myCardHerosNode[i] = CardHeroNode::create();
		m_myCardHerosNode[i]->setPosition(pos);
		this->addChild(m_myCardHerosNode[i], 1, i);
		m_myCardHerosNode[i]->setData(GlobalInstance::myCardHeros[i]);
	}

	std::string str;
	for (int i = 0; i < 1; i++)
	{
		str = StringUtils::format("addbtn%d", i);
		cocos2d::ui::Button* addbtn = (cocos2d::ui::Button*)csbnode->getChildByName(str);
		addbtn->addTouchEventListener(CC_CALLBACK_2(OutTownLayer::onAddBtnClick, this));
		addbtn->setTag(10000 + i);

		str = StringUtils::format("subbtn%d", i);
		cocos2d::ui::Button* subbtn = (cocos2d::ui::Button*)csbnode->getChildByName(str);
		subbtn->addTouchEventListener(CC_CALLBACK_2(OutTownLayer::onSubBtnClick, this));
		subbtn->setTag(20000 + i);

		str = StringUtils::format("carrycount%d", i);
		caryycountlbl[i] = (cocos2d::ui::Text*)csbnode->getChildByName(str);

		//caryycount[i] = MyRes::getMyResCount(carryResids[i]);
	}

	updateHeroCarry();

	for (int i = 1; i <= 2; i++)
	{
		str = StringUtils::format("tomartketbuy%d", i);
		cocos2d::ui::Text* tobuytext = (cocos2d::ui::Text*)csbnode->getChildByName(str);
		tobuytext->setString(ResourceLang::map_lang[str]);
		tobuytext->addTouchEventListener(CC_CALLBACK_2(OutTownLayer::onGoBuyText, this));
	}

	cocos2d::ui::TextField* countTextField = (cocos2d::ui::TextField*)csbnode->getChildByName("coutinput");
	countTextField->setString("");
	countTextField->setVisible(false);

	m_editCount = cocos2d::ui::EditBox::create(Size(countTextField->getContentSize().width, 35), cocos2d::ui::Scale9Sprite::createWithSpriteFrameName("ui/blank.png"));
	m_editCount->setPosition(countTextField->getPosition());
	m_editCount->setFontColor(countTextField->getTextColor());
	m_editCount->setPlaceHolder(ResourceLang::map_lang["outtownplaceholder"].c_str());
	m_editCount->setPlaceholderFontSize(25);
	m_editCount->setInputMode(cocos2d::ui::EditBox::InputMode::NUMERIC);
	m_editCount->setPlaceholderFontColor(Color3B(112, 116, 109));
	m_editCount->setMaxLength(4);//英文字符长度，中文2个
	m_editCount->setTextHorizontalAlignment(TextHAlignment::CENTER);
	m_editCount->setText(" ");
	//editName->setReturnType(EditBox::KeyboardReturnType::DONE);
	m_editCount->setDelegate(this);
	csbnode->addChild(m_editCount);


	this->scheduleOnce(schedule_selector(OutTownLayer::delayShowNewerGuide), newguidetime);

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(OutTownLayer::onTouchBegan, this);

	listener->onTouchMoved = CC_CALLBACK_2(OutTownLayer::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(OutTownLayer::onTouchEnded, this);
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

bool OutTownLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event)
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

void OutTownLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event)
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

void OutTownLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event)
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

void OutTownLayer::delayShowNewerGuide(float dt)
{
	if (!NewGuideLayer::checkifNewerGuide(14))
	{
		if (NewGuideLayer::checkifNewerGuide(FOURTHGUIDESTEP))
		{
			if (NewGuideLayer::checkifNewerGuide(46))
			{
				showNewerGuide(46);
			}
			/*else if (NewGuideLayer::checkifNewerGuide(48))
			{
				showNewerGuide(48);
			}
			else if (NewGuideLayer::checkifNewerGuide(50))
			{
				showNewerGuide(50);
			}*/
			else if (NewGuideLayer::checkifNewerGuide(52))
			{
				showNewerGuide(52);
			}
		}
		else if (NewGuideLayer::checkifNewerGuide(92) && GlobalInstance::getInstance()->getEnoughFightHeros())
		{
			showNewerGuide(92);
		}
	}
}

void OutTownLayer::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	if (step == 46)
	{
		nodes.push_back(m_myCardHerosNode[0]->getChildByName("csbnode")->getChildByName("hbox"));
	}
	else if (step == 48)
	{
		nodes.push_back(m_myCardHerosNode[1]->getChildByName("csbnode")->getChildByName("hbox"));
	}
	else if (step == 50)
	{
		nodes.push_back(m_myCardHerosNode[2]->getChildByName("csbnode")->getChildByName("hbox"));
	}
	else if (step == 52)
	{
		nodes.push_back(actionbtn);
	}
	else if (step == 92)
	{
		nodes.push_back(changebtn);
	}
	g_mainScene->showNewerGuideNode(step, nodes);
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
				int addcount = caryycount[i];
				if (addcount != 0)
				{
					MyRes::Add(carryResids[i], addcount, MYPACKAGE);
					MyRes::Use(carryResids[i], addcount);
				}
			}
			((cocos2d::ui::Button*)pSender)->setEnabled(false);

			GlobalInstance::getInstance()->recoveCardHeroMaxHp();
			GlobalInstance::myOutMapCarry = GlobalInstance::getInstance()->getTotalCarry();
#if USE_TRANSCENE
			Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MyTransitionScene::createScene(TO_MAP)));
#else
			Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainMapScene::createScene()));
#endif			
			break;
		}
		case 1001://关闭
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		case 1002:
		{
			Layer* layer = SelectMyHerosLayer::create(HS_TAKEON);
			this->addChild(layer, 3, "selectmyheroslayer");
			AnimationEffect::openAniEffect((Layer*)layer);
		}
		default:
			break;
		}
	}
}

void OutTownLayer::onAddBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	Node* clicknode = (Node*)pSender;
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		m_longTouchNode = clicknode;
		if (!isScheduled(schedule_selector(OutTownLayer::longTouchUpdate)))
			schedule(schedule_selector(OutTownLayer::longTouchUpdate), 0.1f);
	}
	else if (type == ui::Widget::TouchEventType::ENDED)
	{
		cancelLongTouch();
		addRes(clicknode);
	}
	else if (type == ui::Widget::TouchEventType::CANCELED)
	{
		cancelLongTouch();
	}
}

void OutTownLayer::addRes(Node* clicknode)
{
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

void OutTownLayer::onSubBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	Node* clicknode = (Node*)pSender;
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		m_longTouchNode = clicknode;
		if (!isScheduled(schedule_selector(OutTownLayer::longTouchUpdate)))
			schedule(schedule_selector(OutTownLayer::longTouchUpdate), 0.1f);
	}
	else if (type == ui::Widget::TouchEventType::ENDED)
	{
		cancelLongTouch();
		subRes(clicknode);
	}
	else if (type == ui::Widget::TouchEventType::CANCELED)
	{
		cancelLongTouch();
	}
}

void OutTownLayer::longTouchUpdate(float delay)
{
	m_isLongPress = true;
	if (m_longTouchNode != NULL) {
		if (m_longTouchNode->getTag() / 10000 == 1)
			addRes(m_longTouchNode);
		else if (m_longTouchNode->getTag() / 10000 == 2)
			subRes(m_longTouchNode);

	}
}

void OutTownLayer::cancelLongTouch()
{
	m_isLongPress = false;
	m_longTouchNode = NULL;
	unschedule(schedule_selector(OutTownLayer::longTouchUpdate));
}

void OutTownLayer::subRes(Node* clicknode)
{
	std::string name = clicknode->getName();
	int index = atoi(name.substr(name.length() - 1).c_str());

	if (caryycount[index] <= 0)
		return;

	caryycount[index]--;
	updateCaryyCountLbl();
}

void OutTownLayer::updateCaryyCountLbl()
{
	std::string str = StringUtils::format("%d/%d", caryycount[0] + caryycount[1] + caryycount[2], GlobalInstance::getInstance()->getTotalCarry());
	carrylbl->setString(str);

	str = StringUtils::format("%d", caryycount[0]);
	caryycountlbl[0]->setString(str);
}

void OutTownLayer::updateHeroCarry()
{
	caryycount[0] = GlobalInstance::getInstance()->getTotalCarry() - caryycount[1] - caryycount[2];
	if (caryycount[0] > MyRes::getMyResCount(carryResids[0]))
		caryycount[0] = MyRes::getMyResCount(carryResids[0]);

	updateCaryyCountLbl();
}


bool OutTownLayer::isCarryOver()
{
	int count = 0;
	for (int i = 0; i < 3; i++)
	{
		count += caryycount[i];
	}
	return count >= GlobalInstance::getInstance()->getTotalCarry();
}

void OutTownLayer::onExit()
{
	Layer::onExit();
}

CardHeroNode* OutTownLayer::getMyCardHeroNode(int index)
{
	return m_myCardHerosNode[index];
}

void OutTownLayer::onGoBuyText(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;

	if (type == ui::Widget::TouchEventType::ENDED)
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		if (g_mainScene != NULL && g_mainScene->buildingIsClickOn(6))
		{
			MovingLabel::show(ResourceLang::map_lang["unlockmain_6"]);
		}
		else
		{
			this->removeFromParentAndCleanup(true);
			MarketLayer* layer = MarketLayer::create(Building::map_buildingDatas["5market"]);
			g_mainScene->addChild(layer, 0, "5market");
			AnimationEffect::openAniEffect(layer);
		}
	}
}

void OutTownLayer::editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox)
{
	std::string countstr = StringUtils::format("%d", caryycount[0]);
	editBox->setText(countstr.c_str());
	caryycountlbl[0]->setVisible(false);
}

void OutTownLayer::editBoxEditingDidEndWithAction(cocos2d::ui::EditBox* editBox, EditBoxEndAction action)
{
	std::string edittext = editBox->getText();
	editBox->setText(" ");
	caryycountlbl[0]->setVisible(true);

	if (edittext.length() > 0)
	{
		int editcount = atoi(edittext.c_str());

		if (editcount < 0)
		{
			MovingLabel::show(ResourceLang::map_lang["inputfooderr"]);
		}
		else if (editcount > GlobalInstance::getInstance()->getTotalCarry())
		{
			MovingLabel::show(ResourceLang::map_lang["inputfoodouter"]);
		}
		else
		{
			if (editcount > MyRes::getMyResCount(carryResids[0]))
			{
				caryycount[0] = MyRes::getMyResCount(carryResids[0]);
				MovingLabel::show(ResourceLang::map_lang["inputfoodenough"]);
			}
			else
			{
				caryycount[0] = editcount;
			}
			updateCaryyCountLbl();
		}
	}
}

void OutTownLayer::editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string &text)
{

}

void OutTownLayer::editBoxReturn(cocos2d::ui::EditBox *editBox)
{

}