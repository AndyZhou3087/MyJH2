#include "TaskTalkLayer.h"
#include <algorithm>
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "MyRes.h"
#include "MyMenu.h"
#include "Quest.h"
#include "MapBlockScene.h"
#include "FightingLayer.h"
#include "MovingLabel.h"

TaskTalkLayer::TaskTalkLayer()
{
	isGo = 0;
	m_wordindex = 0;
	m_wordcount = 0;
	isFight = false;
	m_wordlbl = NULL;
}


TaskTalkLayer::~TaskTalkLayer()
{

}

TaskTalkLayer* TaskTalkLayer::create(std::string npcid, std::vector<Npc*> vec_enemys, int type)
{
	TaskTalkLayer *pRet = new(std::nothrow)TaskTalkLayer();
	if (pRet && pRet->init(npcid, vec_enemys, type))
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

bool TaskTalkLayer::init(std::string npcid, std::vector<Npc*> vec_enemys, int type)
{
	m_npcid = npcid;
	m_vec_enemys = vec_enemys;
	m_type = type;

	if (type == 0)
	{
		data = &GlobalInstance::myCurMainData;
	}
	else
	{
		data = &GlobalInstance::myCurBranchData;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* m_csbnode = CSLoader::createNode(ResourcePath::makePath("taskMainDescLayer.csb"));
	this->addChild(m_csbnode);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	int langtype = GlobalInstance::getInstance()->getLang();

	//标题
	cocos2d::ui::Text* name = (cocos2d::ui::Text*)m_csbnode->getChildByName("name");
	name->setString(data->name);

	/*desc = (cocos2d::ui::Text*)m_csbnode->getChildByName("desc");
	desc->setVisible(false);
	desc->setString(data->bossword);*/
	descscoll = (cocos2d::ui::ScrollView*)m_csbnode->getChildByName("descscoll");
	descscoll->addTouchEventListener(CC_CALLBACK_2(TaskTalkLayer::onWordScroll, this));

	checkWordLblColor(data->bossword);

	cocos2d::ui::Text* rewardlabel = (cocos2d::ui::Text*)m_csbnode->getChildByName("rewardlabel");
	rewardlabel->setString(ResourceLang::map_lang["taskrewardtip"]);

	cocos2d::ui::ImageView* headbox = (cocos2d::ui::ImageView*)m_csbnode->getChildByName("box");
	headbox->loadTexture("ui/headbox.png", cocos2d::ui::Widget::TextureResType::PLIST);
	headbox->setContentSize(Sprite::createWithSpriteFrameName("ui/headbox.png")->getContentSize());

	//npc头像
	cocos2d::ui::ImageView* icon = (cocos2d::ui::ImageView*)m_csbnode->getChildByName("icon");
	std::string str = StringUtils::format("mapui/%s.png", GlobalInstance::map_Npcs[data->npcid].icon.c_str());
	/*icon->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
	icon->setContentSize(Sprite::createWithSpriteFrameName(str)->getContentSize());*/
	icon->setVisible(false);

	ClippingNode* m_clippingNode = ClippingNode::create();
	m_clippingNode->setInverted(false);//设置底板可见
	m_clippingNode->setAlphaThreshold(0.5f);//设置透明度Alpha值为0
	this->addChild(m_clippingNode, 1);
	m_clippingNode->setAnchorPoint(Vec2(0.5, 1));
	m_clippingNode->setPosition(Vec2(icon->getPositionX() - 0.5, icon->getPositionY() + 37.5));
	Sprite* head = Sprite::createWithSpriteFrameName(str);
	head->setAnchorPoint(Vec2(0.5, 1));
	head->setPositionY(20);
	m_clippingNode->addChild(head);
	Node* stencil = Node::create();
	Sprite* cnode = Sprite::createWithSpriteFrameName("ui/headclip.png");
	cnode->setAnchorPoint(Vec2(0.5, 1));
	stencil->addChild(cnode);
	m_clippingNode->setStencil(stencil);

	//npc名字
	cocos2d::ui::Text* npcname = (cocos2d::ui::Text*)m_csbnode->getChildByName("npcname");
	npcname->setString(GlobalInstance::map_AllResources[data->npcid].name);

	closebtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("closebtn");
	closebtn->setPosition(Vec2(357, 131));
	closebtn->setTag(0);
	closebtn->addTouchEventListener(CC_CALLBACK_2(TaskTalkLayer::onBtnClick, this));
	closebtn->setTitleText(ResourceLang::map_lang["closetext"]);

	givebtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("accbtn");
	givebtn->setPosition(Vec2(357, 429));
	givebtn->setTag(data->type[0]);
	givebtn->addTouchEventListener(CC_CALLBACK_2(TaskTalkLayer::onBtnClick, this));
	givebtn->setTitleText(data->need1desc);

	fightbtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("getbtn");
	fightbtn->setVisible(true);
	fightbtn->setPosition(Vec2(357, 280));
	if (data->type.size() > 1)
	{
		fightbtn->setTag(data->type[1]);
	}
	fightbtn->addTouchEventListener(CC_CALLBACK_2(TaskTalkLayer::onBtn2Click, this));
	fightbtn->setTitleText(data->need2desc);

	cocos2d::ui::ScrollView* scrollView = (cocos2d::ui::ScrollView*)m_csbnode->getChildByName("ScrollView");
	scrollView->setScrollBarEnabled(false);
	scrollView->setBounceEnabled(true);

	if (data->type.size()<2)
	{
		fightbtn->setVisible(false);
		givebtn->setPosition(Vec2(357, 376));
		closebtn->setPosition(Vec2(357, 183));
		givebtn->setTitleText(data->need1desc);
	}
	else
	{
		//判断是否互斥
		if (m_type == 0)
		{
			if (Quest::getMutexMainQuestType(data->id, data->type[0]))
			{
				givebtn->setTouchEnabled(false);
			}
			if (Quest::getMutexMainQuestType(data->id, data->type[1]))
			{
				fightbtn->setTouchEnabled(false);
			}
		}
		else
		{
			if (Quest::getMutexBranchQuestType(data->id, data->type[0]))
			{
				givebtn->setTouchEnabled(false);
			}
			if (Quest::getMutexBranchQuestType(data->id, data->type[1]))
			{
				fightbtn->setTouchEnabled(false);
			}
		}
	}

	std::vector<std::vector<std::string>> rewards;
	if (data->type.size() == 1)
	{
		rewards = data->reward1;
	}
	else
	{
		for (unsigned int m = 0; m < data->reward1.size(); m++)
		{
			rewards.push_back(data->reward1[m]);
		}
		/*for (unsigned int n = 0; n < data->reward2.size(); n++)
		{
			rewards.push_back(data->reward2[n]);
		}*/
	}

	for (unsigned int i = 0; i < rewards.size(); i++)
	{
		std::vector<std::string> one_res = rewards[i];
		std::string resid = one_res[0];
		int count = atoi(one_res[1].c_str());
		int qu = 0;
		if (one_res.size()>2 && one_res[2].length()>0)
		{
			qu = atoi(one_res[2].c_str());
		}

		std::string qustr = "ui/resbox.png";
		int t = 0;
		for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
		{
			if (resid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
				break;
		}
		if (t >= T_ARMOR && t <= T_NG)
		{
			qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}
		Sprite * box = Sprite::createWithSpriteFrameName(qustr);
		box->setPosition(Vec2(80 + i % 3 * 170, 103 - i / 3 * 163));
		scrollView->addChild(box);

		std::string str = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);

		Sprite * res = Sprite::createWithSpriteFrameName(str);
		res->setPosition(Vec2(box->getContentSize().width / 2, box->getContentSize().height / 2));
		box->addChild(res);

		str = StringUtils::format("%sx%d", GlobalInstance::map_AllResources[resid].name.c_str(), count);
		Label *namelbl = Label::createWithTTF(str, FONT_NAME, 23);
		namelbl->setColor(Color3B(255, 255, 255));
		namelbl->setPosition(Vec2(box->getContentSize().width / 2, -10));
		box->addChild(namelbl);

	}
	
	std::vector<Node*> vec_btnnode;
	if (givebtn->isVisible())
		vec_btnnode.push_back(givebtn);
	if (fightbtn->isVisible())
		vec_btnnode.push_back(fightbtn);
	vec_btnnode.push_back(closebtn);
	for (unsigned int i = 0; i < vec_btnnode.size(); i++)
	{
		vec_btnnode[i]->setPosition(Vec2(360, 430 - 145 * i));
	}

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}

void TaskTalkLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		cocos2d::ui::Button* btn = (cocos2d::ui::Button*)pSender;
		int tag = btn->getTag();

		isGo = data->isFight1;
		std::string bwords = data->bossword1;

		switch (tag)
		{
		case 0:
			if (btn->getTitleText().compare(ResourceLang::map_lang["okbtntext"]) == 0)
			{
				if (isGo == 1)
				{
					g_MapBlockScene->showFightingLayer(m_vec_enemys);
					isGo = 0;
				}
				if (isFight)
				{
					g_MapBlockScene->showFightingLayer(m_vec_enemys);
				}
			}
			this->removeFromParentAndCleanup(true);
			break;
		case QUEST_GIVE: //
			questGive(bwords, data->need1);
			break;
		case QUEST_FIGHT: //
			questFight(bwords);
			break;
		case QUEST_NOTFIGHT:
			questNotFight(bwords);
			break;
		case QUEST_TAKEMY:
			questTakeGoods(bwords, data->need1);
			break;
		default:
			break;
		}
	}
}

void TaskTalkLayer::onBtn2Click(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		cocos2d::ui::Button* btn = (cocos2d::ui::Button*)pSender;
		int tag = btn->getTag();

		isGo = data->isFight2;
		std::string bwords = data->bossword2;

		switch (tag)
		{
		case QUEST_GIVE: //
			questGive(bwords, data->need2);
			break;
		case QUEST_FIGHT: //
			questFight(bwords);
			break;
		case QUEST_NOTFIGHT:
			questNotFight(bwords);
		case QUEST_TAKEMY:
			questTakeGoods(bwords, data->need2);
			break;
		default:
			break;
		}
	}
}

void TaskTalkLayer::onWordScroll(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (m_wordlbl != NULL)
		{
			m_wordlbl->unschedule("schedule_typecallback");
			int index = 0;
			while (m_wordlbl->getLetter(index) != NULL)
			{
				m_wordlbl->getLetter(index)->setScale(1);
				index++;
			}
		}
	}
}

void TaskTalkLayer::questGive(std::string bwords, std::vector<std::map<std::string, int>> need)
{
	bool isEnough = true;
	for (unsigned int i = 0; i < need.size(); i++)
	{
		std::map<std::string, int> one_res = need[i];
		std::map<std::string, int>::iterator oneit = one_res.begin();
		std::string cresid = oneit->first;
		if (cresid.compare("r006") == 0)//银两
		{
			if (GlobalInstance::getInstance()->getMySoliverCount().getValue() < oneit->second)
			{
				isEnough = false;
				MovingLabel::show(ResourceLang::map_lang["nomoresilver"]);
				break;
			}
		}
		else if (cresid.compare("r012") == 0)//元宝
		{
			if (GlobalInstance::getInstance()->getMyCoinCount().getValue() < oneit->second)
			{
				isEnough = false;
				MovingLabel::show(ResourceLang::map_lang["nomorecoin"]);
				break;
			}
		}
		else
		{
			if (MyRes::getMyResCount(cresid, MYPACKAGE) + MyRes::getMyResCount(cresid, MYSTORAGE) < oneit->second)
			{
				isEnough = false;
				MovingLabel::show(ResourceLang::map_lang["reslack"]);
				break;
			}
		}
	}
	if (isEnough)
	{
		bool isfinish = false;
		for (unsigned int i = 0; i < need.size(); i++)
		{
			std::map<std::string, int> one_res = need[i];
			std::map<std::string, int>::iterator oneit = one_res.begin();
			std::string cresid = oneit->first;
			if (m_type == 0)
			{
				if (Quest::checkResQuestData(cresid, oneit->second, m_npcid))
				{
					isfinish = true;
				}
			}
			else
			{
				if (Quest::checkResBranchQuestData(cresid, oneit->second, m_npcid))
				{
					isfinish = true;
				}
			}
		}
		if (isfinish)
		{
			g_MapBlockScene->showUnlockChapter();
		}


		if (bwords.length()>1)
		{
			checkWordLblColor(bwords);
		}
		closebtn->setTitleText(ResourceLang::map_lang["okbtntext"]);
		fightbtn->setVisible(false);
		givebtn->setVisible(false);
	}
}

void TaskTalkLayer::questFight(std::string bwords)
{
	if (bwords.length()>1)
	{
		isFight = true;
		checkWordLblColor(bwords);
		closebtn->setTitleText(ResourceLang::map_lang["okbtntext"]);
		fightbtn->setVisible(false);
		givebtn->setVisible(false);
	}
	else
	{
		g_MapBlockScene->showFightingLayer(m_vec_enemys);
		this->removeFromParentAndCleanup(true);
	}
}

void TaskTalkLayer::questTakeGoods(std::string bwords, std::vector<std::map<std::string, int>> need)
{
	if (bwords.length()>1)
	{
		checkWordLblColor(bwords);
	}
	closebtn->setTitleText(ResourceLang::map_lang["okbtntext"]);
	fightbtn->setVisible(false);
	givebtn->setVisible(false);

	for (unsigned int i = 0; i < need.size(); i++)
	{
		std::map<std::string, int> one_res = need[i];
		std::map<std::string, int>::iterator oneit = one_res.begin();
		std::string cresid = oneit->first;
		if (cresid.compare("r006") == 0)//银两
		{
			DynamicValueInt dal;
			dal.setValue(oneit->second);
			GlobalInstance::getInstance()->addMySoliverCount(dal);
		}
		else if (cresid.compare("r012") == 0)//元宝
		{
			DynamicValueInt dal;
			dal.setValue(oneit->second);
			GlobalInstance::getInstance()->addMyCoinCount(dal);
		}
		else
		{
			MyRes::Add(cresid, oneit->second);
		}
		std::string str = StringUtils::format(ResourceLang::map_lang["getgoodstext"].c_str(), GlobalInstance::map_AllResources[cresid].name.c_str(), oneit->second);
		MovingLabel::show(str);
	}

	if (m_type == 0)
	{
		Quest::finishTaskMain(QUEST_TAKEMY);
	}
	else
	{
		Quest::finishTaskBranch(QUEST_TAKEMY);
	}
}

void TaskTalkLayer::questNotFight(std::string bwords)
{
	if (bwords.length()>1)
	{
		checkWordLblColor(bwords);
	}
	closebtn->setTitleText(ResourceLang::map_lang["okbtntext"]);
	fightbtn->setVisible(false);
	givebtn->setVisible(false);
	if (m_type == 0)
	{
		Quest::finishTaskMain(QUEST_NOTFIGHT);
	}
	else
	{
		Quest::finishTaskBranch(QUEST_NOTFIGHT);
	}
}

void TaskTalkLayer::checkWordLblColor(std::string wordstr)
{
	if (m_wordlbl == NULL)
	{
		m_wordlbl = Label::createWithTTF(wordstr, FONT_NAME, 25);
		m_wordlbl->setLineBreakWithoutSpace(true);
		m_wordlbl->enableShadow(Color4B::BLACK, Size(1, -1));
		m_wordlbl->setAnchorPoint(Vec2(0, 1));
		m_wordlbl->setMaxLineWidth(descscoll->getContentSize().width);
		descscoll->addChild(m_wordlbl, 0, "talklbl");
	}
	else
	{
		m_wordcount = 0;
		isShowWord = false;
		m_wordlbl->unschedule("schedule_typecallback");
		m_wordlbl->setString(wordstr);
	}
	int innerheight = m_wordlbl->getStringNumLines() * 25;//contentlbl->getHeight();
	int contentheight = descscoll->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	descscoll->setInnerContainerSize(Size(descscoll->getContentSize().width, innerheight));
	m_wordlbl->setPosition(Vec2(0, innerheight));

	int index = 0;
	while (m_wordlbl->getLetter(index) != NULL)
	{
		m_wordlbl->getLetter(index)->setColor(Color3B::WHITE);
		m_wordlbl->getLetter(index)->setScale(0);
		index++;
	}

	this->scheduleOnce(schedule_selector(TaskTalkLayer::showTypeText), 0.1f);
}

void TaskTalkLayer::showTypeText(float dt)
{
	m_wordlbl->schedule([&](float dt) {
		isShowWord = true;
		m_wordlbl->getLetter(m_wordcount)->setScale(1.0f);
		m_wordcount += 1;
		std::u32string utf32String;
		StringUtils::UTF8ToUTF32(m_wordlbl->getString(), utf32String);
		int len = utf32String.length();
		if (m_wordcount >= len)
		{
			m_wordcount = 0;
			isShowWord = false;
			m_wordlbl->unschedule("schedule_typecallback");
		}

	}, 0.1f, "schedule_typecallback");
}