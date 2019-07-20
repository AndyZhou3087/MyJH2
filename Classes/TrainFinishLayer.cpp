#include "TrainFinishLayer.h"
#include "Resource.h"
#include "Const.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "MovingLabel.h"
#include "DataSave.h"
#include "AnimationEffect.h"
#include "Building.h"
#include "LoadingBarProgressTimer.h"

USING_NS_CC;

TrainFinishLayer::TrainFinishLayer()
{

}

TrainFinishLayer::~TrainFinishLayer()
{

}


TrainFinishLayer* TrainFinishLayer::create()
{
	TrainFinishLayer*pRet = new(std::nothrow)TrainFinishLayer();
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
bool TrainFinishLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 230));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("trainFinishLayer.csb"));
	this->addChild(csbnode);

	int langtype = GlobalInstance::getInstance()->getLang();

	//关闭按钮
	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1000);
	closebtn->addTouchEventListener(CC_CALLBACK_2(TrainFinishLayer::onBtnClick, this));

	cocos2d::ui::ImageView* closebtntxt = (cocos2d::ui::ImageView*)closebtn->getChildByName("text");
	closebtntxt->loadTexture(ResourcePath::makeTextImgPath("closebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//滚动控件
	m_contentscroll = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollview");
	m_contentscroll->setScrollBarEnabled(false);
	m_contentscroll->setBounceEnabled(true);

	updateContent();

	CommonFuncs::playCommonLvUpAnim(this, "texiao_xlwc", Vec2(360, 1150));

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void TrainFinishLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
		switch (tag)
		{
		case 1000:
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		break;
		default:
			break;
		}
	}
}

void TrainFinishLayer::updateContent()
{
	m_contentscroll->removeAllChildrenWithCleanup(true);

	int itemheight = 170;

	int size = GlobalInstance::vec_myHeros.size();



	std::vector<Hero*> vec_trainheros;
	for (int i = 0; i < size; i++)
	{
		Hero* hero = GlobalInstance::vec_myHeros[i];
		if (hero->getState() == HS_TRAINING)
		{
			int refreshtime = hero->getTrainTime();
			int pasttime = GlobalInstance::servertime - refreshtime;
			if (pasttime >= hero->getTrainHour())
				vec_trainheros.push_back(hero);
		}
	}

	int innerheight = itemheight * vec_trainheros.size();
	int contentheight = m_contentscroll->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	m_contentscroll->setInnerContainerSize(Size(650, innerheight));


	int lv = Building::map_buildingDatas["4trainigroom"]->level.getValue();
	int bexp = Building::map_buildingDatas["4trainigroom"]->vec_exdatatrain[lv];


	for (unsigned int i = 0; i < vec_trainheros.size(); i++)
	{
		Hero* hero = vec_trainheros[i];

		int lastexp = hero->getExp().getValue();
		int addexp = hero->getTrainHour() / 3600 * bexp;
		int lastLevel = hero->getLevel();
		hero->setExpLimit(hero->getTrainHour() / 3600 * bexp);
		int curLevel = hero->getLevel();

		hero->setTrainHour(0);
		hero->setTrainTime(0);
		hero->setState(HS_OWNED);

		Node* node = CSLoader::createNode(ResourcePath::makePath("trainFinishNode.csb"));

		if (i < 8)
		{
			node->setPosition(Vec2(m_contentscroll->getContentSize().width + 600, innerheight - i * itemheight - itemheight / 2));
			node->runAction(EaseSineIn::create(MoveBy::create(0.15f + i * 0.07f, Vec2(-m_contentscroll->getContentSize().width / 2 - 600, 0))));
		}
		else
			node->setPosition(Vec2(m_contentscroll->getContentSize().width/2, innerheight - i * itemheight - itemheight / 2));

		m_contentscroll->addChild(node, 0, i);

		cocos2d::ui::ImageView *headbox = (cocos2d::ui::ImageView*)node->getChildByName("herobox");
		std::string str = StringUtils::format("ui/herobox_%d.png", hero->getPotential());
		headbox->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

		cocos2d::ui::ImageView* headimg = (cocos2d::ui::ImageView*)node->getChildByName("heroimg");
		str = StringUtils::format("ui/h_%d_%d.png", hero->getVocation(), hero->getSex());
		headimg->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

		cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)node->getChildByName("name");
		namelbl->setString(hero->getName());

		cocos2d::ui::Text* exptext = (cocos2d::ui::Text*)node->getChildByName("exptext");
		exptext->setString(ResourceLang::map_lang["trainexptext"]);

		cocos2d::ui::Text* lvtext = (cocos2d::ui::Text*)node->getChildByName("lvtext");
		lvtext->setString(ResourceLang::map_lang["trainlvtext"]);

		cocos2d::ui::Text* addexplbl = (cocos2d::ui::Text*)node->getChildByName("addexp");
		str = StringUtils::format("+%d", addexp);
		addexplbl->setString(str);

		cocos2d::ui::Text* lv = (cocos2d::ui::Text*)node->getChildByName("lv");
		str = StringUtils::format("lv.%d => lv.%d", lastLevel + 1, curLevel + 1);
		lv->setString(str);

		cocos2d::ui::LoadingBar* expbar = (cocos2d::ui::LoadingBar*)node->getChildByName("heroattrexpbar");

		int curlvexp = 0;
		int nextlvexp = GlobalInstance::vec_herosAttr[hero->getVocation()].vec_exp[hero->getLevel()];

		if (hero->getLevel() > 0)
			curlvexp = GlobalInstance::vec_herosAttr[hero->getVocation()].vec_exp[hero->getLevel() - 1];


		cocos2d::ui::Text* explbl = (cocos2d::ui::Text*)node->getChildByName("exp");
		str = StringUtils::format("%d/%d", hero->getExp().getValue() - curlvexp, nextlvexp - curlvexp);
		explbl->setString(str);

		float percent = (hero->getExp().getValue() - curlvexp) * 100.0f / (nextlvexp - curlvexp);

		if (lastLevel != curLevel)
		{
			expbar->runAction(Sequence::create(DelayTime::create(1.0f + i * 0.07f), LoadingBarProgressTo::create(0.2f, 100), DelayTime::create(0.2f), LoadingBarProgressFromTo::create(0.5f, 0, percent), NULL));
		}
		else
		{
			if (lastexp != hero->getExp().getValue())
			{
				expbar->runAction(Sequence::create(DelayTime::create(1.0f + i * 0.07f), LoadingBarProgressTo::create(0.5f, percent), NULL));
			}
			else
			{
				expbar->setPercent(percent);
			}
		}
	}

	GlobalInstance::getInstance()->saveMyHeros();
}