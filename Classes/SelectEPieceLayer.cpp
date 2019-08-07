#include "SelectEPieceLayer.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyMenu.h"
#include "AnimationEffect.h"
#include "MyRes.h"

SelectEPieceLayer::SelectEPieceLayer()
{

}


SelectEPieceLayer::~SelectEPieceLayer()
{

}


SelectEPieceLayer* SelectEPieceLayer::create(Node* changeNode, int count, ShopData * sdata)
{
	SelectEPieceLayer*pRet = new(std::nothrow)SelectEPieceLayer();
	if (pRet && pRet->init(changeNode, count, sdata))
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

bool SelectEPieceLayer::init(Node* changeNode, int count, ShopData * sdata)
{
	if (!Layer::init())
	{
		return false;
	}
	m_count = count;
	m_changeNode = changeNode;
	m_shopdata = sdata;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node *csbnode = CSLoader::createNode(ResourcePath::makePath("selectEquipLayer.csb"));
	this->addChild(csbnode);

	cocos2d::ui::ImageView* bg = (cocos2d::ui::ImageView*)csbnode->getChildByName("bg");
	bg->setContentSize(Size(bg->getContentSize().width, 580));

	scrollview = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollview");
	scrollview->setScrollBarEnabled(false);
	scrollview->setBounceEnabled(true);

	scrollview->setContentSize(Size(scrollview->getContentSize().width, 500));

	loadscrollview();

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

void SelectEPieceLayer::loadscrollview()
{
	int itemheight = 160;
	int ressize = sizeof(qu4epiece)/sizeof(qu4epiece[0]);

	int row = ressize % 4 == 0 ? ressize / 4 : (ressize / 4 + 1);
	int innerheight = itemheight * row;

	int contentheight = scrollview->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollview->setInnerContainerSize(Size(scrollview->getContentSize().width, innerheight));

	for (int m = 0; m < ressize; m++)
	{
		std::string qustr = "ui/resbox.png";


		Sprite * qubox = Sprite::createWithSpriteFrameName(qustr);

		MenuItemSprite* boxItem = MenuItemSprite::create(
			qubox,
			qubox,
			qubox,
			CC_CALLBACK_1(SelectEPieceLayer::onclick, this));
		boxItem->setTag(m);

		boxItem->setPosition(Vec2(qubox->getContentSize().width / 2 + 20 + m % 4 * itemheight, innerheight - itemheight / 2 - m / 4 * itemheight));

		MyMenu* menu = MyMenu::create();
		menu->addChild(boxItem, 0, "item");
		menu->setTouchlimit(scrollview);
		menu->setPosition(Vec2(0, 0));

		scrollview->addChild(menu, 0, m);

		std::string resid = qu4epiece[m];

		std::string str = GlobalInstance::getInstance()->getResUIFrameName(resid, 0);

		Sprite * res = Sprite::createWithSpriteFrameName(str);
		res->setPosition(Vec2(boxItem->getContentSize().width / 2, boxItem->getContentSize().height / 2));
		boxItem->addChild(res);

		Sprite* pieceicon = Sprite::createWithSpriteFrameName("ui/pieceicon.png");
		pieceicon->setAnchorPoint(Vec2(0, 1));
		pieceicon->setPosition(10, boxItem->getContentSize().height - 10);
		boxItem->addChild(pieceicon);

		std::string namestr = GlobalInstance::map_AllResources[resid].name;

		Label *namelbl = Label::createWithTTF(namestr, FONT_NAME, 20);
		namelbl->setColor(Color3B(26, 68, 101));
		namelbl->setPosition(Vec2(boxItem->getContentSize().width / 2, -10));
		boxItem->addChild(namelbl, 0, "resname");

		std::string countstr = StringUtils::format("%d/%d", m_count, MyRes::getMyResCount(resid));
		Label *countlbl = Label::createWithTTF(countstr, FONT_NAME, 23);
		countlbl->setAnchorPoint(Vec2(1, 0));
		countlbl->setColor(Color3B::WHITE);
		countlbl->setPosition(Vec2(boxItem->getContentSize().width - 10, 10));
		boxItem->addChild(countlbl);
	}
}

void SelectEPieceLayer::onclick(Ref* pSender)
{
	SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
	Node* node = (Node*)pSender;

	int tag = node->getTag();
	cocos2d::ui::ImageView* clickimage = (cocos2d::ui::ImageView*)m_changeNode;
	if (clickimage != NULL)
	{
		std::string resid = qu4epiece[tag];
		std::string str = GlobalInstance::getInstance()->getResUIFrameName(resid, 0);
		clickimage->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

		if (m_shopdata != NULL)
		{
			std::vector<std::string> vec_;
			vec_.push_back(resid);
			std::string countstr = StringUtils::format("%d", m_count);
			vec_.push_back(countstr);
			m_shopdata->res[this->getTag()] = vec_;
		}
	}

	AnimationEffect::closeAniEffect(this);
}