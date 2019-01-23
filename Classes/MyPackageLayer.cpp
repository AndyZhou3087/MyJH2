#include "MyPackageLayer.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MyMenu.h"
#include "AnimationEffect.h"
#include "ResDescLayer.h"

MyPackageLayer::MyPackageLayer()
{

}


MyPackageLayer::~MyPackageLayer()
{
}

MyPackageLayer* MyPackageLayer::create()
{
	MyPackageLayer *pRet = new(std::nothrow)MyPackageLayer();
	if (pRet && pRet->init())
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

bool MyPackageLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("simpleLayer.csb"));
	this->addChild(csbnode);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	int langtype = GlobalInstance::getInstance()->getLang();

	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("mypackagetitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desc->setVisible(false);

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1000);
	closebtn->addTouchEventListener(CC_CALLBACK_2(MyPackageLayer::onBtnClick, this));

	scrollview = (cocos2d::ui::ScrollView*)csbnode->getChildByName("contentscroll");
	scrollview->setScrollBarEnabled(false);
	scrollview->setBounceEnabled(true);

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

void MyPackageLayer::updateScrollviewContent()
{
	scrollview->removeAllChildrenWithCleanup(true);

	std::vector<ResBase*> vec_res;

	for (unsigned int i = 0; i < MyRes::vec_MyResources.size(); i++)
	{
		ResBase* res = MyRes::vec_MyResources[i];
		if (res->getWhere() == MYPACKAGE)
			vec_res.push_back(res);
	}

	int ressize = vec_res.size();
	int row = ressize % 4 == 0 ? ressize / 4 : (ressize / 4 + 1);

	int itemheight = 160;

	int innerheight = itemheight * row;

	int contentheight = scrollview->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollview->setInnerContainerSize(Size(scrollview->getContentSize().width, innerheight));


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
		else if (vec_res[m]->getType() >= T_RENS && vec_res[m]->getType() <= T_BOX)
		{
			qu = atoi(vec_res[m]->getId().substr(1).c_str()) - 1;
			qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}

		Sprite * qubox = Sprite::createWithSpriteFrameName(qustr);

		MenuItemSprite* boxItem = MenuItemSprite::create(
			qubox,
			qubox,
			qubox,
			CC_CALLBACK_1(MyPackageLayer::onclick, this));
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

void MyPackageLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
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

void MyPackageLayer::onclick(Ref* pSender)
{
	SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
	Node* node = (Node*)pSender;
	ResDescLayer* layer = ResDescLayer::create((ResBase*)node->getUserData(), 2);
	this->addChild(layer);
	AnimationEffect::openAniEffect((Layer*)layer);
}