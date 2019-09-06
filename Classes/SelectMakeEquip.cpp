#include "SelectMakeEquip.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MyMenu.h"
#include "AnimationEffect.h"
#include "MakeResLayer.h"

SelectMakeEquip::SelectMakeEquip()
{
	m_clickindex = -1;
}


SelectMakeEquip::~SelectMakeEquip()
{
	m_clickindex = -1;
}


SelectMakeEquip* SelectMakeEquip::create(std::string equipid)
{
	SelectMakeEquip*pRet = new(std::nothrow)SelectMakeEquip();
	if (pRet && pRet->init(equipid))
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

bool SelectMakeEquip::init(std::string equipid)
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node *csbnode = CSLoader::createNode(ResourcePath::makePath("selectEquipLayer.csb"));
	this->addChild(csbnode);

	m_equipid = equipid;

	scrollview = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollview");
	scrollview->setScrollBarEnabled(false);
	scrollview->setBounceEnabled(true);

	cocos2d::ui::ImageView* bg = (cocos2d::ui::ImageView*)csbnode->getChildByName("bg");
	bg->setContentSize(Size(bg->getContentSize().width, 450));

	scrollview->setContentSize(Size(scrollview->getContentSize().width, 360));

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


void SelectMakeEquip::updateContent()
{
	scrollview->removeAllChildrenWithCleanup(true);

	vec_equips.clear();

	loadData();

	int itemheight = 160;
	int ressize = vec_equips.size();

	int row = ressize % 4 == 0 ? ressize / 4 : (ressize / 4 + 1);
	int innerheight = itemheight * row;

	int contentheight = scrollview->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollview->setInnerContainerSize(Size(scrollview->getContentSize().width, innerheight));

	for (unsigned int m = 0; m < vec_equips.size(); m++)
	{
		std::string qustr = "ui/resbox.png";

		int qu = -1;
		int lv = 0;
		if (vec_equips[m]->getType() >= T_ARMOR && vec_equips[m]->getType() <= T_FASHION)
		{
			Equip* equip = (Equip*)vec_equips[m];
			lv = equip->getLv().getValue();
			qu = equip->getQU().getValue();

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
			CC_CALLBACK_1(SelectMakeEquip::onclick, this));
		boxItem->setUserData((void*)vec_equips[m]);
		boxItem->setTag(m);

		boxItem->setPosition(Vec2(qubox->getContentSize().width / 2 + 20 + m % 4 * itemheight, innerheight - itemheight / 2 - m / 4 * itemheight + 5));

		MyMenu* menu = MyMenu::create();
		menu->addChild(boxItem, 0, "item");
		menu->setTouchlimit(scrollview);
		menu->setPosition(Vec2(0, 0));

		scrollview->addChild(menu, 0, m);

		std::string resid = vec_equips[m]->getId();

		std::string str = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);

		Sprite * res = Sprite::createWithSpriteFrameName(str);
		res->setPosition(Vec2(boxItem->getContentSize().width / 2, boxItem->getContentSize().height / 2));
		boxItem->addChild(res);

		std::string namestr = GlobalInstance::map_AllResources[resid].name;
		if (vec_equips[m]->getType() >= T_ARMOR && vec_equips[m]->getType() <= T_FASHION)
		{
			Equipable* eres = (Equipable*)vec_equips[m];
			if (eres->getLv().getValue() > 0)
				namestr = StringUtils::format("+%d%s", lv, namestr.c_str());
		}

		Label *namelbl = Label::createWithTTF(namestr, FONT_NAME, 23);
		namelbl->setColor(Color3B(26, 68, 101));
		namelbl->setPosition(Vec2(boxItem->getContentSize().width / 2, -10));
		boxItem->addChild(namelbl, 0, "resname");

		std::string countstr = StringUtils::format("%d", vec_equips[m]->getCount().getValue());
		Label *countlbl = Label::createWithTTF(countstr, FONT_NAME, 23);
		countlbl->setAnchorPoint(Vec2(1, 0));
		countlbl->setColor(Color3B::WHITE);
		countlbl->setPosition(Vec2(boxItem->getContentSize().width - 10, 10));
		boxItem->addChild(countlbl);
	}
}

void SelectMakeEquip::onclick(Ref* pSender)
{
	SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
	Node* node = (Node*)pSender;
	m_clickindex = node->getTag();

	Equip* equip = (Equip*)node->getUserData();

	MakeResLayer* parentlayer = (MakeResLayer*)this->getParent();
	parentlayer->changeEquip(equip);

	AnimationEffect::closeAniEffect(this);
}

void SelectMakeEquip::loadData()
{
	for (unsigned int i = 0; i < MyRes::vec_MyResources.size(); i++)
	{
		ResBase* res = MyRes::vec_MyResources[i];
		if (res->getId() == m_equipid && res->getWhere() == MYSTORAGE)
			vec_equips.push_back((Equip*)res);
	}
}