#include "EquipDescLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MovingLabel.h"
#include "MyRes.h"
#include "StoreHouseLayer.h"
#include "HeroAttrLayer.h"
#include "WgLvLayer.h"
#include "AnimationEffect.h"
#include "StrengthenLayer.h"
#include "Equip.h"
#include "NewGuideLayer.h"
#include "MainScene.h"
#include "HintBoxLayer.h"
#include "SaleGfLayer.h"
#include "SmithyLayer.h"
#include "MainScene.h"

USING_NS_CC;

EquipDescLayer::EquipDescLayer()
{
	salepoint = NULL;
}

EquipDescLayer::~EquipDescLayer()
{
	if (!NewGuideLayer::checkifNewerGuide(75) && NewGuideLayer::checkifNewerGuide(76))
	{
		SmithyLayer* layer = (SmithyLayer*)g_mainScene->getChildByName("2smithy");
		if (layer != NULL)
		{
			layer->delayShowNewerGuide(0);
		}
	}
}


EquipDescLayer* EquipDescLayer::create(ResBase* res, int fromwhere)
{
	EquipDescLayer *pRet = new(std::nothrow)EquipDescLayer();
	if (pRet && pRet->init(res, fromwhere))
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
bool EquipDescLayer::init(ResBase* res, int fromwhere)
{
	if (!Layer::init())
	{
		return false;
	}

	m_res = (Equipable*)res;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* salebtn;
	if (res->getType() >= T_WG && res->getType() <= T_NG)
	{
		csbnode = CSLoader::createNode(ResourcePath::makePath("gfDescLayer.csb"));
		//出售按钮
		salebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("salebtn");
		salebtn->addTouchEventListener(CC_CALLBACK_2(EquipDescLayer::onBtnClick, this));
		salebtn->setTag(1000);
		cocos2d::ui::ImageView* salebtnbtntxt = (cocos2d::ui::ImageView*)salebtn->getChildByName("text");
		salebtnbtntxt->loadTexture(ResourcePath::makeTextImgPath("text_sale", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		salebtnbtntxt->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath("text_sale", langtype))->getContentSize());
	}
	else
		csbnode = CSLoader::createNode(ResourcePath::makePath("equipDescLayer.csb"));

	this->addChild(csbnode);

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	cocos2d::ui::ImageView* resbox_qu = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox_qu");

	std::string qustr;
	int s = m_res->getQU().getValue();
	int t = 0;
	for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
	{
		if (m_res->getId().compare(0, 1, RES_TYPES_CHAR[t]) == 0)
			break;
	}
	if (t >= T_ARMOR && t <= T_FASHION)
	{
		qustr = StringUtils::format("ui/resbox_qu%d.png", s);
	}
	else if (t >= T_WG && t <= T_NG)
	{
		s = GlobalInstance::map_GF[m_res->getId()].qu;
		qustr = StringUtils::format("ui/resbox_qu%d.png", s);
	}
	else if (t >= T_RENS && t <= T_BOX)
	{
		s = atoi(m_res->getId().substr(1).c_str()) - 1;
		qustr = StringUtils::format("ui/resbox_qu%d.png", s);
	}

	CommonFuncs::playResBoxEffect(resbox_qu, s);
	resbox_qu->loadTexture(qustr, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* p_res = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	std::string str = GlobalInstance::getInstance()->getResUIFrameName(res->getId(), s);

	p_res->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	namelbl->setTextColor(Color4B(POTENTIALCOLOR[s]));

	cocos2d::ui::Text* qua = (cocos2d::ui::Text*)csbnode->getChildByName("qua");
	str = ResourceLang::map_lang["potentialtext"];
	qua->setString(str);

	cocos2d::ui::Text* quatext = (cocos2d::ui::Text*)csbnode->getChildByName("quatext");
	std::string st = StringUtils::format("potential_%d", s);
	str = ResourceLang::map_lang[st];
	quatext->setString(str);
	quatext->setTextColor(Color4B(POTENTIALCOLOR[s]));

	for (int i = 0; i <= 5; i++)
	{
		str = StringUtils::format("attrtext_%d",i);
		attrlblArr[i] = (cocos2d::ui::Text*)csbnode->getChildByName(str);
		str = StringUtils::format("addtext_%d", i); 
		addtextArr[i] = (cocos2d::ui::Text*)csbnode->getChildByName(str);
	}

	//updateAttr();

	cocos2d::ui::Text* jobtext = (cocos2d::ui::Text*)csbnode->getChildByName("attrtext_6");
	if (m_res->getType() >= T_WG && m_res->getType() <= T_NG)
	{
		GongFa* gf = (GongFa*)m_res;
		jobtext->setVisible(true);
		std::string jobstr;
		for (unsigned int i = 0; i < GlobalInstance::map_GF[gf->getId()].vec_skillbns.size(); i++)
		{
			int m = GlobalInstance::map_GF[gf->getId()].vec_skillbns[i];
			if (m == 1)
			{
				if (jobstr.length() > 0)
					jobstr.append(ResourceLang::map_lang["comma"]); 
				jobstr.append(GlobalInstance::map_AllResources[GlobalInstance::vec_herosAttr[i].id].name);
			}
		}
		jobtext->setString(jobstr);

		cocos2d::ui::Text* vocationtextlbl = (cocos2d::ui::Text*)csbnode->getChildByName("vocationtext");
		vocationtextlbl->setString(ResourceLang::map_lang["fitjob"]);

		cocos2d::ui::Text* skilltextlbl = (cocos2d::ui::Text*)csbnode->getChildByName("skilltext");
		skilltextlbl->setString(ResourceLang::map_lang["skilltext"]);
		
		int skilltype = GlobalInstance::map_GF[gf->getId()].skill;
		std::string skillids = StringUtils::format("sk%03d", skilltype);

		cocos2d::ui::Text* skillnamelbl = (cocos2d::ui::Text*)csbnode->getChildByName("skillname");
		std::string skillname = StringUtils::format(ResourceLang::map_lang["skillname"].c_str(), GlobalInstance::map_AllResources[skillids].name.c_str());
		skillnamelbl->setString(skillname);

		cocos2d::ui::Text* skilldesclbl = (cocos2d::ui::Text*)csbnode->getChildByName("skilldesc");
		Label* dlbl = (Label*)skilldesclbl->getVirtualRenderer();
		dlbl->setLineSpacing(5);

		std::string allskilldesc = StringUtils::format(ResourceLang::map_lang["gfkilldesc"].c_str(), GlobalInstance::map_GF[gf->getId()].skillrnd);
		
		std::string skilldesc = GlobalInstance::map_AllResources[skillids].desc;
		if (skilltype == SKILL_1 || skilltype == SKILL_13 || skilltype == SKILL_15 || skilltype == SKILL_20)
			skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[gf->getId()].skilleff1);
		else if (skilltype == SKILL_2)
			skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[gf->getId()].skilleff1/100);
		else if (skilltype == SKILL_3 || skilltype == SKILL_4)
			skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[gf->getId()].skilleff2 - 1);
		else if (skilltype == SKILL_5 || skilltype == SKILL_6)
			skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[gf->getId()].skilleff2, GlobalInstance::map_GF[gf->getId()].skilleff1);
		else if (skilltype == SKILL_7)
			skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[gf->getId()].skilleff1);
		else if (skilltype == SKILL_8)
			skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[gf->getId()].skilleff2, GlobalInstance::map_GF[gf->getId()].skilleff1);
		else if (skilltype == SKILL_9 || skilltype == SKILL_10 || skilltype == SKILL_11 || skilltype == SKILL_12 || skilltype == SKILL_14 || skilltype == SKILL_16 || skilltype == SKILL_18)
			skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[gf->getId()].skilleff1, GlobalInstance::map_GF[gf->getId()].skilleff2 - 1);
		else if (skilltype == SKILL_17)
			skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[gf->getId()].skilleff1, GlobalInstance::map_GF[gf->getId()].skilleff1, GlobalInstance::map_GF[gf->getId()].skilleff2 - 1);
		
		allskilldesc.append(skilldesc);
		skilldesclbl->setString(allskilldesc);
	}
	else if (m_res->getType() == T_ARMOR)
	{
		Equipable* eres = (Equipable*)m_res;
		jobtext->setVisible(true);
		std::string jobstr;
		for (unsigned int i = 0; i < GlobalInstance::map_Equip[eres->getId()].vec_bns.size(); i++)
		{
			float m = GlobalInstance::map_Equip[eres->getId()].vec_bns[i];
			if (m >= 1)
			{
				if (jobstr.length() > 0)
					jobstr.append(ResourceLang::map_lang["comma"]);
				jobstr.append(GlobalInstance::map_AllResources[GlobalInstance::vec_herosAttr[i].id].name);

			}
		}
		str = ResourceLang::map_lang["fitjob"];
		str.append(jobstr);
		jobtext->setString(str);
	}
	else if (m_res->getType() >= T_EQUIP && m_res->getType() <= T_FASHION)
	{
		jobtext->setString(ResourceLang::map_lang["fitalljob"]);
	}
	else
	{
		jobtext->setVisible(false);
	}

	//按钮1
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(EquipDescLayer::onBtnClick, this));
	cocos2d::ui::ImageView* srefreshbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	salepoint = (cocos2d::ui::Widget*)actionbtn->getChildByName("redpoint");
	if (salepoint != NULL)
	{
		salepoint->setVisible(false);
	}
	//按钮2
	cocos2d::ui::Widget* lvbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("lvbtn");
	lvbtn->setTag(1);
	lvbtn->addTouchEventListener(CC_CALLBACK_2(EquipDescLayer::onBtnClick, this));
	redpoint = (cocos2d::ui::Widget*)lvbtn->getChildByName("redpoint");
	if (redpoint != NULL)
	{
		redpoint->setVisible(false);
	}

	cocos2d::ui::ImageView* lvbtntxt = (cocos2d::ui::ImageView*)lvbtn->getChildByName("text");
	lvbtntxt->loadTexture(ResourcePath::makeTextImgPath("lvupbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	if (fromwhere == 0)//仓库进入
	{
		if (res->getType() >= T_ARMOR && res->getType() <= T_FASHION)
		{
			actionbtn->setPositionX(240);
			lvbtn->setVisible(true);
			lvbtn->setTag(100);
			lvbtntxt->loadTexture(ResourcePath::makeTextImgPath("strenthbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			lvbtntxt->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath("strenthbtn_text", langtype))->getContentSize());
		}
		else if (res->getType() >= T_WG && res->getType() <= T_NG)
		{
			actionbtn->setPositionX(490);
			salebtn->setVisible(true);
		}
		if (GlobalInstance::map_AllResources[res->getId()].vec_needres.size() > 0)
		{
			status = S_EQUIP_DECOMPOSE;
			srefreshbtntxt->loadTexture(ResourcePath::makeTextImgPath("decomposebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		}
		else {
			status = S_EQUIP_WGLV;
			lvbtn->setVisible(true);
			actionbtn->setVisible(false);
			//srefreshbtntxt->loadTexture(ResourcePath::makeTextImgPath("lvupbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		}
	}
	else if (fromwhere == 1)//takeon界面查看套装装备信息
	{
		status = S_NONE;
		//actionbtn->setVisible(false);
		srefreshbtntxt->loadTexture(ResourcePath::makeTextImgPath("closebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	}
	else if (fromwhere == 2)//换下功法
	{
		status = S_EQUIP_TAKEOFF;

		srefreshbtntxt->loadTexture(ResourcePath::makeTextImgPath("takeoffbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		actionbtn->setPositionX(240);
		lvbtn->setVisible(true);
	}
	else if (fromwhere == 3)//选择功法
	{
		status = S_EQUIP_SEL;

		srefreshbtntxt->loadTexture(ResourcePath::makeTextImgPath("selectbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	}

	this->scheduleOnce(schedule_selector(EquipDescLayer::delayShowNewerGuide), newguidetime);

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		AnimationEffect::closeAniEffect((Layer*)this);
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void EquipDescLayer::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
	updateAttr();
}

void EquipDescLayer::delayShowNewerGuide(float dt)
{
	if (!NewGuideLayer::checkifNewerGuide(33))
	{
		if (NewGuideLayer::checkifNewerGuide(THRIDGUIDESTEP))
		{
			if (NewGuideLayer::checkifNewerGuide(34))
			{
				showNewerGuide(34);
			}
			else if (NewGuideLayer::checkifNewerGuide(37))
			{
				showNewerGuide(37);
			}
		}
	}
}

void EquipDescLayer::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	if (step == 34 || step == 37)
	{
		nodes.push_back(csbnode->getChildByName("actionbtn"));
	}
	g_mainScene->showNewerGuideNode(step, nodes);
}

void EquipDescLayer::updateAttr()
{
	std::vector<float> vec_attrval;

	float hp = m_res->getHp();
	float atk = m_res->getAtk();
	float df = m_res->getDf();
	float atkspeed = m_res->getAtkSpeed();
	float crit = m_res->getCrit();
	float dodge = m_res->getDodge();
	Hero* carryhero = (Hero*)this->getUserData();
	if (carryhero != NULL)
	{
		if (m_res->getType() >= T_WG && m_res->getType() <= T_NG)
		{
			float herobns = GlobalInstance::map_GF[m_res->getId()].vec_herobns[carryhero->getVocation()];
			hp *= herobns;
			atk *= herobns;
			df *= herobns;
			atkspeed *= herobns;
			crit *= herobns;
			dodge *= herobns;
		}
		else if (m_res->getType() >= T_ARMOR && m_res->getType() <= T_FASHION)
		{
			float herobns = GlobalInstance::map_Equip[m_res->getId()].vec_bns[carryhero->getVocation()];
			hp *= herobns;
			if (GlobalInstance::map_EquipSuit.find(m_res->getId()) != GlobalInstance::map_EquipSuit.end() && GlobalInstance::map_EquipSuit[m_res->getId()].vec_suit.size() >= 2)
			{
				std::string suitid = GlobalInstance::map_EquipSuit[m_res->getId()].vec_suit[1];
				int t = 0;
				for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
				{
					if (suitid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
						break;
				}
				if (t >= T_ARMOR && t <= T_FASHION)
				{
					if (carryhero->getEquipable(t) != NULL)
						hp += ((Equip*)m_res)->getSuitHp();
				}
			}

			atk *= herobns;

			df *= herobns;

			if (GlobalInstance::map_EquipSuit.find(m_res->getId()) != GlobalInstance::map_EquipSuit.end() && GlobalInstance::map_EquipSuit[m_res->getId()].vec_suit.size() >= 3)
			{
				std::string suitid = GlobalInstance::map_EquipSuit[m_res->getId()].vec_suit[2];
				int t = 0;
				for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
				{
					if (suitid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
						break;
				}
				if (t >= T_ARMOR && t <= T_FASHION)
				{
					if (carryhero->getEquipable(t) != NULL)
						df += ((Equip*)m_res)->getSuitDf();
				}
			}

			atkspeed *= herobns;
			crit *= herobns;
			dodge *= herobns;
		}
	}

	vec_attrval.push_back(hp);
	vec_attrval.push_back(atk);
	vec_attrval.push_back(df);
	vec_attrval.push_back(atkspeed);
	vec_attrval.push_back(crit);
	vec_attrval.push_back(dodge);

	
	for (int i = 0; i <= 5; i++)
	{
		std::string str = StringUtils::format("addattrtext_%d", i);
		str = StringUtils::format(ResourceLang::map_lang[str].c_str(), vec_attrval[i]);
		attrlblArr[i]->setString(str);
		addtextArr[i]->setPositionX(attrlblArr[i]->getPositionX() + attrlblArr[i]->getContentSize().width / 2 + 10);
		float value = 0.0f;
		if (carryhero != NULL)
		{
			value = GlobalInstance::getInstance()->compareOtherEquip(m_res, carryhero, i);
		}
		if (value != 0.0f)
		{
			addtextArr[i]->setVisible(true);
			if (value > 0)
			{
				str = StringUtils::format("(+%f)", value);
				addtextArr[i]->setString(str);
				addtextArr[i]->setColor(Color3B(57, 251, 87));
			}
			else
			{
				str = StringUtils::format("(%f)", value);
				addtextArr[i]->setString(str);
				addtextArr[i]->setColor(Color3B(255, 0, 0));
			}
		}
		else
		{
			addtextArr[i]->setVisible(false);
		}
	}


	std::string namestr = GlobalInstance::map_AllResources[m_res->getId()].name;
	if (m_res->getLv().getValue() > 0)
	{
		if (m_res->getType() >= T_WG && m_res->getType() <= T_NG)
			namestr = StringUtils::format("+%d%s", m_res->getLv().getValue() + 1, namestr.c_str());
		else
			namestr = StringUtils::format("+%d%s", m_res->getLv().getValue(), namestr.c_str());
	}
	namelbl->setString(namestr);

	if (m_res != NULL)
	{
		if (redpoint != NULL)
		{
			if (m_res->getType() >= T_WG && m_res->getType() <= T_NG)
			{
				if ((m_res->getLv().getValue() + 1) < (int)GlobalInstance::map_GF[m_res->getId()].vec_df.size() && GlobalInstance::getInstance()->getCanUpgradeCount("m00"))
				{
					redpoint->setVisible(true);
				}
				else
				{
					redpoint->setVisible(false);
				}
			}
			else if (m_res->getType() >= T_ARMOR && m_res->getType() <= T_FASHION)
			{
				if (GlobalInstance::getInstance()->strengthMaterial(m_res))
				{
					redpoint->setVisible(true);
				}
				else
				{
					redpoint->setVisible(false);
				}
			}
		}
		if (salepoint != NULL && carryhero != NULL && GlobalInstance::getInstance()->compareHighEquip(m_res->getType(), carryhero))
		{
			salepoint->setVisible(true);
		}
	}
}

void EquipDescLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
		if (tag == 100)
		{
			StrengthenLayer* sLayer = StrengthenLayer::create((Equip*)m_res,1);
			this->addChild(sLayer);
			AnimationEffect::openAniEffect((Layer*)sLayer);
		}
		else if (tag == 1000)
		{
			SaleGfLayer* layer = SaleGfLayer::create((ResBase*)m_res);
			this->addChild(layer);
			AnimationEffect::openAniEffect((Layer*)layer);
		}
		else
		{
			if (status == S_EQUIP_DECOMPOSE)
			{
				StoreHouseLayer* storelayer = (StoreHouseLayer*)this->getParent();
				if (storelayer != NULL)
				{
					if (storelayer->decomposeCheck(m_res) == 0)
					{
						AnimationEffect::closeAniEffect((Layer*)this);
					}
				}
			}
			else if (status == S_EQUIP_TAKEOFF)
			{
				if (tag == 1)
				{
					WgLvLayer* wglayer = WgLvLayer::create(m_res);
					this->addChild(wglayer);
					AnimationEffect::openAniEffect((Layer*)wglayer);
				}
				else
				{
					HeroAttrLayer* heroAttrLayer = (HeroAttrLayer*)this->getParent();
					heroAttrLayer->takeOff(m_res);
					AnimationEffect::closeAniEffect((Layer*)this);
				}
			}
			else if (status == S_EQUIP_SEL)
			{
				HeroAttrLayer* heroAttrLayer = (HeroAttrLayer*)this->getParent()->getParent();
				heroAttrLayer->takeOn(m_res);
				this->getParent()->removeFromParentAndCleanup(true);
			}
			else if (status == S_EQUIP_WGLV)
			{
				WgLvLayer* wglayer = WgLvLayer::create(m_res);
				this->addChild(wglayer);
				AnimationEffect::openAniEffect((Layer*)wglayer);
			}
			else
			{
				AnimationEffect::closeAniEffect(this);
			}
		}
	}
}

void EquipDescLayer::onExit()
{
	Layer::onExit();
}