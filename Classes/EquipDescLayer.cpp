#include "EquipDescLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MovingLabel.h"
#include "MyRes.h"
#include "StoreHouseLayer.h"

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

	m_res = res;
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
	Equip* qres = (Equip*)res;
	int s = qres->getQU().getValue();
	std::string qustr = StringUtils::format("ui/resbox_qu%d.png", s);
	resbox_qu->loadTexture(qustr, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* p_res = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	std::string str = StringUtils::format("ui/%s.png", GlobalInstance::map_Equip[res->getId()].id.c_str());
	if (s == 3)
	{
		str = StringUtils::format("ui/%s_2.png", GlobalInstance::map_Equip[res->getId()].id.c_str());
	}
	else if (s == 4)
	{
		str = StringUtils::format("ui/%s_3.png", GlobalInstance::map_Equip[res->getId()].id.c_str());
	}
	p_res->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	namelbl->setString(GlobalInstance::map_Equip[res->getId()].name);

	cocos2d::ui::Text* attactlb = (cocos2d::ui::Text*)csbnode->getChildByName("attactlb");
	str = StringUtils::format(ResourceLang::map_lang["attlabel"].c_str(), GlobalInstance::map_Equip[res->getId()].atk);
	attactlb->setString(str);

	cocos2d::ui::Text* protectlb = (cocos2d::ui::Text*)csbnode->getChildByName("protectlb");
	str = StringUtils::format(ResourceLang::map_lang["dflabel"].c_str(), GlobalInstance::map_Equip[res->getId()].df);
	protectlb->setString(str);

	cocos2d::ui::Text* livelb = (cocos2d::ui::Text*)csbnode->getChildByName("livelb");
	str = StringUtils::format(ResourceLang::map_lang["livelabel"].c_str(), GlobalInstance::map_Equip[res->getId()].maxhp);
	livelb->setString(str);

	cocos2d::ui::Text* attspeedlb = (cocos2d::ui::Text*)csbnode->getChildByName("attspeedlb");
	str = StringUtils::format(ResourceLang::map_lang["speedlabel"].c_str(), GlobalInstance::map_Equip[res->getId()].speed);
	attspeedlb->setString(str);

	cocos2d::ui::Text* vioattlb = (cocos2d::ui::Text*)csbnode->getChildByName("vioattlb");
	str = StringUtils::format(ResourceLang::map_lang["critlabel"].c_str(), GlobalInstance::map_Equip[res->getId()].avoid*100);
	vioattlb->setString(str);

	cocos2d::ui::Text* evadelb = (cocos2d::ui::Text*)csbnode->getChildByName("evadelb");
	str = StringUtils::format(ResourceLang::map_lang["avoidlabel"].c_str(), GlobalInstance::map_Equip[res->getId()].crit*100);
	evadelb->setString(str);

	cocos2d::ui::Text* qua = (cocos2d::ui::Text*)csbnode->getChildByName("qua");
	str = ResourceLang::map_lang["potentialtext"];
	qua->setString(str);

	cocos2d::ui::Text* quatext = (cocos2d::ui::Text*)csbnode->getChildByName("quatext");
	std::string st = StringUtils::format("potential_%d", s);
	str = ResourceLang::map_lang[st];
	quatext->setString(str);

	//增加了多少攻击力
	cocos2d::ui::Text* attaddlb = (cocos2d::ui::Text*)csbnode->getChildByName("attaddlb");
	attaddlb->setString(str);
	attaddlb->setVisible(false);

	//增加了多少防御力
	cocos2d::ui::Text* prominuslb = (cocos2d::ui::Text*)csbnode->getChildByName("prominuslb");
	prominuslb->setString(str);
	prominuslb->setVisible(false);


	//按钮
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(EquipDescLayer::onBtnClick, this));
	cocos2d::ui::ImageView* srefreshbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	if (fromwhere == 0)//仓库进入
	{
		if (GlobalInstance::map_AllResources[res->getId()].vec_needres.size() > 0)
		{
			status = S_EQUIP_DECOMPOSE;
			srefreshbtntxt->loadTexture(ResourcePath::makeTextImgPath("decomposebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		}
		else {
			status = S_EQUIP_OTHER;
			srefreshbtntxt->loadTexture(ResourcePath::makeTextImgPath("okbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		}
	}
	else
	{
		status = S_EQUIP_SEL;
		srefreshbtntxt->loadTexture(ResourcePath::makeTextImgPath("usebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
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
		if (status == S_EQUIP_DECOMPOSE)
		{
			StoreHouseLayer* storelayer = (StoreHouseLayer*)this->getParent();
			if (storelayer != NULL)
				storelayer->decompose(m_res);
		}
		this->removeFromParentAndCleanup(true);
	}
}

void EquipDescLayer::onExit()
{
	Layer::onExit();
}