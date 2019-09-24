#include "BookLibraryLayer.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MyMenu.h"
#include "AnimationEffect.h"
#include "MovingLabel.h"

BookLibraryLayer::BookLibraryLayer()
{

}


BookLibraryLayer::~BookLibraryLayer()
{
}

BookLibraryLayer* BookLibraryLayer::create()
{
	BookLibraryLayer*pRet = new(std::nothrow)BookLibraryLayer();
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

bool BookLibraryLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("simpleLayer.csb"));
	this->addChild(csbnode);

	int langtype = GlobalInstance::getInstance()->getLang();

	std::string titlestr = "librarybooktitle";
	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->ignoreContentAdaptWithSize(true);
	titleimg->loadTexture(ResourcePath::makeTextImgPath(titlestr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desc->setVisible(false);

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1000);
	closebtn->addTouchEventListener(CC_CALLBACK_2(BookLibraryLayer::onBtnClick, this));

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

void BookLibraryLayer::updateScrollviewContent()
{
	scrollview->removeAllChildrenWithCleanup(true);

	for (int i = 0; i < 14; i++)
	{
		std::string resid = StringUtils::format("ts%02d", (i + 1));
		vec_res.push_back(resid);
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
		int qu = 4;
		std::string qustr = StringUtils::format("ui/resbox_qu%d.png", qu);

		Sprite * qubox = Sprite::createWithSpriteFrameName(qustr);

		MenuItemSprite* boxItem = MenuItemSprite::create(
			qubox,
			qubox,
			qubox,
			CC_CALLBACK_1(BookLibraryLayer::onclick, this));
		//boxItem->setUserData((void*)GlobalInstance::map_GF[vec_res[m]].id.c_str());
		boxItem->setTag(m);
		boxItem->setPosition(Vec2(qubox->getContentSize().width / 2 + 20 + m % 4 * itemheight, innerheight - itemheight / 2 - m / 4 * itemheight + 5));

		MyMenu* menu = MyMenu::create();
		menu->addChild(boxItem);
		menu->setTouchlimit(scrollview);
		menu->setPosition(Vec2(0, 0));
		scrollview->addChild(menu);

		std::string resid = vec_res[m];

		std::string resstr = StringUtils::format("ui/%s.png", resid.c_str());
		Sprite * res = Sprite::createWithSpriteFrameName(resstr);
		res->setPosition(Vec2(boxItem->getContentSize().width / 2, boxItem->getContentSize().height / 2));
		boxItem->addChild(res);

		Label *namelbl = Label::createWithTTF(GlobalInstance::map_AllResources[resid].name, FONT_NAME, 23);
		namelbl->setColor(Color3B(34, 74, 79));
		namelbl->setPosition(Vec2(boxItem->getContentSize().width / 2, -10));
		boxItem->addChild(namelbl);

		if (GlobalInstance::getInstance()->getUnlockChapter() <= (m+1))
		{
			CommonFuncs::changeGray(qubox);
			CommonFuncs::changeGray(res);
		}
	}
}

void BookLibraryLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
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

void BookLibraryLayer::onclick(Ref* pSender)
{
	SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
	Node* node = (Node*)pSender;
	if (GlobalInstance::getInstance()->getUnlockChapter() <= (node->getTag() + 1))
	{
		//std::string str = StringUtils::format(ResourceLang::map_lang["notbookdesc"].c_str(), node->getTag() + 1, GlobalInstance::map_AllResources[vec_res[node->getTag()]].name.c_str());
		std::string str = StringUtils::format(ResourceLang::map_lang["notbookdesc"].c_str(), GlobalInstance::map_AllResources[vec_res[node->getTag()]].name.c_str());
		MovingLabel::show(str);
	}
	else
	{
		MovingLabel::show(GlobalInstance::map_AllResources[vec_res[node->getTag()]].name);
	}

	//GfLibraryInfoLayer* layer = GfLibraryInfoLayer::create((const char*)node->getUserData());
	//this->addChild(layer);
	//AnimationEffect::openAniEffect(layer);
}