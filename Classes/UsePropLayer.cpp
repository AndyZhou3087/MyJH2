#include "UsePropLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "BuyCoinLayer.h"
#include "MapBlockScene.h"

USING_NS_CC;

UsePropLayer::UsePropLayer()
{

}

UsePropLayer::~UsePropLayer()
{

}


UsePropLayer* UsePropLayer::create(std::string strid, int rcount)
{
	UsePropLayer *pRet = new(std::nothrow)UsePropLayer();
	if (pRet && pRet->init(strid, rcount))
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
bool UsePropLayer::init(std::string strid, int rcount)
{
	if (!Layer::init())
	{
		return false;
	}

	m_resid = strid;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 128));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("usePropLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	//按钮
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(UsePropLayer::onBtnClick, this));
	actionbtn->setTag(1000);
	//按钮文字
	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("usebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//按钮2
	cocos2d::ui::Widget* cancelbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("cancelbtn");
	cancelbtn->addTouchEventListener(CC_CALLBACK_2(UsePropLayer::onBtnClick, this));
	cancelbtn->setTag(1001);
	//按钮2文字
	cocos2d::ui::ImageView* cancelbtntxt = (cocos2d::ui::ImageView*)cancelbtn->getChildByName("text");
	cancelbtntxt->loadTexture(ResourcePath::makeTextImgPath("cancelbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox");

	cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");

	cocos2d::ui::Text* countlbl = (cocos2d::ui::Text*)csbnode->getChildByName("count");
	cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	cocos2d::ui::Text* desclbl = (cocos2d::ui::Text*)csbnode->getChildByName("desclbl");

	std::string boxstr = "ui/resbox.png";
	int t = 0;
	int qu = 0;
	std::string resid = strid;
	int count = rcount;
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

	int mcount = 0;
	if (MyRes::getMyResCount(resid) <= 0)
		mcount = rcount;

	namelbl->setString(GlobalInstance::map_AllResources[resid].name);
	std::string countstr = StringUtils::format("%d", mcount);
	countlbl->setString(countstr);

	desclbl->setString(GlobalInstance::map_AllResources[resid].desc);

	cocos2d::ui::ImageView* useicon = (cocos2d::ui::ImageView*)csbnode->getChildByName("icon");
	useicon->ignoreContentAdaptWithSize(true);
	std::string iconstr;

	if (MyRes::getMyResCount(resid) <= 0)
	{
		iconstr = "ui/main_coin.png";
		needcoincount.setValue(GlobalInstance::map_AllResources[resid].coinval * count);
	}
	else
	{
		iconstr = StringUtils::format("ui/%s.png", resid.c_str());
		needcoincount.setValue(count);
		useicon->setScale(0.4f);
	}
	useicon->loadTexture(iconstr, cocos2d::ui::Widget::TextureResType::PLIST);
	
	cocos2d::ui::Text* coincountlbl = (cocos2d::ui::Text*)csbnode->getChildByName("cnumbl");
	std::string str = StringUtils::format("%d", needcoincount.getValue());
	coincountlbl->setString(str);

	//屏蔽下层点击
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

void UsePropLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();

		if (tag == 1000)
		{
			if (MyRes::getMyResCount(m_resid) <= 0)
			{
				if (GlobalInstance::getInstance()->getMyCoinCount().getValue() < needcoincount.getValue())
				{
					Layer* layer = BuyCoinLayer::create(needcoincount.getValue() - GlobalInstance::getInstance()->getMyCoinCount().getValue());
					Director::getInstance()->getRunningScene()->addChild(layer, 10000, "buycoinlayer");
				}
				else
				{
					GlobalInstance::getInstance()->costMyCoinCount(needcoincount);
					AnimationEffect::closeAniEffect(this);
					doResult();
				}
			}
			else
			{
				MyRes::Use(m_resid, needcoincount.getValue());
				AnimationEffect::closeAniEffect(this);
				doResult();
			}
		}
		else
		{
			AnimationEffect::closeAniEffect(this);
		}
	}
}

void UsePropLayer::doResult()
{
	if (g_MapBlockScene != NULL)
	{
		if (m_resid.compare("z002") == 0)
			g_MapBlockScene->removeMazeStone(this->getTag());
		else if (m_resid.compare("z003") == 0)
			g_MapBlockScene->useAllOpen();
		else if (m_resid.compare("z004") == 0)
			g_MapBlockScene->useTranser();
	}
}