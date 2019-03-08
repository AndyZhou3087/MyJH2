#include "MazeDescLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "MyMenu.h"

MazeDescLayer::MazeDescLayer()
{

}

MazeDescLayer::~MazeDescLayer()
{

}


MazeDescLayer* MazeDescLayer::create()
{
	MazeDescLayer *pRet = new(std::nothrow)MazeDescLayer();
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

// on "init" you need to initialize your instance
bool MazeDescLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("mazedescLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	//°´Å¥
	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(0);
	closebtn->addTouchEventListener(CC_CALLBACK_2(MazeDescLayer::onBtnClick, this));

	cocos2d::ui::Text* title = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	title->setString(ResourceLang::map_lang["mazename"]);

	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desc->setString(ResourceLang::map_lang["mazedesc"]);

	cocos2d::ui::ImageView* icon1 = (cocos2d::ui::ImageView*)csbnode->getChildByName("icon1");
	icon1->ignoreContentAdaptWithSize(true);
	icon1->loadTexture(ResourcePath::makeImagePath("trans1.png"), cocos2d::ui::Widget::TextureResType::LOCAL);
	icon1->setColor(Color3B(239, 131, 255));

	cocos2d::ui::Text* desc_1 = (cocos2d::ui::Text*)csbnode->getChildByName("desc_1");
	desc_1->setString(ResourceLang::map_lang["mazetranserdesc"]);

	cocos2d::ui::ImageView* icon2 = (cocos2d::ui::ImageView*)csbnode->getChildByName("icon2");
	icon2->ignoreContentAdaptWithSize(true);
	icon2->loadTexture("mapui/buildblock_51.png", cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* desc_2 = (cocos2d::ui::Text*)csbnode->getChildByName("desc_2");
	desc_2->setString(ResourceLang::map_lang["mazestonedesc"]);

	cocos2d::ui::Text* desc_3 = (cocos2d::ui::Text*)csbnode->getChildByName("desc_3");
	desc_3->setString(ResourceLang::map_lang["mazedesc1"]);

	std::string vec_res_0[] = {"r006-0","r012-0","q001-0","q002-0","s003-0","i002-0","d002-0","m003-0"};
	std::string vec_res_1[] = {"r006-0","r012-0","z002-0","q001-0","q002-0","p001-0","i002-0","i004-0","s004-0","e003-4","g003-4","f003-4"};

	int startx = 830;
	std::vector<std::string> vec_res;
	int c = atoi(GlobalInstance::eventfrommapid.substr(1, GlobalInstance::eventfrommapid.find("-") - 1).c_str());
	if (c == 1)
	{
		for (int m = 0; m < sizeof(vec_res_0) / sizeof(vec_res_0[0]); m++)
		{
			vec_res.push_back(vec_res_0[m]);
		}
		startx = 830;
	}
	else
	{
		desc_3->setVisible(false);
		for (int m = 0; m < sizeof(vec_res_1) / sizeof(vec_res_1[0]); m++)
		{
			vec_res.push_back(vec_res_1[m]);
		}
	}

	for (unsigned int m = 0; m < vec_res.size(); m++)
	{
		std::string qustr = "ui/resbox.png";
		std::vector<std::string> vec_one;
		CommonFuncs::split(vec_res[m], vec_one, "-");
		std::string resid = vec_one[0];
		int qu = atoi(vec_one[1].c_str());

		int t = 0;
		for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
		{
			if (resid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
				break;
		}

		if (t >= T_ARMOR && t <= T_FASHION)
		{
			qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}
		else if (t >= T_WG && t <= T_NG)
		{
			qu = GlobalInstance::map_GF[resid].qu;
			qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}
		else if (t >= T_RENS && t <= T_BOX)
		{
			qu = atoi(resid.substr(1).c_str()) - 1;
			qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}

		Sprite * qubox = Sprite::createWithSpriteFrameName(qustr);

		MenuItemSprite* boxItem = MenuItemSprite::create(
			qubox,
			qubox,
			qubox,
			CC_CALLBACK_1(MazeDescLayer::onclick, this));
		//boxItem->setUserData((void*)vec_res[m]);

		boxItem->setPosition(Vec2(170 + m % 4 * 125, startx -m/4*140));
		boxItem->setScale(0.8f);
		MyMenu* menu = MyMenu::create();
		menu->addChild(boxItem);
		menu->setPosition(Vec2(0, 0));
		csbnode->addChild(menu);

		Node* effectnode = CommonFuncs::playResBoxEffect(boxItem, qu);

		std::string str = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);

		Sprite * res = Sprite::createWithSpriteFrameName(str);
		res->setPosition(Vec2(boxItem->getContentSize().width / 2, boxItem->getContentSize().height / 2));
		boxItem->addChild(res);

		Label *namelbl = Label::createWithTTF(GlobalInstance::map_AllResources[resid].name, FONT_NAME, 28);
		namelbl->setColor(Color3B(255, 255, 255));
		namelbl->setPosition(Vec2(boxItem->getContentSize().width / 2, -18));
		boxItem->addChild(namelbl);
	}

	//ÆÁ±ÎÏÂ²ãµã»÷
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

void MazeDescLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
		switch (tag)
		{
		case 0:
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		default:
			break;
		}
	}
}

void MazeDescLayer::onclick(Ref* pSender)
{

}