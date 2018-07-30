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
	this->addChild(color);

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
	std::string str = StringUtils::format("ui/%s.png", res->getId().c_str());
	if (s == 3)
	{
		str = StringUtils::format("ui/%s-2.png", res->getId().c_str());
	}
	else if (s == 4)
	{
		str = StringUtils::format("ui/%s-3.png", res->getId().c_str());
	}
	p_res->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	namelbl->setString(GlobalInstance::map_AllResources[res->getId()].name);
	namelbl->setTextColor(Color4B(POTENTIALCOLOR[s]));

	cocos2d::ui::Text* qua = (cocos2d::ui::Text*)csbnode->getChildByName("qua");
	str = ResourceLang::map_lang["potentialtext"];
	qua->setString(str);

	cocos2d::ui::Text* quatext = (cocos2d::ui::Text*)csbnode->getChildByName("quatext");
	std::string st = StringUtils::format("potential_%d", s);
	str = ResourceLang::map_lang[st];
	quatext->setString(str);
	quatext->setTextColor(Color4B(POTENTIALCOLOR[s]));

	std::vector<float> vec_attrval;
	
	if (m_res->getType() >= T_ARMOR && m_res->getType() <= T_FASHION)
	{
		float bns = POTENTIAL_BNS[s];
		vec_attrval.push_back(GlobalInstance::map_Equip[res->getId()].maxhp * bns);
		vec_attrval.push_back(GlobalInstance::map_Equip[res->getId()].atk * bns);
		vec_attrval.push_back(GlobalInstance::map_Equip[res->getId()].df * bns);
		vec_attrval.push_back(GlobalInstance::map_Equip[res->getId()].speed * bns);
		vec_attrval.push_back(GlobalInstance::map_Equip[res->getId()].crit * bns);
		vec_attrval.push_back(GlobalInstance::map_Equip[res->getId()].avoid * bns);
	}
	else if (m_res->getType() >= T_WG && m_res->getType() <= T_NG)
	{
		GongFa* gf = (GongFa*)m_res;
		vec_attrval.push_back(gf->getHp());
		vec_attrval.push_back(gf->getAtk());
		vec_attrval.push_back(gf->getDf());
		vec_attrval.push_back(gf->getAtkSpeed());
		vec_attrval.push_back(gf->getCrit());
		vec_attrval.push_back(gf->getDodge());
	}

	for (int i = 0; i <= 5; i++)
	{
		str = StringUtils::format("attrtext_%d",i);
		cocos2d::ui::Text* attrlbl = (cocos2d::ui::Text*)csbnode->getChildByName(str);
		str = StringUtils::format("addattrtext_%d", i);
		str = StringUtils::format(ResourceLang::map_lang[str].c_str(), vec_attrval[i]);
		attrlbl->setString(str);
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
		this->removeFromParentAndCleanup(true);
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void EquipDescLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
		if (status == S_EQUIP_DECOMPOSE)
		{
			StoreHouseLayer* storelayer = (StoreHouseLayer*)this->getParent();
			if (storelayer != NULL)
				storelayer->decompose(m_res);
			this->removeFromParentAndCleanup(true);
		}
		else if (status == S_EQUIP_TAKEOFF)
		{
			if (tag == 1)
			{
				WgLvLayer* wglayer = WgLvLayer::create(m_res);
				this->addChild(wglayer);
			}
			else
			{
				HeroAttrLayer* heroAttrLayer = (HeroAttrLayer*)this->getParent();
				heroAttrLayer->takeOff(m_res);
				this->removeFromParentAndCleanup(true);
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
		}
	}
}

void EquipDescLayer::onExit()
{
	Layer::onExit();
}