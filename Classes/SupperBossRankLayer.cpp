#include "SupperBossRankLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "MovingLabel.h"
#include "WaitingProgress.h"
#include "ResDescLayer.h"
#include "MyRes.h"
#include "SoundManager.h"
#include "DataSave.h"
#include "MapBlockScene.h"

USING_NS_CC;

SupperBossRankLayer::SupperBossRankLayer()
{
	lastclicktag = 1;
	httptag = 0;
	ranktype = 1;
}

SupperBossRankLayer::~SupperBossRankLayer()
{
}


SupperBossRankLayer* SupperBossRankLayer::create(bool isclickrank)
{
	SupperBossRankLayer*pRet = new(std::nothrow)SupperBossRankLayer();
	if (pRet && pRet->init(isclickrank))
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
bool SupperBossRankLayer::init(bool isclickrank)
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 160));
	this->addChild(color, 0, "colorLayer");

	langtype = GlobalInstance::getInstance()->getLang();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("supperBossResultLayer.csb"));
	this->addChild(csbnode);

	title = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	title->ignoreContentAdaptWithSize(true);
	title->loadTexture(ResourcePath::makeTextImgPath("ranktitle_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	tabletitle = (cocos2d::ui::Text*)csbnode->getChildByName("title");
	tabletitle->setString(ResourceLang::map_lang["supperbosstabletile"]);

	cocos2d::ui::Button* closebtn = (cocos2d::ui::Button*)csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(SupperBossRankLayer::onBtnClick, this));
	closebtn->setTag(1000);
	scrollView = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollview");
	scrollView->setScrollBarEnabled(false);
	scrollView->setBounceEnabled(true);

	cocos2d::ui::Text* myinfo = (cocos2d::ui::Text*)csbnode->getChildByName("myinfo");
	myinfo->setString(ResourceLang::map_lang["supperbossmyinfo"]);

	mynackname = (cocos2d::ui::Text*)csbnode->getChildByName("mynackname");
	mynackname->setString(GlobalInstance::getInstance()->getMyNickName());

	myrankicon = (cocos2d::ui::ImageView*)csbnode->getChildByName("myrankicon");

	myrank = (cocos2d::ui::Text*)csbnode->getChildByName("myrank");
	
	tag0 = (cocos2d::ui::ImageView*)csbnode->getChildByName("tag0");
	tag0->addTouchEventListener(CC_CALLBACK_2(SupperBossRankLayer::onBtnClick, this));
	tag0->setTag(0);

	tag1 = (cocos2d::ui::ImageView*)csbnode->getChildByName("tag1");
	tag1->addTouchEventListener(CC_CALLBACK_2(SupperBossRankLayer::onBtnClick, this));
	tag1->setTag(1);

	mycurhurtlbl = (cocos2d::ui::Text*)csbnode->getChildByName("mycurhurt");

	std::string hurtstring = StringUtils::format("%d", GlobalInstance::supperbossinfo.curhurt);
	mycurhurtlbl->setString(hurtstring);

	mytotalhurt = (cocos2d::ui::Text*)csbnode->getChildByName("mytotalhurt");

	mytotalhurt->setString("");

	mytotalhurttext = (cocos2d::ui::Text*)csbnode->getChildByName("mytotalhurttext");

	this->scheduleOnce(schedule_selector(SupperBossRankLayer::resetHeroHp), 0.5f);

	long long int timeSec = GlobalInstance::servertime + 8 * 60 * 60;
	time_t timep = timeSec;
	tm* timeStuct = gmtime(&timep);

	if (timeStuct->tm_hour < GlobalInstance::supperbossinfo.starthour || timeStuct->tm_hour > GlobalInstance::supperbossinfo.endhour)
	{
		lastclicktag = 0;
		tag1->setVisible(false);
	}
	clicktag(lastclicktag);



	if (isclickrank)
	{
		csbnode->getChildByName("mycurhurttext")->setVisible(false);
		mycurhurtlbl->setVisible(false);
		getRankList(lastclicktag);
		mytotalhurttext->setPositionY(mynackname->getPositionY());
		mytotalhurt->setPositionY(mynackname->getPositionY());
	}
	else
	{
		WaitingProgress* waitbox = WaitingProgress::create(ResourceLang::map_lang["datawaitingtext"]);
		Director::getInstance()->getRunningScene()->addChild(waitbox, 100, "waitbox");

		if (GlobalInstance::supperbossinfo.leftfreecount > 0)
			HttpDataSwap::init(this)->postSupperBossHurt(1);
		else
			HttpDataSwap::init(this)->postSupperBossHurt(0);
	}
	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	//listener->onTouchEnded = [=](Touch *touch, Event *event)
	//{
	//	AnimationEffect::closeAniEffect((Layer*)this);
	//};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void SupperBossRankLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
	int tag = clicknode->getTag();
	if (tag != 1004)
		CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{

		if (tag == 1000)
			AnimationEffect::closeAniEffect((Layer*)this);
		else
		{
			if (tag == lastclicktag)
				return;

			lastclicktag = tag;
			clicktag(tag);

			getRankList(tag);

		}
	}
}

void SupperBossRankLayer::clicktag(int tag)
{
	if (tag == 0)
	{
		tag0->loadTexture("ui/zantag1.png", cocos2d::ui::Widget::TextureResType::PLIST);
		cocos2d::ui::ImageView* tag0text = (cocos2d::ui::ImageView*)tag0->getChildByName("text");
		tag0text->loadTexture(ResourcePath::makeTextImgPath("subosstagtext0_1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

		cocos2d::ui::ImageView* tag1text = (cocos2d::ui::ImageView*)tag1->getChildByName("text");
		tag1text->loadTexture(ResourcePath::makeTextImgPath("subosstagtext1_0", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		tag1->loadTexture("ui/zantag0.png", cocos2d::ui::Widget::TextureResType::PLIST);

	}
	else
	{
		tag0->loadTexture("ui/zantag0.png", cocos2d::ui::Widget::TextureResType::PLIST);
		cocos2d::ui::ImageView* tag0text = (cocos2d::ui::ImageView*)tag0->getChildByName("text");
		tag0text->loadTexture(ResourcePath::makeTextImgPath("subosstagtext0_0", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

		cocos2d::ui::ImageView* tag1text = (cocos2d::ui::ImageView*)tag1->getChildByName("text");
		tag1text->loadTexture(ResourcePath::makeTextImgPath("subosstagtext1_1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		tag1->loadTexture("ui/zantag1.png", cocos2d::ui::Widget::TextureResType::PLIST);
	}
}

void SupperBossRankLayer::onFinish(int code)
{
	Director::getInstance()->getRunningScene()->removeChildByName("waitbox");
	if (code == 0)
	{
		if (httptag == 0)
		{
			if (GlobalInstance::supperbossinfo.leftfreecount > 0)
				GlobalInstance::supperbossinfo.leftfreecount--;
			if (GlobalInstance::supperbossinfo.leftcoincount > 0)
				GlobalInstance::supperbossinfo.leftcoincount--;
			getRankList(lastclicktag);
		}
		else if (httptag == 1)
		{
			addRankNodes();
		}

	}
	else
	{
		MovingLabel::show(ResourceLang::map_lang["matchnetworkerr"]);
	}
}

void SupperBossRankLayer::getRankList(int type)
{
	WaitingProgress* waitbox = WaitingProgress::create(ResourceLang::map_lang["datawaitingtext"]);
	Director::getInstance()->getRunningScene()->addChild(waitbox, 100, "waitbox");
	httptag = 1;
	HttpDataSwap::init(this)->getSupperBossRankList(type);
	ranktype = type;
}

void SupperBossRankLayer::addRankNodes()
{
	int myrankno = -1;
	scrollView->removeAllChildrenWithCleanup(true);
	scrollView->jumpToTop();
	int size = GlobalInstance::vec_supperBossRankData.size();
	int itemheight = 80;
	int innerheight = itemheight * size;
	int contentheight = scrollView->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollView->setInnerContainerSize(Size(720, innerheight));

	for (unsigned int i = 0; i < GlobalInstance::vec_supperBossRankData.size(); i++)
	{
		Node* subnode = CSLoader::createNode(ResourcePath::makePath("supperbossRankNode.csb"));

		std::string subnodename = StringUtils::format("item%d", i);
		subnode->setPosition(Vec2(scrollView->getContentSize().width/2, innerheight - i * itemheight - itemheight / 2));
		scrollView->addChild(subnode, 0, subnodename);

		cocos2d::ui::Text* nickname = (cocos2d::ui::Text*)subnode->getChildByName("nackname");
		nickname->setString(GlobalInstance::vec_supperBossRankData[i].nickname);

		int rank = i + 1;
		cocos2d::ui::Text* ranklbl = (cocos2d::ui::Text*)subnode->getChildByName("myrank");
		std::string strrank = StringUtils::format("%d", rank);
		ranklbl->setString(strrank);

		cocos2d::ui::ImageView* rankicon = (cocos2d::ui::ImageView*)subnode->getChildByName("myrankicon");
		if (rank <= 3)
		{
			rankicon->setVisible(true);
			std::string iconstr = StringUtils::format("ui/zanrank%d.png", rank);
			rankicon->loadTexture(iconstr, cocos2d::ui::Widget::TextureResType::PLIST);
		}
		else
		{
			rankicon->setVisible(false);
		}

		cocos2d::ui::Text* countlbl = (cocos2d::ui::Text*)subnode->getChildByName("hurt");
		std::string str = StringUtils::format("%d", GlobalInstance::vec_supperBossRankData[i].hurt);
		countlbl->setString(str);

		if (GlobalInstance::vec_supperBossRankData[i].nickname.compare(GlobalInstance::getInstance()->getMyNickName()) == 0)
		{
			myrankno = i;
		}

	}

	//我的排名

	if (myrankno >= 0)
	{
		if (myrankno <= 3)
		{
			myrankicon->setVisible(true);
			std::string iconstr = StringUtils::format("ui/zanrank%d.png", myrankno + 1);
			myrankicon->loadTexture(iconstr, cocos2d::ui::Widget::TextureResType::PLIST);
		}
		myrank->setVisible(true);

		std::string myrankstr = StringUtils::format("%d", myrankno + 1);
		myrank->setString(myrankstr);
	}

	std::string str = StringUtils::format("%d", GlobalInstance::supperbossinfo.totalhurt);
	mytotalhurt->setString(str);

	std::string strkey = StringUtils::format("supperbosstotalhurt%d", ranktype);
	mytotalhurttext->setString(ResourceLang::map_lang[strkey]);
}

void SupperBossRankLayer::resetHeroHp(float dt)
{
	for (int i = 0; i < 6; i++)
	{
		if (GlobalInstance::myCardHeros[i] != NULL && GlobalInstance::myCardHeros[i]->getState() == HS_DEAD)
		{
			GlobalInstance::myCardHeros[i]->setState(HS_TAKEON);
			GlobalInstance::myCardHeros[i]->setHp(GlobalInstance::myCardHeros[i]->getMaxHp());
			GlobalInstance::getInstance()->saveHero(GlobalInstance::myCardHeros[i]);

			FightHeroNode* fnode = (FightHeroNode*)g_MapBlockScene->getChildByTag(i);
			fnode->setVisible(true);
			fnode->setData(GlobalInstance::myCardHeros[i], F_HERO, FS_READY);
		}
	}
}