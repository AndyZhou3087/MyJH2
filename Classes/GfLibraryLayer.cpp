#include "GfLibraryLayer.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MyMenu.h"
#include "AnimationEffect.h"
#include "GfLibraryInfoLayer.h"

GfLibraryLayer::GfLibraryLayer()
{

}


GfLibraryLayer::~GfLibraryLayer()
{
}

GfLibraryLayer* GfLibraryLayer::create(int type)
{
	GfLibraryLayer *pRet = new(std::nothrow)GfLibraryLayer();
	if (pRet && pRet->init(type))
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

bool GfLibraryLayer::init(int type)
{
	if (!Layer::init())
	{
		return false;
	}

	mtype = type;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("simpleLayer.csb"));
	this->addChild(csbnode);

	int langtype = GlobalInstance::getInstance()->getLang();

	std::string titlestr;
	if (type == T_WG)
		titlestr = "librarywgtitle";
	else
		titlestr = "libraryngtitle";
	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->ignoreContentAdaptWithSize(true);
	titleimg->loadTexture(ResourcePath::makeTextImgPath(titlestr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desc->setVisible(false);

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1000);
	closebtn->addTouchEventListener(CC_CALLBACK_2(GfLibraryLayer::onBtnClick, this));

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

void GfLibraryLayer::updateScrollviewContent()
{
	scrollview->removeAllChildrenWithCleanup(true);

	std::vector<std::string> vec_res;

	std::map<std::string, GFData>::iterator it;
	for (it = GlobalInstance::map_GF.begin(); it != GlobalInstance::map_GF.end(); it++)
	{
		if (mtype == T_WG)
		{
			if (it->first.compare(0, 1, "w") == 0)
				vec_res.push_back(it->first);
		}
		else if (mtype == T_NG)
		{
			if (it->first.compare(0, 1, "x") == 0)
				vec_res.push_back(it->first);
		}
	}

	std::sort(vec_res.begin(), vec_res.end(), larger_gfQu);

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
		int qu = GlobalInstance::map_GF[vec_res[m]].qu;
		std::string qustr = StringUtils::format("ui/resbox_qu%d.png", qu);

		Sprite * qubox = Sprite::createWithSpriteFrameName(qustr);

		MenuItemSprite* boxItem = MenuItemSprite::create(
			qubox,
			qubox,
			qubox,
			CC_CALLBACK_1(GfLibraryLayer::onclick, this));
		boxItem->setUserData((void*)GlobalInstance::map_GF[vec_res[m]].id.c_str());

		boxItem->setPosition(Vec2(qubox->getContentSize().width / 2 + 20 + m % 4 * itemheight, innerheight - itemheight / 2 - m / 4 * itemheight + 5));

		MyMenu* menu = MyMenu::create();
		menu->addChild(boxItem);
		menu->setTouchlimit(scrollview);
		menu->setPosition(Vec2(0, 0));
		scrollview->addChild(menu);

		std::string resid = vec_res[m];

		std::string str = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);

		Sprite * res = Sprite::createWithSpriteFrameName(str);
		res->setPosition(Vec2(boxItem->getContentSize().width / 2, boxItem->getContentSize().height / 2));
		boxItem->addChild(res);

		Label *namelbl = Label::createWithTTF(GlobalInstance::map_AllResources[resid].name, FONT_NAME, 23);
		namelbl->setColor(Color3B(34, 74, 79));
		namelbl->setPosition(Vec2(boxItem->getContentSize().width / 2, -10));
		boxItem->addChild(namelbl);

		if (MyRes::getEquipableCount(resid) <= 0 && MyRes::getEquipableCount(resid, MYEQUIP) <= 0)
		{
			CommonFuncs::changeGray(qubox);
			CommonFuncs::changeGray(res);
		}
	}
}

bool GfLibraryLayer::larger_gfQu(std::string a, std::string b)
{
	if (GlobalInstance::map_GF[a].qu > GlobalInstance::map_GF[b].qu)
		return true;
	return false;
}

void GfLibraryLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
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

void GfLibraryLayer::onclick(Ref* pSender)
{
	SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
	Node* node = (Node*)pSender;
	GfLibraryInfoLayer* layer = GfLibraryInfoLayer::create((const char*)node->getUserData());
	this->addChild(layer);
	AnimationEffect::openAniEffect(layer);
}