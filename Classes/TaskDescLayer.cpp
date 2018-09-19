#include "TaskDescLayer.h"
#include <algorithm>
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "MyRes.h"
#include "MyMenu.h"
#include "MovingLabel.h"
#include "TaskNode.h"
#include "AnimationEffect.h"
#include "MainScene.h"
#include "RewardLayer.h"
#include "NewGuideLayer.h"
#include "Quest.h"

TaskDescLayer::TaskDescLayer()
{

}


TaskDescLayer::~TaskDescLayer()
{

}

TaskDescLayer* TaskDescLayer::create(TaskData* data, int type)
{
	TaskDescLayer *pRet = new(std::nothrow)TaskDescLayer();
	if (pRet && pRet->init(data, type))
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

bool TaskDescLayer::init(TaskData* data, int type)
{
	if (!Layer::init())
	{
		return false;
	}

	m_type = type;
	m_data = data;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* m_csbnode = CSLoader::createNode(ResourcePath::makePath("taskMainDescLayer.csb"));
	this->addChild(m_csbnode);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Text* rewardlabel = (cocos2d::ui::Text*)m_csbnode->getChildByName("rewardlabel");
	rewardlabel->setString(ResourceLang::map_lang["taskrewardtip2"]);

	//标题
	cocos2d::ui::Text* name = (cocos2d::ui::Text*)m_csbnode->getChildByName("name");
	name->setString(data->name);

	cocos2d::ui::ScrollView* descscoll = (cocos2d::ui::ScrollView*)m_csbnode->getChildByName("descscoll");
	Label* contentlbl = Label::createWithTTF(data->desc, FONT_NAME, 25);
	contentlbl->setAnchorPoint(Vec2(0, 1));
	contentlbl->setColor(Color3B(255, 255, 255));
	//contentlbl->setHorizontalAlignment(TextHAlignment::LEFT);
	contentlbl->enableShadow(Color4B::BLACK, Size(1, -1));
	contentlbl->setLineBreakWithoutSpace(true);
	contentlbl->setMaxLineWidth(descscoll->getContentSize().width);
	descscoll->addChild(contentlbl);
	int innerheight = contentlbl->getStringNumLines() * 25;//contentlbl->getHeight();
	int contentheight = descscoll->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	descscoll->setInnerContainerSize(Size(descscoll->getContentSize().width, innerheight));
	contentlbl->setPosition(Vec2(0, innerheight));

	//npc头像
	cocos2d::ui::ImageView* icon = (cocos2d::ui::ImageView*)m_csbnode->getChildByName("icon");
	std::string str = "ui/task_head.png";// StringUtils::format("mapui/%s.png", GlobalInstance::map_Npcs[data->npcid].icon.c_str());
	icon->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
	icon->setContentSize(Sprite::createWithSpriteFrameName(str)->getContentSize());
	//icon->setVisible(false);

	//ClippingNode* m_clippingNode = ClippingNode::create();
	//m_clippingNode->setInverted(false);//设置底板可见
	//m_clippingNode->setAlphaThreshold(0.5f);//设置透明度Alpha值为0
	//this->addChild(m_clippingNode, 1);
	//m_clippingNode->setAnchorPoint(Vec2(0.5, 1));
	//m_clippingNode->setPosition(Vec2(icon->getPositionX(), icon->getPositionY() + 45));
	//Sprite* head = Sprite::createWithSpriteFrameName(str);
	//head->setAnchorPoint(Vec2(0.5, 1));
	//head->setPositionY(20);
	//m_clippingNode->addChild(head);
	//Node* stencil = Node::create();
	//Sprite* cnode = Sprite::createWithSpriteFrameName("ui/headclip.png");
	//cnode->setAnchorPoint(Vec2(0.5, 1));
	//stencil->addChild(cnode);
	//m_clippingNode->setStencil(stencil);

	//npc名字
	cocos2d::ui::Text* npcname = (cocos2d::ui::Text*)m_csbnode->getChildByName("npcname");
	npcname->setString(ResourceLang::map_lang["littlesistext"]);

	closebtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("closebtn");
	closebtn->setPosition(Vec2(357, 183));
	closebtn->setTag(0);
	closebtn->addTouchEventListener(CC_CALLBACK_2(TaskDescLayer::onBtnClick, this));
	closebtn->setTitleText(ResourceLang::map_lang["closetext"]);

	accbtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("accbtn");
	accbtn->setPosition(Vec2(357, 376));
	accbtn->setTag(1);
	accbtn->addTouchEventListener(CC_CALLBACK_2(TaskDescLayer::onBtnClick, this));
	accbtn->setTitleText(ResourceLang::map_lang["acctasktext"]);

	getbtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("getbtn");
	getbtn->setPosition(Vec2(357, 376));
	getbtn->setTag(2);
	getbtn->addTouchEventListener(CC_CALLBACK_2(TaskDescLayer::onBtnClick, this));
	getbtn->setTitleText(ResourceLang::map_lang["getrewardtext"]);

	cocos2d::ui::ScrollView* scrollView = (cocos2d::ui::ScrollView*)m_csbnode->getChildByName("ScrollView");
	scrollView->setScrollBarEnabled(false);
	scrollView->setBounceEnabled(true);

	if (data->isfinish == QUEST_FINISH)
	{
		getbtn->setVisible(true);
		accbtn->setVisible(false);
	}
	else if (data->isfinish == QUEST_GET)
	{
		getbtn->setVisible(false);
		accbtn->setVisible(false);
	}
	else if (data->isfinish == QUEST_ACC)
	{
		getbtn->setVisible(false);
		accbtn->setVisible(true);
		accbtn->setTitleText(ResourceLang::map_lang["accmaintask"]);
		accbtn->setTouchEnabled(false);
	}
	else
	{
		getbtn->setVisible(false);
		accbtn->setVisible(true);
	}
	
	int id;
	if (m_type == 0)
	{
		id = GlobalInstance::myCurMainData.id;
	}
	else
	{
		id = GlobalInstance::myCurBranchData.id;
	}
	if (data->id != id && data->isfinish == QUEST_TASK)
	{
		getbtn->setVisible(false);
		accbtn->setVisible(false);
	}

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
		else if (t == T_RENS || t == T_DAN || t == T_MIJI || t == T_BOX)
		{
			qu = atoi(resid.substr(1).c_str()) - 1;
			qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}


		Sprite * box = Sprite::createWithSpriteFrameName(qustr);
		box->setPosition(Vec2(80 + i % 3 * 170, 103 - i / 3 * 163));
		scrollView->addChild(box);

		CommonFuncs::playResBoxEffect(box, qu);

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
	if (accbtn->isVisible())
		vec_btnnode.push_back(accbtn);
	if (getbtn->isVisible())
		vec_btnnode.push_back(getbtn);
	vec_btnnode.push_back(closebtn);
	for (unsigned int i = 0; i < vec_btnnode.size(); i++)
	{
		vec_btnnode[i]->setPosition(Vec2(360, 430 - 145 * i));
	}


	this->scheduleOnce(schedule_selector(TaskDescLayer::delayShowNewerGuide), newguidetime);

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}

void TaskDescLayer::delayShowNewerGuide(float dt)
{
	if (!NewGuideLayer::checkifNewerGuide(41))
	{
		if (NewGuideLayer::checkifNewerGuide(MIDELEGUIDESTEP))
		{
			if (NewGuideLayer::checkifNewerGuide(42))
			{
				showNewerGuide(42);
			}
			/*else if (NewGuideLayer::checkifNewerGuide(43))
			{
				showNewerGuide(43);
			}*/
		}
		else if (NewGuideLayer::checkifNewerGuide(FIFTHGUIDESTEP) && Quest::isMainQuestFinish(1))
		{
			if (NewGuideLayer::checkifNewerGuide(57))
			{
				showNewerGuide(57);
			}
			/*else if (NewGuideLayer::checkifNewerGuide(58))
			{
				showNewerGuide(58);
			}*/
			else if (NewGuideLayer::checkifNewerGuide(60))
			{
				showNewerGuide(60);
			}
			/*else if (NewGuideLayer::checkifNewerGuide(61))
			{
				showNewerGuide(61);
			}*/
		}
	}
}

void TaskDescLayer::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	if (step == 42 || step == 60)
	{
		nodes.push_back(accbtn);
	}
	else if (step == 43 || step == 58 || step == 61)
	{
		nodes.push_back(closebtn);
	}
	else if (step == 57)
	{
		nodes.push_back(getbtn);
	}
	g_mainScene->showNewerGuideNode(step, nodes);
}

void TaskDescLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
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
		case 1: //接受任务
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_TAKEMISSION);
			accbtn->setTitleText(ResourceLang::map_lang["accmaintask"]);
			accbtn->setTouchEnabled(false);
			accpTask();
			g_mainScene->updateTaskIcon();
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		case 2: //完成后领取奖励
			node->setVisible(false);
			getRewards();
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		default:
			break;
		}

		//红点提示
		TaskLayer* layer = (TaskLayer*)g_mainScene->getChildByName("9assemblyhall");
		if (layer != NULL)
		{
			layer->updateQuestTip();
		}
	}
}

void TaskDescLayer::accpTask()
{
	m_data->isfinish = QUEST_ACC;
	if (m_type == 0)
	{
		GlobalInstance::myCurMainData.isfinish = QUEST_ACC;
	}
	else
	{
		GlobalInstance::myCurBranchData.isfinish = QUEST_ACC;
	}
}

void TaskDescLayer::getRewards()
{
	m_data->isfinish = QUEST_GET;
	std::vector<MSGAWDSDATA> vec_rewards;
	std::vector<std::vector<std::string>> m_rewards;
	for (unsigned int i = 0; i < m_data->type.size(); i++)
	{
		if (m_data->finishtype == m_data->type[i])
		{
			if (i == 0)
			{
				m_rewards = m_data->reward1;
			}
			else
			{
				m_rewards = m_data->reward2;
			}
		}
	}
	for (unsigned int i = 0; i < m_rewards.size(); i++)
	{
		std::vector<std::string> one_res = m_rewards[i];
		std::string resid = one_res[0];
		int count = atoi(one_res[1].c_str());
		int qu = 0;
		int stc = 0;
		if (one_res.size()>2 && one_res[2].length()>0)
		{
			qu = atoi(one_res[2].c_str());
		}

		MSGAWDSDATA wdata;
		wdata.rid = resid;
		wdata.count = count;
		wdata.qu = qu;
		vec_rewards.push_back(wdata);
	}
	if (vec_rewards.size() > 0)
	{
		RewardLayer* layer = RewardLayer::create(vec_rewards);
		g_mainScene->addChild(layer);
		AnimationEffect::openAniEffect(layer);
	}
}