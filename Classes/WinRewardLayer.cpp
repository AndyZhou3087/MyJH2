#include "WinRewardLayer.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MyMenu.h"
#include "MovingLabel.h"
#include "AnimationEffect.h"
#include "MapBlockScene.h"
#include "NewGuideLayer.h"

WinRewardLayer::WinRewardLayer()
{
	m_isLongPress = false;
	m_longTouchTag = -1;
}


WinRewardLayer::~WinRewardLayer()
{
	std::vector<ResBase*>::iterator it;
	for (it = vec_dropdownres.begin(); it != vec_dropdownres.end();)
	{
		delete (*it);
		it = vec_dropdownres.erase(it);
	}

	MyRes::removeSivlerAndCoin();
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
	if (!Layer::init())
	{
		return false;
	}

	m_rewards = reward_res;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

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
	m_allgetbtn = (cocos2d::ui::Button*)csbnode->getChildByName("actionbtn");
	m_allgetbtn->setTag(1000);
	m_allgetbtn->addTouchEventListener(CC_CALLBACK_2(WinRewardLayer::onBtnClick, this));

	//按钮文字
	cocos2d::ui::ImageView* m_allgetbtntxt = (cocos2d::ui::ImageView*)m_allgetbtn->getChildByName("text");
	m_allgetbtntxt->loadTexture(ResourcePath::makeTextImgPath("allgetbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

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
		{
			std::sort(vec_dropdownres.begin(), vec_dropdownres.end(), sortDropResByType);

			std::vector<ResBase*> vec_tmp;;
			std::vector<ResBase*>::iterator it;
			for (it = vec_dropdownres.begin(); it != vec_dropdownres.end();)
			{
				if ((*it)->getId().compare("r006") == 0 || (*it)->getId().compare("r012") == 0)
				{
					vec_tmp.push_back(*it);
					it = vec_dropdownres.erase(it);
				}
				else
					it++;
			}
			for (unsigned int i=0;i<vec_tmp.size();i++)

				vec_dropdownres.insert(vec_dropdownres.begin(), vec_tmp[i]);
			
			vec_res = vec_dropdownres;
		}
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
			if (vec_res[m]->getType() >= T_ARMOR && vec_res[m]->getType() <= T_FASHION)
			{
				qu = ((Equipable*)vec_res[m])->getQU().getValue();
				qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
			}
			else if (vec_res[m]->getType() >= T_WG && vec_res[m]->getType() <= T_NG)
			{
				qu = GlobalInstance::map_GF[vec_res[m]->getId()].qu;
				qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
			}
			else if (vec_res[m]->getType() == T_RENS || vec_res[m]->getType() == T_DAN || vec_res[m]->getType() == T_MIJI || vec_res[m]->getType() == T_BOX)
			{
				qu = atoi(vec_res[m]->getId().substr(1).c_str()) - 1;
				qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
			}

			cocos2d::ui::ImageView* boxItem = cocos2d::ui::ImageView::create(qustr, cocos2d::ui::Widget::TextureResType::PLIST);
			boxItem->addTouchEventListener(CC_CALLBACK_2(WinRewardLayer::onclick, this));
			boxItem->setTouchEnabled(true);

			boxItem->setPosition(Vec2(boxItem->getContentSize().width / 2 + 20 + m % 4 * 160, innerheight - itemheight / 2 - m / 4 * itemheight));

			boxItem->setUserData((void*)vec_res[m]);
			boxItem->setTag(i * 10000 + m);

			sv->addChild(boxItem);

			std::string resid = vec_res[m]->getId();

			std::string str = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);

			Sprite * res = Sprite::createWithSpriteFrameName(str);
			res->setPosition(Vec2(boxItem->getContentSize().width / 2, boxItem->getContentSize().height / 2));
			boxItem->addChild(res);

			Label *namelbl = Label::createWithTTF(GlobalInstance::map_AllResources[resid].name, FONT_NAME, 23);
			namelbl->setColor(Color3B(34, 74, 79));
			namelbl->setPosition(Vec2(boxItem->getContentSize().width / 2, -10));
			boxItem->addChild(namelbl);

			std::string countstr = StringUtils::format("%d", vec_res[m]->getCount().getValue());
			Label *countlbl = Label::createWithTTF(countstr, FONT_NAME, 23);
			countlbl->setAnchorPoint(Vec2(1, 0));
			countlbl->setColor(Color3B::WHITE);
			countlbl->setPosition(Vec2(boxItem->getContentSize().width - 10, 10));
			boxItem->addChild(countlbl);
		}
	}

	std::string str = StringUtils::format("%d/%d", getMyCarryCount(), GlobalInstance::myOutMapCarry/*GlobalInstance::getInstance()->getTotalCarry()*/);
	carrycountlbl->setString(str);

	if (getMyCarryCount() >= GlobalInstance::myOutMapCarry)
	{
		carrycountlbl->setTextColor(Color4B(255, 61, 61, 255));
	}
	else
	{
		carrycountlbl->setTextColor(Color4B(255, 255, 255, 255));
	}
}

void WinRewardLayer::loadScrollviewData()
{
	for (unsigned int i = 0; i < m_rewards.size(); i++)
	{
		std::string rid = m_rewards[i].sid;

		int t = 0;
		for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
		{
			if (rid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
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
		else if (t >= T_WG && t <= T_NG)
		{
			res = new GongFa();
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
				if (vec_dropdownres[i]->getId().compare("r006") != 0 && vec_dropdownres[i]->getId().compare("r012") != 0)
					packagecount += vec_dropdownres[i]->getCount().getValue();
			}

			if (getMyCarryCount() + packagecount > GlobalInstance::myOutMapCarry/*GlobalInstance::getInstance()->getTotalCarry()*/)
			{
				MovingLabel::show(ResourceLang::map_lang["carryovertext"]);

				int cancarry = GlobalInstance::myOutMapCarry/*GlobalInstance::getInstance()->getTotalCarry()*/ - getMyCarryCount();
				for (unsigned int i = 0; i < vec_dropdownres.size(); i++)
				{
					if (cancarry <= 0)
						break;

					int addcount = 0;
					if (vec_dropdownres[i]->getType() >= T_ARMOR && vec_dropdownres[i]->getType() <= T_FASHION)//装备类的是一个一个的
					{
						cancarry--;
						addcount = vec_dropdownres[i]->getCount().getValue();
					}
					else
					{
						if (cancarry >= vec_dropdownres[i]->getCount().getValue())
							addcount = vec_dropdownres[i]->getCount().getValue();
						else
							addcount = cancarry;

						if (vec_dropdownres[i]->getId().compare("r006") != 0 && vec_dropdownres[i]->getId().compare("r012") != 0)
							cancarry -= addcount;
						else
							addcount = vec_dropdownres[i]->getCount().getValue();
					}
					MyRes::Add(vec_dropdownres[i], addcount, MYPACKAGE);
					DynamicValueInt dv;
					dv.setValue(vec_dropdownres[i]->getCount().getValue() - addcount);
					vec_dropdownres[i]->setCount(dv);

				}
				//return;
			}
			else
			{
				btnnode->setEnabled(false);
				float dt = 0.1f;
				if (vec_dropdownres.size() > 0)
				{
					for (unsigned int i = 0; i < vec_dropdownres.size(); i++)
					{
						MyRes::Add(vec_dropdownres[i], vec_dropdownres[i]->getCount().getValue(), MYPACKAGE);
						DynamicValueInt dv;
						vec_dropdownres[i]->setCount(dv);
					}

					dt = 1.0f;
				}
				//this->scheduleOnce(schedule_selector(WinRewardLayer::delayClose), dt);
			}

			loadMyPackageRes();

			std::vector<ResBase*>::iterator it;
			for (it = vec_dropdownres.begin(); it != vec_dropdownres.end();)
			{
				if ((*it)->getCount().getValue() <= 0)
				{
					delete (*it);
					it = vec_dropdownres.erase(it);
				}
				else
					it++;
			}

			updateScrollviewContent();

			break;
		}
		case 1001://continue
		case 1002://close
		{
			//引导回城卷轴
			if (g_MapBlockScene != NULL)
			{
				if (g_MapBlockScene->getFirstFightBoss())
				{
					g_MapBlockScene->delayShowNewerGuide(0);
				}
			}

			AnimationEffect::closeAniEffect((Layer*)this->getParent());
			int r = GlobalInstance::getInstance()->createRandomNum(5);
			SoundManager::getInstance()->playBackMusic(SoundManager::MUSIC_ID_SUBMAP_0 + r);
		}
			break;
		default:
			break;
		}
	}
}

void WinRewardLayer::onclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	//Node* clicknode = (Node*)pSender;
	//if (type == ui::Widget::TouchEventType::BEGAN)
	//{
	//	m_longTouchTag = clicknode->getTag();
	//	if (!isScheduled(schedule_selector(WinRewardLayer::longTouchUpdate)))
	//		schedule(schedule_selector(WinRewardLayer::longTouchUpdate), 0.1f);
	//}
	//else if (type == ui::Widget::TouchEventType::ENDED)
	//{
	//	cancelLongTouch();
	//	longTouchAction(clicknode->getTag());
	//}
	//else if (type == ui::Widget::TouchEventType::CANCELED)
	//{
	//	cancelLongTouch();
	//}


	Node* clicknode = (Node*)pSender;
	int tag = clicknode->getTag();

	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		clickflag = true;
		beginTouchPoint = clicknode->convertToWorldSpace(Vec2(clicknode->getPositionX(), clicknode->getPositionY()));
	}
	else if (type == ui::Widget::TouchEventType::MOVED)
	{
		Vec2 movedPoint = clicknode->convertToWorldSpace(Vec2(clicknode->getPositionX(), clicknode->getPositionY()));

		if (fabs(movedPoint.x - beginTouchPoint.x) >= CLICKOFFSETP || fabs(movedPoint.y - beginTouchPoint.y) >= CLICKOFFSETP)
			clickflag = false;
	}
	else if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (!clickflag)
			return;

		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);

		if (tag / 10000 == 0)//点击的是两个scrollview的 0--掉落，1--背包
		{
			int addcount = 0;
			ResBase* res = (ResBase*)clicknode->getUserData();

			if (res->getId().compare("r006") != 0 && res->getId().compare("r012") != 0)
				addcount = 1;

			if (getMyCarryCount() + addcount > GlobalInstance::myOutMapCarry/*GlobalInstance::getInstance()->getTotalCarry()*/)
			{
				MovingLabel::show(ResourceLang::map_lang["carryovertext"]);
				return;
			}

			reduceDropRes(res, 1, tag % 10000);
		}
		else if (tag / 10000 == 1)
		{
			ResBase* res = (ResBase*)clicknode->getUserData();
			addDropRes(res);
		}
		updateScrollviewContent();
	}
}


void WinRewardLayer::longTouchAction(int tag)
{
	SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
	if (tag / 10000 == 0)//点击的是两个scrollview的 0--掉落，1--背包
	{
		ResBase* res = vec_dropdownres[tag % 10000];//clicknode->getUserData();
		int addcount = 0;
		if (res->getId().compare("r006") != 0 && res->getId().compare("r012") != 0)
			addcount = 1;

		if (getMyCarryCount() + addcount > GlobalInstance::myOutMapCarry/*GlobalInstance::getInstance()->getTotalCarry()*/)
		{
			cancelLongTouch();
			MovingLabel::show(ResourceLang::map_lang["carryovertext"]);
			return;
		}
		int ressize = vec_dropdownres.size();
		if (tag % 10000 < ressize)
		{
			reduceDropRes(res, 1, tag % 10000);
		}
		else
		{
			cancelLongTouch();
		}
	}
	else if (tag / 10000 == 1)
	{
		int pacsize = vec_mypackagres.size();
		if (tag % 10000 < pacsize)
		{
			ResBase* res = vec_mypackagres[tag % 10000];//(ResBase*)clicknode->getUserData();
			addDropRes(res);
		}
		else
		{
			cancelLongTouch();
		}
	}
	updateScrollviewContent();
}

void WinRewardLayer::longTouchUpdate(float delay)
{
	m_isLongPress = true;
	if (m_longTouchTag >= 0) {
		longTouchAction(m_longTouchTag);
	}
}

void WinRewardLayer::cancelLongTouch()
{
	m_isLongPress = false;
	m_longTouchTag = -1;
	unschedule(schedule_selector(WinRewardLayer::longTouchUpdate));
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
		vec_dropdownres.push_back(eres);
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
		vec_dropdownres.push_back(eres);
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
	m_allgetbtn->setEnabled(true);
}

void WinRewardLayer::reduceDropRes(ResBase* res, int count, int iteindex)
{
	MyRes::Add(res, count, MYPACKAGE);
	loadMyPackageRes();

	int type = res->getType();

	if ((type >= T_ARMOR && type <= T_FASHION) || (type >= T_WG && type <= T_NG))
	{
		releaseDropRes(iteindex);
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
			releaseDropRes(iteindex);
		}
	}

	if (vec_dropdownres.size() <= 0)
		m_allgetbtn->setEnabled(false);
}

void WinRewardLayer::releaseDropRes(int interindex)
{
	delete vec_dropdownres[interindex];
	vec_dropdownres[interindex] = NULL;
	vec_dropdownres.erase(vec_dropdownres.begin() + interindex);
}

int WinRewardLayer::getMyCarryCount()
{
	int myPackageCount = MyRes::getMyPackageCount();
	int count = 0;
	for (unsigned int i = 0; i < MyRes::vec_MyResources.size(); i++)
	{
		ResBase* res = MyRes::vec_MyResources[i];
		if (res->getId().compare("r006") == 0 || res->getId().compare("r012") == 0)
			count += res->getCount().getValue();
	}
	return myPackageCount -= count;
}

bool WinRewardLayer::sortDropResByType(ResBase* a, ResBase* b)
{
	if (a->getType() > b->getType())
		return true;
	else 
		return false;
}