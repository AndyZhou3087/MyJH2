#include "TBoxRewardLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MovingLabel.h"
#include "AnimationEffect.h"
#include "MyRes.h"

USING_NS_CC;

TBoxRewardLayer::TBoxRewardLayer()
{

}

TBoxRewardLayer::~TBoxRewardLayer()
{

}


TBoxRewardLayer* TBoxRewardLayer::create(std::string resid)
{
	TBoxRewardLayer *pRet = new(std::nothrow)TBoxRewardLayer();
	if (pRet && pRet->init(resid))
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
bool TBoxRewardLayer::init(std::string resid)
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	ParticleSystemQuad *particle = ParticleSystemQuad::create(ResourcePath::makePath("effect/slsb/zhandoushengli/sl_xuanzhanguang.plist"));
	particle->setPosition(Vec2(360, 800));
	this->addChild(particle);


	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("tboxRewardLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("title");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("reward_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//按钮1
	cocos2d::ui::Widget* okbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("okbtn");
	okbtn->addTouchEventListener(CC_CALLBACK_2(TBoxRewardLayer::onBtnClick, this));

	//按钮1文字
	cocos2d::ui::ImageView* okbtntxt = (cocos2d::ui::ImageView*)okbtn->getChildByName("btntext");
	okbtntxt->loadTexture(ResourcePath::makeTextImgPath("okbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desc->setString(ResourceLang::map_lang["tboxdesctext"]);

	cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox");

	cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");

	cocos2d::ui::Text* countlbl = (cocos2d::ui::Text*)csbnode->getChildByName("countlbl");
	cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");

	int awdszie = GlobalInstance::map_TBoxs[resid].vec_awds.size();

	int r = GlobalInstance::getInstance()->createRandomNum(awdszie);

	std::string awd = GlobalInstance::map_TBoxs[resid].vec_awds[r];
	std::vector<std::string> vec_tmp;
	CommonFuncs::split(awd, vec_tmp, "-");

	int qu = atoi(vec_tmp[2].c_str());
	namelbl->setString(GlobalInstance::map_AllResources[vec_tmp[0]].name);
	countlbl->setString(vec_tmp[1]);

	std::string boxstr = "ui/resbox.png";
	int t = 0;
	for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
	{
		if (resid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
			break;
	}
	if (t >= T_ARMOR && t <= T_NG)
	{
		boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
	}
	resbox->loadTexture(boxstr, cocos2d::ui::Widget::TextureResType::PLIST);
	std::string resstr = StringUtils::format("ui/%s.png", vec_tmp[0].c_str());
	res->loadTexture(resstr, cocos2d::ui::Widget::TextureResType::PLIST);

	MyRes::Add(vec_tmp[0], atoi(vec_tmp[1].c_str()), MYSTORAGE, qu, GlobalInstance::getInstance()->generateStoneCount(qu));
	MyRes::Use(resid);

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

void TBoxRewardLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
		AnimationEffect::closeAniEffect((Layer*)this);
	}
}