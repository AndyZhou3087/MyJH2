#include "StoreHouseLayer.h"
#include <algorithm>
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MyMenu.h"
#include "ResDescLayer.h"
#include "MovingLabel.h"
#include "EquipDescLayer.h"
#include "Quest.h"
#include "AnimationEffect.h"
#include "HintBoxLayer.h"
#include "NewGuideLayer.h"
#include "MainScene.h"
#include "NewGuideLayer.h"

StoreHouseLayer::StoreHouseLayer()
{
	lastCategoryindex = 0;
	isfastcomposing = false;
	newguideboxitem = NULL;
}


StoreHouseLayer::~StoreHouseLayer()
{

}

bool StoreHouseLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	m_csbnode = CSLoader::createNode(ResourcePath::makePath("storeHouseLayer.csb"));
	this->addChild(m_csbnode);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	langtype = GlobalInstance::getInstance()->getLang();

	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)m_csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("storehousetitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	closebtn = (cocos2d::ui::Widget*)m_csbnode->getChildByName("closebtn");
	closebtn->setTag(1001);
	closebtn->addTouchEventListener(CC_CALLBACK_2(StoreHouseLayer::onBtnClick, this));

	//按钮
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)m_csbnode->getChildByName("actionbtn");
	actionbtn->setTag(1000);
	actionbtn->addTouchEventListener(CC_CALLBACK_2(StoreHouseLayer::onBtnClick, this));

	//按钮文字
	actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("fastdecompose_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	actionbtntxt->ignoreContentAdaptWithSize(true);

	scrollview = (cocos2d::ui::ScrollView*)m_csbnode->getChildByName("scrollview");
	scrollview->setScrollBarEnabled(false);
	scrollview->setBounceEnabled(true);

	Node* categoryBtnNode = m_csbnode->getChildByName("catanode");
	for (int i = 0; i < categoryBtnNode->getChildrenCount(); i++)
	{
		std::string btnstr = StringUtils::format("btn%d", i);
		cocos2d::ui::Button* btn = (cocos2d::ui::Button*)categoryBtnNode->getChildByName(btnstr);
		btn->setTag(i);
		btn->addTouchEventListener(CC_CALLBACK_2(StoreHouseLayer::onCategory, this));
		vec_categoryBtn.push_back(btn);
	}
	updateContent(0);

	this->scheduleOnce(schedule_selector(StoreHouseLayer::delayShowNewerGuide), newguidetime);

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}

void StoreHouseLayer::delayShowNewerGuide(float dt)
{
	if (!NewGuideLayer::checkifNewerGuide(88))
	{
		if (NewGuideLayer::checkifNewerGuide(89))
		{
			showNewerGuide(89);
		}
		else if (NewGuideLayer::checkifNewerGuide(91))
		{
			showNewerGuide(91);
		}
	}
}

void StoreHouseLayer::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	if (step == 89)
	{
		if (newguideboxitem != NULL)
		{
			nodes.push_back(newguideboxitem);
		}
	}
	else if (step == 91)
	{
		nodes.push_back(closebtn);
	}
	g_mainScene->showNewerGuideNode(step, nodes);
}

void StoreHouseLayer::updateContent(int category)
{
	scrollview->removeAllChildrenWithCleanup(true);
	scrollview->jumpToTop();

	for (unsigned int i = 0; i < vec_categoryBtn.size(); i++)
	{
		cocos2d::ui::ImageView* btntext = (cocos2d::ui::ImageView*)vec_categoryBtn[i]->getChildByName("text");
		
		if (category != i)
		{
			std::string textstr = StringUtils::format("stcatabtn%d_text_n", i);
			btntext->loadTexture(ResourcePath::makeTextImgPath(textstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			vec_categoryBtn[i]->setBright(true);
		}
		else
		{
			std::string textstr = StringUtils::format("stcatabtn%d_text_s", i);
			vec_categoryBtn[i]->setBright(false);
			btntext->loadTexture(ResourcePath::makeTextImgPath(textstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		}
	}

	map_cateRes.clear();
	loadData();

	int itemheight = 160;

	if (category == 0)
	{
		int catasort[] = { CATA_3 , CATA_1, CATA_2 };
		for (int i = 0; i < 3; i++)
		{
			map_cateRes[category].insert(map_cateRes[category].end(), map_cateRes[catasort[i]].begin(), map_cateRes[catasort[i]].end());
		}
	}
	int ressize = map_cateRes[category].size();
	int row = ressize % 4 == 0 ? ressize / 4 : (ressize / 4 + 1);
	int innerheight = itemheight * row;

	int contentheight = scrollview->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollview->setInnerContainerSize(Size(scrollview->getContentSize().width, innerheight));

	for (unsigned int m = 0; m < map_cateRes[category].size(); m++)
	{
		std::string qustr = "ui/resbox.png";
		int qu = 0;
		if (map_cateRes[category][m]->getType() >= T_ARMOR && map_cateRes[category][m]->getType() <= T_FASHION)
		{
			qu = ((Equipable*)map_cateRes[category][m])->getQU().getValue();
			qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}
		else if (map_cateRes[category][m]->getType() >= T_WG && map_cateRes[category][m]->getType() <= T_NG)
		{
			qu = GlobalInstance::map_GF[map_cateRes[category][m]->getId()].qu;
			qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}
		else if (map_cateRes[category][m]->getType() == T_RENS || map_cateRes[category][m]->getType() == T_DAN || map_cateRes[category][m]->getType() == T_MIJI || map_cateRes[category][m]->getType() == T_BOX)
		{
			qu = atoi(map_cateRes[category][m]->getId().substr(1).c_str()) - 1;
			qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}
		else if (map_cateRes[category][m]->getType() >= T_HEROCARD && map_cateRes[category][m]->getType() <= T_ARMCARD)
		{
			qu = 3;
			qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}

		Sprite * qubox = Sprite::createWithSpriteFrameName(qustr);

		MenuItemSprite* boxItem = MenuItemSprite::create(
			qubox,
			qubox,
			qubox,
			CC_CALLBACK_1(StoreHouseLayer::onclick, this));
		boxItem->setUserData((void*)map_cateRes[category][m]);

		boxItem->setPosition(Vec2(qubox->getContentSize().width / 2 + 20 + m % 4 * 160, innerheight - itemheight / 2 - m / 4 * itemheight + 5));

		MyMenu* menu = MyMenu::create();
		menu->addChild(boxItem);
		menu->setTouchlimit(scrollview);
		menu->setPosition(Vec2(0, 0));

		scrollview->addChild(menu);

		std::string resid = map_cateRes[category][m]->getId();

		//引导体力药水
		if (!NewGuideLayer::checkifNewerGuide(88) && NewGuideLayer::checkifNewerGuide(89))
		{
			if (map_cateRes[category][m]->getId().compare("p001") == 0)
			{
				newguideboxitem = boxItem;
				int posh = innerheight - m / 4 * itemheight + 5;
				int vsizeh = contentheight;
				if (posh > vsizeh)
				{
					float per = (1 - (float)(posh - vsizeh) / (innerheight - vsizeh)) * 100;
					scrollview->jumpToPercentVertical(per);
				}
				else
				{
					scrollview->jumpToBottom();
				}
			}
		}

		std::string str = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);

		Sprite * res = Sprite::createWithSpriteFrameName(str);
		res->setPosition(Vec2(boxItem->getContentSize().width / 2, boxItem->getContentSize().height / 2));
		boxItem->addChild(res);

		std::string namestr = GlobalInstance::map_AllResources[resid].name;
		if (map_cateRes[category][m]->getType() >= T_ARMOR && map_cateRes[category][m]->getType() <= T_NG)
		{
			Equipable* eres = (Equipable*)map_cateRes[category][m];
			if (eres->getLv().getValue() > 0)
				namestr = StringUtils::format("+%d%s", eres->getLv().getValue() + 1, namestr.c_str());
		}

		Label *namelbl = Label::createWithTTF(namestr, FONT_NAME, 23);
		namelbl->setColor(Color3B(34, 74,79));
		namelbl->setPosition(Vec2(boxItem->getContentSize().width / 2, -10));
		boxItem->addChild(namelbl);

		std::string countstr = StringUtils::format("%d", map_cateRes[category][m]->getCount().getValue());
		Label *countlbl = Label::createWithTTF(countstr, FONT_NAME, 23);
		countlbl->setAnchorPoint(Vec2(1, 0));
		countlbl->setColor(Color3B::WHITE);
		countlbl->setPosition(Vec2(boxItem->getContentSize().width - 10, 10));
		boxItem->addChild(countlbl);
	}
}

void StoreHouseLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
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
			isfastcomposing = true;
			MovingLabel::show(ResourceLang::map_lang["decomposehint"]);
			btnnode->setTag(1002);
			actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("cancelbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			updateContent(lastCategoryindex);
			break;
		}
		case 1001:
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		case 1002:
		{
			isfastcomposing = false;
			btnnode->setTag(1000);
			actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("fastdecompose_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			updateContent(lastCategoryindex);
		}
			break;
		default:
			break;
		}
	}
}

void StoreHouseLayer::updateUI()
{
	float scrollpos = scrollview->getScrolledPercentVertical();
	updateContent(lastCategoryindex);
	if (scrollpos > 0)
	{
		scrollview->jumpToPercentVertical(scrollpos);
	}
}

void StoreHouseLayer::onclick(Ref* pSender)
{
	SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
	Node* node = (Node*)pSender;
	ResBase* res = (ResBase*)node->getUserData();
	if (isfastcomposing)
	{
		decomposeCheck(res);
	}
	else
	{
		if (res->getType() >= T_ARMOR && res->getType() <= T_NG)
		{
			EquipDescLayer* layer = EquipDescLayer::create(res, 0);//从仓库进入
			this->addChild(layer, 0, 1111);
			AnimationEffect::openAniEffect((Layer*)layer);
		}
		else
		{
			ResDescLayer* layer = ResDescLayer::create(res, 0);
			this->addChild(layer, 0, 1111);
			AnimationEffect::openAniEffect((Layer*)layer);
		}
	}
}

void StoreHouseLayer::loadData()
{
	for (unsigned int i = 0; i < MyRes::vec_MyResources.size(); i++)
	{
		bool isadd = true;
		ResBase* res = MyRes::vec_MyResources[i];
		int size = GlobalInstance::map_AllResources[res->getId()].vec_needres.size();
		if (size <= 0 && isfastcomposing)
			isadd = false;

		if (res->getWhere() == MYSTORAGE && isadd)
		{
			if (res->getType() == T_WG || res->getType() == T_NG)
			{
				map_cateRes[CATA_1].push_back(MyRes::vec_MyResources[i]);
				sort(map_cateRes[CATA_1].begin(), map_cateRes[CATA_1].end(), lvsort_callback);
			}
			else if (res->getType() == T_ARMOR || res->getType() == T_EQUIP || res->getType() == T_HANDARMOR || res->getType() == T_FASHION)
			{
				map_cateRes[CATA_2].push_back(MyRes::vec_MyResources[i]);
				sort(map_cateRes[CATA_2].begin(), map_cateRes[CATA_2].end(), lvsort_callback);
			}
			else
			{
				map_cateRes[CATA_3].push_back(MyRes::vec_MyResources[i]);
				sort(map_cateRes[CATA_3].begin(), map_cateRes[CATA_3].end(), countsort_callback);
			}
		}
	}
}

bool StoreHouseLayer::countsort_callback(ResBase* a, ResBase* b)
{
	if (a->getCount().getValue() > b->getCount().getValue())
		return true;
	else
		return false;
}

bool StoreHouseLayer::lvsort_callback(ResBase* a, ResBase* b)
{
	if (((Equipable*)a)->getLv().getValue() > ((Equipable*)b)->getLv().getValue())
		return true;
	else
		return false;
}


void StoreHouseLayer::onCategory(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;

		if (isfastcomposing)
			MovingLabel::show(ResourceLang::map_lang["decomposehint"]);

		if (lastCategoryindex == node->getTag())
			return;

		//isfastcomposing = false;
		lastCategoryindex = node->getTag();

		updateContent(node->getTag());
	}
}

int StoreHouseLayer::decomposeCheck(ResBase* res)
{
	if (res->getType() >= T_ARMOR && res->getType() <= T_FASHION)
	{
		Equip* eres = (Equip*)res;

		bool ishasstones = false;
		for (unsigned int n = 0; n < eres->vec_stones.size(); n++)
		{
			if (eres->vec_stones[n].length() >= 3)//有镶嵌宝石
			{
				ishasstones = true;
				break;
			}
		}

		if (eres->getQU().getValue() >= 2 || ishasstones)
		{
			std::string potentialstr = StringUtils::format("potential_%d", eres->getQU().getValue());
			std::string hintstr = StringUtils::format(ResourceLang::map_lang["confirmdecompose"].c_str(), ResourceLang::map_lang[potentialstr].c_str(), GlobalInstance::map_AllResources[eres->getId()].name.c_str());
			HintBoxLayer* hint = HintBoxLayer::create(hintstr, 5);

			if (ishasstones)
			{
				hintstr = StringUtils::format(ResourceLang::map_lang["confirmdecompose1"].c_str(), ResourceLang::map_lang[potentialstr].c_str(), GlobalInstance::map_AllResources[eres->getId()].name.c_str());
				hint = HintBoxLayer::create(hintstr, 7);
			}

			hint->setUserData((void*)eres);
			this->addChild(hint, 0, 100);
			AnimationEffect::openAniEffect((Layer*)hint);
			return 1;
		}
	}

	decompose(res);

	return 0;
}

void StoreHouseLayer::decompose(ResBase* res)
{
	std::string resid = res->getId();
	std::string str;
	int size = GlobalInstance::map_AllResources[resid].vec_needres.size();

	if (res->getType() >= T_ARMOR && res->getType() <= T_FASHION)
	{
		Equip* resequip = (Equip*)res;
		for (unsigned int n = 0; n < resequip->vec_stones.size(); n++)
		{
			if (resequip->vec_stones[n].length() >= 3)//有镶嵌宝石
			{
				MyRes::Add(resequip->vec_stones[n]);
			}
		}

	}
	if (size > 0)
	{
		std::string resstr;
		for (int i = 0; i < size; i++)
		{
			std::map<std::string, int> one_res = GlobalInstance::map_AllResources[resid].vec_needres[i];
			std::map<std::string, int>::iterator oneit = one_res.begin();
			std::string cresid = oneit->first;
			int count = one_res[oneit->first];
			int m = 0;
			for (; m < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); m++)
			{
				if (cresid.compare(0, 1, RES_TYPES_CHAR[m]) == 0)
				{
					break;
				}
			}
			int addcount = 0;
			if (m >= T_ARMOR && m <= T_FASHION)
			{
				addcount = 1;
				int qu = res->getQU().getValue();
				int stc = GlobalInstance::getInstance()->generateStoneCount(qu);
				MyRes::Add(cresid, addcount, MYSTORAGE, qu, stc);
			}
			else if (count >= 2)
			{
				addcount = count / 2;
				MyRes::Add(cresid, addcount);
			}
			if (addcount > 0)
			{
				if (resstr.length() > 0)
					resstr.append(ResourceLang::map_lang["dunhao"]);
				resstr = StringUtils::format("%sx%d ", GlobalInstance::map_AllResources[cresid].name.c_str(), addcount);
				str.append(resstr);
			}
		}
		MyRes::Use(res, 1);

		std::string showstr = StringUtils::format(ResourceLang::map_lang["decomposesucc"].c_str(), GlobalInstance::map_AllResources[resid].name.c_str(), str.c_str());
		MovingLabel::show(showstr);

		updateUI();

		Quest::setDailyTask(DECOMPOSE_EQUIP, 1);
		Quest::setAchieveTypeCount(DECOMPOSE_EQUIP, 1);
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_DECOMPOSE);
	}
	else
	{
		MovingLabel::show(ResourceLang::map_lang["decomposefail"]);
	}
}