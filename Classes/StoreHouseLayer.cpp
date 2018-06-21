#include "StoreHouseLayer.h"
#include <algorithm>
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MyMenu.h"

StoreHouseLayer::StoreHouseLayer()
{
	lastCategoryindex = 0;
}


StoreHouseLayer::~StoreHouseLayer()
{

}

bool StoreHouseLayer::init()
{

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);

	m_csbnode = CSLoader::createNode(ResourcePath::makePath("storeHouseLayer.csb"));
	this->addChild(m_csbnode);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	langtype = GlobalInstance::getInstance()->getLang();

	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)m_csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("storehousetitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)m_csbnode->getChildByName("closebtn");
	closebtn->setTag(1001);
	closebtn->addTouchEventListener(CC_CALLBACK_2(StoreHouseLayer::onBtnClick, this));

	//按钮
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)m_csbnode->getChildByName("actionbtn");
	actionbtn->setTag(1000);
	actionbtn->addTouchEventListener(CC_CALLBACK_2(StoreHouseLayer::onBtnClick, this));

	//按钮文字
	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("fastdecompose_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

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
	loadData();
	updateContent(0);

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}

void StoreHouseLayer::updateContent(int category)
{
	scrollview->removeAllChildrenWithCleanup(true);

	for (unsigned int i = 0; i < vec_categoryBtn.size(); i++)
	{
		cocos2d::ui::ImageView* btntext = (cocos2d::ui::ImageView*)vec_categoryBtn[i]->getChildByName("text");
		
		if (category != i)
		{
			std::string textstr = StringUtils::format("catabtn%d_text_n", i);
			btntext->loadTexture(ResourcePath::makeTextImgPath(textstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			vec_categoryBtn[i]->setBright(true);
		}
		else
		{
			std::string textstr = StringUtils::format("catabtn%d_text_s", i);
			vec_categoryBtn[i]->setBright(false);
			btntext->loadTexture(ResourcePath::makeTextImgPath(textstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		}
	}

	int itemheight = 140;
	int ressize = MyRes::vec_MyResources.size();
	if (category != 0)
	{
		ressize = map_cateRes[category].size();
	}
	else
	{
		int catasort[] = { CATA_3 , CATA_1, CATA_2 };
		map_cateRes[category].clear();
		for (int i = 0; i < 3; i++)
		{
			map_cateRes[category].insert(map_cateRes[category].end(), map_cateRes[catasort[i]].begin(), map_cateRes[catasort[i]].end());
		}
	}
	int row = ressize % 4 == 0 ? ressize / 4 : (ressize / 4 + 1);
	int innerheight = itemheight * row;

	int contentheight = scrollview->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollview->setInnerContainerSize(Size(scrollview->getContentSize().width, innerheight));

	for (unsigned int m = 0; m < map_cateRes[category].size(); m++)
	{

		Sprite * boxbg = Sprite::createWithSpriteFrameName("ui/resbox.png");
		boxbg->setPosition(Vec2(boxbg->getContentSize().width / 2 + 20 + m % 4 * 160, innerheight - itemheight / 2 - m / 4 * itemheight));
		scrollview->addChild(boxbg);

		std::string qustr = "ui/resbox_qu0.png";

		Sprite * qubox = Sprite::createWithSpriteFrameName(qustr);

		MenuItemSprite* boxItem = MenuItemSprite::create(
			qubox,
			qubox,
			qubox,
			CC_CALLBACK_1(StoreHouseLayer::onclick, this));
		boxItem->setUserData((void*)map_cateRes[category][m]);

		boxItem->setPosition(boxbg->getPosition());

		MyMenu* menu = MyMenu::create();
		menu->addChild(boxItem);
		menu->setTouchlimit(scrollview);
		menu->setPosition(Vec2(0, 0));

		scrollview->addChild(menu);

		std::string resid = map_cateRes[category][m]->getId();

		std::string str = StringUtils::format("ui/%s.png", resid.c_str());
		Sprite * res = Sprite::createWithSpriteFrameName(str);
		res->setPosition(Vec2(boxItem->getContentSize().width / 2, boxItem->getContentSize().height / 2));
		boxItem->addChild(res);

		Label *namelbl = Label::createWithTTF(GlobalInstance::map_AllResources[resid].name, FONT_NAME, 23);
		namelbl->setColor(Color3B(34, 74,79));
		namelbl->setPosition(Vec2(boxItem->getContentSize().width / 2, -20));
		boxItem->addChild(namelbl);

		std::string countstr = StringUtils::format("%d", map_cateRes[category][m]->getCount().getValue());
		Label *countlbl = Label::createWithTTF(countstr, FONT_NAME, 23);
		countlbl->setAnchorPoint(Vec2(1, 0));
		countlbl->setColor(Color3B::WHITE);
		countlbl->setPosition(Vec2(boxItem->getContentSize().width - 5, 0));
		boxItem->addChild(countlbl);
	}
}

void StoreHouseLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		this->removeFromParentAndCleanup(true);
	}
}

void StoreHouseLayer::onclick(Ref* pSender)
{
	SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
	Node* node = (Node*)pSender;
}

void StoreHouseLayer::loadData()
{

	for (unsigned int i = 0; i < MyRes::vec_MyResources.size(); i++)
	{
		ResBase* res = MyRes::vec_MyResources[i];
		if (res->getType() == T_WG || res->getType() == T_NG)
			map_cateRes[CATA_1].push_back(MyRes::vec_MyResources[i]);
		else if (res->getType() == T_ARMOR || res->getType() == T_EQUIP || res->getType() == T_HANDARMOR || res->getType() == T_JSTONE)
			map_cateRes[CATA_2].push_back(MyRes::vec_MyResources[i]);
		else
			map_cateRes[CATA_3].push_back(MyRes::vec_MyResources[i]);
	}

	std::map<int, std::vector<ResBase*>>::iterator bit;
	for (bit = map_cateRes.begin(); bit != map_cateRes.end(); ++bit)
	{
		sort(map_cateRes[bit->first].begin(), map_cateRes[bit->first].end(), less_callback);
	}

}

bool StoreHouseLayer::less_callback(ResBase* a, ResBase* b)
{
	if (a->getId() < b->getId())
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
		if (lastCategoryindex == node->getTag())
			return;

		lastCategoryindex = node->getTag();

		updateContent(node->getTag());
	}
}