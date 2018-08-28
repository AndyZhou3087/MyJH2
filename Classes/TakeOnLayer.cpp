#include "TakeOnLayer.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "Equip.h"
#include "json.h"
#include "MyRes.h"
#include "HeroAttrLayer.h"
#include "SelectEquipLayer.h"
#include "EquipDescLayer.h"
#include "SetInStoneLayer.h"
#include "Quest.h"
#include "StrengthenLayer.h"
#include "AnimationEffect.h"

static bool isChangeEquip = false;
TakeOnLayer::TakeOnLayer()
{
	nohasEquip = NULL;
}


TakeOnLayer::~TakeOnLayer()
{
	isChangeEquip = false;
	if (nohasEquip != NULL)
	{
		delete nohasEquip;
		nohasEquip = NULL;
	}
}


TakeOnLayer* TakeOnLayer::create(Equip* res_equip, Hero* herodata)
{
	TakeOnLayer *pRet = new(std::nothrow)TakeOnLayer();

	if (pRet && pRet->init(res_equip, herodata))
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

bool TakeOnLayer::init(Equip* res_equip, Hero* herodata)
{
	if (!Layer::init())
	{
		return false;
	}

	m_herodata = herodata;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	csbnode = CSLoader::createNode(ResourcePath::makePath("takeOnLayer.csb"));
	this->addChild(csbnode);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	langtype = GlobalInstance::getInstance()->getLang();

	m_equip = res_equip;

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1000);
	closebtn->addTouchEventListener(CC_CALLBACK_2(TakeOnLayer::onBtnClick, this));

	int qu = m_equip->getQU().getValue();

	std::string str;
	name = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	str = StringUtils::format("+%d%s", m_equip->getLv().getValue(), GlobalInstance::map_AllResources[m_equip->getId()].name.c_str());
	name->setString(str);
	name->setTextColor(Color4B(POTENTIALCOLOR[qu]));

	cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox");
	str = StringUtils::format("ui/resbox_qu%d.png", qu);
	resbox->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* p_res = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	str = GlobalInstance::getInstance()->getResUIFrameName(m_equip->getId(), qu);

	p_res->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* qutext = (cocos2d::ui::Text*)csbnode->getChildByName("qutext");
	qutext->setString(ResourceLang::map_lang["potentialtext"]);

	cocos2d::ui::Text* qulbl = (cocos2d::ui::Text*)csbnode->getChildByName("qu");
	std::string st = StringUtils::format("potential_%d", qu);
	qulbl->setString(ResourceLang::map_lang[st]);
	qulbl->setTextColor(Color4B(POTENTIALCOLOR[qu]));

	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desc->setString(GlobalInstance::map_AllResources[m_equip->getId()].desc);

	int equ = m_equip->getQU().getValue();

	float bns = POTENTIAL_BNS[equ];

	int basehp = GlobalInstance::map_Equip[m_equip->getId()].maxhp;
	int basedf = GlobalInstance::map_Equip[m_equip->getId()].df;

	if (GlobalInstance::map_EquipSuit.find(m_equip->getId()) != GlobalInstance::map_EquipSuit.end())
	{
		for (unsigned int i = 0; i < 3; i++)
		{
			str = StringUtils::format("desc_%d", i);
			cocos2d::ui::Text* suitresdesc = (cocos2d::ui::Text*)csbnode->getChildByName(str);

			str = StringUtils::format("resbox_%d", i);
			cocos2d::ui::ImageView* suitresbox = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);

			str = StringUtils::format("res_%d", i);
			cocos2d::ui::ImageView* suitres = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);

			str = StringUtils::format("nothing%d", i);
			cocos2d::ui::Text* statlbl = (cocos2d::ui::Text*)csbnode->getChildByName(str);
			statlbl->setString(ResourceLang::map_lang["nothing"]);
			statlbl->setOpacity(168);
			statlbl->setVisible(false);

			if (i < GlobalInstance::map_EquipSuit[m_equip->getId()].vec_suit.size())
			{
				std::string eid = GlobalInstance::map_EquipSuit[m_equip->getId()].vec_suit[i];
				str = StringUtils::format("ui/%s.png", eid.c_str());
				suitres->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
				suitres->addTouchEventListener(CC_CALLBACK_2(TakeOnLayer::onEquipclick, this));

				if (i > 0)
				{
					vec_suitDesc.push_back(suitresdesc);

					cocos2d::ui::Text* statlbl = (cocos2d::ui::Text*)suitresbox->getChildByName("status");

					Equipable* eres = (Equipable*)MyRes::getMyPutOnResById(eid, herodata->getName());
					if (eres == NULL)
					{
						if (MyRes::getMyResCount(eid) <= 0)
						{
							statlbl->setString(ResourceLang::map_lang["nohas"]);
						}
						else
						{
							statlbl->setString(ResourceLang::map_lang["noputon"]);
						}
						suitresdesc->setOpacity(128);
						suitres->setOpacity(128);
						suitresbox->setOpacity(128);

						int m = 0;
						for (; m < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); m++)
						{
							if (eid.compare(0, 1, RES_TYPES_CHAR[m]) == 0)
								break;
						}

						nohasEquip = new Equip();
						nohasEquip->setId(eid);
						nohasEquip->setType(m);
						suitres->setUserData((void*)nohasEquip);
					}
					else
					{
						str = StringUtils::format("ui/resbox_qu%d.png", eres->getQU().getValue());
						suitresbox->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
						suitres->setUserData((void*)eres);

					}
				}
				else
				{
					str = StringUtils::format("ui/resbox_qu%d.png", m_equip->getQU().getValue());
					suitresbox->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
					suitres->setUserData((void*)m_equip);
				}
			}
			else
			{
				//suitres->setOpacity(128);
				suitresbox->setOpacity(128);
				statlbl->setVisible(true);
				if (i!=0)
				{
					str = StringUtils::format(ResourceLang::map_lang["nothingeffect"].c_str(), i);
					suitresdesc->setString(str);
					suitresdesc->setOpacity(128);
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			str = StringUtils::format("resbox_%d", i);
			cocos2d::ui::ImageView* suitresbox = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);
			//suitresbox->setOpacity(128);
			str = StringUtils::format("res_%d", i);
			cocos2d::ui::ImageView* suitres = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);
			//suitres->setVisible(false);
			str = StringUtils::format("nothing%d", i);
			cocos2d::ui::Text* statlbl = (cocos2d::ui::Text*)csbnode->getChildByName(str);
			statlbl->setString(ResourceLang::map_lang["nothing"]);
			statlbl->setOpacity(168);

			if (i != 0)
			{
				str = StringUtils::format("desc_%d", i);
				cocos2d::ui::Text* suitresdesc = (cocos2d::ui::Text*)csbnode->getChildByName(str);
				str = StringUtils::format(ResourceLang::map_lang["nothingeffect"].c_str(), i);
				suitresdesc->setString(str);
				suitresdesc->setOpacity(128);
			}
		}
	}
	updateUI();

	//选择，更换按钮
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->setTag(1001);
	actionbtn->addTouchEventListener(CC_CALLBACK_2(TakeOnLayer::onBtnClick, this));
	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("replacebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//卸下按钮
	cocos2d::ui::Widget* takeoffbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn_0");
	takeoffbtn->setTag(1003);
	takeoffbtn->addTouchEventListener(CC_CALLBACK_2(TakeOnLayer::onBtnClick, this));
	cocos2d::ui::ImageView* takeoffbtntxt = (cocos2d::ui::ImageView*)takeoffbtn->getChildByName("text");
	takeoffbtntxt->loadTexture(ResourcePath::makeTextImgPath("takeoffbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//强化按钮
	cocos2d::ui::Widget* strenthbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn_1");
	strenthbtn->setTag(1004);
	strenthbtn->addTouchEventListener(CC_CALLBACK_2(TakeOnLayer::onBtnClick, this));
	cocos2d::ui::ImageView* strenthbtntxt = (cocos2d::ui::ImageView*)strenthbtn->getChildByName("text");
	strenthbtntxt->loadTexture(ResourcePath::makeTextImgPath("strenthbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	if (MyRes::getMyPutOnResById(m_equip->getId(), herodata->getName()) == NULL)
	{
		actionbtn->setTag(1002);
		actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("selectbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		takeoffbtn->setVisible(false);
	}

	if (isChangeEquip)
	{
		actionbtn->setPositionX(360);
		strenthbtn->setVisible(false);
	}

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}

void TakeOnLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* btnnode = (Node*)pSender;
		int tag = btnnode->getTag();
		cocos2d::ui::ImageView* btntext = (cocos2d::ui::ImageView*)btnnode->getChildByName("text");
		switch (tag)
		{
		case 1000:
		{
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		}
		case 1001://更换
		{
			isChangeEquip = true;
			SelectEquipLayer* layer = SelectEquipLayer::create(m_equip->getType(), m_herodata);
			this->addChild(layer);
			AnimationEffect::openAniEffect((Layer*)layer);
			break;
		}
		case 1002://选择
		{
			if (isChangeEquip)
			{
				HeroAttrLayer* heroAttrLayer = (HeroAttrLayer*)this->getParent()->getParent()->getParent();
				heroAttrLayer->changeEquip(m_equip);
				this->getParent()->getParent()->removeFromParentAndCleanup(true);
			}
			else
			{
				HeroAttrLayer* heroAttrLayer = (HeroAttrLayer*)this->getParent()->getParent();
				heroAttrLayer->takeOn(m_equip);
				updateAttr();
				this->getParent()->removeFromParentAndCleanup(true);
			}
			break;
		}
		case 1003://卸下
		{
			HeroAttrLayer* heroAttrLayer = (HeroAttrLayer*)this->getParent();
			heroAttrLayer->takeOff(m_equip);
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		}
		case 1004://强化
		{
			StrengthenLayer* sLayer = StrengthenLayer::create(m_equip,0);
			this->addChild(sLayer);
			AnimationEffect::openAniEffect((Layer*)sLayer);
			break;
		}
		default:
			break;
		}
	}
}

void TakeOnLayer::onEquipclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;

		EquipDescLayer* layer = EquipDescLayer::create((ResBase*)node->getUserData(), 1);
		this->addChild(layer);
		AnimationEffect::openAniEffect((Layer*)layer);
	}
}

void TakeOnLayer::onStoneclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		Node* node = (Node*)pSender;

		if (m_equip->vec_stones[node->getTag()].length() > 1)
		{
			SetInStoneLayer* layer = SetInStoneLayer::create(m_equip, node->getTag(), m_herodata);
			this->addChild(layer,0, node->getTag());
			AnimationEffect::openAniEffect((Layer*)layer);
		}
		else
		{
			SelectEquipLayer* layer = SelectEquipLayer::create(T_STONE, m_herodata);
			this->addChild(layer, 0, node->getTag());
			AnimationEffect::openAniEffect((Layer*)layer);
		}
	}
}

void TakeOnLayer::setInStone(ResBase* stoneres, int which)
{
	Quest::setDailyTask(SET_GEM, 1);
	Quest::setAchieveTypeCount(SET_GEM, 1);
	std::string stoneid = stoneres->getId();
	MyRes::Use(stoneid);
	MyRes::Add(stoneid, 1, MYEQUIP);
	m_equip->vec_stones[which] = stoneid;
	updateUI();
}

void TakeOnLayer::setOutStone(std::string stoneid, int which)
{
	MyRes::Add(stoneid);
	MyRes::Use(stoneid, 1, MYEQUIP);
	m_equip->vec_stones[which] = "o";
	updateUI();
}

void TakeOnLayer::chageStone(ResBase* stoneres, int which)
{
	std::string laststone = m_equip->vec_stones[which];
	MyRes::Add(laststone);
	MyRes::Use(laststone, 1, MYEQUIP);

	std::string stoneid = stoneres->getId();
	MyRes::Use(stoneid);
	MyRes::Add(stoneid, 1, MYEQUIP);
	m_equip->vec_stones[which] = stoneid;
	updateUI();
}

void TakeOnLayer::updateUI()
{
	for (unsigned int i = 0; i < vec_suitDesc.size(); i++)
	{
		std::string attrstr;
		float suitval = 0;
		if (i == 0)
		{
			attrstr = "addattrtext_0";
			suitval = m_equip->getSuitHp();
		}
		else if (i == 1)
		{
			attrstr = "addattrtext_2";
			suitval = m_equip->getSuitDf();
		}
		std::string suittextstr = StringUtils::format("suit%d", i+1);
		std::string s2 = StringUtils::format("%s%s", ResourceLang::map_lang[suittextstr].c_str(), ResourceLang::map_lang[attrstr].c_str());
		std::string str = StringUtils::format(s2.c_str(), suitval);
		vec_suitDesc[i]->setString(str);
	}

	std::string stonedescstr[] = { "addattrtext_1","addattrtext_2","addattrtext_0","addattrtext_5","addattrtext_4" };

	for (unsigned int i = 0; i < 3; i++)
	{
		std::string str = StringUtils::format("stone%d", i);
		cocos2d::ui::ImageView* stone = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);

		str = StringUtils::format("equipstonebox_%d", i);
		cocos2d::ui::ImageView* equipstonebox = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);

		str = StringUtils::format("stonedesc%d", i);
		cocos2d::ui::Text* stonedesc = (cocos2d::ui::Text*)csbnode->getChildByName(str);
		if (i < m_equip->vec_stones.size())
		{
			std::string stoneid = m_equip->vec_stones[i];
			if (stoneid.length() > 1)
			{
				str = StringUtils::format("ui/%s.png", m_equip->vec_stones[i].c_str());
				stone->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
				stone->ignoreContentAdaptWithSize(true);
				stone->setVisible(true);

				int intid = atoi(stoneid.substr(1).c_str()) - 1;
				int intv = intid / 3;
				std::string ss = ResourceLang::map_lang[stonedescstr[intv]];
				str = StringUtils::format(ss.c_str(), STONE_BNS[intv][intid % 3]);

				stonedesc->setString(str);
				stonedesc->setVisible(false);
			}
			else
			{
				stone->setVisible(false);
				stonedesc->setVisible(false);
			}
			stone->addTouchEventListener(CC_CALLBACK_2(TakeOnLayer::onStoneclick, this));
			stone->setTag(i);

			equipstonebox->addTouchEventListener(CC_CALLBACK_2(TakeOnLayer::onStoneclick, this));
			equipstonebox->setTag(i);
		}
		else
		{
			stone->setVisible(false);
			stonedesc->setVisible(false);
			equipstonebox->setVisible(false);
		}
	}

	updateAttr();
	MyRes::saveData();
}

void TakeOnLayer::updateAttr()
{	
	std::string sstr = StringUtils::format("+%d%s", m_equip->getLv().getValue(), GlobalInstance::map_AllResources[m_equip->getId()].name.c_str());
	name->setString(sstr);
	float suithp = 0;
	float suitdf = 0;
	if (GlobalInstance::map_EquipSuit[m_equip->getId()].vec_suit.size() >= 2)
	{
		if (MyRes::getMyPutOnResById(GlobalInstance::map_EquipSuit[m_equip->getId()].vec_suit[1], m_herodata->getName()) != NULL)
			suithp = m_equip->getSuitHp();
	}
	if (GlobalInstance::map_EquipSuit[m_equip->getId()].vec_suit.size() >= 3)
	{
		if (MyRes::getMyPutOnResById(GlobalInstance::map_EquipSuit[m_equip->getId()].vec_suit[2], m_herodata->getName()) != NULL)
			suitdf = m_equip->getSuitDf();
	}

	float attrval[] = {
		m_equip->getHp() + suithp,
		m_equip->getAtk(),
		m_equip->getDf() + suitdf,
		m_equip->getAtkSpeed(),
		m_equip->getCrit(),
		m_equip->getDodge()
	};
	//角色属性文字
	for (int i = 0; i <= 5; i++)
	{
		std::string str = StringUtils::format("attrtext_%d", i);
		cocos2d::ui::Text* attrlbl = (cocos2d::ui::Text*)csbnode->getChildByName(str);
		str = StringUtils::format("addattrtext_%d", i);
		str = StringUtils::format(ResourceLang::map_lang[str].c_str(), attrval[i]);
		attrlbl->setString(str);
	}
}