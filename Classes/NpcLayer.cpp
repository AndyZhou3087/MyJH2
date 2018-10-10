#include "NpcLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "MovingLabel.h"
#include "AnimationEffect.h"
#include "SoundManager.h"
#include "Const.h"
#include "DataSave.h"

USING_NS_CC;

NpcLayer::NpcLayer()
{
	isShowWord = false;
	m_wordindex = -1;
	m_wordcount = 0;
	m_wordlbl = NULL;
}

NpcLayer::~NpcLayer()
{

}


NpcLayer* NpcLayer::create(std::string npcid)
{
	NpcLayer *pRet = new(std::nothrow)NpcLayer();
	if (pRet && pRet->init(npcid))
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
bool NpcLayer::init(std::string npcid)
{
	if (!Layer::init())
	{
		return false;
	}

	m_npcid = npcid;
	for (unsigned int n = 0; n < GlobalInstance::map_npcrelation[npcid].talk.size(); n++)
	{
		m_wordslist.push_back(GlobalInstance::map_npcrelation[npcid].talk[n]);
		m_wordslist.push_back(GlobalInstance::map_npcrelation[npcid].mytalk[n]);
	}
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color, 0, "colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("npcLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();
	//npc
	npc = (cocos2d::ui::ImageView*)csbnode->getChildByName("npc");
	std::string str = StringUtils::format("images/%s.png", npcid.c_str());
	npc->loadTexture(str, cocos2d::ui::Widget::TextureResType::LOCAL);
	npc->ignoreContentAdaptWithSize(true);
	//hero
	hero = (cocos2d::ui::ImageView*)csbnode->getChildByName("hero");
	str = StringUtils::format("images/hfull_%d_0.png", DataSave::getInstance()->getHeadId());
	hero->loadTexture(str, cocos2d::ui::Widget::TextureResType::LOCAL);
	hero->ignoreContentAdaptWithSize(true);
	hero->setVisible(false);

	hero->setOpacity(150);
	npc->setOpacity(255);
	//npcname
	cocos2d::ui::Text* npcname = (cocos2d::ui::Text*)csbnode->getChildByName("npcname");
	npcname->setString(GlobalInstance::map_AllResources[npcid].name);
	//npcdesc
	cocos2d::ui::Text* npcdesc = (cocos2d::ui::Text*)csbnode->getChildByName("npcdesc");
	npcdesc->setString(GlobalInstance::map_npcrelation[npcid].desc);

	//relationship
	cocos2d::ui::Text* relationship = (cocos2d::ui::Text*)csbnode->getChildByName("relationship");
	str = StringUtils::format("npcrelation_%d", GlobalInstance::map_myfriendly[npcid].relation);
	relationship->setString(ResourceLang::map_lang[str]);

	cocos2d::ui::Text* text0 = (cocos2d::ui::Text*)csbnode->getChildByName("text0");
	text0->setString(ResourceLang::map_lang["npcfriendlytext"]);

	cocos2d::ui::Text* text1 = (cocos2d::ui::Text*)csbnode->getChildByName("text1");
	text1->setString(ResourceLang::map_lang["npcrelationtext"]);

	//好感度进度条
	int friendly = 0;
	if (GlobalInstance::map_myfriendly.find(npcid) != GlobalInstance::map_myfriendly.end())
	{
		friendly = GlobalInstance::map_myfriendly[npcid].friendly;
	}

	int maxfriendly = GlobalInstance::map_npcrelation[npcid].friendmax;
	if (friendly > maxfriendly)
		friendly = maxfriendly;
	else if (friendly < -maxfriendly)
		friendly = -maxfriendly;

	int per = maxfriendly / 5;
	int count = abs(friendly / per);
	for (int m = 0; m < 5; m++)
	{
		std::string barstr = StringUtils::format("friendlybar_%d", m);
		cocos2d::ui::LoadingBar* friendbar = (cocos2d::ui::LoadingBar*)csbnode->getChildByName(barstr);
		friendbar->setPercent(0);
		if (friendly < 0)
		{
			friendbar->loadTexture("ui/friendly_bar_1.png", cocos2d::ui::TextureResType::PLIST);
		}
		else
		{
			friendbar->loadTexture("ui/friendly_bar_0.png", cocos2d::ui::TextureResType::PLIST);
		}
		if (m < count)
		{
			friendbar->setPercent(100);
		}
		else if (m == count)
		{
			float percent = fabs(friendly % per * 100.0f / per);
			friendbar->setPercent(percent);
		}
	}

	std::string btnstr[6] = { "talkbtn","givebtn","friendbtn","masterbtn","marrybtn","fightbtn" };
	for (int i = 0; i < 6; i++)
	{
		cocos2d::ui::ImageView* npcbtn = (cocos2d::ui::ImageView*)csbnode->getChildByName(btnstr[i]);
		npcbtn->setTag(i);
		npcbtn->addTouchEventListener(CC_CALLBACK_2(NpcLayer::onBtnClick, this));
		cocos2d::ui::ImageView* npcbtntxt = (cocos2d::ui::ImageView*)npcbtn->getChildByName("imagetext");
		str = StringUtils::format("npctext_%d", i);
		npcbtntxt->loadTexture(ResourcePath::makeTextImgPath(str, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		if (i == 2)
		{
			if (GlobalInstance::map_myfriendly[npcid].relation == NPC_FRIEND)
			{
				npcbtn->loadTexture("ui/npc_friend1.png", cocos2d::ui::Widget::TextureResType::PLIST);
				npcbtntxt->loadTexture(ResourcePath::makeTextImgPath("npctext_2_1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			}
		}
		else if (i == 3)
		{
			if (GlobalInstance::map_myfriendly[npcid].relation == NPC_MASTER)
			{
				npcbtn->loadTexture("ui/npc_master1.png", cocos2d::ui::Widget::TextureResType::PLIST);
				npcbtntxt->loadTexture(ResourcePath::makeTextImgPath("npctext_3_1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			}
		}
		else if (i == 4)
		{
			if (GlobalInstance::map_myfriendly[npcid].relation == NPC_COUPEL)
			{
				npcbtn->loadTexture("ui/npc_marry1.png", cocos2d::ui::Widget::TextureResType::PLIST);
				npcbtntxt->loadTexture(ResourcePath::makeTextImgPath("npctext_4_1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			}
		}
	}

	//关闭
	cocos2d::ui::Button* closebtn = (cocos2d::ui::Button*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1000);
	closebtn->addTouchEventListener(CC_CALLBACK_2(NpcLayer::onBtnClick, this));
	//按钮文字
	cocos2d::ui::ImageView* closebtntext = (cocos2d::ui::ImageView*)closebtn->getChildByName("imagetext");
	closebtntext->loadTexture(ResourcePath::makeTextImgPath("backbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	scrollView = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollView");
	scrollView->setScrollBarEnabled(false);
	scrollView->setBounceEnabled(true);


	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		/*if (isShowWord)
		{
			if (m_wordslist.size() <= 0)
				return;

			if (m_wordslist[m_wordindex].length() > 0)
			{
				showFastWords();
				return;
			}
		}
		m_wordindex++;

		int size = m_wordslist.size();

		if (m_wordindex >= size)
		{
			return;
		}
		else
		{
			checkWordLblColor(m_wordslist[m_wordindex]);
		}*/
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void NpcLayer::checkWordLblColor(std::string wordstr)
{
	if (isScheduled(schedule_selector(NpcLayer::showTypeText)))
	{
		unschedule(schedule_selector(NpcLayer::showTypeText));
	}
	if (m_wordlbl == NULL)
	{
		m_wordlbl = Label::createWithTTF(wordstr, FONT_NAME, 20);
		m_wordlbl->setLineBreakWithoutSpace(true);
		m_wordlbl->setAnchorPoint(Vec2(0, 1));
		m_wordlbl->setMaxLineWidth(scrollView->getContentSize().width);
		scrollView->addChild(m_wordlbl, 0, "talklbl");
	}
	else
	{
		m_wordcount = 0;
		m_wordlbl->unschedule("schedule_typecallback");
		m_wordlbl->setString(wordstr);
	}
	int innerheight = m_wordlbl->getStringNumLines() * 20;//contentlbl->getHeight();
	int contentheight = scrollView->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollView->setInnerContainerSize(Size(scrollView->getContentSize().width, innerheight));
	m_wordlbl->setPosition(Vec2(0, innerheight));

	int index = 0;
	while (m_wordlbl->getLetter(index) != NULL)
	{
		m_wordlbl->getLetter(index)->setColor(Color3B::WHITE);
		m_wordlbl->getLetter(index)->setScale(0);
		index++;
	}

	this->scheduleOnce(schedule_selector(NpcLayer::showTypeText), 0.1f);
}

void NpcLayer::showTypeText(float dt)
{
	hero->setVisible(true);
	if (m_wordindex % 2 == 0)
	{
		hero->setOpacity(150);
		npc->setOpacity(255);
	}
	else
	{
		hero->setOpacity(255);
		npc->setOpacity(150);
	}

	m_wordlbl->schedule([&](float dt) {
		isShowWord = true;
		m_wordlbl->getLetter(m_wordcount)->setScale(1.0f);
		m_wordcount += 1;
		std::u32string utf32String;
		StringUtils::UTF8ToUTF32(m_wordlbl->getString(), utf32String);
		int len = utf32String.length();
		if (m_wordcount % 64 == 0)
		{
			scrollView->jumpToPercentVertical(scrollView->getScrolledPercentVertical() + 100 / ceil(len / 64));
		}
		if (m_wordcount >= len)
		{
			isShowWord = false;
			m_wordcount = 0;
			m_wordlbl->unschedule("schedule_typecallback");
		}

	}, 0.1f, "schedule_typecallback");
}

void NpcLayer::showFastWords()
{
	if (m_wordlbl != NULL)
	{
		isShowWord = false;
		m_wordcount = 0;
		m_wordlbl->unschedule("schedule_typecallback");
		int index = 0;
		while (m_wordlbl->getLetter(index) != NULL)
		{
			m_wordlbl->getLetter(index)->setScale(1);
			index++;
		}
	}

}

void NpcLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
		switch (tag)
		{
		case 1000://关闭
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		case 0://
		{
			if (isShowWord)
			{
				if (m_wordslist.size() <= 0)
					return;

				if (m_wordslist[m_wordindex].length() > 0)
				{
					showFastWords();
					return;
				}
			}
			m_wordindex++;

			int size = m_wordslist.size();

			if (m_wordindex >= size)
			{
				return;
			}
			else
			{
				checkWordLblColor(m_wordslist[m_wordindex]);
			}
			break;
		}
		case 1:
		{
			break;
		}
		case 2://
		{


			break;
		}
		case 3:
		{
			break;
		}
		case 4://
		{


			break;
		}
		case 5:
		{
			break;
		}
		default:
			break;
		}
	}
}

void NpcLayer::onExit()
{
	Layer::onExit();
}