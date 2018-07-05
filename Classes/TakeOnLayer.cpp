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

static bool isChangeEquip = false;
TakeOnLayer::TakeOnLayer()
{

}


TakeOnLayer::~TakeOnLayer()
{
	isChangeEquip = false;
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

	Node *csbnode = CSLoader::createNode(ResourcePath::makePath("takeOnLayer.csb"));
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

	float hpbns = 0.0f;
	float atkbns = 0.0f;
	float dfbns = 0.0f;
	float critbns = 0.0f;
	float dodgebns = 0.0f;

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

				if (i > 0)
				{

					std::string attrstr;
					if (i == 1)
						attrstr = "addattrtext_0";
					else if (i == 2)
						attrstr = "addattrtext_2";
					std::string suittextstr = StringUtils::format("suit%d", i);
					std::string s2 = StringUtils::format("%s%s", ResourceLang::map_lang[suittextstr].c_str(), ResourceLang::map_lang[attrstr].c_str());
					str = StringUtils::format(s2.c_str(), map_suit[m_equip->getId()].vec_bns[i-1]);
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
					}
					else
					{
						str = StringUtils::format("ui/resbox_qu%d.png", eres->getQU().getValue());
						suitresbox->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
						if (i == 1)
							hpbns += map_suit[m_equip->getId()].vec_bns[i - 1];
						else if (i == 2)
							dfbns = map_suit[m_equip->getId()].vec_bns[i - 1];
					}
				}
				else
				{
					str = StringUtils::format("ui/resbox_qu%d.png", m_equip->getQU().getValue());
					suitresbox->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
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

	std::string stonedescstr[] = {"addattrtext_1","addattrtext_2","addattrtext_0","addattrtext_5","addattrtext_4"};
	float stonebns[] = { 10.0f,10.0f,10.0f,10.0f,10.0f };
	for (int i = 0; i < 3; i++)
	{
		str = StringUtils::format("stone%d", i);
		cocos2d::ui::ImageView* stone = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);

		str = StringUtils::format("stonedesc%d", i);
		cocos2d::ui::Text* stonedesc = (cocos2d::ui::Text*)csbnode->getChildByName(str);
		if (i < m_equip->vec_stones.size())
		{
			std::string stoneid = m_equip->vec_stones[i];
			if (stoneid.length() > 0)
			{
				str = StringUtils::format("ui/%s.png", m_equip->vec_stones[i].c_str());
				stone->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
				int intv = (atoi(stoneid.substr(1).c_str()) -1)/3;
				std::string ss = ResourceLang::map_lang[stonedescstr[intv]];
				str = StringUtils::format(ss.c_str(), stonebns[intv]);
				stonedesc->setString(str);

				if (intv == 0)
					atkbns += stonebns[intv];
				else if (intv == 1)
					dfbns += stonebns[intv];
				else if (intv == 2)
					hpbns+= stonebns[intv];
				else if (intv == 3)
					dodgebns+= stonebns[intv];
				else if (intv == 4)
					critbns += stonebns[intv];
			}
			else
			{
				stone->setVisible(false);
				stonedesc->setVisible(false);
			}
		}
		else
		{
			stone->setVisible(false);
			stonedesc->setVisible(false);
		}
	}

	langtype = GlobalInstance::getInstance()->getLang();
	cocos2d::ui::ImageView* proptextimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("equipprop");
	proptextimg->loadTexture(ResourcePath::makeTextImgPath("equipprop_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	float bns = GlobalInstance::map_Equip[m_equip->getId()].vec_bns[qu];
	int max = GlobalInstance::map_Equip[m_equip->getId()].maxhp;
	float hp = bns * GlobalInstance::map_Equip[m_equip->getId()].maxhp;
	hp *= (1 + hpbns/100);

	float atk = bns * GlobalInstance::map_Equip[m_equip->getId()].atk;
	atk *= (1 + atkbns/100);

	float df = bns *GlobalInstance::map_Equip[m_equip->getId()].df;

	df *= (1 + dfbns/100);

	float atkspeed = GlobalInstance::map_Equip[m_equip->getId()].speed;

	float crit = bns*GlobalInstance::map_Equip[m_equip->getId()].crit;
	crit += critbns;

	float dodge = bns*GlobalInstance::map_Equip[m_equip->getId()].avoid;
	dodge += dodgebns;

	float attrval[] = {
		hp,
		atk,
		df,
		atkspeed,
		crit,
		dodge
	};
	//角色属性文字
	for (int i = 0; i <= 5; i++)
	{
		str = StringUtils::format("attrtext_%d", i);
		cocos2d::ui::Text* attrlbl = (cocos2d::ui::Text*)csbnode->getChildByName(str);
		str = StringUtils::format("addattrtext_%d", i);
		str = StringUtils::format(ResourceLang::map_lang[str].c_str(), attrval[i]);
		attrlbl->setString(str);
	}
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

void TakeOnLayer::onclick(Ref* pSender)
{
	SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
	Node* node = (Node*)pSender;
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