#include "EquipLibraryInfoLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "MyRes.h"
#include "AnimationEffect.h"
#include "Equip.h"

USING_NS_CC;

EquipLibraryInfoLayer::EquipLibraryInfoLayer()
{
}

EquipLibraryInfoLayer::~EquipLibraryInfoLayer()
{
}


EquipLibraryInfoLayer* EquipLibraryInfoLayer::create(ResBase* res)
{
	EquipLibraryInfoLayer *pRet = new(std::nothrow)EquipLibraryInfoLayer();
	if (pRet && pRet->init(res))
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
bool EquipLibraryInfoLayer::init(ResBase* res)
{
	if (!Layer::init())
	{
		return false;
	}

	m_res = (Equipable*)res;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	int langtype = GlobalInstance::getInstance()->getLang();

	csbnode = CSLoader::createNode(ResourcePath::makePath("equipLibraryInfoLayer.csb"));

	this->addChild(csbnode);

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	cocos2d::ui::ImageView* resbox_qu = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox_qu");

	int s = m_res->getQU().getValue();
	std::string qustr = StringUtils::format("ui/resbox_qu%d.png", s);

	CommonFuncs::playResBoxEffect(resbox_qu, m_res->getType(), s, m_res->getLv().getValue());
	resbox_qu->loadTexture(qustr, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* p_res = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	std::string str = GlobalInstance::getInstance()->getResUIFrameName(res->getId(), s);

	p_res->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	namelbl->setTextColor(Color4B(POTENTIALCOLOR[s]));

	std::string st = StringUtils::format("potential_%d", s);

	std::string namestr = StringUtils::format(ResourceLang::map_lang["libraryinfoequipname"].c_str(), GlobalInstance::map_AllResources[res->getId()].name.c_str(), ResourceLang::map_lang[st].c_str());
	namelbl->setString(namestr);

	Node* contentnode = csbnode->getChildByName("contentnode");
	for (int i = 0; i <= 5; i++)
	{
		str = StringUtils::format("attrtext_%d",i);
		attrlblArr[i] = (cocos2d::ui::Text*)contentnode->getChildByName(str);
	}

	cocos2d::ui::Text* vocationtextlbl = (cocos2d::ui::Text*)contentnode->getChildByName("vocationtext");
	vocationtextlbl->setString(ResourceLang::map_lang["fitjob"]);

	cocos2d::ui::Text* jobtext = (cocos2d::ui::Text*)contentnode->getChildByName("attrtext_6");
	if (m_res->getType() == T_ARMOR)
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
		jobtext->setString(jobstr);
	}
	else if (m_res->getType() >= T_EQUIP && m_res->getType() <= T_FASHION)
	{
		jobtext->setString(ResourceLang::map_lang["fitalljob"]);
	}
	else
	{
		jobtext->setVisible(false);
	}

	cocos2d::ui::Text* desctext = (cocos2d::ui::Text*)contentnode->getChildByName("desctext");
	Label* desclbl = (Label*)desctext->getVirtualRenderer();
	desclbl->setLineSpacing(5);
	desctext->setString(GlobalInstance::map_AllResources[m_res->getId()].desc);

	cocos2d::ui::Text* statustext = (cocos2d::ui::Text*)contentnode->getChildByName("statustext");
	statustext->setString(ResourceLang::map_lang["libraryinfostatus"]);

	cocos2d::ui::Text* status = (cocos2d::ui::Text*)contentnode->getChildByName("status");

	int haswhere = 0;
	std::string whostr;
	std::string equipstrid = m_res->getId();
	for (unsigned int i = 0; i < MyRes::vec_MyResources.size(); i++)
	{
		if (MyRes::vec_MyResources[i]->getId().compare(equipstrid) == 0)
		{
			if (MyRes::vec_MyResources[i]->getQU().getValue() >= m_res->getQU().getValue())
			{
				Equipable* equip = (Equipable*)MyRes::vec_MyResources[i];
				haswhere = MyRes::vec_MyResources[i]->getWhere();
				whostr = equip->getWhos();
				break;
			}
		}
	}

	if (haswhere <= 0)
	{
		status->setString(ResourceLang::map_lang["libraryinfostatus0"]);
	}
	else if (haswhere == MYEQUIP)
	{
		std::string estr = ResourceLang::map_lang["libraryinfostatus1"];
		estr.append(whostr);
		status->setString(estr);
		status->setTextColor(Color4B(0, 255, 0, 255));
	}
	else
	{
		status->setString(ResourceLang::map_lang["libraryinfostatus2"]);
		status->setTextColor(Color4B(0, 255, 0, 255));
	}

	cocos2d::ui::Text* getwheretitle = (cocos2d::ui::Text*)contentnode->getChildByName("getwheretitle");
	getwheretitle->setString(ResourceLang::map_lang["libraryinfogetwhere"]);

	cocos2d::ui::Text* getwheretext = (cocos2d::ui::Text*)contentnode->getChildByName("getwheretext");
	Label* glbl = (Label*)getwheretext->getVirtualRenderer();
	glbl->setLineSpacing(5);
	glbl->setString(ResourceLang::map_lang["libraryequipgetwhere"]);

	//°´Å¥
	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1);
	closebtn->addTouchEventListener(CC_CALLBACK_2(EquipLibraryInfoLayer::onBtnClick, this));


	//ÆÁ±ÎÏÂ²ãµã»÷
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

void EquipLibraryInfoLayer::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
	updateAttr();
}


void EquipLibraryInfoLayer::updateAttr()
{
	std::vector<float> vec_attrval;

	float hp = m_res->getHp();
	float atk = m_res->getAtk();
	float df = m_res->getDf();
	float atkspeed = m_res->getAtkSpeed();
	float crit = m_res->getCrit();
	float dodge = m_res->getDodge();

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
	}
}

void EquipLibraryInfoLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		AnimationEffect::closeAniEffect(this);
	}
}

void EquipLibraryInfoLayer::onExit()
{
	Layer::onExit();
}