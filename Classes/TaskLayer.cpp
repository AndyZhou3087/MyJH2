#include "TaskLayer.h"
#include <algorithm>
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "MyRes.h"
#include "MyMenu.h"
#include "MovingLabel.h"
#include "TaskNode.h"
#include "TaskNode.h"
#include "TaskDailyNode.h"
#include "DataSave.h"
#include "Quest.h"
#include "AnimationEffect.h"
#include "SimpleResPopLayer.h"
#include "MainScene.h"
#include "NewGuideLayer.h"

TaskLayer::TaskLayer()
{
	lastCategoryindex = 0;
	for (int i = 0; i < 2; i++)
	{
		questtipArr[i] = NULL;
	}
}


TaskLayer::~TaskLayer()
{
	GlobalInstance::getInstance()->saveMyTaskMainData();
	GlobalInstance::getInstance()->saveMyTaskBranchData();
	GlobalInstance::getInstance()->saveMyDailyTaskData();
	//if (!NewGuideLayer::checkifNewerGuide(FIFTHGUIDESTEP))
	//{
	//	g_mainScene->setScrollGliding();
	//}
}

bool TaskLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	m_csbnode = CSLoader::createNode(ResourcePath::makePath("taskLayer.csb"));
	this->addChild(m_csbnode);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	langtype = GlobalInstance::getInstance()->getLang();

	pnode = (cocos2d::ui::Widget*)m_csbnode->getChildByName("pnode");
	mypoint = (cocos2d::ui::Text*)pnode->getChildByName("mypoint");
	std::string pstr = StringUtils::format("%d", DataSave::getInstance()->getMyyDailyPoint());
	mypoint->setString(pstr);

	probar = (cocos2d::ui::LoadingBar*)pnode->getChildByName("probar");
	float per = DataSave::getInstance()->getMyyDailyPoint() * 100 / 200;
	probar->setPercent(per);

	//БъЬт
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)m_csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("halltitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)m_csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(TaskLayer::onBtnClick, this));

	scrollview = (cocos2d::ui::ScrollView*)m_csbnode->getChildByName("scrollview");
	scrollview->setScrollBarEnabled(false);
	scrollview->setBounceEnabled(true);

	cocos2d::ui::Text* hintdesc = (cocos2d::ui::Text*)m_csbnode->getChildByName("hintdesc");
	hintdesc->setString(ResourceLang::map_lang["taskhintdesc"]);

	Node* categoryBtnNode = m_csbnode->getChildByName("catanode");
	for (int i = 0; i < categoryBtnNode->getChildrenCount(); i++)
	{
		std::string btnstr = StringUtils::format("btn%d", i);
		cocos2d::ui::Button* btn = (cocos2d::ui::Button*)categoryBtnNode->getChildByName(btnstr);
		btn->setTag(i);
		btn->addTouchEventListener(CC_CALLBACK_2(TaskLayer::onCategory, this));
		vec_categoryBtn.push_back(btn);

		btnstr = StringUtils::format("questtip%d", i);
		cocos2d::ui::Widget* questtip = (cocos2d::ui::Widget*)btn->getChildByName(btnstr);
		questtipArr[i] = questtip;
	}

	updateQuestTip();
	updateContent(0);

	updateDaily(0);
	this->schedule(schedule_selector(TaskLayer::updateDaily), 1.0f);


	int t = (GlobalInstance::servertime+8*60*60) / 60 / 60 / 24;
	if (t > DataSave::getInstance()->getMyFreshDate())
	{
		Quest::resetDailyTask();
	}

	updateDailyTip();

	this->scheduleOnce(schedule_selector(TaskLayer::delayShowNewerGuide), newguidetime);

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}

void TaskLayer::delayShowNewerGuide(float dt)
{
	if (!NewGuideLayer::checkifNewerGuide(14))
	{
		if (NewGuideLayer::checkifNewerGuide(MIDELEGUIDESTEP))
		{
			if (NewGuideLayer::checkifNewerGuide(41))
			{
				showNewerGuide(41);
			}
			else if (NewGuideLayer::checkifNewerGuide(44))
			{
				showNewerGuide(44);
			}
		}
		else if (NewGuideLayer::checkifNewerGuide(FIFTHGUIDESTEP) && Quest::isMainQuestFinish(1))
		{
			if (NewGuideLayer::checkifNewerGuide(56))
			{
				showNewerGuide(56);
			}
			else if (NewGuideLayer::checkifNewerGuide(59))
			{
				showNewerGuide(59);
			}
			else if (NewGuideLayer::checkifNewerGuide(62))
			{
				showNewerGuide(62);
			}
		}
	}
}

void TaskLayer::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	if (step == 41 || step == 59)
	{
		nodes.push_back(scrollview->getChildren().at(0)->getChildByName("csbnode")->getChildByName("resitem"));
	}
	else if (step == 44)
	{
		nodes.push_back(m_csbnode->getChildByName("closebtn"));
	}
	else if (step == 56)
	{
		nodes.push_back(scrollview->getChildren().at(1)->getChildByName("csbnode")->getChildByName("resitem"));
	}
	else if (step == 62)
	{
		nodes.push_back(vec_categoryBtn[1]);
		nodes.push_back(vec_categoryBtn[2]);
	}
	g_mainScene->showNewerGuideNode(step, nodes);
}

void TaskLayer::updateContent(int category)
{
	scrollview->removeAllChildrenWithCleanup(true);

	for (unsigned int i = 0; i < vec_categoryBtn.size(); i++)
	{
		cocos2d::ui::ImageView* btntext = (cocos2d::ui::ImageView*)vec_categoryBtn[i]->getChildByName("text");

		if (category != i)
		{
			std::string textstr = StringUtils::format("task_text_%d_n", i);
			btntext->loadTexture(ResourcePath::makeTextImgPath(textstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			vec_categoryBtn[i]->setBright(true);
		}
		else
		{
			std::string textstr = StringUtils::format("task_text_%d_s", i);
			vec_categoryBtn[i]->setBright(false);
			btntext->loadTexture(ResourcePath::makeTextImgPath(textstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		}
	}

	int ressize;
	int mcount = 0;
	if (category == 0)
	{
		ressize = GlobalInstance::vec_TaskMain.size();
		sort(GlobalInstance::vec_TaskMain.begin(), GlobalInstance::vec_TaskMain.end(), larger_callback);
		scrollview->setContentSize(Size(scrollview->getContentSize().width, 990));
		pnode->setVisible(false);

		for (int i = 0; i < ressize; i++)
		{
			if (GlobalInstance::vec_TaskMain[i].isfinish > QUEST_TASK || GlobalInstance::vec_TaskMain[i].id == GlobalInstance::myCurMainData.id)
			{
				mcount++;
			}
		}
	}
	else if (category == 1)
	{
		ressize = GlobalInstance::vec_TaskBranch.size();
		sort(GlobalInstance::vec_TaskBranch.begin(), GlobalInstance::vec_TaskBranch.end(), larger_branchcallback);
		scrollview->setContentSize(Size(scrollview->getContentSize().width, 990));
		pnode->setVisible(false);

		for (int i = 0; i < ressize; i++)
		{
			if (GlobalInstance::vec_TaskBranch[i].isfinish > QUEST_TASK || GlobalInstance::vec_TaskBranch[i].id == GlobalInstance::myCurBranchData.id)
			{
				mcount++;
			}
		}
	}
	else
	{
		ressize = GlobalInstance::map_DTdata.size();
		mcount = ressize;
		scrollview->setContentSize(Size(scrollview->getContentSize().width, 830));
		pnode->setVisible(true);
	}

	int itemheight = 140;
	int innerheight = itemheight * mcount;

	int contentheight = scrollview->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollview->setInnerContainerSize(Size(scrollview->getContentSize().width, innerheight));

	if (category != 2)
	{
		int m_count = 0;
		int b_count = 0;
		for (int i = 0; i < ressize; i++)
		{
			Node* node;
			if (category == 0)
			{
				if (GlobalInstance::vec_TaskMain[i].isfinish > QUEST_TASK || GlobalInstance::vec_TaskMain[i].id == GlobalInstance::myCurMainData.id)
				{
					node = TaskNode::create(&GlobalInstance::vec_TaskMain[i]);
					scrollview->addChild(node);

					node->setPosition(Vec2(scrollview->getContentSize().width + 600, innerheight - m_count * itemheight - itemheight / 2));
					node->runAction(EaseSineIn::create(MoveBy::create(0.15f + m_count*0.07f, Vec2(-scrollview->getContentSize().width / 2 - 600, 0))));

					m_count++;
				}
			}
			else if (category == 1)
			{
				if (GlobalInstance::vec_TaskBranch[i].isfinish > QUEST_TASK || GlobalInstance::vec_TaskBranch[i].id == GlobalInstance::myCurBranchData.id)
				{
					node = TaskNode::create(&GlobalInstance::vec_TaskBranch[i], 1);
					scrollview->addChild(node);

					node->setPosition(Vec2(scrollview->getContentSize().width + 600, innerheight - b_count * itemheight - itemheight / 2));
					node->runAction(EaseSineIn::create(MoveBy::create(0.15f + b_count*0.07f, Vec2(-scrollview->getContentSize().width / 2 - 600, 0))));
					b_count++;
				}
			}
		}
	}
	else
	{
		std::vector<DailyTaskData*> vec_fin;
		std::vector<DailyTaskData*> vec_unfin;
		std::vector<DailyTaskData*> vec_get;

		std::map<std::string, DailyTaskData>::iterator it;
		for (it = GlobalInstance::map_DTdata.begin(); it != GlobalInstance::map_DTdata.end(); it++)
		{
			DailyTaskData data = GlobalInstance::map_DTdata[it->first];
			if (data.state == DAILY_RECEIVE)
			{
				vec_get.push_back(&GlobalInstance::map_DTdata[it->first]);
			}
			else if (data.state == DAILY_FINISHED)
			{
				vec_fin.push_back(&GlobalInstance::map_DTdata[it->first]);
			}
			else
			{
				vec_unfin.push_back(&GlobalInstance::map_DTdata[it->first]);
			}
		}
		std::vector<DailyTaskData*> vec_all;
		for (unsigned int i = 0; i < vec_fin.size(); i++)
		{
			vec_all.push_back(vec_fin[i]);

		}
		for (unsigned int i = 0; i < vec_unfin.size(); i++)
		{
			vec_all.push_back(vec_unfin[i]);
		}
		for (unsigned int i = 0; i < vec_get.size(); i++)
		{
			vec_all.push_back(vec_get[i]);
		}

		for (unsigned int i = 0; i < vec_all.size(); i++)
		{
			Node* node = TaskDailyNode::create(vec_all[i]);
			scrollview->addChild(node);

			node->setPosition(Vec2(scrollview->getContentSize().width + 600, innerheight - i * itemheight - itemheight / 2));
			node->runAction(EaseSineIn::create(MoveBy::create(0.15f + i*0.07f, Vec2(-scrollview->getContentSize().width / 2 - 600, 0))));
			//node->setPosition(Vec2(scrollview->getContentSize().width / 2, innerheight - i*itemheight - itemheight*0.5));
		}
	}
	
}

void TaskLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		AnimationEffect::closeAniEffect((Layer*)this);
	}
}

bool TaskLayer::larger_callback(TaskData a, TaskData b)
{
	int needcountA = a.isfinish;
	int needcountB = b.isfinish;
	if (needcountA < needcountB)
		return true;
	else
		return false;
}

bool TaskLayer::larger_branchcallback(TaskData a, TaskData b)
{
	int needcountA = a.isfinish;
	int needcountB = b.isfinish;
	if (needcountA < needcountB)
		return true;
	else
		return false;
}

void TaskLayer::updateDaily(float dt)
{
	int m_point = DataSave::getInstance()->getMyyDailyPoint();
	std::string pstr = StringUtils::format("%d", m_point);
	mypoint->setString(pstr);
	float per = m_point * 100 / 200;
	probar->setPercent(per);
	loadData(lastCategoryindex);
}

void TaskLayer::loadData(int category)
{
	if (category == 2)
	{
		int m_point = DataSave::getInstance()->getMyyDailyPoint();

		std::map<int, int>::iterator it;
		for (it = Quest::map_PointReward.begin(); it != Quest::map_PointReward.end(); it++)
		{
			std::string str = StringUtils::format("point_%d", it->first);
			cocos2d::ui::Widget* point = (cocos2d::ui::Widget*)pnode->getChildByName(str);
			point->addTouchEventListener(CC_CALLBACK_2(TaskLayer::onPointClick, this));
			point->setTag(it->first);
			/*if (m_point < it->first || Quest::map_PointReward[it->first] == 1)
			{
				point->setTouchEnabled(false);
			}
			else
			{
				point->setTouchEnabled(true);
			}*/

			str = StringUtils::format("%dpoint", it->first);
			cocos2d::ui::Text* plabel = (cocos2d::ui::Text*)pnode->getChildByName(str);
			str = StringUtils::format(ResourceLang::map_lang["pointtext"].c_str(), it->first);
			plabel->setString(str);
		}
	}
}

void TaskLayer::onPointClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	//CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* pnode = (Node*)pSender;
		int tag = pnode->getTag();
		cocos2d::ui::ImageView* node = (cocos2d::ui::ImageView*)pSender;

		std::string resid;
		switch (tag)
		{
		case 50:
			resid = "s001";
			break;
		case 100:
			resid = "s002";
			break;
		case 150:
			resid = "s003";
			break;
		case 200:
			resid = "s004";
			break;
		default:
			break;
		}

		int m_point = DataSave::getInstance()->getMyyDailyPoint();
		if (m_point < tag)
		{
			SimpleResPopLayer* layer = SimpleResPopLayer::create(resid);
			this->addChild(layer);
			AnimationEffect::openAniEffect(layer);

			MovingLabel::show(ResourceLang::map_lang["nomorepoint"]);
			return;
		}
		else if (Quest::map_PointReward[tag] == 1)
		{
			MovingLabel::show(ResourceLang::map_lang["getpointreward"]);
			return;
		}

		MyRes::Add(resid);
		std::string str = StringUtils::format(ResourceLang::map_lang["dailytype_9"].c_str(), GlobalInstance::map_AllResources[resid].name.c_str(), 1);
		MovingLabel::show(str);
		Quest::saveDailyPointReward(tag);
	}
}

void TaskLayer::onCategory(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		if (lastCategoryindex == node->getTag())
			return;

		lastCategoryindex = node->getTag();

		updateContent(node->getTag());
	}
}

void TaskLayer::skipContent()
{
	lastCategoryindex = 1;
	updateContent(1);
}

void TaskLayer::updateQuestTip()
{
	if (!Quest::isShowQuestTip())
	{
		if (questtipArr[0] != NULL)
		{
			questtipArr[0]->setVisible(false);
		}
	}
	if (!Quest::isShowBranchQuestTip())
	{
		if (questtipArr[1] != NULL)
		{
			questtipArr[1]->setVisible(false);
		}
	}
}

void TaskLayer::updateDailyTip()
{
	if (questtipArr[2] != NULL)
	{
		if (!Quest::getDailyTaskGetRewards())
		{
			questtipArr[2]->setVisible(false);
		}
		else
		{
			questtipArr[2]->setVisible(true);
		}
	}
}