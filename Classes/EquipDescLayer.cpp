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

USING_NS_CC;

EquipDescLayer::EquipDescLayer()
{

}

EquipDescLayer::~EquipDescLayer()
{

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

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("equipDescLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	cocos2d::ui::ImageView* resbox_qu = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox_qu");
	int s = m_res->getQU().getValue();
	std::string qustr = StringUtils::format("ui/resbox_qu%d.png", s);
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
	}

	updateAttr();

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
		str = StringUtils::format(ResourceLang::map_lang["fitjob"].c_str(), jobstr.c_str());
		jobtext->setString(str);
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
		str = StringUtils::format(ResourceLang::map_lang["fitjob"].c_str(), jobstr.c_str());
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

	//增加了多少攻击力
	cocos2d::ui::Text* attaddlb = (cocos2d::ui::Text*)csbnode->getChildByName("attaddlb");
	attaddlb->setString(str);
	attaddlb->setVisible(false);

	//增加了多少防御力
	cocos2d::ui::Text* prominuslb = (cocos2d::ui::Text*)csbnode->getChildByName("prominuslb");
	prominuslb->setString(str);
	prominuslb->setVisible(false);


	//按钮1
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(EquipDescLayer::onBtnClick, this));
	cocos2d::ui::ImageView* srefreshbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	//按钮2
	cocos2d::ui::Widget* lvbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("lvbtn");
	lvbtn->setTag(1);
	lvbtn->addTouchEventListener(CC_CALLBACK_2(EquipDescLayer::onBtnClick, this));
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
		if (GlobalInstance::map_AllResources[res->getId()].vec_needres.size() > 0)
		{
			status = S_EQUIP_DECOMPOSE;
			srefreshbtntxt->loadTexture(ResourcePath::makeTextImgPath("decomposebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		}
		else {
			status = S_EQUIP_WGLV;
			srefreshbtntxt->loadTexture(ResourcePath::makeTextImgPath("lvupbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		}
	}
	else if (fromwhere == 1)//takeon界面查看套装装备信息
	{
		actionbtn->setVisible(false);
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

void EquipDescLayer::updateAttr()
{
	std::vector<float> vec_attrval;

	vec_attrval.push_back(m_res->getHp());
	vec_attrval.push_back(m_res->getAtk());
	vec_attrval.push_back(m_res->getDf());
	vec_attrval.push_back(m_res->getAtkSpeed());
	vec_attrval.push_back(m_res->getCrit());
	vec_attrval.push_back(m_res->getDodge());

	for (int i = 0; i <= 5; i++)
	{
		std::string str = StringUtils::format("addattrtext_%d", i);
		str = StringUtils::format(ResourceLang::map_lang[str].c_str(), vec_attrval[i]);
		attrlblArr[i]->setString(str);
	}
	std::string namestr = GlobalInstance::map_AllResources[m_res->getId()].name;

	if (m_res->getLv().getValue() > 0)
		namestr = StringUtils::format("+%d%s", m_res->getLv().getValue(), namestr.c_str());
	namelbl->setString(namestr);
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
		else
		{
			if (status == S_EQUIP_DECOMPOSE)
			{
				StoreHouseLayer* storelayer = (StoreHouseLayer*)this->getParent();
				if (storelayer != NULL)
					storelayer->decompose(m_res);
				AnimationEffect::closeAniEffect((Layer*)this);
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
		}
	}
}

void EquipDescLayer::onExit()
{
	Layer::onExit();
}