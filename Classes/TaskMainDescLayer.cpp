#include "TaskMainDescLayer.h"
#include <algorithm>
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "MyRes.h"
#include "MyMenu.h"
#include "MovingLabel.h"
#include "TaskMainNode.h"

TaskMainDescLayer::TaskMainDescLayer()
{

}


TaskMainDescLayer::~TaskMainDescLayer()
{

}

TaskMainDescLayer* TaskMainDescLayer::create(TaskMainData* data)
{
	TaskMainDescLayer *pRet = new(std::nothrow)TaskMainDescLayer();
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

bool TaskMainDescLayer::init(TaskMainData* data)
{
	m_data = data;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);

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

	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)m_csbnode->getChildByName("desc");
	desc->setString(data->desc);

	//npc头像
	cocos2d::ui::ImageView* icon = (cocos2d::ui::ImageView*)m_csbnode->getChildByName("icon");
	std::string str = "ui/h_0_0.png";//StringUtils::format("mapui/%s.png", data->npcid.c_str());
	icon->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
	icon->setContentSize(Sprite::createWithSpriteFrameName(str)->getContentSize());

	//npc名字
	cocos2d::ui::Text* npcname = (cocos2d::ui::Text*)m_csbnode->getChildByName("npcname");
	npcname->setString(GlobalInstance::map_Npcs[data->npcid].name);

	cocos2d::ui::Button* closebtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("closebtn");
	closebtn->setPosition(Vec2(357, 183));
	closebtn->setTag(0);
	closebtn->addTouchEventListener(CC_CALLBACK_2(TaskMainDescLayer::onBtnClick, this));
	closebtn->setTitleText(ResourceLang::map_lang["closetext"]);

	accbtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("accbtn");
	accbtn->setPosition(Vec2(357, 376));
	accbtn->setTag(1);
	accbtn->addTouchEventListener(CC_CALLBACK_2(TaskMainDescLayer::onBtnClick, this));
	accbtn->setTitleText(ResourceLang::map_lang["acctasktext"]);

	cocos2d::ui::Button* getbtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("getbtn");
	getbtn->setPosition(Vec2(357, 376));
	getbtn->setTag(2);
	getbtn->addTouchEventListener(CC_CALLBACK_2(TaskMainDescLayer::onBtnClick, this));
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

	if (data->id != GlobalInstance::myCurMainData.id && data->isfinish == QUEST_TASK)
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
		for (unsigned int n = 0; n < data->reward2.size(); n++)
		{
			rewards.push_back(data->reward2[n]);
		}
	}

	for (unsigned int i = 0; i < rewards.size(); i++)
	{
		std::vector<std::string> one_res = rewards[i];
		std::string resid = one_res[0];
		int count = atoi(one_res[1].c_str());
		int qu = -1;
		if (one_res.size()>2 && one_res[2].length()>0)
		{
			qu = atoi(one_res[2].c_str());
		}

		std::string qustr = "ui/resbox.png";
		if (qu >= 0)
		{
			qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}
		Sprite * box = Sprite::createWithSpriteFrameName(qustr);
		box->setPosition(Vec2(80 + i % 3 * 170, 103 - i / 3 * 163));
		scrollView->addChild(box);

		std::string str = StringUtils::format("ui/%s.png", resid.c_str());
		if (qu == 3)
		{
			str = StringUtils::format("ui/%s_2.png", resid.c_str());
		}
		else if (qu == 4)
		{
			str = StringUtils::format("ui/%s_3.png", resid.c_str());
		}
		Sprite * res = Sprite::createWithSpriteFrameName(str);
		res->setPosition(Vec2(box->getContentSize().width / 2, box->getContentSize().height / 2));
		box->addChild(res);

		str = StringUtils::format("%sx%d", GlobalInstance::map_AllResources[resid].name.c_str(), count);
		Label *namelbl = Label::createWithTTF(str, FONT_NAME, 23);
		namelbl->setColor(Color3B(255, 255, 255));
		namelbl->setPosition(Vec2(box->getContentSize().width / 2, -20));
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

void TaskMainDescLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
		switch (tag)
		{
		case 0:
			this->removeFromParentAndCleanup(true);
			break;
		case 1: //接受任务
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

void TaskMainDescLayer::accpTask()
{
	m_data->isfinish = QUEST_ACC;
	GlobalInstance::myCurMainData.isfinish = QUEST_ACC;
}

void TaskMainDescLayer::getRewards()
{
	m_data->isfinish = QUEST_GET;
	for (unsigned int i = 0; i < rewards.size(); i++)
	{
		std::vector<std::string> one_res = rewards[i];
		std::string resid = one_res[0];
		int count = atoi(one_res[1].c_str());
		int qu = -1;
		if (one_res.size()>2 && one_res[2].length()>0)
		{
			qu = atoi(one_res[2].c_str());
		}
		MyRes::Add(resid, count, MYSTORAGE, qu);
	}
}