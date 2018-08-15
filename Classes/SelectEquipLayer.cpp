#include "SelectEquipLayer.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MyMenu.h"
#include "Equipable.h"
#include "TakeOnLayer.h"
#include "SetInStoneLayer.h"
#include "EquipDescLayer.h"
#include "AnimationEffect.h"

SelectEquipLayer::SelectEquipLayer()
{

}


SelectEquipLayer::~SelectEquipLayer()
{

}


SelectEquipLayer* SelectEquipLayer::create(int restype, Hero* herodata)
{
	SelectEquipLayer *pRet = new(std::nothrow)SelectEquipLayer();
	if (pRet && pRet->init(restype, herodata))
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

bool SelectEquipLayer::init(int restype, Hero* herodata)
{
	if (!Layer::init())
	{
		return false;
	}

	m_herodata = herodata;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node *csbnode = CSLoader::createNode(ResourcePath::makePath("selectEquipLayer.csb"));
	this->addChild(csbnode);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	m_restype = restype;

	//cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)m_csbnode->getChildByName("closebtn");
	//closebtn->setTag(1001);
	//closebtn->addTouchEventListener(CC_CALLBACK_2(StoreHouseLayer::onBtnClick, this));


	scrollview = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollview");
	scrollview->setScrollBarEnabled(false);
	scrollview->setBounceEnabled(true);

	loadData();
	updateContent();

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

void SelectEquipLayer::updateContent()
{
	scrollview->removeAllChildrenWithCleanup(true);


	int itemheight = 160;
	int ressize = MyRes::vec_MyResources.size();

	vec_res.clear();
	loadData();

	int row = ressize % 4 == 0 ? ressize / 4 : (ressize / 4 + 1);
	int innerheight = itemheight * row;

	int contentheight = scrollview->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollview->setInnerContainerSize(Size(scrollview->getContentSize().width, innerheight));

	for (unsigned int m = 0; m < vec_res.size(); m++)
	{
		std::string qustr = "ui/resbox.png";

		int qu = -1;
		int lv = 0;
		if (vec_res[m]->getType() >= T_ARMOR && vec_res[m]->getType() <= T_FASHION)
		{
			Equip* equip = (Equip*)vec_res[m];
			lv = equip->getLv().getValue();
			qu = equip->getQU().getValue();

		}
		else if (vec_res[m]->getType() >= T_WG && vec_res[m]->getType() <= T_NG)
		{
			GongFa* gf = (GongFa*)vec_res[m];
			lv = gf->getLv().getValue() + 1;
			qu = gf->getQU().getValue();
		}
		else
		{
			lv = vec_res[m]->getCount().getValue();
		}
		if (qu >= 0)
		{
			qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}

		Sprite * qubox = Sprite::createWithSpriteFrameName(qustr);

		MenuItemSprite* boxItem = MenuItemSprite::create(
			qubox,
			qubox,
			qubox,
			CC_CALLBACK_1(SelectEquipLayer::onclick, this));
		boxItem->setUserData((void*)vec_res[m]);

		boxItem->setPosition(Vec2(qubox->getContentSize().width / 2 + 20 + m % 4 * 160, innerheight - itemheight / 2 - m / 4 * itemheight));

		MyMenu* menu = MyMenu::create();
		menu->addChild(boxItem);
		menu->setTouchlimit(scrollview);
		menu->setPosition(Vec2(0, 0));

		scrollview->addChild(menu);

		std::string resid = vec_res[m]->getId();

		std::string str = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);

		Sprite * res = Sprite::createWithSpriteFrameName(str);
		res->setPosition(Vec2(boxItem->getContentSize().width / 2, boxItem->getContentSize().height / 2));
		boxItem->addChild(res);

		Label *namelbl = Label::createWithTTF(GlobalInstance::map_AllResources[resid].name, FONT_NAME, 23);
		namelbl->setColor(Color3B(26, 68, 101));
		namelbl->setPosition(Vec2(boxItem->getContentSize().width / 2, -10));
		boxItem->addChild(namelbl);

		std::string countstr = StringUtils::format("%d", lv);
		Label *countlbl = Label::createWithTTF(countstr, FONT_NAME, 23);
		countlbl->setAnchorPoint(Vec2(1, 0));
		countlbl->setColor(Color3B::WHITE);
		countlbl->setPosition(Vec2(boxItem->getContentSize().width - 10, 10));
		boxItem->addChild(countlbl);
	}
}

void SelectEquipLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* btnnode = (Node*)pSender;
		int tag = btnnode->getTag();
		switch (tag)
		{
		case 1000:
		{

			break;
		}
		default:
			break;
		}
	}
}

void SelectEquipLayer::onclick(Ref* pSender)
{
	SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
	Node* node = (Node*)pSender;
	ResBase* res = (ResBase*)node->getUserData();
	Layer* layer;
	if (res->getType() >= T_ARMOR && res->getType() <= T_FASHION)
	{
		layer = TakeOnLayer::create((Equip*)res, m_herodata);
	}
	else if (res->getType() >= T_WG && res->getType() <= T_NG)
	{
		layer = EquipDescLayer::create(res, 3);
	}
	else
	{
		layer = SetInStoneLayer::create(res, this->getTag(), m_herodata);
	}
	this->addChild(layer, 0, this->getTag());
	AnimationEffect::openAniEffect((Layer*)layer);
}

void SelectEquipLayer::loadData()
{
	for (unsigned int i = 0; i < MyRes::vec_MyResources.size(); i++)
	{
		ResBase* res = MyRes::vec_MyResources[i];
		if (res->getType() == m_restype && res->getWhere() == MYSTORAGE)
			vec_res.push_back(res);
	}
}