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

}


TaskTalkLayer::~TaskTalkLayer()
{

}

TaskTalkLayer* TaskTalkLayer::create(std::string npcid, std::vector<Npc*> vec_enemys)
{
	TaskTalkLayer *pRet = new(std::nothrow)TaskTalkLayer();
	if (pRet && pRet->init(npcid, vec_enemys))
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

bool TaskTalkLayer::init(std::string npcid, std::vector<Npc*> vec_enemys)
{
	m_npcid = npcid;
	m_vec_enemys = vec_enemys;

	TaskMainData* data = &GlobalInstance::myCurMainData;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);

	Node* m_csbnode = CSLoader::createNode(ResourcePath::makePath("taskMainDescLayer.csb"));
	this->addChild(m_csbnode);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	int langtype = GlobalInstance::getInstance()->getLang();

	//����
	cocos2d::ui::Text* name = (cocos2d::ui::Text*)m_csbnode->getChildByName("name");
	name->setString(data->name);

	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)m_csbnode->getChildByName("desc");
	desc->setString(data->bossword);

	//npcͷ��
	cocos2d::ui::ImageView* icon = (cocos2d::ui::ImageView*)m_csbnode->getChildByName("icon");
	std::string str = StringUtils::format("mapui/%s.png", data->npcid.c_str());
	icon->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	//npc����
	cocos2d::ui::Text* npcname = (cocos2d::ui::Text*)m_csbnode->getChildByName("npcname");
	npcname->setString(GlobalInstance::map_Npcs[data->npcid].name);

	closebtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("closebtn");
	closebtn->setPosition(Vec2(357, 131));
	closebtn->setTag(0);
	closebtn->addTouchEventListener(CC_CALLBACK_2(TaskTalkLayer::onBtnClick, this));

	givebtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("accbtn");
	givebtn->setPosition(Vec2(357, 429));
	givebtn->setTag(1);
	givebtn->addTouchEventListener(CC_CALLBACK_2(TaskTalkLayer::onBtnClick, this));
	givebtn->setTitleText(data->need1desc);

	fightbtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("getbtn");
	fightbtn->setVisible(true);
	fightbtn->setPosition(Vec2(357, 285));
	fightbtn->setTag(2);
	fightbtn->addTouchEventListener(CC_CALLBACK_2(TaskTalkLayer::onBtnClick, this));
	fightbtn->setTitleText(data->need2desc);

	cocos2d::ui::ScrollView* scrollView = (cocos2d::ui::ScrollView*)m_csbnode->getChildByName("ScrollView");
	scrollView->setScrollBarEnabled(false);
	scrollView->setBounceEnabled(true);

	if (data->type.size()<2)
	{
		if (data->type[0] == QUEST_GIVE)
		{
			fightbtn->setVisible(false);
			givebtn->setPosition(Vec2(357, 376));
		}
		else
		{
			givebtn->setVisible(false);
			fightbtn->setPosition(Vec2(357, 376));
		}
		closebtn->setPosition(Vec2(357, 183));
	}
	else
	{
		//�ж��Ƿ񻥳�
		if (Quest::getMutexMainQuestType(data->id, data->type[0]))
		{
			givebtn->setTouchEnabled(false);
		}
		if (Quest::getMutexMainQuestType(data->id, data->type[1]))
		{
			fightbtn->setTouchEnabled(false);
		}
	}

	std::vector<std::vector<std::string>> rewards;
	if (data->type.size() == 1)
	{
		if (data->type[0] == QUEST_GIVE)
		{
			rewards = data->reward1;
		}
		else
		{
			rewards = data->reward2;
		}
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

void TaskTalkLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
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
		case 1: //����1
		{
			bool isEnough = true;
			int isGo = GlobalInstance::myCurMainData.isGo;
			for (unsigned int i = 0; i < GlobalInstance::myCurMainData.need1.size(); i++)
			{
				std::map<std::string, int> one_res = GlobalInstance::myCurMainData.need1[i];
				std::map<std::string, int>::iterator oneit = one_res.begin();
				std::string cresid = oneit->first;
				if (MyRes::getMyResCount(cresid, MYPACKAGE) < oneit->second)
				{
					isEnough = false;
					MovingLabel::show(ResourceLang::map_lang["reslack"]);
					break;
				}
				Quest::setResQuestData(cresid, oneit->second, m_npcid);
			}
			if (isEnough)
			{
				if (isGo == 1)
				{
					g_MapBlockScene->addChild(FightingLayer::create(m_vec_enemys));
				}
				this->removeFromParentAndCleanup(true);
			}
		}
			break;
		case 2: //����2
			g_MapBlockScene->addChild(FightingLayer::create(m_vec_enemys));
			this->removeFromParentAndCleanup(true);
			break;
		default:
			break;
		}
	}
}