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

USING_NS_CC;

int equiptype[] = { T_ARMOR, T_EQUIP, T_NG, T_WG, T_HANDARMOR, T_FASHION };

#define S001EXP 1500
#define S002EXP 5000
#define S003EXP 15000
#define S004EXP 50000

HeroAttrLayer::HeroAttrLayer()
{
	isMovingAction = false;
}

HeroAttrLayer::~HeroAttrLayer()
{
	//保存数据
	GlobalInstance::getInstance()->saveMyHeros();
}


HeroAttrLayer* HeroAttrLayer::create(Hero* herodata, int fromwhere)
{
	HeroAttrLayer *pRet = new(std::nothrow)HeroAttrLayer();
	if (pRet && pRet->init(herodata, fromwhere))
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
bool HeroAttrLayer::init(Hero* herodata, int fromwhere)
{
    if ( !Layer::init() )
    {
        return false;
    }

	m_heroData = herodata;

	lastVaction = herodata->getVocation();
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 250));
	this->addChild(color,0,"colorLayer");
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("heroAttrLayer.csb"));
	this->addChild(csbnode);
	langtype = GlobalInstance::getInstance()->getLang();

	blankclick = (cocos2d::ui::ImageView*)csbnode->getChildByName("blankclick");
	blankclick->setSwallowTouches(true);

	//英雄全身图
	herofullimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("hfull");
	herofullimg->ignoreContentAdaptWithSize(true);
	std::string fullimgstr = StringUtils::format("hfull_%d_%d.png", herodata->getVocation(), herodata->getSex());
	herofullimg->loadTexture(ResourcePath::makeImagePath(fullimgstr), cocos2d::ui::Widget::TextureResType::LOCAL);
	herofullimg->addTouchEventListener(CC_CALLBACK_2(HeroAttrLayer::onHeroFullClick, this));

	cocos2d::ui::Button* hintbtn = (cocos2d::ui::Button*)csbnode->getChildByName("hintbtn");
	hintbtn->addTouchEventListener(CC_CALLBACK_2(HeroAttrLayer::onHeroHintClick, this));
	hintbtn->setSwallowTouches(true);

	//升级栏
	lvnode = csbnode->getChildByName("lvnode");
	lvnode->setVisible(false);

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
	for (int i = 0; i < equipnode->getChildrenCount(); i++)
	{
		cocos2d::ui::Widget* node = (cocos2d::ui::Widget*)equipnode->getChildren().at(i);
		node->setTag(i);
		node->addTouchEventListener(CC_CALLBACK_2(HeroAttrLayer::onEquipClick, this));
		//cocos2d::ui::ImageView* qubox = (cocos2d::ui::ImageView*)node->getChildByName("qubox");
		//qubox->runAction(RepeatForever::create(Sequence::create(FadeOut::create(1), FadeIn::create(1), NULL)));
		ResBase* eres = MyRes::getMyPutOnResByType(equiptype[i], m_heroData->getName());
		updateEquipUi(eres, i);
		if (eres != NULL)
		{
			m_heroData->setEquipable((Equipable*)eres, eres->getType());
		}
	}
	//属性信息
	heroattrbottom = csbnode->getChildByName("heroattrbottom");
	moditybtn = (cocos2d::ui::Widget*)heroattrbottom->getChildByName("moditybtn");

	//品质
	cocos2d::ui::ImageView* heroattrqu = (cocos2d::ui::ImageView*)heroattrbottom->getChildByName("heroattrqu");
	std::string str = StringUtils::format("heroattrqu_%d", herodata->getPotential());
	heroattrqu->loadTexture(ResourcePath::makeTextImgPath(str, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//名字TextField控件效果不好，改用EditBox
	cocos2d::ui::TextField* nameinput = (cocos2d::ui::TextField*)heroattrbottom->getChildByName("nameinput");
	nameinput->setVisible(false);

	m_editName = cocos2d::ui::EditBox::create(nameinput->getContentSize(), cocos2d::ui::Scale9Sprite::createWithSpriteFrameName(ResourcePath::makePath("ui/blank.png")));
	m_editName->setPosition(nameinput->getPosition());
	m_editName->setFontColor(Color4B(26, 68, 101, 255));
	m_editName->setInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
	m_editName->setMaxLength(10);
	m_editName->setFontSize(nameinput->getFontSize());
	m_editName->setText(herodata->getName().c_str());
	//editName->setReturnType(EditBox::KeyboardReturnType::DONE);
	m_editName->setDelegate(this);
	heroattrbottom->addChild(m_editName);
	
	int herostate = m_heroData->getState();

	if (herostate == HS_READY)
	{
		equipnode->setVisible(false);
		moditybtn->setVisible(false);
		m_editName->setEnabled(false);
	}


	//角色职业
	vocation = (cocos2d::ui::Text*)heroattrbottom->getChildByName("vocation");
	str = StringUtils::format("vocation_%d", herodata->getVocation());
	vocation->setString(ResourceLang::map_lang[str]);

	//角色属性文字
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

	int curlvexp = 0;
	int nextlvexp = 0;
	int expsize = GlobalInstance::vec_herosAttr[herodata->getVocation()].vec_exp.size();
	
	if (herodata->getLevel() >= expsize)
		nextlvexp = GlobalInstance::vec_herosAttr[herodata->getVocation()].vec_exp[expsize - 1];
	else 
		nextlvexp = GlobalInstance::vec_herosAttr[herodata->getVocation()].vec_exp[herodata->getLevel()];

	if (herodata->getLevel() > 0)
		curlvexp = GlobalInstance::vec_herosAttr[herodata->getVocation()].vec_exp[herodata->getLevel()-1];

	//经验值
	explbl = (cocos2d::ui::Text*)heroattrbottom->getChildByName("exp");
	str = StringUtils::format("%d/%d", herodata->getExp().getValue() - curlvexp, nextlvexp - curlvexp);
	explbl->setString(str);

	expbar = (cocos2d::ui::LoadingBar*)heroattrbottom->getChildByName("heroattrexpbar");
	float percent = (herodata->getExp().getValue() - curlvexp)*100.0f / (nextlvexp - curlvexp);
	expbar->setPercent(percent);

	updataAtrrUI(0);
	this->schedule(schedule_selector(HeroAttrLayer::updataAtrrUI), 1.0f);

	//按钮
	std::string btnname[] = { "firebtn", "changebtn", "backbtn", "recruitbtn"};//与BTNTYPE对应
	for (int i = 0; i < sizeof(btnname) / sizeof(btnname[0]); i++)
	{
		int tag = i + ATTR_FIREBTN;
		cocos2d::ui::Button* btn = (cocos2d::ui::Button*)heroattrbottom->getChildByName(btnname[i]);
		btn->setTag(tag);
		btn->addTouchEventListener(CC_CALLBACK_2(HeroAttrLayer::onBtnClick, this));
		btnArr[i] = btn;

		if (tag == ATTR_FIREBTN)
		{
			cocos2d::ui::ImageView* txtimg = (cocos2d::ui::ImageView*)btn->getChildByName("text");
			txtimg->loadTexture(ResourcePath::makeTextImgPath("firebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			if (herostate == HS_READY || herostate == HS_TAKEON || herostate == HS_TRAINING)
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
			if (herostate == HS_READY || herostate == HS_TRAINING)
			{
				btn->setVisible(false);
			}
			else if (herostate == HS_OWNED || herostate == HS_TAKEON)
			{
				if (herostate == HS_TAKEON)
				{
					btn->setPositionX(220);
				}
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
				if (m_heroData->getLevel() + 1 == m_heroData->getMaxLevel())
				{
					btn->setEnabled(false);
				}
			}
		}
		else if (tag == ATTR_RECRUITBTN)
		{
			cocos2d::ui::ImageView* txtimg = (cocos2d::ui::ImageView*)btn->getChildByName("text");
			txtimg->loadTexture(ResourcePath::makeTextImgPath("recruitbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
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
			if (herostate == HS_READY || herostate == HS_TAKEON)
			{
				btn->setPositionX(500);
			}
			else if (herostate == HS_OWNED)
			{
				btn->setPositionX(600);
			}
			else if (herostate == HS_TRAINING)
			{
				btn->setPositionX(360);
			}
		}
		if (fromwhere != 0)
		{
			if (tag == ATTR_BACKBTN)
				btn->setPositionX(360);
			else
				btn->setVisible(false);
		}
	}

	this->scheduleOnce(schedule_selector(HeroAttrLayer::delayShowNewerGuide), 0.1f);

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
			else if (NewGuideLayer::checkifNewerGuide(32))
			{
				showNewerGuide(32);
			}
			else if (NewGuideLayer::checkifNewerGuide(35))
			{
				showNewerGuide(35);
			}
			else if (NewGuideLayer::checkifNewerGuide(38))
			{
				showNewerGuide(38);
			}
		}
	}
}

void HeroAttrLayer::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	if (step == 25 || step == 27 || step == 29)
	{
		nodes.push_back(heroattrbottom->getChildByName("recruitbtn"));
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

		if (herostate == HS_TAKEON || herostate == HS_TRAINING)
		{
			btnArr[0]->setVisible(false);
			btnArr[1]->setVisible(true);
			btnArr[1]->setPositionX(220);
			btnArr[2]->setPositionX(500);
		}
		else if (herostate == HS_OWNED)
		{
			btnArr[0]->setVisible(true);
			btnArr[1]->setVisible(true);
			btnArr[2]->setPositionX(600);;
		}
		btnArr[3]->setVisible(false);
	}
	
}

void HeroAttrLayer::editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox)
{
	lastchangedname = editBox->getText();
}

void HeroAttrLayer::editBoxEditingDidEndWithAction(cocos2d::ui::EditBox* editBox, EditBoxEndAction action)
{
	std::string editstr = editBox->getText();
	if (editstr.length() == 0)
	{
		editBox->setText(m_heroData->getName().c_str());
		MovingLabel::show(ResourceLang::map_lang["nicknameempty"]);
		return;
	}

	if (GlobalInstance::getInstance()->checkifSameName(editBox->getText()))
	{
		editBox->setText(m_heroData->getName().c_str());
		MovingLabel::show(ResourceLang::map_lang["samename"]);
	}
	else
	{
		m_heroData->setName(editBox->getText());

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
				outTown->getMyCardHeroNode(this->getTag())->setData(GlobalInstance::myCardHeros[this->getTag()]);
				SelectMyHerosLayer* sellayer = (SelectMyHerosLayer*)outTown->getChildByName("selectmyheroslayer");
				sellayer->getMyHeroNode(this->getTag())->updateData();
			}
		}
		else if (g_MapBlockScene != NULL)
		{
			//g_MapBlockScene->getFightHeroNode(this->getTag())->setData((Npc*)GlobalInstance::myCardHeros[this->getTag()]);
			moditybtn->setVisible(false);
			m_editName->setEnabled(false);
		}
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
			GlobalInstance::getInstance()->fireHero(this->getTag());
			m_heroData = NULL;
			InnRoomLayer* innroomLayer = (InnRoomLayer*)g_mainScene->getChildByName("6innroom");
			if (innroomLayer != NULL)
			{
				innroomLayer->refreshMyHerosUi();
			}
			else
			{
				OutTownLayer* outTown = (OutTownLayer*)g_mainScene->getChildByName("0outtown");
				SelectMyHerosLayer* sellayer = (SelectMyHerosLayer*)outTown->getChildByName("selectmyheroslayer");
				sellayer->refreshMyHerosUi();
			}
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		}
		case ATTR_CHANGEBTN:
			if ((m_heroData->getLevel() + 1) % 10 == 0 && (m_heroData->getVocation() < 4 || (m_heroData->getLevel() + 1) / 10 == m_heroData->getChangeCount() + 1))
			{
				if (m_heroData->getLevel() + 1 == 10)
				{
					ChangeVocationLayer* clayer = ChangeVocationLayer::create(m_heroData);
					this->addChild(clayer);
					AnimationEffect::openAniEffect((Layer*)clayer);
				}
				else
				{
					ChangeVocationLayer* clayer = ChangeVocationLayer::create(m_heroData, 1);
					this->addChild(clayer);
					AnimationEffect::openAniEffect((Layer*)clayer);
				}
			}
			else
			{
				updateAtrBtnUI();
				lvnode->setVisible(true);
				equipnode->setVisible(false);
			}
			break;
		case ATTR_RECRUITBTN:
		{
			int herosize = GlobalInstance::vec_myHeros.size();
			if (herosize < (10 + Building::map_buildingDatas["6innroom"]->level.getValue()))
			{
				SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_RECRUITSUCC);
				m_heroData->setState(HS_OWNED);
				//加入到我的英雄列表
				Hero* myhero = new Hero(m_heroData);

				GlobalInstance::vec_myHeros.push_back(myhero);
				//保存数据
				GlobalInstance::getInstance()->saveMyHeros();
				GlobalInstance::getInstance()->saveRand3Heros();
				InnRoomLayer* innroomLayer = (InnRoomLayer*)g_mainScene->getChildByName("6innroom");
				RandHeroNode* heroNode = (RandHeroNode*)this->getParent()->getChildByTag(this->getTag());
				heroNode->markRecruited(true);
				innroomLayer->refreshMyHerosUi();
				clicknode->setEnabled(false);
				//MovingLabel::show(ResourceLang::map_lang["recruitsucc"]);

				CommonFuncs::playCommonLvUpAnim(this->getParent(), "texiao_zmcg");

				AnimationEffect::closeAniEffect((Layer*)this);

				break;
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["myheromax"]);
			}
		}
		case ATTR_BACKBTN:
			if (lvnode->isVisible())
			{
				updateAtrBtnUI();
				lvnode->setVisible(false);
				equipnode->setVisible(true);
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

void HeroAttrLayer::onGoodsClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	cocos2d::ui::Button* clicknode = (cocos2d::ui::Button*)pSender;
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		if (m_heroData->getLevel() + 1 == m_heroData->getMaxLevel())
		{
			MovingLabel::show(ResourceLang::map_lang["wgmostlv"]);
			changeButton();
			return;
		}

		int tag = clicknode->getTag();
		int count;
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
			/*DynamicValueInt dal;
			dal.setValue(count);*/
			int lastLevel = m_heroData->getLevel();
			m_heroData->setExpLimit(count);
			int curLevel = m_heroData->getLevel();
			if (lastLevel <= curLevel - 1)
			{
				CommonFuncs::playCommonLvUpAnim(this->getParent(), "texiao_sjcg");
			}
			std::string s = StringUtils::format(ResourceLang::map_lang["winexp"].c_str(), count);
			MovingLabel::show(s, Color4B(0, 128, 0, 255), Vec2(360, 320));
		}
		else
		{
			MovingLabel::show(ResourceLang::map_lang["reslack"]);
		}
		str = StringUtils::format("%d", MyRes::getMyResCount(str));
		goodarr[tag - 1]->setString(str);
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
	if (m_heroData->getLevel() + 1 == m_heroData->getMaxLevel())
	{
		txtimg->loadTexture(ResourcePath::makeTextImgPath("lvupbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		btnArr[1]->setEnabled(false);
	}
}

void HeroAttrLayer::onEquipClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		cocos2d::ui::Button* clicknode = (cocos2d::ui::Button*)pSender;
		clickindex = clicknode->getTag();
		Layer* layer;
		ResBase* res = MyRes::getMyPutOnResByType(equiptype[clickindex], m_heroData->getName());

	
		if (res == NULL)
			layer = SelectEquipLayer::create(equiptype[clickindex], m_heroData);
		else
		{
			if (equiptype[clickindex] == T_WG || equiptype[clickindex] == T_NG)
			{
				layer = EquipDescLayer::create(res, 2);
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
	m_heroData->setEquipable((Equipable*)res, res->getType());
	updateEquipUi(res, clickindex);
}

void HeroAttrLayer::takeOff(ResBase* res)
{
	res->setWhere(MYSTORAGE);
	updateEquipUi(NULL, clickindex);
	m_heroData->setEquipable(NULL, res->getType());
}

void HeroAttrLayer::changeEquip(ResBase* res)
{
	Equipable* takeOnEquip = (Equipable*)MyRes::getMyPutOnResByType(res->getType(), m_heroData->getName());
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
	std::string qustr;
	std::string resstr;
	if (res != NULL)
	{
		int type = res->getType();
		qubox->setAnchorPoint(Vec2(0.5, 0.5));
		qubox->setPosition(Vec2(qubox->getParent()->getContentSize().width / 2, qubox->getParent()->getContentSize().height / 2));


		int qu =  ((Equipable*)res)->getQU().getValue();
		if (type >= T_ARMOR && type <= T_FASHION)
		{
			qubox->setScale(0.84f);
			resimg->setScale(0.84f);
		}
		else if (type >= T_WG && type <= T_NG)
		{
			qubox->setScale(1.0f);
			resimg->setScale(1.0f);
		}
		qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
		resstr = GlobalInstance::getInstance()->getResUIFrameName(res->getId(), qu);
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
	}
	qubox->loadTexture(qustr, cocos2d::ui::Widget::TextureResType::PLIST);
	resimg->loadTexture(resstr, cocos2d::ui::Widget::TextureResType::PLIST);
	MyRes::saveData();

	for (int i = 0; i < equipnode->getChildrenCount(); i++)
	{
		cocos2d::ui::Widget* node = (cocos2d::ui::Widget*)equipnode->getChildren().at(i);
		cocos2d::ui::ImageView* qubox = (cocos2d::ui::ImageView*)node->getChildByName("qubox");
		if (qubox->getRenderFile().file.compare("ui/heroattradd.png") == 0)
		{
			if (MyRes::hasResByType(equiptype[i]))
				qubox->runAction(RepeatForever::create(Sequence::create(FadeOut::create(1), FadeIn::create(1), NULL)));
			else
				qubox->setVisible(false);
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
		int nextlvexp = 0;
		int expsize = GlobalInstance::vec_herosAttr[m_heroData->getVocation()].vec_exp.size();

		if (m_heroData->getLevel() >= expsize)
			nextlvexp = GlobalInstance::vec_herosAttr[m_heroData->getVocation()].vec_exp[expsize - 1];
		else
			nextlvexp = GlobalInstance::vec_herosAttr[m_heroData->getVocation()].vec_exp[m_heroData->getLevel()];

		if (m_heroData->getLevel() > 0)
			curlvexp = GlobalInstance::vec_herosAttr[m_heroData->getVocation()].vec_exp[m_heroData->getLevel() - 1];

		//经验值
		attrstr = StringUtils::format("%d/%d", m_heroData->getExp().getValue() - curlvexp, nextlvexp - curlvexp);
		explbl->setString(attrstr);

		float percent = (m_heroData->getExp().getValue() - curlvexp)*100.0f / (nextlvexp - curlvexp);
		expbar->setPercent(percent);

		if (lastVaction != m_heroData->getVocation())
		{
			lastVaction = m_heroData->getVocation();
			updateVocationUI();
		}

	}
}

void HeroAttrLayer::updateVocationUI()
{
	std::string attrstr = StringUtils::format("vocation_%d", m_heroData->getVocation());
	vocation->setString(ResourceLang::map_lang[attrstr]);
	attrstr = StringUtils::format("hfull_%d_%d.png", m_heroData->getVocation(), m_heroData->getSex());
	herofullimg->loadTexture(ResourcePath::makeImagePath(attrstr), cocos2d::ui::Widget::TextureResType::LOCAL);
}

void HeroAttrLayer::onHeroFullClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (lvnode->isVisible() || isMovingAction)
			return;
		else
		{
			isMovingAction = true;
			if (heroattrbottom->getPositionY() >= 0)
			{
				heroattrbottom->runAction(Sequence::create(MoveTo::create(0.3f, Vec2(0, -heroattrbottom->getContentSize().height)), CallFunc::create(CC_CALLBACK_0(HeroAttrLayer::finishMovingAction, this)), NULL));
				equipnode->runAction(Sequence::create(MoveTo::create(0.4f, Vec2(360, -560)), NULL));
				blankclick->setVisible(false);
			}
			else
			{
				heroattrbottom->runAction(Sequence::create(MoveTo::create(0.3f, Vec2(0, 0)), CallFunc::create(CC_CALLBACK_0(HeroAttrLayer::finishMovingAction, this)), NULL));
				equipnode->runAction(MoveTo::create(0.3f, Vec2(360, 490)));
				blankclick->setVisible(true);
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


void HeroAttrLayer::onExit()
{
	Layer::onExit();
}

