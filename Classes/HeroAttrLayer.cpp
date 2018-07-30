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
#include "HeroLvupLayer.h"
#include "ChangeVocationLayer.h"

USING_NS_CC;

int equiptype[] = { T_ARMOR, T_EQUIP, T_NG, T_WG, T_HANDARMOR, T_FASHION };

HeroAttrLayer::HeroAttrLayer()
{

}

HeroAttrLayer::~HeroAttrLayer()
{
	//保存数据
	GlobalInstance::getInstance()->saveMyHeros();
}


HeroAttrLayer* HeroAttrLayer::create(Hero* herodata)
{
	HeroAttrLayer *pRet = new(std::nothrow)HeroAttrLayer();
	if (pRet && pRet->init(herodata))
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
bool HeroAttrLayer::init(Hero* herodata)
{
    if ( !Layer::init() )
    {
        return false;
    }

	m_heroData = herodata;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 250));
	this->addChild(color);
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("heroAttrLayer.csb"));
	this->addChild(csbnode);
	langtype = GlobalInstance::getInstance()->getLang();

	//英雄全身图
	cocos2d::ui::ImageView* herofullimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("hfull");
	herofullimg->ignoreContentAdaptWithSize(true);
	std::string str = StringUtils::format("hfull_%d_%d.png", herodata->getVocation(), herodata->getSex());
	herofullimg->loadTexture(ResourcePath::makeImagePath(str), cocos2d::ui::Widget::TextureResType::LOCAL);

	//装备栏
	equipnode = csbnode->getChildByName("equipnode");
	for (int i = 0; i < equipnode->getChildrenCount(); i++)
	{
		cocos2d::ui::Widget* node = (cocos2d::ui::Widget*)equipnode->getChildren().at(i);
		node->setTag(i);
		node->addTouchEventListener(CC_CALLBACK_2(HeroAttrLayer::onEquipClick, this));
		ResBase* eres = MyRes::getMyPutOnResByType(equiptype[i], m_heroData->getName());
		if (eres != NULL)
		{
			updateEquipUi(eres, i);
			m_heroData->setEquipable((Equipable*)eres, eres->getType());
		}
	}
	//属性信息
	heroattrbottom = csbnode->getChildByName("heroattrbottom");
	moditybtn = (cocos2d::ui::Widget*)heroattrbottom->getChildByName("moditybtn");

	//品质
	cocos2d::ui::ImageView* heroattrqu = (cocos2d::ui::ImageView*)heroattrbottom->getChildByName("heroattrqu");
	str = StringUtils::format("heroattrqu_%d", herodata->getPotential());
	heroattrqu->loadTexture(ResourcePath::makeTextImgPath(str, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//名字TextField控件效果不好，改用EditBox
	cocos2d::ui::TextField* nameinput = (cocos2d::ui::TextField*)heroattrbottom->getChildByName("nameinput");
	nameinput->setVisible(false);

	m_editName = cocos2d::ui::EditBox::create(nameinput->getContentSize(), cocos2d::ui::Scale9Sprite::createWithSpriteFrameName(ResourcePath::makePath("ui/blank.png")));
	m_editName->setPosition(nameinput->getPosition());
	m_editName->setAnchorPoint(Vec2(0, 0.5));
	m_editName->setFontColor(Color4B(26, 68, 101, 255));
	m_editName->setInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
	m_editName->setMaxLength(5);
	m_editName->setFontSize(30);
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
	this->schedule(schedule_selector(HeroAttrLayer::updataAtrrUI), 2.0f);

	//按钮
	std::string btnname[] = { "firebtn", "changebtn", "backbtn", "recruitbtn", "lvgbtn"};//与BTNTYPE对应
	for (int i = 0; i < sizeof(btnname) / sizeof(btnname[0]); i++)
	{
		int tag = i + ATTR_FIREBTN;
		cocos2d::ui::Button* btn = (cocos2d::ui::Button*)heroattrbottom->getChildByName(btnname[i]);
		btn->setTag(tag);
		btn->addTouchEventListener(CC_CALLBACK_2(HeroAttrLayer::onBtnClick, this));

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
			txtimg->loadTexture(ResourcePath::makeTextImgPath("changebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			if (herostate == HS_READY || herostate == HS_TAKEON || herostate == HS_TRAINING)
			{
				btn->setVisible(false);
			}
			else if (herostate == HS_OWNED)
			{
				//前4种职业等级10可转职，后面可突破
				if ((m_heroData->getLevel() + 1) % 10 == 0)
				{
					btn->setVisible(true);
					if (m_heroData->getLevel() + 1 != 10)
					{
						txtimg->loadTexture(ResourcePath::makeTextImgPath("break_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
					}
					if (m_heroData->getLevel() + 1 == m_heroData->getMaxLevel())
					{
						btn->setEnabled(false);
					}
				}
				else
					btn->setVisible(false);
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
		else if (tag == ATTR_LVBTN)
		{
			cocos2d::ui::ImageView* txtimg = (cocos2d::ui::ImageView*)btn->getChildByName("text");
			txtimg->loadTexture(ResourcePath::makeTextImgPath("lvupbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			if (herostate == HS_READY || herostate == HS_TRAINING)
			{
				btn->setVisible(false);
			}
			else if (herostate == HS_TAKEON)
			{
				btn->setPositionX(220);
			}
			else if (herostate == HS_OWNED)
			{
				//前4种职业等级10可转职，
				if ((m_heroData->getLevel() + 1) % 10 == 0)
					btn->setVisible(false);
				else
					btn->setVisible(true);
			}
		}
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

void HeroAttrLayer::editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox)
{

}

void HeroAttrLayer::editBoxEditingDidEndWithAction(cocos2d::ui::EditBox* editBox, EditBoxEndAction action)
{
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
			this->removeFromParentAndCleanup(true);
			break;
		}
		case ATTR_LVBTN:
		{
			HeroLvupLayer* lvlayer = HeroLvupLayer::create(m_heroData);
			this->addChild(lvlayer);
		}
			break;
		case ATTR_CHANGEBTN:
			if (m_heroData->getLevel() + 1 == 10)
			{
				ChangeVocationLayer* clayer = ChangeVocationLayer::create(m_heroData);
				this->addChild(clayer);
			}
			else
			{
				ChangeVocationLayer* clayer = ChangeVocationLayer::create(m_heroData, 1);
				this->addChild(clayer);
			}
			break;
		case ATTR_RECRUITBTN:
		{
			if (GlobalInstance::vec_myHeros.size() < (10 + Building::map_buildingDatas["6innroom"]->level.getValue()))
			{
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
				MovingLabel::show(ResourceLang::map_lang["recruitsucc"]);
				this->removeFromParentAndCleanup(true);

				break;
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["myheromax"]);
			}
		}
		case ATTR_BACKBTN:
			this->removeFromParentAndCleanup(true);
			break;
		default:
			break;
		}
	}
}

void HeroAttrLayer::changeButton()
{
	cocos2d::ui::Button* btn = (cocos2d::ui::Button*)heroattrbottom->getChildByName("changebtn");
	cocos2d::ui::ImageView* txtimg = (cocos2d::ui::ImageView*)btn->getChildByName("text");
	cocos2d::ui::Button* lvbtn = (cocos2d::ui::Button*)heroattrbottom->getChildByName("lvgbtn");
	lvbtn->setVisible(false);
	btn->setVisible(true);
	if (m_heroData->getLevel() + 1 == 10)
	{
		txtimg->loadTexture(ResourcePath::makeTextImgPath("changebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	}
	else
	{
		txtimg->loadTexture(ResourcePath::makeTextImgPath("break_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	}
}

void HeroAttrLayer::changelvButton()
{
	cocos2d::ui::Button* btn = (cocos2d::ui::Button*)heroattrbottom->getChildByName("changebtn");
	cocos2d::ui::Button* lvbtn = (cocos2d::ui::Button*)heroattrbottom->getChildByName("lvgbtn");
	lvbtn->setVisible(true);
	lvbtn->setTouchEnabled(true);
	btn->setVisible(false);
}

void HeroAttrLayer::onEquipClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
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
		resstr = StringUtils::format("ui/%s.png", res->getId().c_str());
	}
	else
	{
		qubox->setAnchorPoint(Vec2(0, 1));
		if (clickindex == 2 || clickindex == 3)//武功，内功
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
}

void HeroAttrLayer::updataAtrrUI(float dt)
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

	attrstr = StringUtils::format("vocation_%d", m_heroData->getVocation());
	vocation->setString(ResourceLang::map_lang[attrstr]);
}

void HeroAttrLayer::onExit()
{
	Layer::onExit();
}
