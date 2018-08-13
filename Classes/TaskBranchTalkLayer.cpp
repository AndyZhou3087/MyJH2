#include "TaskBranchTalkLayer.h"
#include <algorithm>
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "MyRes.h"
#include "MyMenu.h"
#include "MovingLabel.h"
#include "Quest.h"
#include "MapBlockScene.h"
#include "FightingLayer.h"

TaskBranchTalkLayer::TaskBranchTalkLayer()
{

}


TaskBranchTalkLayer::~TaskBranchTalkLayer()
{

}

TaskBranchTalkLayer* TaskBranchTalkLayer::create(std::string npcid, std::vector<Npc*> vec_enemys)
{
	TaskBranchTalkLayer *pRet = new(std::nothrow)TaskBranchTalkLayer();
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

bool TaskBranchTalkLayer::init(std::string npcid, std::vector<Npc*> vec_enemys)
{
	m_npcid = npcid;
	m_vec_enemys = vec_enemys;

	TaskBranchData* data = &GlobalInstance::myCurBranchData;
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
	Label* contentlbl = Label::createWithTTF(data->bossword, FONT_NAME, 25);
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
	std::string str = "ui/h_0_0.png"; //StringUtils::format("mapui/%s.png", data->npcid.c_str());
	icon->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
	icon->setContentSize(Sprite::createWithSpriteFrameName(str)->getContentSize());

	//npc名字
	cocos2d::ui::Text* npcname = (cocos2d::ui::Text*)m_csbnode->getChildByName("npcname");
	npcname->setString(GlobalInstance::map_AllResources[data->npcid].name);

	closebtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("closebtn");
	closebtn->setPosition(Vec2(357, 183));
	closebtn->setTag(0);
	closebtn->addTouchEventListener(CC_CALLBACK_2(TaskBranchTalkLayer::onBtnClick, this));
	closebtn->setTitleText(ResourceLang::map_lang["closetext"]);

	givebtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("accbtn");
	givebtn->setPosition(Vec2(357, 376));
	givebtn->setTag(1);
	givebtn->addTouchEventListener(CC_CALLBACK_2(TaskBranchTalkLayer::onBtnClick, this));
	givebtn->setTitleText(data->needdesc);

	cocos2d::ui::ScrollView* scrollView = (cocos2d::ui::ScrollView*)m_csbnode->getChildByName("ScrollView");
	scrollView->setScrollBarEnabled(false);
	scrollView->setBounceEnabled(true);

	if (data->type != QUEST_GIVE)
	{
		scrollView->setVisible(false);
		rewardlabel->setVisible(false);
	}

	std::vector<std::vector<std::string>> rewards = data->need;

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
			str = StringUtils::format("ui/%s-2.png", resid.c_str());
		}
		else if (qu == 4)
		{
			str = StringUtils::format("ui/%s-3.png", resid.c_str());
		}
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

void TaskBranchTalkLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
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
		case 1: //条件1
			if (GlobalInstance::myCurBranchData.type == QUEST_GIVE)
			{
				for (unsigned int i = 0; i < GlobalInstance::myCurBranchData.need.size(); i++)
				{
					std::vector<std::string> one_res = GlobalInstance::myCurBranchData.need[i];
					std::string cresid = one_res[0];
					int count = atoi(one_res[1].c_str());
					if (MyRes::getMyResCount(cresid, MYPACKAGE) < count)
					{
						MovingLabel::show(ResourceLang::map_lang["reslack"]);
						break;
					}
					Quest::setResQuestData(cresid, count, m_npcid);
					this->removeFromParentAndCleanup(true);
				}
			}
			else
			{
				g_MapBlockScene->showFightingLayer(m_vec_enemys);
				this->removeFromParentAndCleanup(true);
			}
			break;
		default:
			break;
		}
	}
}