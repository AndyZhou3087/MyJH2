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
#include "MyMenu.h"
#include "BuyCoinLayer.h"

USING_NS_CC;

std::string carryResid = "r001";
OutTownLayer::OutTownLayer()
{
	m_isLongPress = false;
	m_longTouchNode = NULL;

	caryycount = 0;
	lastselectformation = GlobalInstance::myTakeOnFormation;
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

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("outTownLayer.csb"));
	this->addChild(csbnode);
	langtype = GlobalInstance::getInstance()->getLang();
	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("outtowntitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	scrollview = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollview");
	scrollview->setScrollBarEnabled(false);
	scrollview->setBounceEnabled(true);
	scrollview->setSwallowTouches(true);


	formationInfoNode = csbnode->getChildByName("formationnode");

	studybtn = (cocos2d::ui::Widget*)formationInfoNode->getChildByName("studybtn");
	studybtn->addTouchEventListener(CC_CALLBACK_2(OutTownLayer::onFormationClick, this));
	studybtntxt = (cocos2d::ui::ImageView*)studybtn->getChildByName("btntext");
	studybtntxt->loadTexture(ResourcePath::makeTextImgPath("learnformation_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	addFormationUi();
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

	carryoutherobox = csbnode->getChildByName("carryoutherobox");

	bigformation = csbnode->getChildByName("formation");
	bigformation->setVisible(false);

	//更换队形按钮
	changebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("changebtn");
	changebtn->setTag(1002);
	changebtn->addTouchEventListener(CC_CALLBACK_2(OutTownLayer::onBtnClick, this));

	cocos2d::ui::ImageView* changetxt = (cocos2d::ui::ImageView*)changebtn->getChildByName("text");
	changetxt->loadTexture(ResourcePath::makeTextImgPath("changequene_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* foodname = (cocos2d::ui::Text*)csbnode->getChildByName("r001box")->getChildByName("namelbl");
	foodname->setString(GlobalInstance::map_AllResources["r001"].name);

	cocos2d::ui::Text* carrytextlbl = (cocos2d::ui::Text*)csbnode->getChildByName("carrytextlbl");
	carrytextlbl->setString(ResourceLang::map_lang["carrytext"]);

	carrylbl = (cocos2d::ui::Text*)csbnode->getChildByName("carrylbl");

	changehintindex = GlobalInstance::getInstance()->createRandomNum(3);
	std::string changehintkey = StringUtils::format("changelineuphint%d", changehintindex);
	changehintlbl = (cocos2d::ui::Text*)csbnode->getChildByName("changehint");
	changehintlbl->setString(ResourceLang::map_lang[changehintkey]);

	this->schedule(schedule_selector(OutTownLayer::updateChangeHint), 3);

	cocos2d::ui::Text* hintdesc = (cocos2d::ui::Text*)csbnode->getChildByName("hintdesc");
	hintdesc->setString(ResourceLang::map_lang["outtownhintdesc"]);

	for (int i = 0; i < 6; i++)
	{
		Vec2 pos = Vec2(150 + i % 3 * 210, 705 - i / 3 * 245);
		Sprite* cardnodebg = Sprite::create(ResourcePath::makeImagePath("cardherobox_.png"));
		cardnodebg->setPosition(Vec2(pos.x, pos.y+14));
		this->addChild(cardnodebg, 0);

		m_myCardHerosNode[i] = CardHeroNode::create();
		m_myCardHerosNode[i]->setPosition(pos);
		this->addChild(m_myCardHerosNode[i], 1, i);
		m_myCardHerosNode[i]->setData(GlobalInstance::myCardHeros[i]);
	}

	std::string str = "addbtn0";

	cocos2d::ui::Button* addbtn = (cocos2d::ui::Button*)csbnode->getChildByName(str);
	addbtn->addTouchEventListener(CC_CALLBACK_2(OutTownLayer::onAddBtnClick, this));
	addbtn->setTag(10000);

	str = "subbtn0";
	cocos2d::ui::Button* subbtn = (cocos2d::ui::Button*)csbnode->getChildByName(str);
	subbtn->addTouchEventListener(CC_CALLBACK_2(OutTownLayer::onSubBtnClick, this));
	subbtn->setTag(20000);

	str = "carrycount0";
	caryycountlbl = (cocos2d::ui::Text*)csbnode->getChildByName(str);

	updateHeroCarry();

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

	selectFormation(lastselectformation);

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
					m_myCardHerosNode[clickHero]->runAction(MoveTo::create(0.2f, Vec2(150 + i % 3 * 210, /*745 + */705 - i / 3 * 245)));
					m_myCardHerosNode[clickHero]->setTag(i);
					GlobalInstance::myCardHeros[clickHero]->setPos(i + 1);
					
					m_myCardHerosNode[i]->setTag(clickHero);
					if (GlobalInstance::myCardHeros[i] != NULL)
					{
						m_myCardHerosNode[i]->runAction(MoveTo::create(0.2f, Vec2(150 + clickHero % 3 * 210, /*745 + */705 - clickHero / 3 * 245)));
						GlobalInstance::myCardHeros[i]->setPos(clickHero + 1);
					}
					else
					{
						m_myCardHerosNode[i]->setPosition(Vec2(150 + clickHero % 3 * 210, /*745 + */705 - clickHero / 3 * 245));
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
			if (!carryoutherobox->getBoundingBox().containsPoint(m_myCardHerosNode[clickHero]->getPosition()))
			{
				m_myCardHerosNode[clickHero]->setData(NULL);
				GlobalInstance::myCardHeros[clickHero]->setState(HS_OWNED);
				GlobalInstance::myCardHeros[clickHero]->setPos(0);
				GlobalInstance::myCardHeros[clickHero] = NULL;
			}
			m_myCardHerosNode[clickHero]->setPosition(Vec2(150 + clickHero % 3 * 210, /*745 + */705 - clickHero / 3 * 245));
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
			
			int addcount = caryycount;
			if (addcount != 0)
			{
				MyRes::Add(carryResid, addcount, MYPACKAGE);
				MyRes::Use(carryResid, addcount);
			}

			((cocos2d::ui::Button*)pSender)->setEnabled(false);

			GlobalInstance::getInstance()->recoveCardHeroMaxHp();
			GlobalInstance::myOutMapCarry = GlobalInstance::getInstance()->getTotalCarry();

			checkFormation();
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
	if (isCarryOver())
	{
		MovingLabel::show(ResourceLang::map_lang["carryovertext"]);
	}
	else
	{
		if (caryycount >= MyRes::getMyResCount(carryResid))
		{
			std::string str = StringUtils::format(ResourceLang::map_lang["lacktext"].c_str(), GlobalInstance::map_AllResources[carryResid].name.c_str());
			MovingLabel::show(str);
			return;
		}
		caryycount++;
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

	if (caryycount <= 0)
		return;

	caryycount--;
	updateCaryyCountLbl();
}

void OutTownLayer::updateCaryyCountLbl()
{
	std::string str = StringUtils::format("%d/%d", caryycount, GlobalInstance::getInstance()->getTotalCarry());
	carrylbl->setString(str);

	str = StringUtils::format("%d", caryycount);
	caryycountlbl->setString(str);
}

void OutTownLayer::updateHeroCarry()
{
	caryycount = GlobalInstance::getInstance()->getTotalCarry();
	if (caryycount > MyRes::getMyResCount(carryResid))
		caryycount = MyRes::getMyResCount(carryResid);

	updateCaryyCountLbl();
}


bool OutTownLayer::isCarryOver()
{
	int count = caryycount;
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

void OutTownLayer::editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox)
{
	std::string countstr = StringUtils::format("%d", caryycount);
	editBox->setText(countstr.c_str());
	caryycountlbl->setVisible(false);
}

void OutTownLayer::editBoxEditingDidEndWithAction(cocos2d::ui::EditBox* editBox, EditBoxEndAction action)
{
	std::string edittext = editBox->getText();
	editBox->setText(" ");
	caryycountlbl->setVisible(true);

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
			if (editcount > MyRes::getMyResCount(carryResid))
			{
				caryycount = MyRes::getMyResCount(carryResid);
				MovingLabel::show(ResourceLang::map_lang["inputfoodenough"]);
			}
			else
			{
				caryycount = editcount;
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

void OutTownLayer::addFormationUi()
{
	int itemwidth = 140;

	int ressize = GlobalInstance::map_formations.size();
	int innerwidth = itemwidth * ressize;

	int contentwidth = scrollview->getContentSize().width;
	if (innerwidth < contentwidth)
		innerwidth = contentwidth;
	scrollview->setInnerContainerSize(Size(innerwidth, scrollview->getContentSize().height));

	std::vector<S_FORMATION*> vec_formationdatas;
	std::map<std::string, S_FORMATION>::iterator it;

	for (it = GlobalInstance::map_formations.begin(); it != GlobalInstance::map_formations.end(); it++)
	{
		vec_formationdatas.push_back(&it->second);
		vec_formationboxs.push_back(NULL);
	}

	std::sort(vec_formationdatas.begin(), vec_formationdatas.end(), sortbylearned);

	for (int m = 0; m < ressize; m++)
	{
		std::string formationid = vec_formationdatas[m]->id;

		std::string boxstr = "ui/formationbox_n.png";

		cocos2d::ui::ImageView* box = cocos2d::ui::ImageView::create(boxstr, cocos2d::ui::Widget::TextureResType::PLIST);
		box->addTouchEventListener(CC_CALLBACK_2(OutTownLayer::onFormationClick, this));
		box->setTouchEnabled(true);
		int findex = atoi(formationid.substr(2).c_str());
		box->setTag(findex);
		//box->setUserData((void*)GlobalInstance::map_formations[formationid].id.c_str());
		box->setPosition(Vec2(box->getContentSize().width / 2 + m * itemwidth, scrollview->getContentSize().height / 2 + 10));
		scrollview->addChild(box);
		vec_formationboxs[findex - 1] = box;
		std::string iconstr = "ui/formation1_d.png";

		if (GlobalInstance::map_formations[formationid].lv >= 0)
			iconstr = "ui/formation1_n.png";
		cocos2d::ui::ImageView* icon = cocos2d::ui::ImageView::create(iconstr, cocos2d::ui::Widget::TextureResType::PLIST);
		icon->setPosition(Vec2(box->getContentSize().width / 2, box->getContentSize().height/2));
		box->addChild(icon, 0, "f");


		Label* namelbl = Label::createWithTTF(GlobalInstance::map_AllResources[formationid].name, FONT_NAME, 20);
		namelbl->setColor(Color3B(34, 74, 79));
		namelbl->setPosition(Vec2(box->getContentSize().width / 2, -5));
		box->addChild(namelbl);
	}
}

bool OutTownLayer::sortbylearned(S_FORMATION* a, S_FORMATION* b)
{
	if (a->lv > b->lv)
		return true;
	return false;
}

void OutTownLayer::onFormationClick(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type)
{

	cocos2d::ui::Widget* clicknode = (cocos2d::ui::Widget*)pSender;
	int clicktag = clicknode->getTag();
	
	if (clicktag > 2000)
		CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		clickflag = true;
		beginTouchPoint = clicknode->convertToWorldSpace(Vec2(clicknode->getPositionX(), clicknode->getPositionY()));
	}
	else if (type == ui::Widget::TouchEventType::MOVED)
	{
		Vec2 movedPoint = clicknode->convertToWorldSpace(Vec2(clicknode->getPositionX(), clicknode->getPositionY()));

		if (fabs(movedPoint.x - beginTouchPoint.x) >= CLICKOFFSETP * 2 || fabs(movedPoint.y - beginTouchPoint.y) >= CLICKOFFSETP * 2)
			clickflag = false;
	}

	else if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (!clickflag)
			return;

		if (clicktag < 2000)
		{
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);

			if (GlobalInstance::getInstance()->getUnlockChapter() >= 2)
				selectFormation(clicknode->getTag());
			else
			{
				MovingLabel::show(ResourceLang::map_lang["unlockformationhint"]);
			}
		}
		else if (clicktag < 3000)
		{
			int learnindex = clicktag % 2000;

			int needcoin = 200;

			cocos2d::ui::Text* coincountlbl = (cocos2d::ui::Text*)formationInfoNode->getChildByName("countlbl");
			std::string str = StringUtils::format("%d", needcoin);
			coincountlbl->setString(str);

			if (GlobalInstance::getInstance()->getMyCoinCount().getValue() >= needcoin)
			{
				cocos2d::ui::ImageView* boxf = (cocos2d::ui::ImageView*)vec_formationboxs[learnindex - 1]->getChildByName("f");
				boxf->loadTexture("ui/formation1_n.png", cocos2d::ui::Widget::TextureResType::PLIST);

				std::string formationid = StringUtils::format("zx%03d", learnindex);

				GlobalInstance::map_formations[formationid].lv = 0;
				GlobalInstance::getInstance()->saveMyFormation();
				updateFormationInfo(learnindex);
				DynamicValueInt dvint;
				dvint.setValue(needcoin);
				GlobalInstance::getInstance()->costMyCoinCount(dvint);
				std::string showstr = StringUtils::format(ResourceLang::map_lang["learnsucc"].c_str(), GlobalInstance::map_AllResources[formationid].name.c_str());
				MovingLabel::show(showstr);

			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["nomorecoin"]);
				Layer* layer = BuyCoinLayer::create(needcoin - GlobalInstance::getInstance()->getMyCoinCount().getValue());
				Director::getInstance()->getRunningScene()->addChild(layer, 100, "buycoinlayer");
			}
		}
		else
		{
			MovingLabel::show(ResourceLang::map_lang["formationlvupnotopen"]);
		}
	}
}

void OutTownLayer::selectFormation(int index)
{
	if (index > 0)
	{
		if (lastselectformation > 0)
			vec_formationboxs[lastselectformation - 1]->loadTexture("ui/formationbox_n.png", cocos2d::ui::Widget::TextureResType::PLIST);

		vec_formationboxs[index - 1]->loadTexture("ui/formationbox_s.png", cocos2d::ui::Widget::TextureResType::PLIST);

		lastselectformation = index;

		std::string formationid = StringUtils::format("zx%03d", index);
		if (GlobalInstance::map_formations[formationid].lv >= 0)
		{
			takeOnFormation(index);
		}

	}
	else if (lastselectformation > 0)
	{
		vec_formationboxs[lastselectformation - 1]->loadTexture("ui/formationbox_n.png", cocos2d::ui::Widget::TextureResType::PLIST);
	}
	updateFormationInfo(index);

}

void OutTownLayer::updateFormationInfo(int index)
{
	std::string formationid = StringUtils::format("zx%03d", index);

	cocos2d::ui::Text* fname = (cocos2d::ui::Text*)formationInfoNode->getChildByName("fname");

	cocos2d::ui::Text* fdesc = (cocos2d::ui::Text*)formationInfoNode->getChildByName("desc");

	Label* desclbl = (Label*)fdesc->getVirtualRenderer();
	desclbl->setLineSpacing(10);

	cocos2d::ui::Text* table = (cocos2d::ui::Text*)formationInfoNode->getChildByName("formationinfotable");

	if (index == 0)
	{
		fname->setString(ResourceLang::map_lang["npcrelation_0"]);
		fdesc->setString(ResourceLang::map_lang["npcrelation_0"]);
		table->setVisible(false);

		studybtn->setVisible(false);
		formationInfoNode->getChildByName("countlbl")->setVisible(false);
		formationInfoNode->getChildByName("coin")->setVisible(false);
	}
	else
	{
		

		fname->setString(GlobalInstance::map_AllResources[formationid].name);

		if (GlobalInstance::map_formations[formationid].lv >= 0)
		{
			studybtn->setTag(3000 + index);
			studybtn->setVisible(true);
			studybtntxt->loadTexture(ResourcePath::makeTextImgPath("lvupbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

			table->setVisible(true);

			std::string descstr = GlobalInstance::map_AllResources[formationid].desc;
			for (unsigned int i = 0; i < GlobalInstance::map_formations[formationid].vec_addattr.size(); i++)
			{
				float attrval = GlobalInstance::map_formations[formationid].vec_addattr[i];

				if (attrval > 0.00001)
				{
					std::string astr = StringUtils::format("%.2f", attrval * 100);
					CommonFuncs::replace(descstr, "#", astr);
				}
			}
			fdesc->setString(descstr);
			formationInfoNode->getChildByName("countlbl")->setVisible(false);
			formationInfoNode->getChildByName("coin")->setVisible(false);
		}
		else
		{
			studybtn->setTag(2000 + index);
			studybtn->setVisible(true);
			studybtntxt->loadTexture(ResourcePath::makeTextImgPath("learnformation_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			table->setVisible(false);

			fdesc->setString(ResourceLang::map_lang["notlearntext"]);
			formationInfoNode->getChildByName("countlbl")->setVisible(studybtn->isVisible());
			formationInfoNode->getChildByName("coin")->setVisible(studybtn->isVisible());
		}

		for (int i = 0; i < 6; i++)
		{
			std::string herokey = StringUtils::format("vocationbox%d", i);

			if (index > 0)
			{
				std::string formationid = StringUtils::format("zx%03d", index);
				cocos2d::ui::ImageView* headimg = (cocos2d::ui::ImageView*)table->getChildByName(herokey)->getChildByName("v");
				herokey = StringUtils::format("ui/cardvocation%d.png", GlobalInstance::map_formations[formationid].vec_formation[i] - 1);
				headimg->loadTexture(herokey, cocos2d::ui::Widget::TextureResType::PLIST);
			}

			std::string namekey = StringUtils::format("vocname%d", i);

			if (index > 0)
			{
				std::string formationid = StringUtils::format("zx%03d", index);
				cocos2d::ui::Text* herovocname = (cocos2d::ui::Text*)table->getChildByName(namekey);
				namekey = StringUtils::format("vocation_%d", GlobalInstance::map_formations[formationid].vec_formation[i] - 1);
				herovocname->setString(ResourceLang::map_lang[namekey]);
			}
		}
	}

	//formationInfoNode->getChildByName("countlbl")->setVisible(studybtn->isVisible());
	//formationInfoNode->getChildByName("coin")->setVisible(studybtn->isVisible());
}

void OutTownLayer::updateChangeHint(float dt)
{
	changehintindex++;
	if (changehintindex > 2)
		changehintindex = 0;
	std::string changehintkey = StringUtils::format("changelineuphint%d", changehintindex);
	changehintlbl->setString(ResourceLang::map_lang[changehintkey]);
}

void OutTownLayer::checkFormation()
{
	GlobalInstance::myTakeOnFormation = 0;
	std::string matchformation;
	std::map<std::string, S_FORMATION>::iterator it;
	for (it = GlobalInstance::map_formations.begin(); it != GlobalInstance::map_formations.end(); it++)
	{
		int matchcount = 0;
		for (int i = 0; i < 6; i++)
		{
			if (GlobalInstance::myCardHeros[i] != NULL && GlobalInstance::myCardHeros[i]->getVocation() + 1 == it->second.vec_formation[i])
			{
				matchcount++;
			}
		}
		if (matchcount >= 6)
		{
			matchformation = it->first;
			break;
		}
	}

	if (matchformation.length() > 0)
	{
		int takeonf = atoi(matchformation.substr(2).c_str());
		if (GlobalInstance::map_formations[matchformation].lv >= 0)
		{
			GlobalInstance::myTakeOnFormation = takeonf;
		}
	}
	else
	{
		bigformation->stopAllActions();
		bigformation->setVisible(false);
	}
	selectFormation(GlobalInstance::myTakeOnFormation);
	GlobalInstance::getInstance()->saveMyFormation();
}

void OutTownLayer::takeOnFormation(int formationindex)
{
	vec_formationCandidate.clear();
	vec_selformation.clear();

	std::string formationid = StringUtils::format("zx%03d", formationindex);

	for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
	{
		for (unsigned int j = 0; j < GlobalInstance::map_formations[formationid].vec_formation.size(); j++)
		{
			if (GlobalInstance::vec_myHeros[i]->getVocation() + 1 == GlobalInstance::map_formations[formationid].vec_formation[j])
			{
				vec_formationCandidate.push_back(GlobalInstance::vec_myHeros[i]);
				break;
			}
		}
	}
	std::sort(vec_formationCandidate.begin(), vec_formationCandidate.end(), OutTownLayer::formationCandidateSort);


	std::vector<Hero*>::iterator it;
	for (unsigned int j = 0; j < GlobalInstance::map_formations[formationid].vec_formation.size(); j++)
	{
		for (it = vec_formationCandidate.begin(); it != vec_formationCandidate.end();)
		{
			Hero* hero = *it;
	
			if (hero->getVocation() + 1 == GlobalInstance::map_formations[formationid].vec_formation[j])
			{
				vec_selformation.push_back(hero);
				it = vec_formationCandidate.erase(it);
				break;
			}

			else
			{
				it++;
			}
		}
	}

	if (vec_selformation.size() >= 6)
	{
		for (int i = 0; i < 6; i++)
		{
			if (GlobalInstance::myCardHeros[i] != NULL)
			{
				GlobalInstance::myCardHeros[i]->setState(HS_OWNED);
				GlobalInstance::myCardHeros[i]->setPos(0);
				GlobalInstance::myCardHeros[i] = NULL;
			}
			GlobalInstance::myCardHeros[i] = vec_selformation[i];
			GlobalInstance::myCardHeros[i]->setState(HS_TAKEON);
			GlobalInstance::myCardHeros[i]->setPos(i + 1);
			m_myCardHerosNode[i]->setData(GlobalInstance::myCardHeros[i]);
		}
		bigformation->stopAllActions();
		bigformation->setOpacity(255);
		bigformation->setVisible(true);
		bigformation->runAction(Repeat::create(Sequence::create(FadeIn::create(0.5), FadeOut::create(0.5), FadeIn::create(0.5), NULL), 3));
	}
	else
	{
		std::string str = StringUtils::format(ResourceLang::map_lang["noformation"].c_str(), GlobalInstance::map_AllResources[formationid].name.c_str());
		MovingLabel::show(str);
	}
}

bool OutTownLayer::formationCandidateSort(Hero* a, Hero* b)
{
	if (a->getPotential() * 10000 + a->getLevel() > b->getPotential() * 10000 + b->getLevel())
	{
		return true;
	}
	return false;
}