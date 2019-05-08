#include "HeroAttrLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "RandHeroNode.h"
#include "InnRoomLayer.h"
#include "MainScene.h"
#include "MovingLabel.h"
#include "OutTownLayer.h"
#include "SelectMyHerosLayer.h"
#include "MapBlockScene.h"
#include "SelectEquipLayer.h"
#include "Equipable.h"
#include "MyRes.h"
#include "TakeOnLayer.h"
#include "EquipDescLayer.h"
#include "ChangeVocationLayer.h"
#include "AnimationEffect.h"
#include "SoundManager.h"
#include "SimplePopLayer.h"
#include "NewGuideLayer.h"
#include "LoadingBarProgressTimer.h"
#include "HintBoxLayer.h"
#include "Const.h"
#include "WaitingProgress.h"
#include "BuyResLayer.h"
#include "PopNewHeroLayer.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "iosfunc.h"
#endif

USING_NS_CC;

int equiptype[] = { T_ARMOR, T_EQUIP, T_WG, T_NG, T_HANDARMOR, T_FASHION };

#define S001EXP 1500
#define S002EXP 5000
#define S003EXP 15000
#define S004EXP 50000

int recuitCostsilver[] = { 500, 1000, 2000, 3000, 10000 };

HeroAttrLayer::HeroAttrLayer()
{
	isMovingAction = false;
	m_isLongPress = false;
	m_longTouchNode = NULL;
	isCanClickFullHero = true;
	redtip = NULL;
	pageMoveClickIndex = 0;
	isDraging = false;
	for (int i=0;i<6;i++)
		effectnode[i] = NULL;
}

HeroAttrLayer::~HeroAttrLayer()
{
	//保存数据
	GlobalInstance::getInstance()->saveMyHeros();
	MyRes::saveData();
}


HeroAttrLayer* HeroAttrLayer::create(Hero* herodata, int fromwhere, int clickwhere)
{
	HeroAttrLayer *pRet = new(std::nothrow)HeroAttrLayer();
	if (pRet && pRet->init(herodata, fromwhere, clickwhere))
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
bool HeroAttrLayer::init(Hero* herodata, int fromwhere, int clickwhere)
{
    if ( !Layer::init() )
    {
        return false;
    }
	m_fromwhere = fromwhere;
	m_heroData = herodata;

	lastVaction = herodata->getVocation();

	lastLv = herodata->getLevel();

	lastexp = herodata->getExp().getValue();

	breakcount = herodata->getChangeCount();

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 250));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("heroAttrLayer.csb"));
	this->addChild(csbnode);
	langtype = GlobalInstance::getInstance()->getLang();

	cantclick = (cocos2d::ui::ImageView*)csbnode->getChildByName("cantclick");
	cantclick->setSwallowTouches(true);
	//cantclick->setVisible(false);

	cocos2d::ui::Button* hintbtn = (cocos2d::ui::Button*)csbnode->getChildByName("hintbtn");
	hintbtn->addTouchEventListener(CC_CALLBACK_2(HeroAttrLayer::onHeroHintClick, this));
	hintbtn->setSwallowTouches(true);

	//升级栏
	lvnode = csbnode->getChildByName("lvnode");
	lvnode->setVisible(false);

	addexplbl = (cocos2d::ui::Text*)lvnode->getChildByName("addexpdesc");

	for (int i = 1; i < 5; i++)
	{
		std::string str = StringUtils::format("s00%dimg", i);
		cocos2d::ui::ImageView* goodimg = (cocos2d::ui::ImageView*)lvnode->getChildByName(str);
		goodimg->addTouchEventListener(CC_CALLBACK_2(HeroAttrLayer::onGoodsClick, this));
		goodimg->setTag(i);

		str = StringUtils::format("s00%dtext", i);
		goodarr[i - 1] = (cocos2d::ui::Text*)lvnode->getChildByName(str);
		str = StringUtils::format("s00%d", i);
		str = StringUtils::format("%d", MyRes::getMyResCount(str));
		goodarr[i - 1]->setString(str);
	}

	//装备栏
	equipnode = csbnode->getChildByName("equipnode");
	int rc = 0;
	for (int i = 0; i < equipnode->getChildrenCount(); i++)
	{
		cocos2d::ui::Widget* node = (cocos2d::ui::Widget*)equipnode->getChildren().at(i);
		node->setTag(i);
		node->addTouchEventListener(CC_CALLBACK_2(HeroAttrLayer::onEquipClick, this));
		redpointArr[i] = (cocos2d::ui::Widget*)node->getChildByName("redpoint");
	}
	//属性信息
	heroattrbottom = csbnode->getChildByName("heroattrbottom");
	moditybtn = (cocos2d::ui::Widget*)heroattrbottom->getChildByName("moditybtn");
	heroattrnamebox = (cocos2d::ui::Widget*)heroattrbottom->getChildByName("heroattrnamebox");

	//品质
	heroattrqu = (cocos2d::ui::ImageView*)heroattrbottom->getChildByName("heroattrqu");

	//名字TextField控件效果不好，改用EditBox
	cocos2d::ui::TextField* nameinput = (cocos2d::ui::TextField*)heroattrbottom->getChildByName("nameinput");
	nameinput->setVisible(false);

	m_editName = cocos2d::ui::EditBox::create(nameinput->getContentSize(), cocos2d::ui::Scale9Sprite::createWithSpriteFrameName(ResourcePath::makePath("ui/blank.png")));
	m_editName->setPosition(nameinput->getPosition());
	m_editName->setFontColor(Color4B(26, 68, 101, 255));
	m_editName->setInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
	m_editName->setMaxLength(10);
	m_editName->setFontSize(nameinput->getFontSize());
	//m_editName->setText(herodata->getName().c_str());
	//editName->setReturnType(EditBox::KeyboardReturnType::DONE);
	m_editName->setDelegate(this);
	heroattrbottom->addChild(m_editName);

	//角色职业
	vocation = (cocos2d::ui::Text*)heroattrbottom->getChildByName("vocation");

	//角色属性文字
	std::string str;
	for (int i = 0; i <= 5; i++)
	{
		str = StringUtils::format("attrtext_%d", i);
		cocos2d::ui::Text* attrtext = (cocos2d::ui::Text*)heroattrbottom->getChildByName(str);
		attrtext->setString(ResourceLang::map_lang[str]);
	}

	//生命值
	hplbl = (cocos2d::ui::Text*)heroattrbottom->getChildByName("hp");

	//int hp = herodata->getHp();

	//std::string attrstr = StringUtils::format("%d/%d", hp, (int)herodata->getMaxHp());
	//hplbl->setString(attrstr);

	//攻击值
	atkbl = (cocos2d::ui::Text*)heroattrbottom->getChildByName("atk");
	//attrstr = StringUtils::format("%d", (int)herodata->getAtk());
	//atkbl->setString(attrstr);

	//防御值
	dflbl = (cocos2d::ui::Text*)heroattrbottom->getChildByName("df");
	//attrstr = StringUtils::format("%d", (int)herodata->getDf());
	//dflbl->setString(attrstr);

	//攻击速度值
	atkspeedlbl = (cocos2d::ui::Text*)heroattrbottom->getChildByName("atkspeed");
	//attrstr = StringUtils::format("%.3f", 1.0f/herodata->getAtkSpeed());
	//atkspeedlbl->setString(attrstr);

	//暴击值
	critlbl = (cocos2d::ui::Text*)heroattrbottom->getChildByName("crit");
	//attrstr = StringUtils::format("%.3f%%", herodata->getCrit());
	//critlbl->setString(attrstr);

	//闪避值
	dodgelbl = (cocos2d::ui::Text*)heroattrbottom->getChildByName("dodge");
	//attrstr = StringUtils::format("%.3f%%", herodata->getDodge());
	//dodgelbl->setString(attrstr);

	//等级
	lvUIText = (cocos2d::ui::Text*)heroattrbottom->getChildByName("lv");
	str = StringUtils::format("Lv.%d", herodata->getLevel() + 1);
	lvUIText->setString(str);

	//经验值
	explbl = (cocos2d::ui::Text*)heroattrbottom->getChildByName("exp");

	expbar = (cocos2d::ui::LoadingBar*)heroattrbottom->getChildByName("heroattrexpbar");
	expbar->setPercent(0);

	//按钮
	std::string btnname[] = { "firebtn", "changebtn", "backbtn", "recruitbtn"};//与BTNTYPE对应
	for (int i = 0; i < sizeof(btnname) / sizeof(btnname[0]); i++)
	{
		int tag = i + ATTR_FIREBTN;
		cocos2d::ui::Button* btn = (cocos2d::ui::Button*)heroattrbottom->getChildByName(btnname[i]);
		btn->setTag(tag);
		btn->addTouchEventListener(CC_CALLBACK_2(HeroAttrLayer::onBtnClick, this));
		btnArr[i] = btn;
		if (i == 1)
		{
			redtip = (cocos2d::ui::Widget*)btn->getChildByName("redtip");
		}
	}
	//设置ui
	loadHeroUI();
	for (int i = 0; i < sizeof(btnname) / sizeof(btnname[0]); i++)
	{
		int tag = i + ATTR_FIREBTN;
		cocos2d::ui::Button* btn = (cocos2d::ui::Button*)heroattrbottom->getChildByName(btnname[i]);
		if (m_fromwhere != 0)
		{
			if (tag == ATTR_BACKBTN)
				btn->setPositionX(360);
			else
				btn->setVisible(false);
		}
	}

	if (g_mainScene->getChildByName("0outtown") != NULL)
		clickwhere = 3;
	else if (g_mainScene->getChildByName("8pkground") != NULL)
		clickwhere = 4;

	m_clickhere = clickwhere;

	if (clickwhere == 3 || clickwhere == 4)
	{
		btnArr[0]->setVisible(false);
		btnArr[1]->setVisible(true);
		btnArr[1]->setPositionX(220);
		btnArr[2]->setPositionX(500);
	}

	//滑动
	pageView = (cocos2d::ui::PageView*)csbnode->getChildByName("pageView");
	pageView->addEventListener((ui::PageView::ccPageViewCallback)CC_CALLBACK_2(HeroAttrLayer::JumpSceneCallback, this));
	pageView->addTouchEventListener(CC_CALLBACK_2(HeroAttrLayer::onHeroFullClick, this));
	pageView->setSwallowTouches(false);
	if (fromwhere != 0)
	{
		pageView->setEnabled(false);
	}

	int pageIndex = 0;
	if (clickwhere != 0)
	{
		if (fromwhere == 0)
		{
			for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
			{
				if (GlobalInstance::vec_myHeros[i]->getState() != HS_DEAD)
				{
					vec_norheros.push_back(GlobalInstance::vec_myHeros[i]);
				}
			}
		}
		else if (fromwhere == 4)//竞技场看阵容
		{
			for (int i = 0; i < 6; i++)
			{
				if (GlobalInstance::matchPairHeros[i] != NULL)
				{
					vec_norheros.push_back(GlobalInstance::matchPairHeros[i]);
				}
			}
			for (int i = 0; i < equipnode->getChildrenCount(); i++)
			{
				cocos2d::ui::Widget* node = (cocos2d::ui::Widget*)equipnode->getChildren().at(i);
				node->setEnabled(false);
				redpointArr[i]->setOpacity(0);
			}
			moditybtn->setVisible(false);
			m_editName->setEnabled(false);
			pageView->setEnabled(true);
		}
		else
		{
			for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
			{
				vec_norheros.push_back(GlobalInstance::vec_myHeros[i]);
			}
		}
	}
	else if (clickwhere == 0)
	{
		for (unsigned int i = 0; i < GlobalInstance::vec_rand3Heros.size(); i++)
		{
			vec_norheros.push_back(GlobalInstance::vec_rand3Heros[i]);
		}
	}

	for (unsigned int i = 0; i < vec_norheros.size(); i++)
	{
		cocos2d::ui::ImageView* imageView = cocos2d::ui::ImageView::create();
		imageView->ignoreContentAdaptWithSize(true);
		std::string fullimgstr = StringUtils::format("hfull_%d_%d.png", vec_norheros[i]->getVocation(), vec_norheros[i]->getSex());
		imageView->loadTexture(ResourcePath::makeImagePath(fullimgstr), cocos2d::ui::Widget::TextureResType::LOCAL);
		auto layout = cocos2d::ui::Layout::create();
		layout->setContentSize(pageView->getContentSize());

		imageView->setPosition(Vec2(layout->getContentSize().width / 2, layout->getContentSize().height / 2));
		layout->addChild(imageView, 0, "fullimg");

		pageView->addPage(layout);
		if (herodata->getName().compare(vec_norheros[i]->getName()) == 0)
		{
			pageIndex = i;
		}
	}
	if (clickwhere == 2)
	{
		pageIndex = vec_norheros.size() - 1;
	}
	pageView->setCurrentPageIndex(pageIndex);


	this->schedule(schedule_selector(HeroAttrLayer::updataAtrrUI), 1.0f);

	this->scheduleOnce(schedule_selector(HeroAttrLayer::delayShowNewerGuide), newguidetime);

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch * touch, Event * event)
	{
		isDraging = false;
		m_startClickX = touch->getLocation().x;
		m_startClickY = touch->getLocation().y;
		return true;
	};

	listener->onTouchMoved = [=](Touch * touch, Event * event)
	{
		if (fabsf(m_startClickX - touch->getLocation().x) > CLICKOFFSETP || fabsf(m_startClickY - touch->getLocation().y) > CLICKOFFSETP)
		{
			isDraging = true;
		}
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

void HeroAttrLayer::delayShowNewerGuide(float dt)
{
	if (!NewGuideLayer::checkifNewerGuide(24))
	{
		if (NewGuideLayer::checkifNewerGuide(THRIDGUIDESTEP))
		{
			if (NewGuideLayer::checkifNewerGuide(25))
			{
				showNewerGuide(25);
			}
			else if (NewGuideLayer::checkifNewerGuide(27))
			{
				showNewerGuide(27);
			}
			else if (NewGuideLayer::checkifNewerGuide(29))
			{
				showNewerGuide(29);
			}
			else if (NewGuideLayer::checkifNewerGuide(10))
			{
				showNewerGuide(10);
			}
			else if (NewGuideLayer::checkifNewerGuide(32))
			{
				showNewerGuide(32);
			}
			else if (NewGuideLayer::checkifNewerGuide(35))
			{
				showNewerGuide(35);
			}
			else if (NewGuideLayer::checkifNewerGuide(93))
			{
				showNewerGuide(93);
			}
			else if (NewGuideLayer::checkifNewerGuide(38))
			{
				showNewerGuide(38);
			}
		}
		else if (GlobalInstance::getInstance()->getHerosChangeLevelCount() > 0 && !NewGuideLayer::checkifNewerGuide(67))
		{
			if (NewGuideLayer::checkifNewerGuide(71))
			{
				showNewerGuide(71);
			}
		}
		else if (!NewGuideLayer::checkifNewerGuide(78))
		{
			if (NewGuideLayer::checkifNewerGuide(79))
			{
				showNewerGuide(79);
			}
		}
	}
}

void HeroAttrLayer::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	if (step == 25 || step == 27 || step == 29)
	{
		nodes.push_back(btnArr[3]);
	}
	else if (step == 10)
	{
		nodes.push_back(equipnode->getChildren().at(1));
	}
	else if (step == 32)
	{
		nodes.push_back(equipnode->getChildren().at(2));
	}
	else if (step == 35)
	{
		nodes.push_back(equipnode->getChildren().at(3));
	}
	else if (step == 38)
	{
		nodes.push_back(heroattrbottom->getChildByName("backbtn"));
	}
	else if (step == 71)
	{
		nodes.push_back(heroattrbottom->getChildByName("changebtn"));
	}
	else if (step == 79)
	{
		nodes.push_back(equipnode->getChildren().at(0));
	}
	else if (step == 93)
	{
		nodes.push_back(m_editName);
	}
	g_mainScene->showNewerGuideNode(step, nodes);
}

void HeroAttrLayer::updateAtrBtnUI()
{
	if (equipnode->isVisible())
	{
		btnArr[0]->setVisible(false);
		btnArr[1]->setVisible(false);
		btnArr[2]->setPositionX(360);
		btnArr[3]->setVisible(false);
	}
	else
	{
		int herostate = m_heroData->getState();

		if (herostate == HS_TRAINING)
		{
			btnArr[0]->setVisible(false);
			btnArr[1]->setVisible(true);
			btnArr[1]->setPositionX(220);
			btnArr[2]->setPositionX(500);
		}
		else if (herostate == HS_OWNED || herostate == HS_TAKEON)
		{
			if (m_clickhere == 3 || m_clickhere == 4)
			{
				btnArr[0]->setVisible(false);
				btnArr[1]->setVisible(true);
				btnArr[1]->setPositionX(220);
				btnArr[2]->setPositionX(500);
			}
			else
			{
				btnArr[0]->setVisible(true);
				btnArr[1]->setVisible(true);
				btnArr[2]->setPositionX(600);
			}
		}
		btnArr[3]->setVisible(false);
	}
	
}

void HeroAttrLayer::loadHeroUI()
{
	for (int i = 0; i < equipnode->getChildrenCount(); i++)
	{
		ResBase* eres = m_heroData->getEquipable(equiptype[i]);
		updateEquipUi(eres, i);
	}

	//品质
	std::string str = StringUtils::format("heroattrqu_%d", m_heroData->getPotential());
	heroattrqu->loadTexture(ResourcePath::makeTextImgPath(str, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//名字EditBox
	m_editName->setText(m_heroData->getName().c_str());

	int herostate = m_heroData->getState();
	if (herostate == HS_READY)
	{
		equipnode->setVisible(false);
		moditybtn->setVisible(false);
		m_editName->setEnabled(false);
	}

	//角色职业
	str = StringUtils::format("vocation_%d", m_heroData->getVocation());
	vocation->setString(ResourceLang::map_lang[str]);

	//按钮
	std::string btnname[] = { "firebtn", "changebtn", "backbtn", "recruitbtn" };//与BTNTYPE对应
	for (int i = 0; i < sizeof(btnname) / sizeof(btnname[0]); i++)
	{
		int tag = i + ATTR_FIREBTN;
		cocos2d::ui::Button* btn = btnArr[i];

		if (tag == ATTR_FIREBTN)
		{
			cocos2d::ui::ImageView* txtimg = (cocos2d::ui::ImageView*)btn->getChildByName("text");
			txtimg->loadTexture(ResourcePath::makeTextImgPath("firebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			btn->setPositionX(128);
			if (herostate == HS_READY || herostate == HS_TRAINING)
			{
				btn->setVisible(false);
			}
			else if (herostate == HS_OWNED)
			{
				btn->setVisible(true);
			}
		}
		else if (tag == ATTR_CHANGEBTN)
		{
			cocos2d::ui::ImageView* txtimg = (cocos2d::ui::ImageView*)btn->getChildByName("text");
			txtimg->loadTexture(ResourcePath::makeTextImgPath("lvupbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			btn->setPositionX(363);
			if (herostate == HS_READY || herostate == HS_TRAINING)
			{
				btn->setVisible(false);
			}
			else if (herostate == HS_OWNED || herostate == HS_TAKEON)
			{
				//前4种职业等级10可转职，后面可突破
				if ((m_heroData->getLevel() + 1) % 10 == 0 && (m_heroData->getVocation() < 4 || (m_heroData->getLevel() + 1) / 10 == m_heroData->getChangeCount() + 1))
				{
					if (m_heroData->getLevel() + 1 != 10)
					{
						txtimg->loadTexture(ResourcePath::makeTextImgPath("break_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
					}
					else
					{
						txtimg->loadTexture(ResourcePath::makeTextImgPath("changebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
					}
				}
				if (m_heroData->getExp().getValue() >= GlobalInstance::vec_herosAttr[m_heroData->getVocation()].vec_exp[m_heroData->getMaxLevel() - 1])
				{
					btn->setEnabled(false);
				}
			}
		}
		else if (tag == ATTR_RECRUITBTN)
		{
			cocos2d::ui::ImageView* txtimg = (cocos2d::ui::ImageView*)btn->getChildByName("text");
			txtimg->loadTexture(ResourcePath::makeTextImgPath("recruitbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			btn->setPositionX(204);
			if (herostate == HS_READY)
			{
				btn->setVisible(true);
			}
			else
			{
				btn->setVisible(false);
			}
		}
		else if (tag == ATTR_BACKBTN)
		{
			cocos2d::ui::ImageView* txtimg = (cocos2d::ui::ImageView*)btn->getChildByName("text");
			txtimg->loadTexture(ResourcePath::makeTextImgPath("backbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			btn->setPositionX(500);
			if (herostate == HS_READY)
			{
				btn->setPositionX(500);
			}
			else if (herostate == HS_OWNED || herostate == HS_TAKEON)
			{
				btn->setPositionX(600);
			}
			else if (herostate == HS_TRAINING)
			{
				btn->setPositionX(360);
			}
		}
	}
	updataAtrrUI(0);
}

void HeroAttrLayer::JumpSceneCallback(cocos2d::Ref* pScene, cocos2d::ui::PageView::EventType type)
{
	if (type == ui::PageView::EventType::TURNING)
	{
		cocos2d::ui::PageView * m_pageView = dynamic_cast<cocos2d::ui::PageView *>(pScene);
		int defaultindex = m_pageView->getCurrentPageIndex();

		/*heroattrbottom->stopAllActions();
		equipnode->stopAllActions();
		heroattrbottom->runAction(Sequence::create(MoveTo::create(0.3f, Vec2(0, 0)),CallFunc::create(CC_CALLBACK_0(HeroAttrLayer::finishMovingAction, this)), NULL));
		equipnode->runAction(MoveTo::create(0.3f, Vec2(360, 490)));*/
		cantclick->setVisible(true);
		for (int i = 0; i < equipnode->getChildrenCount(); i++)
		{
			if (effectnode[i] != NULL)
			{
				effectnode[i]->removeFromParentAndCleanup(true);
				effectnode[i] = NULL;
			}
		}

		m_heroData = vec_norheros[defaultindex];

		loadHeroUI();
	}
}

void HeroAttrLayer::editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox)
{
	lastchangedname = editBox->getText();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	if (equipnode->isVisible())
	{
		equipnode->setPositionY(690);
		heroattrbottom->setPositionY(200);
	}
#endif
}

void HeroAttrLayer::editBoxEditingDidEndWithAction(cocos2d::ui::EditBox* editBox, EditBoxEndAction action)
{
	std::string editstr = editBox->getText();
	if (editstr.length() == 0)
	{
		editBox->setText(m_heroData->getName().c_str());
		MovingLabel::show(ResourceLang::map_lang["nicknameempty"]);
	}
	else if (std::string::npos != editstr.find("-") || std::string::npos != editstr.find(";"))
	{
		editBox->setText(m_heroData->getName().c_str());
		MovingLabel::show(ResourceLang::map_lang["errname"]);
	}
	else if (m_heroData->getName().compare(editstr) == 0)
	{

	}
	else if (GlobalInstance::getInstance()->checkifSameName(editBox->getText()))
	{
		editBox->setText(m_heroData->getName().c_str());
		MovingLabel::show(ResourceLang::map_lang["samename"]);
	}
	else
	{
		std::string utf8str = editBox->getText();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		utf8str = gbkToUTF8(editBox->getText());
#endif
		modifyName(utf8str);
	}

	if (equipnode->isVisible())
	{
		equipnode->setPositionY(490);
		heroattrbottom->setPositionY(0);
	}

	//引导改名
	if (!NewGuideLayer::checkifNewerGuide(93))
	{
		delayShowNewerGuide(0);
	}
}

void HeroAttrLayer::editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string &text)
{
	int encount = 0;
	int cncount = 0;
	CommonFuncs::checkEnAndCnCount(text, encount, cncount);
	if (encount + cncount * 2 > editBox->getMaxLength())
	{
		editBox->setText(lastchangedname.c_str());
	}
	else
	{
		lastchangedname = text;
	}

}

void HeroAttrLayer::editBoxReturn(cocos2d::ui::EditBox *editBox)
{
	if (equipnode->isVisible())
	{
		equipnode->setPositionY(490);
		heroattrbottom->setPositionY(0);
		isCanClickFullHero = false;
	}
}

void HeroAttrLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	cocos2d::ui::Button* clicknode = (cocos2d::ui::Button*)pSender;

	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		int tag = clicknode->getTag();
		switch (tag)
		{
		case ATTR_FIREBTN:
		{
			if (m_heroData->getPotential() >= 2)
			{
				std::string potentialstr = StringUtils::format("potential_%d", m_heroData->getPotential());
				std::string hintstr = StringUtils::format(ResourceLang::map_lang["firecomfirmtext"].c_str(), ResourceLang::map_lang[potentialstr].c_str());
				HintBoxLayer* hint = HintBoxLayer::create(hintstr, 2);
				this->addChild(hint, 0, this->getTag());
				AnimationEffect::openAniEffect(hint);
			}
			else
			{
				fireHero();
			}

			break;
		}
		case ATTR_CHANGEBTN:
			if ((m_heroData->getLevel() + 1) % 10 == 0 && (m_heroData->getVocation() < 4 || (m_heroData->getLevel() + 1) / 10 == m_heroData->getChangeCount() + 1))
			{
				int forwhere = 0;
				if (m_heroData->getLevel() + 1 == 10)
				{
					forwhere = 0;
				}
				else
				{
					forwhere = 1;
				}
				ChangeVocationLayer* clayer = ChangeVocationLayer::create(m_heroData, forwhere);
				this->addChild(clayer);
				AnimationEffect::openAniEffect(clayer);
			}
			else
			{
				updateAtrBtnUI();
				lvnode->setVisible(true);
				addexplbl->runAction(RepeatForever::create(Sequence::create(FadeOut::create(1), FadeIn::create(1), NULL)));
				equipnode->setVisible(false);
				if (m_fromwhere == 0)
				{
					pageView->setEnabled(false);
				}
			}
			break;
		case ATTR_RECRUITBTN:
		{
			int liveherocount = 0;
			int herosize = GlobalInstance::vec_myHeros.size();
			for (int i = 0; i < herosize; i++)
			{
				if (GlobalInstance::vec_myHeros[i]->getState() != HS_DEAD)
					liveherocount++;
			}
			if (liveherocount < (10 + Building::map_buildingDatas["6innroom"]->level.getValue()))
			{
				std::string st = StringUtils::format("potential_%d", m_heroData->getPotential());

				std::string str = StringUtils::format(ResourceLang::map_lang["silverrecuithero"].c_str(), recuitCostsilver[m_heroData->getPotential()], ResourceLang::map_lang[st].c_str(), m_heroData->getName().c_str());
				str = CommonFuncs::replace_all(str, "\\n", "\n");
				HintBoxLayer* hlayer = HintBoxLayer::create(str, 12);
				this->addChild(hlayer);
				AnimationEffect::openAniEffect(hlayer);
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["myheromax"]);
			}
		}
		break;
		case ATTR_BACKBTN:
			if (lvnode->isVisible())
			{
				updateAtrBtnUI();
				lvnode->setVisible(false);
				addexplbl->stopAllActions();
				addexplbl->setOpacity(255);
				equipnode->setVisible(true);
				if (m_fromwhere == 0)
				{
					pageView->setEnabled(true);
				}
			}
			else
			{
				AnimationEffect::closeAniEffect((Layer*)this);
			}
			break;
		default:
			break;
		}
	}
}

void HeroAttrLayer::recruitHero()
{
	if (GlobalInstance::getInstance()->getMySoliverCount().getValue() >= recuitCostsilver[m_heroData->getPotential()])
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_RECRUITSUCC);
		m_heroData->setState(HS_OWNED);
		//加入到我的英雄列表
		Hero* myhero = new Hero(m_heroData);

		std::string heroid = StringUtils::format("%d%02d", GlobalInstance::getInstance()->getSysSecTime(), GlobalInstance::getInstance()->createRandomNum(100));

		myhero->setId(heroid);

		GlobalInstance::vec_myHeros.push_back(myhero);
		//保存数据
		GlobalInstance::getInstance()->saveMyHeros();
		GlobalInstance::getInstance()->saveRand3Heros();
		//InnRoomLayer* innroomLayer = (InnRoomLayer*)g_mainScene->getChildByName("6innroom");
		RandHeroNode* heroNode = (RandHeroNode*)this->getParent()->getChildByTag(this->getTag());
		heroNode->markRecruited(true);
		//innroomLayer->refreshMyHerosUi();
		btnArr[3]->setEnabled(false);

		DynamicValueInt constdv;
		constdv.setValue(recuitCostsilver[m_heroData->getPotential()]);
		GlobalInstance::getInstance()->costMySoliverCount(constdv);

		CommonFuncs::playCommonLvUpAnim(this->getParent(), "texiao_zmcg");

		AnimationEffect::closeAniEffect(this);
		GlobalInstance::isHasNewhero = true;

		if (g_mainScene != NULL && m_heroData->getPotential() >= 3)
		{
			PopNewHeroLayer* player = PopNewHeroLayer::create(m_heroData);
			g_mainScene->addChild(player);
			AnimationEffect::openAniEffect(player);
		}
	}
	else
	{
		int needsilver = recuitCostsilver[m_heroData->getPotential()] - GlobalInstance::getInstance()->getMySoliverCount().getValue();
		int count = needsilver/1000;
		int s = needsilver%1000;
		if (s != 0)
			count += 1;
		MovingLabel::show(ResourceLang::map_lang["nomoresilver"]);
		std::vector< MSGAWDSDATA> vec_res;
		MSGAWDSDATA rdata;
		rdata.rid = "r013";
		rdata.count = count;
		rdata.qu = 0;
		vec_res.push_back(rdata);
		BuyResLayer* layer = BuyResLayer::create(vec_res);
		this->addChild(layer);
		AnimationEffect::openAniEffect(layer);
	}
}

void HeroAttrLayer::fireHero()
{
	InnRoomLayer* innroomLayer = (InnRoomLayer*)g_mainScene->getChildByName("6innroom");
	if (innroomLayer != NULL)
	{
		innroomLayer->fireHero(this->getTag());
	}
	else
	{
		GlobalInstance::getInstance()->fireHero(this->getTag());
		OutTownLayer* outTown = (OutTownLayer*)g_mainScene->getChildByName("0outtown");
		if (outTown != NULL)
		{
			SelectMyHerosLayer* sellayer = (SelectMyHerosLayer*)outTown->getChildByName("selectmyheroslayer");
			if (sellayer != NULL)
				sellayer->refreshMyHerosUi();
		}
	}
	m_heroData = NULL;
	AnimationEffect::closeAniEffect(this);
}

void HeroAttrLayer::onGoodsClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	cocos2d::ui::Button* clicknode = (cocos2d::ui::Button*)pSender;
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		m_longTouchNode = clicknode;
		if (!isScheduled(schedule_selector(HeroAttrLayer::longTouchUpdate)))
			schedule(schedule_selector(HeroAttrLayer::longTouchUpdate), 0.3f);
	}
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		cancelLongTouch();
		addHeroExp(clicknode->getTag());
	}
	else if (type == ui::Widget::TouchEventType::CANCELED)
	{
		cancelLongTouch();
	}
}

void HeroAttrLayer::addHeroExp(int tag)
{
	//if (m_heroData->getLevel() + 1 == m_heroData->getMaxLevel())
	if (m_heroData->getExp().getValue() >= GlobalInstance::vec_herosAttr[m_heroData->getVocation()].vec_exp[m_heroData->getMaxLevel() - 1])
	{
		MovingLabel::show(ResourceLang::map_lang["wgmostlv"]);
		changeButton();
		return;
	}

	int count = 0;
	switch (tag)
	{
	case 1:
		count = S001EXP;
		break;
	case 2:
		count = S002EXP;
		break;
	case 3:
		count = S003EXP;
		break;
	case 4:
		count = S004EXP;
		break;
	default:
		break;
	}

	if ((m_heroData->getLevel() + 1) % 10 == 0 && (m_heroData->getVocation() < 4 || (m_heroData->getLevel() + 1) / 10 == m_heroData->getChangeCount() + 1))
	{
		changeButton();
		MovingLabel::show(ResourceLang::map_lang["changebreak"]);
		return;
	}

	std::string str = StringUtils::format("s00%d", tag);
	if (MyRes::getMyResCount(str) >= 1)
	{
		MyRes::Use(str);
		str = StringUtils::format("%d", MyRes::getMyResCount(str));
		goodarr[tag - 1]->setString(str);

		/*DynamicValueInt dal;
		dal.setValue(count);*/
		int lastLevel = m_heroData->getLevel();
		m_heroData->setExpLimit(count);
		int curLevel = m_heroData->getLevel();
		if (lastLevel <= curLevel - 1)
		{
			CommonFuncs::playCommonLvUpAnim(this->getParent(), "texiao_sjcg");
			changeButton();
		}
		std::string s = StringUtils::format(ResourceLang::map_lang["winexp"].c_str(), count);
		MovingLabel::show(s, Color4B(0, 255, 0, 255), Vec2(360, 320));
		GlobalInstance::getInstance()->saveHero(m_heroData);
	}
	else
	{
		MovingLabel::show(ResourceLang::map_lang["reslack"]);
	}
}

void HeroAttrLayer::changeButton()
{
	cocos2d::ui::ImageView* txtimg = (cocos2d::ui::ImageView*)btnArr[1]->getChildByName("text");
	if ((m_heroData->getLevel() + 1) % 10 == 0 && (m_heroData->getVocation() < 4 || (m_heroData->getLevel() + 1) / 10 == m_heroData->getChangeCount() + 1))
	{
		if (m_heroData->getLevel() + 1 == 10)
		{
			txtimg->loadTexture(ResourcePath::makeTextImgPath("changebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		}
		else
		{
			txtimg->loadTexture(ResourcePath::makeTextImgPath("break_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		}
	}
	else
	{
		txtimg->loadTexture(ResourcePath::makeTextImgPath("lvupbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	}
	//if (m_heroData->getLevel() + 1 == m_heroData->getMaxLevel())
	if (m_heroData->getExp().getValue() >= GlobalInstance::vec_herosAttr[m_heroData->getVocation()].vec_exp[m_heroData->getMaxLevel() - 1])
	{
		txtimg->loadTexture(ResourcePath::makeTextImgPath("lvupbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		btnArr[1]->setEnabled(false);
	}
}

void HeroAttrLayer::changeHeroVocImg()
{
	changeButton();
	cocos2d::ui::ImageView* herofullimg = (cocos2d::ui::ImageView*)pageView->getItem(pageView->getCurrentPageIndex())->getChildByName("fullimg");
	std::string fullimgstr = StringUtils::format("hfull_%d_%d.png", m_heroData->getVocation(), m_heroData->getSex());
	herofullimg->loadTexture(ResourcePath::makeImagePath(fullimgstr), cocos2d::ui::Widget::TextureResType::LOCAL);
}

void HeroAttrLayer::onEquipClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		cocos2d::ui::Button* clicknode = (cocos2d::ui::Button*)pSender;
		clickindex = clicknode->getTag();

		if (m_heroData->getState() == HS_DEAD)
		{
			MovingLabel::show(ResourceLang::map_lang["herodeathonequip"]);
			return;
		}

		Layer* layer;
		ResBase* res = m_heroData->getEquipable(equiptype[clickindex]);

		if (res == NULL)
		{
			if (!MyRes::hasResByType(equiptype[clickindex]))
			{
				std::string hinstr;

				if (equiptype[clickindex] == T_WG || equiptype[clickindex] == T_NG)
					hinstr = ResourceLang::map_lang["nogfputon"];
				else
					hinstr = ResourceLang::map_lang["noequipputon"];
				MovingLabel::show(hinstr);
				return;
			}
			else
				layer = SelectEquipLayer::create(equiptype[clickindex], m_heroData);
		}
		else
		{
			if (equiptype[clickindex] == T_WG || equiptype[clickindex] == T_NG)
			{
				layer = EquipDescLayer::create(res, 2);
				layer->setUserData((void*)m_heroData);
			}
			else
			{
				layer = TakeOnLayer::create((Equip*)res, m_heroData);
			}
		}
		this->addChild(layer);
		AnimationEffect::openAniEffect((Layer*)layer);
	}
}

void HeroAttrLayer::takeOn(ResBase* res)
{
	Equipable* equipable = (Equipable*)res;
	equipable->setWhere(MYEQUIP);
	equipable->setWhos(m_heroData->getName());
	MyRes::saveData();

	m_heroData->setEquipable((Equipable*)res, res->getType());
	updateEquipUi(res, clickindex);

	m_heroData->setHp(m_heroData->getMaxHp());
}

void HeroAttrLayer::takeOff(ResBase* res)
{
	Equipable* equipable = (Equipable*)res;
	equipable->setWhere(MYSTORAGE);
	equipable->setWhos("");
	m_heroData->setEquipable(NULL, res->getType());

	MyRes::saveData();
	updateEquipUi(NULL, clickindex);
	m_heroData->setHp(m_heroData->getMaxHp());
}

void HeroAttrLayer::changeEquip(ResBase* res)
{
	Equipable* takeOnEquip = (Equipable*)m_heroData->getEquipable(res->getType());
	if (takeOnEquip != NULL)
	{
		takeOnEquip->setWhere(MYSTORAGE);
		takeOnEquip->setWhos("");
	}
	takeOn(res);
}

void HeroAttrLayer::updateEquipUi(ResBase* res, int barindex)
{
	cocos2d::ui::Widget* node = (cocos2d::ui::Widget*)equipnode->getChildren().at(barindex);
	cocos2d::ui::ImageView* qubox = (cocos2d::ui::ImageView*)node->getChildByName("qubox");
	qubox->ignoreContentAdaptWithSize(true);
	qubox->stopAllActions();
	qubox->setVisible(true);
	qubox->setOpacity(255);
	cocos2d::ui::ImageView* resimg = (cocos2d::ui::ImageView*)node->getChildByName("img");
	resimg->ignoreContentAdaptWithSize(true);

	cocos2d::ui::Text* lvtext = (cocos2d::ui::Text*)node->getChildByName("lvtext");

	std::string qustr;
	std::string resstr;

	if (res != NULL)
	{
		int type = res->getType();
		qubox->setAnchorPoint(Vec2(0.5, 0.5));
		qubox->setPosition(Vec2(qubox->getParent()->getContentSize().width / 2, qubox->getParent()->getContentSize().height / 2));

		std::string lvstr;
		lvtext->setVisible(true);

		int qu =  ((Equipable*)res)->getQU().getValue();

		if (type >= T_ARMOR && type <= T_FASHION)
		{
			qubox->setScale(0.84f);
			resimg->setScale(0.84f);

			int slv = ((Equipable*)res)->getLv().getValue();
			if(slv > 0)
				lvstr = StringUtils::format("%+d", slv);
		}
		else if (type >= T_WG && type <= T_NG)
		{
			qubox->setScale(1.0f);
			resimg->setScale(1.0f);
			qu = GlobalInstance::map_GF[res->getId()].qu;

			int lv = ((Equipable*)res)->getLv().getValue();
			lvstr = StringUtils::format("lv.%d", lv + 1);
		}
		qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
		resstr = GlobalInstance::getInstance()->getResUIFrameName(res->getId(), qu);
		lvtext->setString(lvstr);
		if (effectnode[barindex] == NULL)
			effectnode[barindex] = CommonFuncs::playResBoxEffect(qubox->getParent(), type, qu, ((Equipable*)res)->getLv().getValue());
		if (effectnode[barindex] != NULL)
		{
			if(type >= T_ARMOR && type <= T_FASHION)
				effectnode[barindex]->setScale(0.84f);
			else
				effectnode[barindex]->setScale(1.0f);
		}
	}
	else
	{
		qubox->setAnchorPoint(Vec2(0, 1));
		if (barindex == 2 || barindex == 3)//武功，内功
			qubox->setPosition(Vec2(8, 117));
		else
			qubox->setPosition(Vec2(8, 98));
		qubox->setScale(1.0f);
		resimg->setScale(1.0f);
		qustr = "ui/heroattradd.png";
		resstr = ResourcePath::makeTextImgPath(StringUtils::format("equiptext_%d", barindex), langtype);
		lvtext->setVisible(false);

		if (effectnode[barindex] != NULL)
		{
			effectnode[barindex]->removeFromParentAndCleanup(true);
			effectnode[barindex] = NULL;
		}
	}
	qubox->loadTexture(qustr, cocos2d::ui::Widget::TextureResType::PLIST);
	resimg->loadTexture(resstr, cocos2d::ui::Widget::TextureResType::PLIST);

	for (int i = 0; i < equipnode->getChildrenCount(); i++)
	{
		cocos2d::ui::Widget* node = (cocos2d::ui::Widget*)equipnode->getChildren().at(i);
		cocos2d::ui::ImageView* qubox = (cocos2d::ui::ImageView*)node->getChildByName("qubox");
		if (qubox->getRenderFile().file.compare("ui/heroattradd.png") == 0)
		{
			if (m_fromwhere != 4)
			{
				if (MyRes::hasResByType(equiptype[i]))
					qubox->runAction(RepeatForever::create(Sequence::create(FadeOut::create(1), FadeIn::create(1), NULL)));
				else
					qubox->setVisible(false);
			}
			else
			{
				qubox->setVisible(false);
			}
		}
	}

}

void HeroAttrLayer::updataAtrrUI(float dt)
{
	if (m_heroData != NULL)
	{
		int hp = m_heroData->getHp();
		std::string attrstr = StringUtils::format("%d/%d", hp, (int)m_heroData->getMaxHp());
		hplbl->setString(attrstr);

		//攻击值
		attrstr = StringUtils::format("%d", (int)m_heroData->getAtk());
		atkbl->setString(attrstr);

		//防御值
		attrstr = StringUtils::format("%d", (int)m_heroData->getDf());
		dflbl->setString(attrstr);

		//攻击速度值
		attrstr = StringUtils::format("%.3f", m_heroData->getAtkSpeed());
		atkspeedlbl->setString(attrstr);

		//暴击值
		attrstr = StringUtils::format("%.3f%%", m_heroData->getCrit());
		critlbl->setString(attrstr);

		//闪避值
		attrstr = StringUtils::format("%.3f%%", m_heroData->getDodge());
		dodgelbl->setString(attrstr);

		//等级
		attrstr = StringUtils::format("Lv.%d", m_heroData->getLevel() + 1);
		lvUIText->setString(attrstr);

		int curlvexp = 0;
		int nextlvexp = GlobalInstance::vec_herosAttr[m_heroData->getVocation()].vec_exp[m_heroData->getLevel()];

		if (m_heroData->getLevel() > 0)
			curlvexp = GlobalInstance::vec_herosAttr[m_heroData->getVocation()].vec_exp[m_heroData->getLevel() - 1];

		//经验值
		attrstr = StringUtils::format("%d/%d", m_heroData->getExp().getValue() - curlvexp, nextlvexp - curlvexp);
		explbl->setString(attrstr);

		float percent = (m_heroData->getExp().getValue() - curlvexp)*100.0f / (nextlvexp - curlvexp);

		if (lastLv != m_heroData->getLevel())
		{
			lastLv = m_heroData->getLevel();
			expbar->runAction(Sequence::create(LoadingBarProgressTo::create(0.2f, 100), DelayTime::create(0.1f), LoadingBarProgressFromTo::create(0.2f, 0, percent), NULL));
		}
		else
		{
			if (lastexp != m_heroData->getExp().getValue())
			{
				lastexp = m_heroData->getExp().getValue();
				expbar->runAction(Sequence::create(LoadingBarProgressTo::create(0.2f, percent), NULL));
			}
			else
			{
				lastexp = m_heroData->getExp().getValue();
				expbar->setPercent(percent);
			}
		}

		if (lastVaction != m_heroData->getVocation())
		{
			lastVaction = m_heroData->getVocation();
			updateVocationUI();
		}

		if (redtip != NULL)
		{
			int c = (m_heroData->getLevel() + 1) / 10;
			std::string needresid = StringUtils::format("d%03d", c);
			if (!redtip->isVisible() && (((m_heroData->getLevel() + 1) / 10 == m_heroData->getChangeCount() && GlobalInstance::getInstance()->getCanUpgradeCount("s00")) || ((m_heroData->getLevel() + 1) / 10 != m_heroData->getChangeCount() && MyRes::getMyResCount(needresid) > 0)) && (m_heroData->getLevel() + 1) < 50)
			{
				redtip->setVisible(true);
			}
			else if (redtip->isVisible() && (((m_heroData->getLevel() + 1) / 10 != m_heroData->getChangeCount() && MyRes::getMyResCount(needresid) <= 0) || !GlobalInstance::getInstance()->getCanUpgradeCount("s00") || (m_heroData->getLevel() + 1) == 50))
			{
				redtip->setVisible(false);
			}
		}

		
		for (int i = 0; i < 6; i++)
		{
			Equipable* eres = (Equipable*)m_heroData->getEquipable(equiptype[i]);
			redpointArr[i]->setVisible(false);
			if (eres != NULL)
			{
				if (i != 2 && i != 3)
				{
					if (GlobalInstance::getInstance()->strengthMaterial(eres) || GlobalInstance::getInstance()->compareHighEquip(equiptype[i], m_heroData))
					{
						redpointArr[i]->setVisible(true);
					}
				}
				else
				{
					if ((GlobalInstance::getInstance()->getCanUpgradeCount("m00") && (eres->getLv().getValue() + 1) < (int)GlobalInstance::map_GF[eres->getId()].vec_df.size()) || GlobalInstance::getInstance()->compareHighEquip(equiptype[i], m_heroData))
					{
						redpointArr[i]->setVisible(true);
					}
				}
			}
			//更新等级
			cocos2d::ui::Widget* node = (cocos2d::ui::Widget*)equipnode->getChildren().at(i);
			cocos2d::ui::Text* lvtext = (cocos2d::ui::Text*)node->getChildByName("lvtext");
			std::string lvstr;
			if (eres != NULL)
			{
				if (eres->getType() >= T_ARMOR && eres->getType() <= T_FASHION)
				{
					int slv = eres->getLv().getValue();
					if (slv > 0)
						lvstr = StringUtils::format("%+d", slv);
				}
				else if (eres->getType() >= T_WG && eres->getType() <= T_NG)
				{
					int lv = eres->getLv().getValue();
					lvstr = StringUtils::format("lv.%d", lv + 1);
				}
				lvtext->setString(lvstr);

				if (effectnode[i] == NULL)
					effectnode[i] = CommonFuncs::playResBoxEffect(node, eres->getType(), eres->getQU().getValue(), eres->getLv().getValue());
				if (effectnode[i] != NULL)
				{
					if (eres->getType() >= T_ARMOR && eres->getType() <= T_FASHION)
						effectnode[i]->setScale(0.84f);
					else
						effectnode[i]->setScale(1.0f);
				}
			}
			else
			{
				lvtext->setVisible(false);
			}
		}
	}
}

void HeroAttrLayer::updateVocationUI()
{
	std::string attrstr = StringUtils::format("vocation_%d", m_heroData->getVocation());
	vocation->setString(ResourceLang::map_lang[attrstr]);
}

void HeroAttrLayer::onHeroFullClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
	if (type == cocos2d::ui::Widget::TouchEventType::BEGAN)
	{
		pageMoveClickIndex = 1;

	}
	else if (type == cocos2d::ui::Widget::TouchEventType::MOVED)
	{
		pageMoveClickIndex = 2;
		//if (!cantclick->isVisible())
		//{
		//	heroattrbottom->stopAllActions();
		//	equipnode->stopAllActions();
		//	heroattrbottom->runAction(Sequence::create(MoveTo::create(0.2f, Vec2(0, -heroattrbottom->getContentSize().height)), CallFunc::create(CC_CALLBACK_0(HeroAttrLayer::finishMovingAction, this)), NULL));
		//	equipnode->runAction(Sequence::create(MoveTo::create(0.2f, Vec2(360, -560)), NULL));
		//	cantclick->setVisible(true);
		//}
	}
	else if (type == cocos2d::ui::Widget::TouchEventType::CANCELED)
	{
		//if (cantclick->isVisible())
		//{
		//	heroattrbottom->stopAllActions();
		//	equipnode->stopAllActions();
		//	heroattrbottom->runAction(Sequence::create(MoveTo::create(0.2f, Vec2(0, 0)), CallFunc::create(CC_CALLBACK_0(HeroAttrLayer::finishMovingAction, this)), NULL));
		//	equipnode->runAction(MoveTo::create(0.2f, Vec2(360, 490)));
		//	cantclick->setVisible(false);
		//}
	}
	else if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (!isCanClickFullHero)
		{
			isCanClickFullHero = true;
			return;
		}
		if (isDraging)
			return;

		if (lvnode->isVisible() || isMovingAction || !isCanClickFullHero)
			return;
		else
		{
			isMovingAction = true;
			heroattrbottom->stopAllActions();
			equipnode->stopAllActions();
			if (pageMoveClickIndex == 1)
			{
				if (heroattrbottom->getPositionY() >= 0)
				{
					heroattrbottom->runAction(Sequence::create(MoveTo::create(0.3f, Vec2(0, -heroattrbottom->getContentSize().height)), DelayTime::create(0.12f), CallFunc::create(CC_CALLBACK_0(HeroAttrLayer::finishMovingAction, this)), NULL));
					equipnode->runAction(Sequence::create(MoveTo::create(0.3f, Vec2(360, -560)), NULL));
					cantclick->setVisible(false);
				}
				else
				{
					heroattrbottom->runAction(Sequence::create(MoveTo::create(0.3f, Vec2(0, 0)), DelayTime::create(0.02f), CallFunc::create(CC_CALLBACK_0(HeroAttrLayer::finishMovingAction, this)), NULL));
					equipnode->runAction(MoveTo::create(0.3f, Vec2(360, 490)));
					cantclick->setVisible(true);
				}
			}
			else
			{
				if (heroattrbottom->getPositionY() >= 0)
				{
					heroattrbottom->setPositionY(0);
					equipnode->setPositionY(490);
					heroattrbottom->runAction(Sequence::create(DelayTime::create(0.5f), MoveTo::create(0.3f, Vec2(0, -heroattrbottom->getContentSize().height)), CallFunc::create(CC_CALLBACK_0(HeroAttrLayer::finishMovingAction, this)), NULL));
					equipnode->runAction(Sequence::create(DelayTime::create(0.5f), MoveTo::create(0.3f, Vec2(360, -560)), NULL));
					cantclick->setVisible(false);
				}
				else
				{
					heroattrbottom->setPositionY(-heroattrbottom->getContentSize().height);
					equipnode->setPositionY(-560);
					heroattrbottom->runAction(Sequence::create(DelayTime::create(0.5f), MoveTo::create(0.3f, Vec2(0, 0)), CallFunc::create(CC_CALLBACK_0(HeroAttrLayer::finishMovingAction, this)), NULL));
					equipnode->runAction(Sequence::create(DelayTime::create(0.5f), MoveTo::create(0.3f, Vec2(360, 490)), NULL));
					cantclick->setVisible(true);
				}
			}
			
			
		}
	}
}

void HeroAttrLayer::onHeroHintClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		std::string hidstr = StringUtils::format("h%03d", m_heroData->getVocation() + 1);
		SimplePopLayer* layer = SimplePopLayer::create(GlobalInstance::map_AllResources[hidstr].desc);
		this->addChild(layer, 10);
		AnimationEffect::openAniEffect(layer);
	}
}

void HeroAttrLayer::finishMovingAction()
{
	isMovingAction = false;
}

void HeroAttrLayer::longTouchUpdate(float delay)
{
	m_isLongPress = true;
	if (m_longTouchNode != NULL) {
		addHeroExp(m_longTouchNode->getTag());
	}
}

void HeroAttrLayer::cancelLongTouch()
{
	m_isLongPress = false;
	m_longTouchNode = NULL;
	unschedule(schedule_selector(HeroAttrLayer::longTouchUpdate));
}

void HeroAttrLayer::modifyName(std::string utf8name)
{
	if (utf8name.length() > 0)
	{
		WaitingProgress* waitbox = WaitingProgress::create(ResourceLang::map_lang["doingtext"]);
		Director::getInstance()->getRunningScene()->addChild(waitbox, 1, "waitbox");

		HttpDataSwap::init(this)->checklegalword(utf8name);
	}
	else
	{
		MovingLabel::show(ResourceLang::map_lang["heronameempty"]);
	}
}

void HeroAttrLayer::onFinish(int errcode)
{
	Director::getInstance()->getRunningScene()->removeChildByName("waitbox");
	if (errcode == 0)
	{
		updateHeroNameAction(m_editName->getText());
		MovingLabel::show(ResourceLang::map_lang["modifyheronamesucc"]);
	}
	else
	{
		if (errcode == 2)//1--player not exit;2--timesout;3--used;4--senstive
			MovingLabel::show(ResourceLang::map_lang["senstiveword"]);
		else
			MovingLabel::show(ResourceLang::map_lang["modifyheronamefail"]);

		m_editName->setText(m_heroData->getName().c_str());
	}
}

void HeroAttrLayer::updateHeroNameAction(std::string newname)
{

	for (int i = 0; i < equipnode->getChildrenCount(); i++)
	{
		Equipable* eres = (Equipable*)m_heroData->getEquipable(equiptype[i]);
		if (eres != NULL)
		{
			eres->setWhos(newname);
		}
	}
	m_heroData->setName(newname);

	if (g_mainScene != NULL)
	{
		InnRoomLayer* innroomLayer = (InnRoomLayer*)g_mainScene->getChildByName("6innroom");
		if (innroomLayer != NULL)
		{
			innroomLayer->getMyHeroNode(this->getTag())->updateData();
		}
		else
		{
			OutTownLayer* outTown = (OutTownLayer*)g_mainScene->getChildByName("0outtown");

			if (outTown != NULL)
			{
				CardHeroNode* mycardheroNode = (CardHeroNode*)outTown->getMyCardHeroNode(this->getTag());
				if (mycardheroNode != NULL && this->getTag() < 6)
					mycardheroNode->setData(GlobalInstance::myCardHeros[this->getTag()]);

				SelectMyHerosLayer* sellayer = (SelectMyHerosLayer*)outTown->getChildByName("selectmyheroslayer");
				if (sellayer != NULL)
				{
					MyHeroNode* myheroNode = (MyHeroNode*)sellayer->getMyHeroNode(this->getTag());
					if (myheroNode != NULL)
						myheroNode->updateData();
				}
			}
		}
	}
	else if (g_MapBlockScene != NULL)
	{
		//g_MapBlockScene->getFightHeroNode(this->getTag())->setData((Npc*)GlobalInstance::myCardHeros[this->getTag()]);
		moditybtn->setVisible(false);
		m_editName->setEnabled(false);
	}
}

void HeroAttrLayer::onExit()
{
	Layer::onExit();
}

