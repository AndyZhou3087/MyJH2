#include "EquipLibraryLayer.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "MyRes.h"
#include "MyMenu.h"
#include "AnimationEffect.h"
#include "MovingLabel.h"
#include "EquipLibraryInfoLayer.h"

#define ITEMHEIGHT 160

EquipLibraryLayer::EquipLibraryLayer()
{
	lastCategoryindex = 0;
	res_equip = NULL;
}


EquipLibraryLayer::~EquipLibraryLayer()
{
	if (res_equip != NULL)
	{
		delete res_equip;
		res_equip = NULL;
	}
}

EquipLibraryLayer* EquipLibraryLayer::create(int category)
{
	EquipLibraryLayer *pRet = new(std::nothrow)EquipLibraryLayer();
	if (pRet && pRet->init(category))
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

bool EquipLibraryLayer::init(int category)
{
	if (!Layer::init())
	{
		return false;
	}
	lastCategoryindex = category;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("libraryequipLayer.csb"));
	this->addChild(csbnode);

	int langtype = GlobalInstance::getInstance()->getLang();

	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->ignoreContentAdaptWithSize(true);
	titleimg->loadTexture(ResourcePath::makeTextImgPath("libraryequiptitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1000);
	closebtn->addTouchEventListener(CC_CALLBACK_2(EquipLibraryLayer::onBtnClick, this));

	Node* categoryBtnNode = csbnode->getChildByName("catanode");
	for (int i = 0; i < categoryBtnNode->getChildrenCount(); i++)
	{
		std::string btnstr = StringUtils::format("btn%d", i);
		cocos2d::ui::Button* btn = (cocos2d::ui::Button*)categoryBtnNode->getChildByName(btnstr);
		btn->setTag(i);
		btn->addTouchEventListener(CC_CALLBACK_2(EquipLibraryLayer::onCategory, this));
		vec_categoryBtn.push_back(btn);
	}

	catetitle = (cocos2d::ui::ImageView*)csbnode->getChildByName("catatitletext");
	catetitle->ignoreContentAdaptWithSize(true);

	equipbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("equipbox");

	scrollview = (cocos2d::ui::ScrollView*)csbnode->getChildByName("contentscroll");
	scrollview->setScrollBarEnabled(false);
	scrollview->setBounceEnabled(true);

	cataEquipData();

	updateCata(lastCategoryindex);

	cocos2d::ui::Text* hintlbl = (cocos2d::ui::Text*)csbnode->getChildByName("hintdesc");
	hintlbl->setString(ResourceLang::map_lang["libraryequiphint"]);

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}

void EquipLibraryLayer::updateCata(int category)
{
	int langtype = GlobalInstance::getInstance()->getLang();

	for (unsigned int i = 0; i < vec_categoryBtn.size(); i++)
	{
		cocos2d::ui::ImageView* btntext = (cocos2d::ui::ImageView*)vec_categoryBtn[i]->getChildByName("text");

		if (category != i)
		{
			std::string textstr = StringUtils::format("libequipcata%d_text_n", i);
			btntext->loadTexture(ResourcePath::makeTextImgPath(textstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			vec_categoryBtn[i]->setBright(true);
		}
		else
		{
			std::string textstr = StringUtils::format("libequipcata%d_text_s", i);
			vec_categoryBtn[i]->setBright(false);
			btntext->loadTexture(ResourcePath::makeTextImgPath(textstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		}
	}

	std::string catatitlestr = StringUtils::format("libqu5text_%d", category);
	catetitle->loadTexture(ResourcePath::makeTextImgPath(catatitlestr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	int equipboxSizeH[] = { 360, 200 };
	equipbox->removeAllChildrenWithCleanup(true);
	equipbox->setContentSize(Size(equipbox->getContentSize().width, equipboxSizeH[category]));

	scrollview->removeAllChildrenWithCleanup(true);

	std::vector<SIMPLE_EQUIP> vec_res;
	std::vector<SIMPLE_EQUIP> vec_resqu5arm;
	std::vector<SIMPLE_EQUIP> vec_resqu5other;

	std::map<int, std::vector<SIMPLE_EQUIP>>::iterator eit;
	for (eit = map_resdatas.begin(); eit != map_resdatas.end(); eit++)
	{
		int cata = eit->first;
		if (category == 0)//神兵
		{
			if (cata < 9)//普通橙色武器
			{
				vec_res.insert(vec_res.end(), map_resdatas[cata].begin(), map_resdatas[cata].end());
			}
			else if (cata == 100)//神兵
			{
				vec_resqu5arm.insert(vec_resqu5arm.end(), map_resdatas[cata].begin(), map_resdatas[cata].end());
			}
		}
		else
		{
			if (cata >= 9 && cata < 100)
			{
				vec_res.insert(vec_res.end(), map_resdatas[cata].begin(), map_resdatas[cata].end());
			}
			else if (cata > 100 && cata < 1000)
			{
				vec_resqu5other.insert(vec_resqu5other.end(), map_resdatas[cata].begin(), map_resdatas[cata].end());
			}
		}
	}
	int scrollSizeH[] = {610 + 70,770 + 70};
	scrollview->setContentSize(Size(scrollview->getContentSize().width, scrollSizeH[category]));
	if (category == 0)
	{
		addResUI(vec_resqu5arm, equipbox, equipboxSizeH[category] - 20);
	}
	else
	{
		addResUI(vec_resqu5other, equipbox, equipboxSizeH[category] - 20);
	}


	int ressize = vec_res.size();
	int row = ressize % 4 == 0 ? ressize / 4 : (ressize / 4 + 1);

	int innerheight = ITEMHEIGHT * row;

	int contentheight = scrollview->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollview->setInnerContainerSize(Size(scrollview->getContentSize().width, innerheight));
	addResUI(vec_res, scrollview, innerheight);

}

void EquipLibraryLayer::addResUI(std::vector<SIMPLE_EQUIP> vec_res, Node* parent, int parentheight)
{
	for (unsigned int m = 0; m < vec_res.size(); m++)
	{
		std::string qustr;
		
		bool ishas = false;
		if (vec_res[m].type < 100)
		{
			for (unsigned int i = 0; i <MyRes::vec_MyResources.size(); i++)
			{
				if (MyRes::vec_MyResources[i]->getId().compare(vec_res[m].strid) == 0)
				{
					if (MyRes::vec_MyResources[i]->getQU().getValue() >= vec_res[m].qu)
					{
						ishas = true;
						vec_res[m].qu = MyRes::vec_MyResources[i]->getQU().getValue();
					}
				}
			}
		}

		qustr = StringUtils::format("ui/resbox_qu%d.png", vec_res[m].qu);

		Sprite * qubox = Sprite::createWithSpriteFrameName(qustr);

		MenuItemSprite* boxItem = MenuItemSprite::create(
			qubox,
			qubox,
			qubox,
			CC_CALLBACK_1(EquipLibraryLayer::onclick, this));
		boxItem->setUserData((void*)GlobalInstance::map_Equip[vec_res[m].strid].id.c_str());
		boxItem->setTag(vec_res[m].qu);
		boxItem->setPosition(Vec2(qubox->getContentSize().width / 2 + 35 + m % 4 * (ITEMHEIGHT - 10), parentheight - ITEMHEIGHT / 2 - m / 4 * ITEMHEIGHT + 5));

		MyMenu* menu = MyMenu::create();
		menu->addChild(boxItem);
		if (vec_res[m].type < 100)
			menu->setTouchlimit(scrollview);
		menu->setPosition(Vec2(0, 0));
		parent->addChild(menu);

		std::string resid = vec_res[m].strid;

		std::string str = GlobalInstance::getInstance()->getResUIFrameName(resid, vec_res[m].qu);

		Sprite * res = Sprite::createWithSpriteFrameName(str);
		res->setPosition(Vec2(boxItem->getContentSize().width / 2, boxItem->getContentSize().height / 2));
		boxItem->addChild(res);

		Label *namelbl = Label::createWithTTF(GlobalInstance::map_AllResources[resid].name, FONT_NAME, 23);
		namelbl->setColor(Color3B(34, 74, 79));
		namelbl->setPosition(Vec2(boxItem->getContentSize().width / 2, -10));
		boxItem->addChild(namelbl);

		if (vec_res[m].type < 100 && !ishas)
		{
			CommonFuncs::changeGray(qubox);
			CommonFuncs::changeGray(res);
		}
	}
}

void EquipLibraryLayer::cataEquipData()
{
	std::map<std::string, EquipData>::iterator it;
	for (it = GlobalInstance::map_Equip.begin(); it != GlobalInstance::map_Equip.end(); it++)
	{
		EquipData edata = GlobalInstance::map_Equip[it->first];
		SIMPLE_EQUIP sdata;
		sdata.strid = edata.id;
		sdata.qu = 3;
		sdata.type = edata.type;

		int etype = edata.type;
		if (it->first.compare(0, 1, "a") == 0)
		{
			if (etype < 100)
				map_resdatas[etype].push_back(sdata);
			else if (etype < 1000)
			{
				sdata.qu = 5;
				map_resdatas[100].push_back(sdata);
			}
			else
				map_resdatas[1000].push_back(sdata);
		}
		else if (it->first.compare(0, 1, "e") == 0)
		{
			if (etype < 100)
				map_resdatas[etype].push_back(sdata);
			else if (etype < 1000)
			{
				sdata.qu = 5;
				map_resdatas[200].push_back(sdata);
			}
			else
				map_resdatas[2000].push_back(sdata);
		}
		else if (it->first.compare(0, 1, "g") == 0)
		{
			if (etype < 100)
				map_resdatas[etype].push_back(sdata);
			else if (etype < 1000)
			{
				sdata.qu = 5;
				map_resdatas[300].push_back(sdata);
			}
			else
				map_resdatas[3000].push_back(sdata);
		}
		else if (it->first.compare(0, 1, "f") == 0)
		{
			if (etype < 100)
				map_resdatas[etype].push_back(sdata);
			else if (etype < 1000)
			{
				sdata.qu = 5;
				map_resdatas[400].push_back(sdata);
			}
			else
				map_resdatas[4000].push_back(sdata);
		}
	}
	std::map<int, std::vector<SIMPLE_EQUIP>>::iterator eit;
	for (eit = map_resdatas.begin(); eit != map_resdatas.end(); eit++)
	{
		std::sort(map_resdatas[eit->first].begin(), map_resdatas[eit->first].end(), EquipLibraryLayer::larger_EquipQu);
		
	}
}

bool EquipLibraryLayer::larger_EquipQu(SIMPLE_EQUIP a, SIMPLE_EQUIP b)
{
	if (a.strid.compare(b.strid) > 0)
		return true;
	return false;
}

void EquipLibraryLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		cocos2d::ui::Button* btnnode = (cocos2d::ui::Button*)pSender;
		int tag = btnnode->getTag();
		switch (tag)
		{
		case 1000://close
		{
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		}
		default:
			break;
		}
	}
}

void EquipLibraryLayer::onclick(Ref* pSender)
{
	SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
	Node* node = (Node*)pSender;
	int tag = node->getTag();
	if (tag >= 5)
	{
		MovingLabel::show(ResourceLang::map_lang["waitingequip"]);
	}
	else
	{
		if (res_equip != NULL)
		{
			delete res_equip;
			res_equip = NULL;
		}
		std::string resid = (char*)node->getUserData();

		int t = -1;
		for (int k = 0; k < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); k++)
		{
			if (resid.compare(0, 1, RES_TYPES_CHAR[k]) == 0)
			{
				t = k;
				break;
			}
		}

		res_equip = new Equip();
		res_equip->setId(resid);
		res_equip->setType(t);
		DynamicValueInt dv;
		dv.setValue(tag);
		res_equip->setQU(dv);
		EquipLibraryInfoLayer* layer = EquipLibraryInfoLayer::create(res_equip);
		this->addChild(layer);
		AnimationEffect::openAniEffect(layer);
	}
}

void EquipLibraryLayer::onCategory(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		if (lastCategoryindex == node->getTag())
			return;

		lastCategoryindex = node->getTag();

		updateCata(node->getTag());
	}
}
