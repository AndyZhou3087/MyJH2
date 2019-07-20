#include "ZanLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "MovingLabel.h"
#include "WaitingProgress.h"
#include "ResDescLayer.h"
#include "MyRes.h"
#include "ZanRuleLayer.h"
#include "SoundManager.h"
#include "DataSave.h"

USING_NS_CC;

ZanLayer::ZanLayer()
{
	lastclicktag = 1;
	httptag = 0;
	paisetoplayerindex = 0;
	j002res = NULL;
}

ZanLayer::~ZanLayer()
{
	if (j002res != NULL)
	{
		delete j002res;
		j002res = NULL;
	}
}


ZanLayer* ZanLayer::create()
{
	ZanLayer*pRet = new(std::nothrow)ZanLayer();
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
bool ZanLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 160));
	this->addChild(color, 0, "colorLayer");

	langtype = GlobalInstance::getInstance()->getLang();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("zanLayer.csb"));
	this->addChild(csbnode);

	title = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	title->ignoreContentAdaptWithSize(true);
	title->loadTexture(ResourcePath::makeTextImgPath("zantitle1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	tabletitle = (cocos2d::ui::Text*)csbnode->getChildByName("title");
	tabletitle->setString(ResourceLang::map_lang["zantitle1"]);

	cocos2d::ui::Button* closebtn = (cocos2d::ui::Button*)csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(ZanLayer::onBtnClick, this));
	closebtn->setTag(1000);
	scrollView = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollview");
	scrollView->setScrollBarEnabled(false);
	scrollView->setBounceEnabled(true);

	helpbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("helpbtn");
	helpbtn->addTouchEventListener(CC_CALLBACK_2(ZanLayer::onBtnClick, this));
	helpbtn->setTag(1001);

	actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionnode")->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(ZanLayer::onBtnClick, this));
	actionbtn->setTag(1002);

	cocos2d::ui::Text* costcoincountlbl = (cocos2d::ui::Text*)csbnode->getChildByName("actionnode")->getChildByName("countlbl");
	costcoincountlbl->setString("5");

	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("zanpublish_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* myinfo = (cocos2d::ui::Text*)csbnode->getChildByName("myinfo");
	myinfo->setString(ResourceLang::map_lang["zanmyinfo"]);

	mynackname = (cocos2d::ui::Text*)csbnode->getChildByName("mynackname");
	mynackname->setString(GlobalInstance::getInstance()->getMyNickName());

	myrankicon = (cocos2d::ui::ImageView*)csbnode->getChildByName("myrankicon");

	myrank = (cocos2d::ui::Text*)csbnode->getChildByName("myrank");
	
	tag0 = (cocos2d::ui::ImageView*)csbnode->getChildByName("tag0");
	tag0->addTouchEventListener(CC_CALLBACK_2(ZanLayer::onBtnClick, this));
	tag0->setTag(0);

	myzancountbox = (cocos2d::ui::Widget*)csbnode->getChildByName("zancountbox");

	myzancountbox->addTouchEventListener(CC_CALLBACK_2(ZanLayer::onBtnClick, this));
	myzancountbox->setTag(1004);

	cocos2d::ui::Text* weektext = (cocos2d::ui::Text*)myzancountbox->getChildByName("desc");
	weektext->setString(ResourceLang::map_lang["zanweektext"]);

	myzancount = (cocos2d::ui::Text*)myzancountbox->getChildByName("myj002count");
	myzancount->setString("");

	j002countbox = (cocos2d::ui::Widget*)csbnode->getChildByName("j002countbox");

	j002count = (cocos2d::ui::Text*)j002countbox->getChildByName("myj002count");
	j002count->setString("");
	
	cocos2d::ui::Text* zanexchangetext = (cocos2d::ui::Text*)j002countbox->getChildByName("desc");
	zanexchangetext->setString(ResourceLang::map_lang["zanexchangetext"]);

	lefttime = (cocos2d::ui::Text*)csbnode->getChildByName("lefttime");
	lefttime->setString("");

	tag1 = (cocos2d::ui::ImageView*)csbnode->getChildByName("tag1");
	tag1->addTouchEventListener(CC_CALLBACK_2(ZanLayer::onBtnClick, this));
	tag1->setTag(1);

	myzanquancount = (cocos2d::ui::Text*)csbnode->getChildByName("myzanquancount");
	std::string countstr = StringUtils::format("%d", MyRes::getMyResCount("j004"));
	myzanquancount->setString(countstr);

	clicktag(lastclicktag);

	WaitingProgress* waitbox = WaitingProgress::create(ResourceLang::map_lang["datawaitingtext"]);
	Director::getInstance()->getRunningScene()->addChild(waitbox, 100, "waitbox");

	HttpDataSwap::init(this)->praiseRankList();
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

void ZanLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
	int tag = clicknode->getTag();
	if (tag != 1004)
		CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{

		if (tag == 1000)
			AnimationEffect::closeAniEffect((Layer*)this);
		else if (tag == 1001)
		{
			ZanRuleLayer* layer = ZanRuleLayer::create();
			this->addChild(layer);
			AnimationEffect::openAniEffect(layer);
		}
		else if (tag == 1002)
		{
			if (GlobalInstance::getInstance()->getMyCoinCount().getValue() >= 5)
			{
				httptag = 1;
				WaitingProgress* waitbox = WaitingProgress::create(ResourceLang::map_lang["datawaitingtext"]);
				Director::getInstance()->getRunningScene()->addChild(waitbox, 100, "waitbox");

				HttpDataSwap::init(this)->askApraise();
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["nomorecoin"]);
			}
		}
		else if (tag == 1003)
		{
			if (j002res != NULL)
			{
				delete j002res;
				j002res = NULL;
			}
			j002res = new ResBase();
			j002res->setId("j002");
			j002res->setType(T_FRAGMENT);
			DynamicValueInt dvqu;
			dvqu.setValue(0);
			j002res->setQU(dvqu);
			DynamicValueInt dvcount;
			dvcount.setValue(GlobalInstance::myj002count.getValue());
			j002res->setCount(dvcount);
			ResDescLayer* layer = ResDescLayer::create(j002res, 0);
			this->addChild(layer);
			AnimationEffect::openAniEffect(layer);
		}
		else if (tag == 1004)
		{
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
			helpbtn->setVisible(false);
			lastclicktag = -1;

			tag0->loadTexture("ui/zantag0.png", cocos2d::ui::Widget::TextureResType::PLIST);
			cocos2d::ui::ImageView* tag0text = (cocos2d::ui::ImageView*)tag0->getChildByName("text");
			tag0text->loadTexture(ResourcePath::makeTextImgPath("zantagtext0_0", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

			cocos2d::ui::ImageView* tag1text = (cocos2d::ui::ImageView*)tag1->getChildByName("text");
			tag1text->loadTexture(ResourcePath::makeTextImgPath("zantagtext1_0", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			tag1->loadTexture("ui/zantag0.png", cocos2d::ui::Widget::TextureResType::PLIST);
			title->loadTexture(ResourcePath::makeTextImgPath("zantitle2", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			tabletitle->setString(ResourceLang::map_lang["zantitle2"]);

			addToMyPraiseList();
		}
		else
		{
			if (tag == lastclicktag)
				return;

			helpbtn->setVisible(true);
			lastclicktag = tag;
			clicktag(tag);

			if (tag == 0)
			{
				getPraiseRankList();
				
			}
			else
			{
				lefttime->setVisible(false);
				WaitingProgress* waitbox = WaitingProgress::create(ResourceLang::map_lang["datawaitingtext"]);
				Director::getInstance()->getRunningScene()->addChild(waitbox, 100, "waitbox");
				httptag = 0;
				HttpDataSwap::init(this)->praiseRankList();
			}
		}
	}
}

void ZanLayer::clicktag(int tag)
{
	if (tag == 0)
	{
		tag0->loadTexture("ui/zantag1.png", cocos2d::ui::Widget::TextureResType::PLIST);
		cocos2d::ui::ImageView* tag0text = (cocos2d::ui::ImageView*)tag0->getChildByName("text");
		tag0text->loadTexture(ResourcePath::makeTextImgPath("zantagtext0_1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

		cocos2d::ui::ImageView* tag1text = (cocos2d::ui::ImageView*)tag1->getChildByName("text");
		tag1text->loadTexture(ResourcePath::makeTextImgPath("zantagtext1_0", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		tag1->loadTexture("ui/zantag0.png", cocos2d::ui::Widget::TextureResType::PLIST);
		title->loadTexture(ResourcePath::makeTextImgPath("zantitle0", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		tabletitle->setString(ResourceLang::map_lang["zantitle0"]);
		myrankicon->setVisible(false);
		myrank->setVisible(false);

		mynackname->setPositionX(180);
		myzancountbox->setPositionX(420);
		myzancountbox->setPositionY(105);
		myzancountbox->setEnabled(false);
		myzancountbox->getChildByName("desc")->setPosition(Vec2(25, 60));
		myzancountbox->getChildByName("desc1")->setVisible(false);
		myzancountbox->setScale(1);
		j002countbox->setVisible(false);
		actionbtn->getParent()->setVisible(true);
	}
	else
	{
		tag0->loadTexture("ui/zantag0.png", cocos2d::ui::Widget::TextureResType::PLIST);
		cocos2d::ui::ImageView* tag0text = (cocos2d::ui::ImageView*)tag0->getChildByName("text");
		tag0text->loadTexture(ResourcePath::makeTextImgPath("zantagtext0_0", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

		cocos2d::ui::ImageView* tag1text = (cocos2d::ui::ImageView*)tag1->getChildByName("text");
		tag1text->loadTexture(ResourcePath::makeTextImgPath("zantagtext1_1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		tag1->loadTexture("ui/zantag1.png", cocos2d::ui::Widget::TextureResType::PLIST);
		title->loadTexture(ResourcePath::makeTextImgPath("zantitle1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		tabletitle->setString(ResourceLang::map_lang["zantitle1"]);

		myrankicon->setVisible(false);
		myrank->setVisible(false);

		mynackname->setPositionX(270);
		myzancountbox->setPositionX(470);
		myzancountbox->setPositionY(130);
		myzancountbox->setScale(0.8f);
		myzancountbox->getChildByName("desc")->setPosition(Vec2(-65, 23));
		myzancountbox->getChildByName("desc1")->setVisible(true);
		myzancountbox->setEnabled(true);
		j002countbox->setVisible(true);

		j002countbox->setPositionX(470);
		j002countbox->setPositionY(78);

		actionbtn->getParent()->setVisible(false);

		cocos2d::ui::Widget* exchangebtn = (cocos2d::ui::Widget*)j002countbox->getChildByName("exchangebtn");
		exchangebtn->addTouchEventListener(CC_CALLBACK_2(ZanLayer::onBtnClick, this));
		exchangebtn->setTag(1003);

		cocos2d::ui::ImageView* exchangebtntxt = (cocos2d::ui::ImageView*)exchangebtn->getChildByName("text");
		exchangebtntxt->loadTexture(ResourcePath::makeTextImgPath("exchangebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	}
}

void ZanLayer::onFinish(int code)
{
	Director::getInstance()->getRunningScene()->removeChildByName("waitbox");
	if (code == 0)
	{
		if (httptag == 0)
		{
			addPaiseRankList();
		}
		else if (httptag == 1)
		{
			DynamicValueInt dv;
			dv.setValue(5);
			GlobalInstance::getInstance()->costMyCoinCount(dv);
			GlobalInstance::myPraisedata.lefttime += 24 * 60 * 60;
			if (!this->isScheduled(schedule_selector(ZanLayer::updateLefttime)))
				this->schedule(schedule_selector(ZanLayer::updateLefttime), 1);

			getPraiseRankList();
		}
		else if (httptag == 2)
		{
			std::string str = StringUtils::format("%d", GlobalInstance::myPraisedata.weekcount);
			myzancount->setString(str);
			addAskList();
			this->schedule(schedule_selector(ZanLayer::updateLefttime), 1);
		}
		else if (httptag == 3)
		{
			MyRes::Use("j004", 1);
			std::string countstr = StringUtils::format("%d", MyRes::getMyResCount("j004"));
			myzanquancount->setString(countstr);
			MovingLabel::show(ResourceLang::map_lang["praisesucc"]);
			//不取服务器数据，先客户端加上
			updatePraiseCount(paisetoplayerindex);
		}
		else if (httptag == 4)
		{
			MovingLabel::show(ResourceLang::map_lang["praisesucc"]);
			MyRes::Add("j001", 10);

			std::string j002countstr = StringUtils::format("%d", GlobalInstance::myj002count.getValue());
			j002count->setString(j002countstr);

			std::string contentstr = StringUtils::format(ResourceLang::map_lang["newtemplet10"].c_str(), GlobalInstance::getInstance()->getMyNickName().c_str());
			MainScene::addNews(contentstr, 2);

		}
	}
	else
	{
		if (httptag == 3 && code > 0)
			MovingLabel::show(ResourceLang::map_lang["praisefail"]);
		else
			MovingLabel::show(ResourceLang::map_lang["matchnetworkerr"]);
	}
}

void ZanLayer::updateLefttime(float dt)
{
	GlobalInstance::myPraisedata.lefttime--;

	if (GlobalInstance::myPraisedata.lefttime >= 0)
	{
		lefttime->setVisible(true);
		std::string timestr = StringUtils::format("%02d:%02d:%02d", GlobalInstance::myPraisedata.lefttime / 3600, GlobalInstance::myPraisedata.lefttime % 3600 / 60, GlobalInstance::myPraisedata.lefttime % 3600 % 60);

		std::string showstr = StringUtils::format(ResourceLang::map_lang["zantimeuptext"].c_str(), timestr.c_str());
		lefttime->setString(showstr);
	}
	else
	{
		this->unschedule(schedule_selector(ZanLayer::updateLefttime));
		if (lefttime->getString().length() > 0)
		{
			lefttime->setString("");
			getPraiseRankList();
		}
		
	}
}

void ZanLayer::getPraiseRankList()
{
	WaitingProgress* waitbox = WaitingProgress::create(ResourceLang::map_lang["datawaitingtext"]);
	Director::getInstance()->getRunningScene()->addChild(waitbox, 100, "waitbox");
	httptag = 2;
	HttpDataSwap::init(this)->askApraiseRankList();
}

void ZanLayer::addAskList()
{
	scrollView->removeAllChildrenWithCleanup(true);
	scrollView->jumpToTop();
	int size = GlobalInstance::vec_PaiseRankData.size();
	int itemheight = 80;
	int innerheight = itemheight * size;
	int contentheight = scrollView->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollView->setInnerContainerSize(Size(720, innerheight));

	for (unsigned int i = 0; i < GlobalInstance::vec_PaiseRankData.size(); i++)
	{
		Node* subnode = CSLoader::createNode(ResourcePath::makePath("zanqiuNode.csb"));

		std::string subnodename = StringUtils::format("item%d", i);
		subnode->setPosition(Vec2(scrollView->getContentSize().width/2, innerheight - i * itemheight - itemheight / 2));
		scrollView->addChild(subnode, 0, subnodename);

		cocos2d::ui::Text* nickname = (cocos2d::ui::Text*)subnode->getChildByName("nackname");
		nickname->setString(GlobalInstance::vec_PaiseRankData[i].nickname);

		cocos2d::ui::Text* countlbl = (cocos2d::ui::Text*)subnode->getChildByName("zancount");
		std::string str = StringUtils::format("%d", GlobalInstance::vec_PaiseRankData[i].weekcount);
		countlbl->setString(str);

		cocos2d::ui::Widget* zanbtn = (cocos2d::ui::Widget*)subnode->getChildByName("zanbtn");
		zanbtn->addTouchEventListener(CC_CALLBACK_2(ZanLayer::onitemBtnClick, this));
		zanbtn->setTag(i);

		if (GlobalInstance::vec_PaiseRankData[i].playerid.compare(GlobalInstance::getInstance()->UUID()) == 0)
		{
			zanbtn->setVisible(false);
		}
	}
}

void ZanLayer::updatePraiseCount(int itemindex)
{
	std::string subnodename = StringUtils::format("item%d", itemindex);
	cocos2d::ui::Text* zancountlbl = (cocos2d::ui::Text*)scrollView->getChildByName(subnodename)->getChildByName("zancount");

	GlobalInstance::vec_PaiseRankData[itemindex].weekcount += 1;
	std::string str = StringUtils::format("%d", GlobalInstance::vec_PaiseRankData[itemindex].weekcount);
	zancountlbl->setString(str);
}

void ZanLayer::onitemBtnClick(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (MyRes::getMyResCount("j004") >= 1)
		{
			Node* clicknode = (Node*)pSender;
			int tag = clicknode->getTag();
			WaitingProgress* waitbox = WaitingProgress::create(ResourceLang::map_lang["datawaitingtext"]);
			Director::getInstance()->getRunningScene()->addChild(waitbox, 100, "waitbox");
			httptag = 3;
			paisetoplayerindex = tag;
			HttpDataSwap::init(this)->praise(GlobalInstance::vec_PaiseRankData[tag].playerid);
		}
		else
		{
			MovingLabel::show(ResourceLang::map_lang["nopraisehint"]);
		}
	}
}

void ZanLayer::addPaiseRankList()
{
	int myrankno = -1;
	scrollView->removeAllChildrenWithCleanup(true);
	scrollView->jumpToTop();
	int size = GlobalInstance::vec_PaiseRankData.size();
	int itemheight = 80;
	int innerheight = itemheight * size;
	int contentheight = scrollView->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollView->setInnerContainerSize(Size(720, innerheight));

	int lastcount = -1;
	int rank = -1;
	for (unsigned int i = 0; i < GlobalInstance::vec_PaiseRankData.size(); i++)
	{

		if (GlobalInstance::vec_PaiseRankData[i].weekcount != lastcount)
		{
			rank++;
			lastcount = GlobalInstance::vec_PaiseRankData[i].weekcount;
		}
		Node* subnode = CSLoader::createNode(ResourcePath::makePath("zanrankNode.csb"));

		std::string subnodename = StringUtils::format("item%d", i);
		subnode->setPosition(Vec2(scrollView->getContentSize().width / 2, innerheight - i * itemheight - itemheight / 2));
		scrollView->addChild(subnode, 0, subnodename);

		cocos2d::ui::Text* nickname = (cocos2d::ui::Text*)subnode->getChildByName("nackname");
		nickname->setString(GlobalInstance::vec_PaiseRankData[i].nickname);

		cocos2d::ui::Text* ranklbl = (cocos2d::ui::Text*)subnode->getChildByName("myrank");
		std::string strrank = StringUtils::format("%d", rank + 1);
		ranklbl->setString(strrank);

		cocos2d::ui::ImageView* rankicon = (cocos2d::ui::ImageView*)subnode->getChildByName("myrankicon");
		if (rank <= 3)
		{
			rankicon->setVisible(true);
			std::string iconstr = StringUtils::format("ui/zanrank%d.png", rank + 1);
			rankicon->loadTexture(iconstr, cocos2d::ui::Widget::TextureResType::PLIST);
		}
		else
		{
			rankicon->setVisible(false);
		}

		cocos2d::ui::Text* countlbl = (cocos2d::ui::Text*)subnode->getChildByName("zancount");
		std::string str = StringUtils::format("%d", GlobalInstance::vec_PaiseRankData[i].weekcount);
		countlbl->setString(str);

		if (GlobalInstance::vec_PaiseRankData[i].playerid.compare(GlobalInstance::getInstance()->getMyID()) == 0)
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

		std::string str = StringUtils::format("%d", GlobalInstance::vec_PaiseRankData[myrankno].weekcount);
		myzancount->setString(str);
	}
	else
	{
		myzancount->setString("0");
	}

	std::string j002countstr = StringUtils::format("%d", GlobalInstance::myj002count.getValue());
	j002count->setString(j002countstr);
}

void ZanLayer::exchange(int count)
{
	httptag = 4;
	WaitingProgress* waitbox = WaitingProgress::create(ResourceLang::map_lang["datawaitingtext"]);
	Director::getInstance()->getRunningScene()->addChild(waitbox, 100, "waitbox");
	HttpDataSwap::init(this)->exchangePraise(count);
}

void ZanLayer::addToMyPraiseList()
{
	scrollView->removeAllChildrenWithCleanup(true);
	scrollView->jumpToTop();
	int size = GlobalInstance::vec_PaiseRankData.size();
	int itemheight = 80;
	int innerheight = itemheight * size;
	int contentheight = scrollView->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollView->setInnerContainerSize(Size(720, innerheight));

	for (unsigned int i = 0; i < GlobalInstance::vec_ToMyPaiseData.size(); i++)
	{
		Node* subnode = CSLoader::createNode(ResourcePath::makePath("toMyPraiseNode.csb"));

		std::string subnodename = StringUtils::format("item%d", i);
		subnode->setPosition(Vec2(scrollView->getContentSize().width / 2, innerheight - i * itemheight - itemheight / 2));
		scrollView->addChild(subnode, 0, subnodename);

		cocos2d::ui::Text* nickname = (cocos2d::ui::Text*)subnode->getChildByName("nackname");
		nickname->setString(GlobalInstance::vec_ToMyPaiseData[i].nickname);

		cocos2d::ui::Text* timelbl = (cocos2d::ui::Text*)subnode->getChildByName("time");

		long long int timeSec = GlobalInstance::vec_ToMyPaiseData[i].lefttime + 8 * 60 * 60;
		char str[64] = { 0 };
		time_t timep = timeSec;
		tm* timeStuct = gmtime(&timep);
		sprintf(str, "%04d-%02d-%02d %02d:%02d:%02d",
			timeStuct->tm_year + 1900,
			timeStuct->tm_mon + 1,
			timeStuct->tm_mday,
			timeStuct->tm_hour,
			timeStuct->tm_min,
			timeStuct->tm_sec
			);
		timelbl->setString(str);
	}

}