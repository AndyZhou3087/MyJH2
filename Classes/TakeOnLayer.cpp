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

static bool isChangeEquip = false;
TakeOnLayer::TakeOnLayer()
{
	nohasEquip = NULL;
	stone_atkbns = 0;
	stone_dfbns = 0;
	stone_hpbns = 0;
	stone_dodgebns = 0;
	stone_critbns = 0;
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
	m_herodata = herodata;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);

	csbnode = CSLoader::createNode(ResourcePath::makePath("takeOnLayer.csb"));
	this->addChild(csbnode);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	m_equip = res_equip;

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1000);
	closebtn->addTouchEventListener(CC_CALLBACK_2(TakeOnLayer::onBtnClick, this));

	int qu = m_equip->getQU().getValue();

	std::string str;
	cocos2d::ui::Text* name = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	str = StringUtils::format("+%d%s", m_equip->getLv().getValue(), GlobalInstance::map_AllResources[m_equip->getId()].name.c_str());
	name->setString(str);
	name->setTextColor(Color4B(POTENTIALCOLOR[qu]));

	cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox");
	str = StringUtils::format("ui/resbox_qu%d.png", qu);
	resbox->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* p_res = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	str = StringUtils::format("ui/%s.png", m_equip->getId().c_str());
	if (qu == 3)
	{
		str = StringUtils::format("ui/%s_2.png", m_equip->getId().c_str());
	}
	else if (qu == 4)
	{
		str = StringUtils::format("ui/%s_3.png", m_equip->getId().c_str());
	}
	p_res->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* qutext = (cocos2d::ui::Text*)csbnode->getChildByName("qutext");
	qutext->setString(ResourceLang::map_lang["potentialtext"]);

	cocos2d::ui::Text* qulbl = (cocos2d::ui::Text*)csbnode->getChildByName("qu");
	std::string st = StringUtils::format("potential_%d", qu);
	qulbl->setString(ResourceLang::map_lang[st]);
	qulbl->setTextColor(Color4B(POTENTIALCOLOR[qu]));

	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desc->setString(GlobalInstance::map_AllResources[m_equip->getId()].desc);

	parseSuitJson();

	int equ = m_equip->getQU().getValue();

	float bns = POTENTIAL_BNS[equ];

	suithpbns = 0.0f;
	suitdfbns = 0.0f;

	int basehp = GlobalInstance::map_Equip[m_equip->getId()].maxhp;
	int basedf = GlobalInstance::map_Equip[m_equip->getId()].df;

	if (map_suit.find(m_equip->getId()) != map_suit.end())
	{
		for (unsigned int i = 0; i < 3; i++)
		{
			str = StringUtils::format("desc_%d", i);
			cocos2d::ui::Text* suitresdesc = (cocos2d::ui::Text*)csbnode->getChildByName(str);

			str = StringUtils::format("resbox_%d", i);
			cocos2d::ui::ImageView* suitresbox = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);

			str = StringUtils::format("res_%d", i);
			cocos2d::ui::ImageView* suitres = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);

			if (i < map_suit[m_equip->getId()].vec_suit.size())
			{
				std::string eid = map_suit[m_equip->getId()].vec_suit[i];
				str = StringUtils::format("ui/%s.png", eid.c_str());
				suitres->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
				suitres->addTouchEventListener(CC_CALLBACK_2(TakeOnLayer::onEquipclick, this));

				if (i > 0)
				{

					std::string attrstr;
					float suitval = 0;
					if (i == 1)
					{
						attrstr = "addattrtext_0";
						suitval = map_suit[m_equip->getId()].vec_bns[i - 1] * basehp / 100;
					}
					else if (i == 2)
					{
						attrstr = "addattrtext_2";
						suitval = map_suit[m_equip->getId()].vec_bns[i - 1] * basedf / 100;
					}
					std::string suittextstr = StringUtils::format("suit%d", i);
					std::string s2 = StringUtils::format("%s%s", ResourceLang::map_lang[suittextstr].c_str(), ResourceLang::map_lang[attrstr].c_str());
					str = StringUtils::format(s2.c_str(), suitval);
					suitresdesc->setString(str);

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

						nohasEquip = new Equip();
						nohasEquip->setId(eid);
						suitres->setUserData((void*)nohasEquip);
					}
					else
					{
						str = StringUtils::format("ui/resbox_qu%d.png", eres->getQU().getValue());
						suitresbox->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
						if (i == 1)
						{
							suithpbns += basehp * map_suit[m_equip->getId()].vec_bns[i - 1] / 100;
						}
						else if (i == 2)
						{
							edf += basedf * map_suit[m_equip->getId()].vec_bns[i - 1] / 100;
						}
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
				suitres->setOpacity(128);
				suitresbox->setOpacity(128);
			}
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			str = StringUtils::format("resbox_%d", i);
			cocos2d::ui::ImageView* suitresbox = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);
			suitresbox->setOpacity(128);
			str = StringUtils::format("res_%d", i);
			cocos2d::ui::ImageView* suitres = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);
			suitres->setVisible(false);
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
			this->removeFromParentAndCleanup(true);
			break;
		}
		case 1001://更换
		{
			isChangeEquip = true;
			SelectEquipLayer* layer = SelectEquipLayer::create(m_equip->getType(), m_herodata);
			this->addChild(layer);
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
			this->removeFromParentAndCleanup(true);
			break;
		}
		case 1004://强化
		{
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
	}
}

void TakeOnLayer::onStoneclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;

		if (m_equip->vec_stones[node->getTag()].length() > 1)
		{
			SetInStoneLayer* layer = SetInStoneLayer::create(m_equip, node->getTag(), m_herodata);
			this->addChild(layer,0, node->getTag());
		}
		else
		{
			SelectEquipLayer* layer = SelectEquipLayer::create(T_STONE, m_herodata);
			this->addChild(layer, 0, node->getTag());
		}
	}
}

void TakeOnLayer::setInStone(ResBase* stoneres, int which)
{
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
	std::string stonedescstr[] = { "addattrtext_1","addattrtext_2","addattrtext_0","addattrtext_5","addattrtext_4" };

	stone_atkbns = 0;
	stone_dfbns = 0;
	stone_hpbns = 0;
	stone_dodgebns = 0;
	stone_critbns = 0;

	for (int i = 0; i < 3; i++)
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

				int intv = (atoi(stoneid.substr(1).c_str()) - 1) / 3;
				std::string ss = ResourceLang::map_lang[stonedescstr[intv]];
				str = StringUtils::format(ss.c_str(), stonebns[intv]);

				if (intv == 0)
					stone_atkbns += stonebns[intv];
				else if (intv == 1)
					stone_dfbns += stonebns[intv];
				else if (intv == 2)
					stone_hpbns += stonebns[intv];
				else if (intv == 3)
				{
					str = str.substr(0, str.length() - 1);
					stone_dodgebns += stonebns[intv];
				}
				else if (intv == 4)
				{
					str = str.substr(0, str.length() - 1);
					stone_critbns += stonebns[intv];
				}
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
	int equ = m_equip->getQU().getValue();
	float ebns = POTENTIAL_BNS[equ];

	ehp = ebns * GlobalInstance::map_Equip[m_equip->getId()].maxhp;
	eatk = ebns * GlobalInstance::map_Equip[m_equip->getId()].atk;
	edf = ebns *  GlobalInstance::map_Equip[m_equip->getId()].df;

	eatkspeed = ebns * GlobalInstance::map_Equip[m_equip->getId()].speed;
	ecrit = ebns * GlobalInstance::map_Equip[m_equip->getId()].crit;
	edodge = ebns * GlobalInstance::map_Equip[m_equip->getId()].avoid;

	if (MyRes::getMyPutOnResByType(m_equip->getType(), m_herodata->getName()) != NULL)
	{
		int hqu = m_herodata->getVocation();
		float hbns = GlobalInstance::map_Equip[m_equip->getId()].vec_bns[hqu];
		ehp += hbns * GlobalInstance::map_Equip[m_equip->getId()].maxhp;
		eatk += hbns * GlobalInstance::map_Equip[m_equip->getId()].atk;
		edf += hbns *  GlobalInstance::map_Equip[m_equip->getId()].df;

		eatkspeed += hbns * GlobalInstance::map_Equip[m_equip->getId()].speed;
		ecrit = + hbns * GlobalInstance::map_Equip[m_equip->getId()].crit;
		edodge = + hbns * GlobalInstance::map_Equip[m_equip->getId()].avoid;
	}

	float attrval[] = {
		ehp + stone_hpbns + suithpbns,
		eatk + stone_atkbns,
		edf + stone_dfbns + suitdfbns,
		eatkspeed,
		ecrit + stone_critbns,
		edodge + stone_dodgebns
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

void TakeOnLayer::parseSuitJson()
{
	rapidjson::Document doc = ReadJsonFile(ResourcePath::makePath("json/suitequip.json"));
	rapidjson::Value& allData = doc["sq"];
	for (unsigned int i = 0; i < allData.Size(); i++)
	{
		rapidjson::Value& jsonvalue = allData[i];
		if (jsonvalue.IsObject())
		{
			EquipSuit data;
			rapidjson::Value& v = jsonvalue["id"];
			data.id = v.GetString();

			v = jsonvalue["suite"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_suit.push_back(v[m].GetString());
			}
			v = jsonvalue["bns"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_bns.push_back(v[m].GetDouble());
			}
			map_suit[data.id] = data;
		}
	}
}