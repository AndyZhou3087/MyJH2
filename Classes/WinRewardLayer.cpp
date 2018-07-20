#include "WinRewardLayer.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MyMenu.h"
#include "MovingLabel.h"

WinRewardLayer::WinRewardLayer()
{

}


WinRewardLayer::~WinRewardLayer()
{
	for (unsigned int i = 0; i < vec_dropdownres.size(); i++)
	{
		delete vec_dropdownres[i];
	}
}

WinRewardLayer* WinRewardLayer::create(std::vector<FOURProperty> reward_res)
{
	WinRewardLayer *pRet = new(std::nothrow)WinRewardLayer();
	if (pRet && pRet->init(reward_res))
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

bool WinRewardLayer::init(std::vector<FOURProperty> reward_res)
{

	m_rewards = reward_res;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("winRewardLayer.csb"));
	this->addChild(csbnode);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	int langtype = GlobalInstance::getInstance()->getLang();

	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("winrewardtitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1002);
	closebtn->addTouchEventListener(CC_CALLBACK_2(WinRewardLayer::onBtnClick, this));

	//按钮
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->setTag(1000);
	actionbtn->addTouchEventListener(CC_CALLBACK_2(WinRewardLayer::onBtnClick, this));

	//按钮文字
	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("allgetbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);


	//按钮
	cocos2d::ui::Widget* continuebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn_0");
	continuebtn->setTag(1001);
	continuebtn->addTouchEventListener(CC_CALLBACK_2(WinRewardLayer::onBtnClick, this));

	//按钮文字
	cocos2d::ui::ImageView* continuebtntxt = (cocos2d::ui::ImageView*)continuebtn->getChildByName("text");
	continuebtntxt->loadTexture(ResourcePath::makeTextImgPath("continuegamebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* droptxt = (cocos2d::ui::ImageView*)csbnode->getChildByName("winsepbox")->getChildByName("text");
	droptxt->loadTexture(ResourcePath::makeTextImgPath("drop_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* mypackagetxt = (cocos2d::ui::ImageView*)csbnode->getChildByName("winsepbox_0")->getChildByName("text");
	mypackagetxt->loadTexture(ResourcePath::makeTextImgPath("package_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* carrytext = (cocos2d::ui::Text*)csbnode->getChildByName("carrytext");
	carrytext->setString(ResourceLang::map_lang["carrytext"]);

	carrycountlbl = (cocos2d::ui::Text*)csbnode->getChildByName("carrycount");

	cocos2d::ui::ScrollView* dropscrollview = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollview");
	dropscrollview->setScrollBarEnabled(false);
	dropscrollview->setBounceEnabled(true);
	vec_scrollview.push_back(dropscrollview);

	cocos2d::ui::ScrollView* mypackagescrollview = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollview0");
	mypackagescrollview->setScrollBarEnabled(false);
	mypackagescrollview->setBounceEnabled(true);
	vec_scrollview.push_back(mypackagescrollview);

	loadScrollviewData();
	updateScrollviewContent();

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}

void WinRewardLayer::updateScrollviewContent()
{
	for (unsigned int i = 0; i < 2; i++)
	{
		cocos2d::ui::ScrollView* sv = vec_scrollview[i];
		sv->removeAllChildrenWithCleanup(true);

		std::vector<ResBase*> vec_res;
		if (i == 0)
			vec_res = vec_dropdownres;
		else
			vec_res = vec_mypackagres;
		int ressize = vec_res.size();
		int row = ressize % 4 == 0 ? ressize / 4 : (ressize / 4 + 1);

		int itemheight = 160;

		int innerheight = itemheight * row;

		int contentheight = sv->getContentSize().height;
		if (innerheight < contentheight)
			innerheight = contentheight;
		sv->setInnerContainerSize(Size(sv->getContentSize().width, innerheight));


		for (unsigned int m = 0; m < vec_res.size(); m++)
		{
			std::string qustr = "ui/resbox.png";
			int qu = 0;
			if (vec_res[m]->getType() >= T_ARMOR && vec_res[m]->getType() <= T_NG)
			{
				qu = ((Equipable*)vec_res[m])->getQU().getValue();
				qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
			}

			Sprite * qubox = Sprite::createWithSpriteFrameName(qustr);

			MenuItemSprite* boxItem = MenuItemSprite::create(
				qubox,
				qubox,
				qubox,
				CC_CALLBACK_1(WinRewardLayer::onclick, this));
			boxItem->setUserData((void*)vec_res[m]);
			boxItem->setTag(i * 10000 + m);

			boxItem->setPosition(Vec2(qubox->getContentSize().width / 2 + 20 + m % 4 * 160, innerheight - itemheight / 2 - m / 4 * itemheight));

			MyMenu* menu = MyMenu::create();
			menu->addChild(boxItem);
			menu->setTouchlimit(sv);
			menu->setPosition(Vec2(0, 0));
			sv->addChild(menu);

			std::string resid = vec_res[m]->getId();

			std::string str = StringUtils::format("ui/%s.png", resid.c_str());
			if (qu == 3)
			{
				str = StringUtils::format("ui/%s-2.png", resid.c_str());
			}
			else if (qu == 4)
			{
				str = StringUtils::format("ui/%s-3.png", resid.c_str());
			}

			Sprite * res = Sprite::createWithSpriteFrameName(str);
			res->setPosition(Vec2(boxItem->getContentSize().width / 2, boxItem->getContentSize().height / 2));
			boxItem->addChild(res);

			Label *namelbl = Label::createWithTTF(GlobalInstance::map_AllResources[resid].name, FONT_NAME, 23);
			namelbl->setColor(Color3B(34, 74, 79));
			namelbl->setPosition(Vec2(boxItem->getContentSize().width / 2, -20));
			boxItem->addChild(namelbl);

			std::string countstr = StringUtils::format("%d", vec_res[m]->getCount().getValue());
			Label *countlbl = Label::createWithTTF(countstr, FONT_NAME, 23);
			countlbl->setAnchorPoint(Vec2(1, 0));
			countlbl->setColor(Color3B::WHITE);
			countlbl->setPosition(Vec2(boxItem->getContentSize().width - 10, 10));
			boxItem->addChild(countlbl);
		}
	}
	std::string str = StringUtils::format("%d/%d", MyRes::getMyPackageCount(), GlobalInstance::getInstance()->getTotalCarry());
	carrycountlbl->setString(str);
}

void WinRewardLayer::loadScrollviewData()
{
	std::string types[] = { "r","a","e","h","f","w","x","s","c","d","m","b","y" };

	for (unsigned int i = 0; i < m_rewards.size(); i++)
	{
		std::string rid = m_rewards[i].sid;

		int t = 0;
		for (; t < sizeof(types) / sizeof(types[0]); t++)
		{
			if (rid.compare(0, 1, types[t]) == 0)
				break;
		}
		int qu = m_rewards[i].intPara2;
		ResBase* res;
		if (t >= T_ARMOR && t <= T_FASHION)
		{
			res = new Equip();
			int stonecount = GlobalInstance::getInstance()->generateStoneCount(qu);

			for (int m = 0; m < stonecount; m++)
			{
				((Equip*)res)->vec_stones.push_back("o");
			}
		}
		else
			res = new ResBase();
		res->setType(t);
		res->setId(rid);
		DynamicValueInt dv;
		dv.setValue(m_rewards[i].intPara1);
		res->setCount(dv);
		dv.setValue(qu);
		res->setQU(dv);
		res->setWhere(MYPACKAGE);

		vec_dropdownres.push_back(res);
	}
	loadMyPackageRes();
}

void WinRewardLayer::loadMyPackageRes()
{
	vec_mypackagres.clear();
	for (unsigned int i = 0; i < MyRes::vec_MyResources.size(); i++)
	{
		ResBase* res = MyRes::vec_MyResources[i];
		if (res->getWhere() == MYPACKAGE)
			vec_mypackagres.push_back(res);
	}
}

void WinRewardLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		cocos2d::ui::Button* btnnode = (cocos2d::ui::Button*)pSender;
		int tag = btnnode->getTag();
		switch (tag)
		{
		case 1000://allget
		{
			int packagecount = 0;

			for (unsigned int i = 0; i < vec_dropdownres.size(); i++)
			{
				packagecount += vec_dropdownres[i]->getCount().getValue();
			}

			if (MyRes::getMyPackageCount() + packagecount > GlobalInstance::getInstance()->getTotalCarry())
			{
				MovingLabel::show(ResourceLang::map_lang["carryovertext"]);
				return;
			}

			btnnode->setEnabled(false);
			float dt = 0.1f;
			if (vec_dropdownres.size() > 0)
			{
				for (unsigned int i = 0; i < vec_dropdownres.size(); i++)
				{
					MyRes::Add(vec_dropdownres[i], vec_dropdownres[i]->getCount().getValue(), MYPACKAGE);
				}
				vec_dropdownres.clear();
				loadMyPackageRes();

				updateScrollviewContent();
				dt = 1.0f;
			}
			//this->scheduleOnce(schedule_selector(WinRewardLayer::delayClose), dt);
			break;
		}
		case 1001://continue
		{
			this->getParent()->removeFromParentAndCleanup(true);
		}
			break;
		case 1002://close
			this->getParent()->removeFromParentAndCleanup(true);
			break;
		default:
			break;
		}
	}
}

void WinRewardLayer::onclick(Ref* pSender)
{
	SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
	Node* node = (Node*)pSender;
	int tag = node->getTag();
	if (tag / 10000 == 0)//点击的是两个scrollview的 0--掉落，1--背包
	{
		if (MyRes::getMyPackageCount() + 1 > GlobalInstance::getInstance()->getTotalCarry())
		{
			MovingLabel::show(ResourceLang::map_lang["carryovertext"]);
			return;
		}
		ResBase* res = (ResBase*)node->getUserData();
		reduceDropRes(res, 1, tag % 10000);
	}
	else if (tag / 10000 == 1)
	{
		ResBase* res = (ResBase*)node->getUserData();
		addDropRes(res);
	}
	updateScrollviewContent();
}

void WinRewardLayer::delayClose(float dt)
{
	this->getParent()->removeFromParentAndCleanup(true);
}

void WinRewardLayer::addDropRes(ResBase* res)
{
	int type = res->getType();

	if (type >= T_ARMOR && type <= T_FASHION)
	{
		Equip* ores = (Equip*)res;
		Equip* eres = new Equip();
		eres->setId(ores->getId());
		eres->setCount(ores->getCount());
		eres->setLv(ores->getLv());
		eres->setQU(ores->getQU());
		eres->setType(ores->getType());
		eres->setWhere(ores->getWhere());
		eres->setWhos(ores->getWhos());
		vec_dropdownres.push_back(res);
	}
	else if (type >= T_WG && type <= T_NG)
	{
		GongFa* ores = (GongFa*)res;
		GongFa* eres = new GongFa();
		eres->setId(ores->getId());
		eres->setCount(ores->getCount());
		eres->setExp(ores->getExp());
		eres->setQU(ores->getQU());
		eres->setType(ores->getType());
		eres->setWhere(ores->getWhere());
		eres->setWhos(ores->getWhos());
		vec_dropdownres.push_back(res);
	}
	else
	{
		//std::string resid;
		int index = -1;
		for (unsigned int i = 0; i < vec_dropdownres.size(); i++)
		{
			if (vec_dropdownres[i]->getId().compare(res->getId()) == 0)
			{
				index = i;
				break;
			}
		}

		if (index >= 0)
		{
			DynamicValueInt dv;
			dv.setValue(vec_dropdownres[index]->getCount().getValue() + 1);
			vec_dropdownres[index]->setCount(dv);
		}
		else
		{
			ResBase* nres = new ResBase();
			nres->setId(res->getId());
			nres->setType(type);
			nres->setWhere(MYPACKAGE);
			DynamicValueInt dv;
			dv.setValue(1);
			nres->setCount(dv);
			vec_dropdownres.push_back(nres);
		}
	}
	MyRes::Use(res, 1, MYPACKAGE);
	loadMyPackageRes();
}

void WinRewardLayer::reduceDropRes(ResBase* res, int count, int iteindex)
{
	int type = res->getType();

	if ((type >= T_ARMOR && type <= T_FASHION) || (type >= T_WG && type <= T_NG))
	{
		vec_dropdownres.erase(vec_dropdownres.begin() + iteindex);
	}
	else
	{
		if (vec_dropdownres[iteindex]->getCount().getValue() > count)
		{
			DynamicValueInt dv;
			dv.setValue(vec_dropdownres[iteindex]->getCount().getValue() - count);
			vec_dropdownres[iteindex]->setCount(dv);
		}
		else
		{
			vec_dropdownres.erase(vec_dropdownres.begin() + iteindex);
		}
	}
	MyRes::Add(res, count, MYPACKAGE);
	loadMyPackageRes();
}