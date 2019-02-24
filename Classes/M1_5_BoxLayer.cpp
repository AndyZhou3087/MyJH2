#include "M1_5_BoxLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "MovingLabel.h"
#include "AnimationEffect.h"
#include "GlobalInstance.h"

USING_NS_CC;

M1_5_BoxLayer::M1_5_BoxLayer()
{

}

M1_5_BoxLayer::~M1_5_BoxLayer()
{

}


M1_5_BoxLayer* M1_5_BoxLayer::create()
{
	M1_5_BoxLayer *pRet = new(std::nothrow)M1_5_BoxLayer();
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
bool M1_5_BoxLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 128));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("m1_5_BoxLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	//按钮1
	cocos2d::ui::Widget* cancelbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("cancelbtn");
	cancelbtn->addTouchEventListener(CC_CALLBACK_2(M1_5_BoxLayer::onBtnClick, this));

	//按钮1文字
	cocos2d::ui::ImageView* cancelbtntxt = (cocos2d::ui::ImageView*)cancelbtn->getChildByName("text");
	cancelbtntxt->loadTexture(ResourcePath::makeTextImgPath("closebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)csbnode->getChildByName("content");
	desc->setString(ResourceLang::map_lang["m1_5_desc"]);

	std::string ress[] = { "m004", "s004", "q001", "q002", "q003"};

	for (int i = 0; i < 5; i++)
	{
		std::string str = StringUtils::format("resbox_%d", i);
		cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);

		cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)resbox->getChildByName("res");

		cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)resbox->getChildByName("name");


		std::string boxstr = "ui/resbox.png";
		int t = 0;
		int qu = 0;
		std::string resid = ress[i];
		for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
		{
			if (resid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
				break;
		}
		if (t >= T_ARMOR && t <= T_FASHION)
		{
			boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}
		else if (t >= T_WG && t <= T_NG)
		{
			qu = GlobalInstance::map_GF[resid].qu;
			boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}
		else if (t >= T_RENS && t <= T_BOX)
		{
			qu = atoi(resid.substr(1).c_str()) - 1;
			boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}

		CommonFuncs::playResBoxEffect(resbox, qu);

		resbox->loadTexture(boxstr, cocos2d::ui::Widget::TextureResType::PLIST);
		std::string resstr = StringUtils::format("ui/%s.png", resid.c_str());
		res->loadTexture(resstr, cocos2d::ui::Widget::TextureResType::PLIST);

		namelbl->setString(GlobalInstance::map_AllResources[resid].name);
	}

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		AnimationEffect::closeAniEffect(this);
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void M1_5_BoxLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
		removeSelf();
	}
}

void M1_5_BoxLayer::removeSelf()
{
	AnimationEffect::closeAniEffect(this);
}