#include "TaskBranchDescLayer.h"
#include <algorithm>
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "MyRes.h"
#include "MyMenu.h"
#include "MovingLabel.h"
#include "TaskBranchNode.h"
#include "AnimationEffect.h"

TaskBranchDescLayer::TaskBranchDescLayer()
{

}


TaskBranchDescLayer::~TaskBranchDescLayer()
{

}

TaskBranchDescLayer* TaskBranchDescLayer::create(TaskBranchData* data)
{
	TaskBranchDescLayer *pRet = new(std::nothrow)TaskBranchDescLayer();
	if (pRet && pRet->init(data))
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

bool TaskBranchDescLayer::init(TaskBranchData* data)
{
	if (!Layer::init())
	{
		return false;
	}

	m_data = data;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* m_csbnode = CSLoader::createNode(ResourcePath::makePath("taskMainDescLayer.csb"));
	this->addChild(m_csbnode);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Text* rewardlabel = (cocos2d::ui::Text*)m_csbnode->getChildByName("rewardlabel");
	rewardlabel->setString(ResourceLang::map_lang["taskrewardtip"]);

	//标题
	cocos2d::ui::Text* name = (cocos2d::ui::Text*)m_csbnode->getChildByName("name");
	name->setString(data->name);

	cocos2d::ui::ScrollView* descscoll = (cocos2d::ui::ScrollView*)m_csbnode->getChildByName("descscoll");
	Label* contentlbl = Label::createWithTTF(data->desc, FONT_NAME, 25);
	contentlbl->setAnchorPoint(Vec2(0, 1));
	contentlbl->setColor(Color3B(255, 255, 255));
	//contentlbl->setHorizontalAlignment(TextHAlignment::LEFT);
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
	std::string str = StringUtils::format("mapui/%s.png", GlobalInstance::map_Npcs[data->npcid].icon.c_str());
	/*icon->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
	icon->setContentSize(Sprite::createWithSpriteFrameName(str)->getContentSize());*/
	icon->setVisible(false);

	ClippingNode* m_clippingNode = ClippingNode::create();
	m_clippingNode->setInverted(false);//设置底板可见
	m_clippingNode->setAlphaThreshold(0.5f);//设置透明度Alpha值为0
	this->addChild(m_clippingNode, 1);
	m_clippingNode->setAnchorPoint(Vec2(0.5, 1));
	m_clippingNode->setPosition(Vec2(icon->getPositionX(), icon->getPositionY() + 45));
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

	cocos2d::ui::Button* closebtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("closebtn");
	closebtn->setPosition(Vec2(357, 183));
	closebtn->setTag(0);
	closebtn->addTouchEventListener(CC_CALLBACK_2(TaskBranchDescLayer::onBtnClick, this));
	closebtn->setTitleText(ResourceLang::map_lang["closetext"]);

	accbtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("accbtn");
	accbtn->setPosition(Vec2(357, 376));
	accbtn->setTag(1);
	accbtn->addTouchEventListener(CC_CALLBACK_2(TaskBranchDescLayer::onBtnClick, this));
	accbtn->setTitleText(ResourceLang::map_lang["acctasktext"]);

	cocos2d::ui::Button* getbtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("getbtn");
	getbtn->setPosition(Vec2(357, 376));
	getbtn->setTag(2);
	getbtn->addTouchEventListener(CC_CALLBACK_2(TaskBranchDescLayer::onBtnClick, this));
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

	if (data->id != GlobalInstance::myCurBranchData.id && data->isfinish == QUEST_TASK)
	{
		getbtn->setVisible(false);
		accbtn->setVisible(false);
	}

	std::vector<std::vector<std::string>> rewards = data->reward;

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


	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}

void TaskBranchDescLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
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
			break;
		case 2: //完成后领取奖励
			node->setVisible(false);
			getRewards();
			break;
		default:
			break;
		}
	}
}

void TaskBranchDescLayer::accpTask()
{
	m_data->isfinish = QUEST_ACC;
	GlobalInstance::myCurBranchData.isfinish = QUEST_ACC;
}

void TaskBranchDescLayer::getRewards()
{
	m_data->isfinish = QUEST_GET;
	std::vector<std::vector<std::string>> rewarr = m_data->reward;
	for (unsigned int i = 0; i < rewarr.size(); i++)
	{
		std::vector<std::string> one_res = rewarr[i];
		std::string resid = one_res[0];
		int count = atoi(one_res[1].c_str());
		int qu = 0;
		int stc = 0;
		if (one_res.size()>2 && one_res[2].length()>0)
		{
			qu = atoi(one_res[2].c_str());
			stc = GlobalInstance::getInstance()->generateStoneCount(qu);
		}
		MyRes::Add(resid, count, MYSTORAGE, qu, stc);
	}
}